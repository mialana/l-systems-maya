#include "LSystemCmd.h"

#include <maya/MPoint.h>
#include <maya/MGlobal.h>
#include <maya/MPointArray.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MDGModifier.h>
#include <maya/MArgDatabase.h>
#include <maya/MPlug.h>
#include <maya/MSyntax.h>
#include <maya/MPxCommand.h>
#include <list>
#include <string>

constexpr const char k_STEP_SIZE_SHORT[] = "-ss";
constexpr const char k_STEP_SIZE_LONG[] = "-stepSize";

constexpr const char k_ANGLE_SHORT[] = "-ag";
constexpr const char k_ANGLE_LONG[] = "-angle";

constexpr const char k_GRAMMAR_SHORT[] = "-gr";
constexpr const char k_GRAMMAR_LONG[] = "-grammar";

constexpr const char k_ITERATIONS_SHORT[] = "-it";
constexpr const char k_ITERATIONS_LONG[] = "-iterations";

LSystemCmd::LSystemCmd() : MPxCommand() {}

LSystemCmd::~LSystemCmd() {}

static MSyntax getSyntax()
{
    MSyntax syntax;
    syntax.addFlag(k_STEP_SIZE_SHORT, k_STEP_SIZE_LONG, MSyntax::kDouble);
    syntax.addFlag(k_ANGLE_SHORT, k_ANGLE_LONG, MSyntax::kDouble);
    syntax.addFlag(k_GRAMMAR_SHORT, k_GRAMMAR_LONG, MSyntax::kString);
    syntax.addFlag(k_ITERATIONS_SHORT, k_ITERATIONS_LONG, MSyntax::kLong);
    return syntax;
}

MStatus LSystemCmd::extrudeCurveFn()
{
    MStatus status;
    MDGModifier dg;

    MObject circleNode = dg.createNode("makeNurbCircle", &status);
    dg.doIt();

    MFnDependencyNode circleFn(circleNode);
    circleFn.findPlug("radius").setDouble(1.0);

    // Create transform
    MFnDagNode transformFn;
    MObject transformObj = transformFn.create("transform");
    transformFn.setName("nurbsCircle1");

    // Create nurbsCurve shape under transform
    MFnDagNode shapeFn;
    MObject shapeObj = shapeFn.create("nurbsCurve", transformObj);

    MPlug outCurve = circleFn.findPlug("outputCurve", true);
    MPlug create = shapeFn.findPlug("create", true);

    dg.connect(outCurve, create);
    dg.doIt();
    
    // Create extrude node
    MObject extrudeObj = dg.createNode("extrude");
    dg.doIt();

    MFnDependencyNode extrudeFn(extrudeObj);
    extrudeFn.setName("extrudeCircle1");

    // Find plugs
    MPlug profilePlug = extrudeFn.findPlug("inputProfile", true);
    MPlug pathPlug = extrudeFn.findPlug("inputPath", true);

    // Get shape nodes
    MObject circleShape = circleFn.object();
    MObject curveShape = this->mCurveFn.object();

    MFnDependencyNode circleShapeFn(shapeObj);
    MFnDependencyNode curveShapeFn(curveShape);

    // Connect worldSpace outputs
    dg.connect(circleShapeFn.findPlug("worldSpace", true).elementByLogicalIndex(0), profilePlug);
    dg.connect(curveShapeFn.findPlug("worldSpace", true).elementByLogicalIndex(0), pathPlug);

    MFnDagNode surfXformFn;
    MObject surfXform = surfXformFn.create("transform");
    surfXformFn.setName("extrudeSurface1");

    // Create nurbsSurface shape
    MFnDagNode surfShapeFn;
    MObject surfShape = surfShapeFn.create("nurbsSurface", surfXform);

    // Connect extrude outputSurface to surface.create
    MPlug outSurface = extrudeFn.findPlug("outputSurface", true);
    MPlug surfCreate = surfShapeFn.findPlug("create", true);

    dg.doIt();

    return status;
}

