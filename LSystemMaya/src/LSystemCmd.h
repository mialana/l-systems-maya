#pragma once

#include <maya/MPxCommand.h>
#include <string>

#include <LSystem.h>

#define CHECK_MSTATUS_AND_RETURN_IT_VERBOSE(_status)                                               \
    {                                                                                              \
        MStatus _s = (_status);                                                                    \
        if (_s == MStatus::kSuccess)                                                               \
        {                                                                                          \
            MGlobal::displayInfo("Operation successful!");                                         \
        }                                                                                          \
        else                                                                                       \
        {                                                                                          \
            MString err = "Operation failed: " + _s.errorString();                                 \
            MGlobal::displayError(err);                                                            \
            return _s;                                                                             \
        }                                                                                          \
    }

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
