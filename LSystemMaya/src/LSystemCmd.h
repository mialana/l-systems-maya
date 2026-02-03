#pragma once

#include <maya/MPxCommand.h>
#include <maya/MFnNurbsCurve.h>
#include <string>

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
    MStatus doIt(const MArgList& args);

private:
    MFnNurbsCurve mCurveFn;
};
