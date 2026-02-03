#include "LSystemCmd.h"

#include <maya/MPoint.h>
#include <maya/MGlobal.h>
#include <maya/MPointArray.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MDGModifier.h>
#include <maya/MDagModifier.h>
#include <maya/MPlug.h>
#include <maya/MSyntax.h>
#include <list>

LSystemCmd::LSystemCmd() : MPxCommand() {}

LSystemCmd::~LSystemCmd() {}

static MSyntax getSyntax()
{
    MSyntax syntax;
    syntax.addFlag("-ss", "-step_size", MSyntax::kDouble);
    syntax.addFlag("-ag", "-angle", MSyntax::kDouble);
    syntax.addFlag("-gr", "-grammar", MSyntax::kString);
    syntax.addFlag("-it", "-iterations", MSyntax::kLong);
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

MStatus LSystemCmd::doIt(const MArgList& args)
{
    MStatus makeCurveFnStatus = this->makeCurveFn();

    CHECK_MSTATUS_AND_RETURN_IT(makeCurveFnStatus);

    MStatus extrudeCurveFnStatus = this->extrudeCurveFn();
    CHECK_MSTATUS(extrudeCurveFnStatus);

    return extrudeCurveFnStatus;
}
