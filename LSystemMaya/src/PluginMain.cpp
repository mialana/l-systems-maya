#include <maya/MPxCommand.h>
#include <maya/MFnPlugin.h>
#include <maya/MIOStream.h>
#include <maya/MString.h>
#include <maya/MArgList.h>
#include <maya/MGlobal.h>
#include <maya/MSimple.h>
#include <maya/MDoubleArray.h>
#include <maya/MPoint.h>
#include <maya/MPointArray.h>
#include <maya/MFnNurbsCurve.h>
#include <maya/MDGModifier.h>
#include <maya/MPlugArray.h>
#include <maya/MVector.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MStringArray.h>
#include <format>
#include <list>

#include "LSystemCmd.h"

constexpr char k_GUI_NAME[] = "LSystemGUI";
constexpr char k_MENU_ITEM_CMD_FN[] = "createGUI";

constexpr char k_INSTALL_MENU_FMT[] = R"mel(
    if (`menu -exists {0}`)
        deleteUI {0};

    menu -label "LSystem GUI" -parent "MayaWindow" -tearOff true {0};
    menuItem -label "LSystemCommand" -command "{1}";
    )mel";

constexpr char k_UNINSTALL_MENU_FMT[] = "deleteUI {0};";

MStatus initializePlugin(MObject obj)
{
    MStatus status = MStatus::kSuccess;
    MFnPlugin plugin(obj, "MyPlugin", "1.0", "Any");

    // Register Command
    CHECK_MSTATUS_AND_RETURN_IT_VERBOSE(plugin.registerCommand("LSystemCmd", LSystemCmd::creator));

    CHECK_MSTATUS_AND_RETURN_IT_VERBOSE(
        MGlobal::executeCommand("source \"" + plugin.loadPath() + "/LSystem_gui.mel\""));

    status = MGlobal::executeCommandOnIdle(
        std::format(k_INSTALL_MENU_FMT, k_GUI_NAME, k_MENU_ITEM_CMD_FN).c_str());

    return status;
}

MStatus uninitializePlugin(MObject obj)
{
    MStatus status = MStatus::kSuccess;
    MFnPlugin plugin(obj);

    status = plugin.deregisterCommand("LSystemCmd");
    CHECK_MSTATUS_AND_RETURN_IT_VERBOSE(status);

    status = MGlobal::executeCommand(std::format(k_UNINSTALL_MENU_FMT, k_GUI_NAME).c_str());

    return status;
}