MStatus LSystemCmd::makeCurveFn()
{
    MStatus status;
    MPoint start(0, 0, 0);
    MPoint end(10, 10, 10);

    MPointArray points;
    points.append(start);
    points.append(end);

    MDoubleArray knots;
    knots.append(0.0);
    knots.append(1.0);

    this->mCurveFn.create(points, knots, 1, MFnNurbsCurve::kOpen, false, false, MObject::kNullObj, &status);

    return status;
}

MStatus LSystemCmd::createGeometry()
{
    // configure l-system base code
    mSystem.loadProgramFromString(mGrammar);
    mSystem.setDefaultAngle(mAngle);
    mSystem.setDefaultStep(mStepSize);
    std::vector<LSystem::Branch> branches;
    mSystem.process(this->mIterations, branches);

    static vec3 radius;
    static MString startX, startY, startZ, endX, endY, endZ, radiusX, radiusY, radiusZ;    
    static MString label, cmd;
    
    for (uint32_t iter = 0; iter < branches.size(); iter++)
    {
        label = std::to_string(iter + 1).c_str();
        const LSystem::Branch& branch = branches[iter];
        const vec3& start = branch.first;
        const vec3& end = branch.second;

        startX = to_string(start[0]).c_str();
        startY = to_string(start[1]).c_str();
        startZ = to_string(start[2]).c_str();

        endX = to_string(end[0]).c_str();
        endX = to_string(end[1]).c_str();
        endZ = to_string(end[2]).c_str();

        radius = (end - start).Normalize();
        radiusX = to_string(radius[0]).c_str();
        radiusY = to_string(radius[1]).c_str();
        radiusZ = to_string(radius[2]).c_str();

        cmd = "curve -d 1 -p " + startX + " " + startY + " " + startZ + "-p " + endX + " " + endY
              + " " + endZ + " -k 0 -k 1 -name \" curve" + label
              + "\";"
                "circle -radius 0.1 -nr "
              + radiusX + " " + radiusY + " " + radiusZ + "-c " + startX + " " + startY + " "
              + startZ + " -name \"nurbsCircle" + label
              + "\";"
                "select -r nurbsCircle"
              + label + " curve" + label
              + " ;"
                "extrude -ch true -rn false -po 1 -et 2 -rotation 0 -scale 1 -rsp 1 \"nurbsCircle "
              + label + "\" \"curve" + label + "\"";
        MGlobal::executeCommand(cmd);
    }
}

MStatus LSystemCmd::doIt(const MArgList& args)
{
    /*MStatus makeCurveFnStatus = this->makeCurveFn();

    CHECK_MSTATUS_AND_RETURN_IT(makeCurveFnStatus);

    MStatus extrudeCurveFnStatus = this->extrudeCurveFn();
    CHECK_MSTATUS(extrudeCurveFnStatus);*/

    MStatus status;

    MArgDatabase argData(getSyntax(), args, &status);
    CHECK_MSTATUS_AND_RETURN_IT(status);

    int tmpIterations;
    MString tmpGrammar;

    if (!argData.isFlagSet(k_GRAMMAR_LONG))
    {
        MGlobal::displayError("Input grammar is required.");
        return MS::kFailure;
    }

    argData.getFlagArgument(k_GRAMMAR_LONG, 0, tmpGrammar);
    this->mGrammar = tmpGrammar.asChar(); // convert to c string

    if (argData.isFlagSet(k_STEP_SIZE_LONG))
    {
        argData.getFlagArgument(k_STEP_SIZE_LONG, 0, this->mStepSize);
    }
    if (argData.isFlagSet(k_ANGLE_LONG))
    {
        argData.getFlagArgument(k_ANGLE_LONG, 0, this->mAngle);
    }
    if (argData.isFlagSet(k_ITERATIONS_LONG))
    {
        argData.getFlagArgument(k_ITERATIONS_LONG, 0, tmpIterations);
        this->mIterations = static_cast<int64_t>(tmpIterations);
    }

    
    return status;
}
