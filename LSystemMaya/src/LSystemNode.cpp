#include "LSystemNode.h"

#include <cmath>
#include <minmax.h>
#include <fstream>

#include <maya/MPoint.h>
#include <maya/MTime.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnStringData.h>
#include <maya/MFnMeshData.h>

#include "cylinder.h"
#include "macros.h"

const MTypeId LSystemNode::kNodeId{ 0x24681050 }; // random hex code ID

MObject LSystemNode::sGrammarAttr;
MObject LSystemNode::sOutputMeshAttr;

MObject LSystemNode::sAngleAttr;
MObject LSystemNode::sStepSizeAttr;

MObject LSystemNode::sTimeAttr;

MStatus LSystemNode::initialize()
{
    MFnTypedAttribute typedAttr;  // typed attribute creator
    typedAttr.setCached(true);
    sGrammarAttr = typedAttr.create("grammar", "gr", MFnData::kString,
                                    MFnStringData().create(""));
    typedAttr.setUsedAsFilename(true);

    sOutputMeshAttr = typedAttr.create("outputMesh", "out", MFnData::kMesh);

    MFnNumericAttribute numericAttr; // numeric attribute creator
    numericAttr.setCached(true);
    sStepSizeAttr = numericAttr.create("stepSize", "ss", MFnNumericData::kDouble, 22.5);
    sAngleAttr = numericAttr.create("angle", "ag", MFnNumericData::kDouble, 5.0);
    
    MFnUnitAttribute unitAttr; // unit attribute creator
    unitAttr.setCached(true);
    sTimeAttr = unitAttr.create("time", "t", MFnUnitAttribute::kTime, 2);

    MStatus status;
    
    status = addAttribute(sGrammarAttr);
    CHECK_MSTATUS_AND_RETURN_IT_VERBOSE(status, "Add Grammar Attribute");

    status = addAttribute(sOutputMeshAttr);
    CHECK_MSTATUS_AND_RETURN_IT_VERBOSE(status, "Add Output Mesh Attribute");
    
    status = addAttribute(sStepSizeAttr);
    CHECK_MSTATUS_AND_RETURN_IT_VERBOSE(status, "Add Step Size Attribute");

    status = addAttribute(sAngleAttr);
    CHECK_MSTATUS_AND_RETURN_IT_VERBOSE(status, "Add Angle Attribute");

    status = addAttribute(sTimeAttr);
    CHECK_MSTATUS_AND_RETURN_IT_VERBOSE(status, "Add Time Attribute");

    // create links
    status = attributeAffects(sTimeAttr, sOutputMeshAttr);
    CHECK_MSTATUS_AND_RETURN_IT_VERBOSE(status, "Connect Time & Output Mesh Attribute");

    status = attributeAffects(sAngleAttr, sOutputMeshAttr);
    CHECK_MSTATUS_AND_RETURN_IT_VERBOSE(status, "Connect Angle & Output Mesh Attribute");

    status = attributeAffects(sStepSizeAttr, sOutputMeshAttr);
    CHECK_MSTATUS_AND_RETURN_IT_VERBOSE(status, "Connect Step Size & Output Mesh Attribute");

    status = attributeAffects(sGrammarAttr, sOutputMeshAttr);
    CHECK_MSTATUS_AND_RETURN_IT_VERBOSE(status, "Connect Grammar & Output Mesh Attribute");

    return status;
}

MStatus LSystemNode::compute(const MPlug& plug, MDataBlock& data)
{
    if (plug != sOutputMeshAttr)
    {
        return MStatus::kSuccess;
    }

    MStatus status;

    MDataHandle grammarHandle = data.inputValue(sGrammarAttr, &status);
    CHECK_MSTATUS_AND_RETURN_IT_VERBOSE(status, "Query Grammar Attribute Handle");


    MDataHandle meshHandle = data.outputValue(sOutputMeshAttr, &status);
    CHECK_MSTATUS_AND_RETURN_IT_VERBOSE(status, "Query Output Mesh Attribute Handle");

    MDataHandle degreeHandle = data.inputValue(sAngleAttr, &status);
    CHECK_MSTATUS_AND_RETURN_IT_VERBOSE(status, "Query Angle Attribute Handle");
    MDataHandle stepHandle = data.inputValue(sStepSizeAttr, &status);
    CHECK_MSTATUS_AND_RETURN_IT_VERBOSE(status, "Query Step Size Attribute Handle");

    MDataHandle timeHandle = data.inputValue(sTimeAttr, &status);
    CHECK_MSTATUS_AND_RETURN_IT_VERBOSE(status, "Query Time Attribute Handle");

    MString grammarFilepath = grammarHandle.asString();
    if (grammarFilepath == "")
    {
        return MStatus::kSuccess;
    }
    std::ifstream fileStream(grammarFilepath.asChar(), std::ios::in);  // load file contents into stream
    if (!fileStream)
    {
        status = MStatus::kFailure;
        MGlobal::displayError(MString("Could not open grammar file: ") + grammarFilepath);
        return status;
    }

    static std::string grammar{ std::istreambuf_iterator<char>(fileStream),
                                std::istreambuf_iterator<char>() };

    double stepSize = stepHandle.asDouble();
    double angle = degreeHandle.asDouble();
    int32_t time = floor(timeHandle.asTime().value());
    
    if ((grammar != mGrammarCache) || mBranches.empty())
    {
        mSystem.loadProgramFromString(grammar.c_str()); // only load when necessary
        mGrammarCache = grammar;
    }

    uint32_t iterations = max(1, time);
    if ((iterations == mIterationsCache) && (angle == mAngleCache) && (stepSize == mStepSizeCache) && !mBranches.empty())
    {
        return MStatus::kSuccess; // prevent re-process if no attributes have changed
    }
        
    mSystem.setDefaultAngle(angle);
    mSystem.setDefaultStep(stepSize);
    
    mSystem.process(iterations, mBranches);

    mIterationsCache = iterations;
    mStepSizeCache = stepSize;
    mAngleCache = angle;

    mPoints.clear();
    mFaceCounts.clear();
    mFaceConnects.clear();
    
    MFnMeshData meshDataFn;

    MObject mesh = meshDataFn.create(&status);
    CHECK_MSTATUS_AND_RETURN_IT_VERBOSE(status, "Create New Mesh");

    MPoint startPoint;
    MPoint endPoint;
    for (int i = 0; i < mBranches.size(); i++)
    {
        const vec3& start = mBranches[i].first;
        const vec3& end = mBranches[i].second;

        startPoint = MPoint(start.n);
        endPoint = MPoint(end.n);

        CylinderMesh cylinder = CylinderMesh(startPoint, endPoint);
        cylinder.appendToMesh(mPoints, mFaceCounts, mFaceConnects);
    }

    MFnMesh meshFn;
    meshFn.create(mPoints.length(), mFaceCounts.length(), mPoints, mFaceCounts, mFaceConnects, mesh,
                  &status);
    CHECK_MSTATUS_AND_RETURN_IT_VERBOSE(status, "Populate Mesh");
    
    meshHandle.set(mesh);
    data.setClean(plug);
    
    return status;
}
