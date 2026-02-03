#include "LSystemCmd.h"

#include <maya/MPoint.h>
#include <maya/MGlobal.h>
#include <maya/MPointArray.h>
#include <maya/MFnNurbsCurve.h>
#include <list>

LSystemCmd::LSystemCmd() : MPxCommand() {}

LSystemCmd::~LSystemCmd() {}

MStatus LSystemCmd::doIt(const MArgList& args)
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

    MFnNurbsCurve curve;
    curve.create(points, knots, 1, MFnNurbsCurve::kOpen, false, false, MObject::kNullObj, &status);

    return MStatus::kSuccess;
}
