#pragma once

#include <maya/MPxCommand.h>
#include <string>

#include <LSystem.h>

class LSystemCmd : public MPxCommand
{
public:
    LSystemCmd() = default;
    virtual ~LSystemCmd();

    static void* creator()
    {
        return new LSystemCmd();
    }

    MStatus createGeometry();
    MStatus doIt(const MArgList& args);

    // stored arguments as normal C data structures
    std::string mGrammar;
    double mStepSize = 22.5;
    double mAngle = 1.0;
    int32_t mIterations = 3;

private:
    LSystem mSystem;
    std::vector<LSystem::Branch> mBranches;
};
