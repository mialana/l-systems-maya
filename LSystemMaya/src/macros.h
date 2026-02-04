#include <maya/MGlobal.h>

#define CHECK_MSTATUS_AND_RETURN_IT_VERBOSE(_status, _context)                                     \
    do                                                                                             \
    {                                                                                              \
        MStatus s = (_status);                                                                     \
        MString msg;                                                                               \
        if (s == MStatus::kSuccess)                                                                \
        {                                                                                          \
            msg.format("Operation success (^1s)", _context);                                       \
            MGlobal::displayInfo(msg);                                                             \
        }                                                                                          \
        else                                                                                       \
        {                                                                                          \
            msg.format("Operation failed (^1s): ^2s", _context, s.errorString());             \
            MGlobal::displayError(msg);                                                            \
            return s;                                                                              \
        }                                                                                          \
    } while (0)
