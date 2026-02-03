#pragma once

#include <maya/MPxCommand.h>
#include <string>

#include <LSystem.h>

class LSystemCmd : public MPxCommand
{
public:
    LSystemCmd();
    virtual ~LSystemCmd();

    static void* creator()
    {
        return new LSystemCmd();
    }
    MStatus createGeometry();
    MStatus doIt(const MArgList& args);

    // stored arguments as normal C data structures
    double mStepSize = 22.5;
    double mAngle = 1.0;
    string mGrammar;
    int32_t mIterations = 3;

private:
    LSystem mSystem;
};
