#pragma once

#include <maya/MPxCommand.h>
#include <maya/MFnNurbsCurve.h>
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

    MStatus extrudeCurveFn();
    MStatus makeCurveFn();
    MStatus createGeometry();
    MStatus doIt(const MArgList& args);

    // stored arguments as normal C data structures
    double mStepSize = 22.5;
    double mAngle = 1.0;
    string mGrammar;
    int64_t mIterations;

private:
    MFnNurbsCurve mCurveFn;

    LSystem mSystem;
};
