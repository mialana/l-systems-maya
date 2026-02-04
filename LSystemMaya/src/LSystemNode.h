#pragma once

#include <maya/MPointArray.h>
#include <maya/MFnMesh.h>
#include <maya/MPxNode.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>

#include "LSystem.h"

class LSystemNode : public MPxNode
{
public:
    LSystemNode() = default;
    virtual ~LSystemNode() = default;

    static void* creator()
    {
        return new LSystemNode;
    };

    static MStatus initialize();
    virtual MStatus compute(const MPlug& plug, MDataBlock& data) override;

    static const MTypeId kNodeId;

    // typed attributes
    static MObject sGrammarAttr;
    static MObject sOutputMeshAttr;

    // numeric attributes
    static MObject sStepSizeAttr;
    static MObject sAngleAttr;

    // unit attributes
    static MObject sTimeAttr;

private:
    LSystem mSystem;
    std::vector<LSystem::Branch> mBranches;

    MPointArray mPoints;
    MIntArray mFaceCounts;
    MIntArray mFaceConnects;

    // cached grammar string to prevent system from reloading every frame
    std::string mGrammarCache;
    uint32_t mIterationsCache;
    double mAngleCache;
    double mStepSizeCache;
};
