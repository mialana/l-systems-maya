#include <format>

#include <maya/MPxCommand.h>
#include <maya/MFnPlugin.h>
#include <maya/MString.h>
#include <maya/MGlobal.h>

#include "LSystemCmd.h"
#include "LSystemNode.h"
#include "macros.h"

constexpr char k_GUI_NAME[] = "LSystemGUI";
constexpr char k_MENU_ITEM_CMD_FN[] = "createLSystemGUI";
constexpr char k_MENU_ITEM_NODE_FN[] = "createLSystemNode";

constexpr char k_INSTALL_MENU_FMT[] = R"mel(
    if (`control -exists {0}`)
        deleteUI {0};

    menu -label "LSystem GUI" -parent "MayaWindow" -tearOff true {0};
    menuItem -label "LSystemCommand" -command "{1}";
    menuItem -label "LSystemNode" -command ("{2}");
    )mel";

constexpr char k_UNINSTALL_MENU_FMT[] = "deleteUI {0};";

MStatus initializePlugin(MObject obj)
{
    MStatus status = MStatus::kSuccess;
    MFnPlugin plugin(obj, "MyPlugin", "1.0", "Any");

    // Register Command
    CHECK_MSTATUS_AND_RETURN_IT_VERBOSE(plugin.registerCommand("LSystemCmd", LSystemCmd::creator),
                                        "Command Registration");
    CHECK_MSTATUS_AND_RETURN_IT_VERBOSE(plugin.registerNode("LSystemNode", LSystemNode::kNodeId,
                                                            LSystemNode::creator,
                                                            LSystemNode::initialize),
                                        "Register Node");

    CHECK_MSTATUS_AND_RETURN_IT_VERBOSE(MGlobal::executeCommand("source \"" + plugin.loadPath()
                                                                + "/LSystem_gui.mel\""),
                                        "Source GUI MEL Script");

    status = MGlobal::executeCommandOnIdle(
        std::format(k_INSTALL_MENU_FMT, k_GUI_NAME, k_MENU_ITEM_CMD_FN, k_MENU_ITEM_NODE_FN).c_str());

    CHECK_MSTATUS_AND_RETURN_IT_VERBOSE(status, "Install Menu");

    return status;
}

MStatus uninitializePlugin(MObject obj)
{
    MStatus status = MStatus::kSuccess;
    MFnPlugin plugin(obj);

    CHECK_MSTATUS_AND_RETURN_IT_VERBOSE(plugin.deregisterCommand("LSystemCmd"),
                                        "Command Deregistration");

    CHECK_MSTATUS_AND_RETURN_IT_VERBOSE(plugin.deregisterNode(LSystemNode::kNodeId),
                                        "Deregister Node");

    status = MGlobal::executeCommand(std::format(k_UNINSTALL_MENU_FMT, k_GUI_NAME).c_str());
    CHECK_MSTATUS_AND_RETURN_IT_VERBOSE(status, "Uninstall Menu");

    return status;
}
