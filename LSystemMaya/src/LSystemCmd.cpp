#include "LSystemCmd.h"

#include <maya/MGlobal.h>
#include <maya/MArgDatabase.h>
#include <maya/MSyntax.h>
#include <maya/MPxCommand.h>
#include <format>
#include <string>

constexpr const char k_STEP_SIZE_SHORT[] = "-ss";
constexpr const char k_STEP_SIZE_LONG[] = "-stepSize";

constexpr const char k_ANGLE_SHORT[] = "-ag";
constexpr const char k_ANGLE_LONG[] = "-angle";

constexpr const char k_GRAMMAR_SHORT[] = "-gr";
constexpr const char k_GRAMMAR_LONG[] = "-grammar";

constexpr const char k_ITERATIONS_SHORT[] = "-it";
constexpr const char k_ITERATIONS_LONG[] = "-iterations";

constexpr const char k_CMD_FORMAT[]
    = R"(curve -d 1 -p {0} {1} {2} -p {3} {4} {5} -k 0 -k 1 -name "curve{6}";
circle -radius 0.1 -nr {7} {8} {9} -c {0} {1} {2} -name "nurbsCircle{6}";
select -r nurbsCircle{6} curve{6};
extrude -ch true -rn false -po 1 -et 2 -rotation 0 -scale 1 -rsp 1 "nurbsCircle{6}" "curve{6}")";

LSystemCmd::LSystemCmd() : MPxCommand() {}

LSystemCmd::~LSystemCmd() {}

static MSyntax getSyntax()
{
    MSyntax syntax;
    syntax.addFlag(k_STEP_SIZE_SHORT, k_STEP_SIZE_LONG, MSyntax::kDouble);
    syntax.addFlag(k_ANGLE_SHORT, k_ANGLE_LONG, MSyntax::kDouble);
    syntax.addFlag(k_GRAMMAR_SHORT, k_GRAMMAR_LONG, MSyntax::kString);
    syntax.addFlag(k_ITERATIONS_SHORT, k_ITERATIONS_LONG, MSyntax::kLong);
    return syntax;
}

MStatus LSystemCmd::createGeometry()
{
    // configure l-system base code
    mSystem.loadProgramFromString(mGrammar);
    mSystem.setDefaultAngle(mAngle);
    mSystem.setDefaultStep(mStepSize);
    std::vector<LSystem::Branch> branches;
    mSystem.process(this->mIterations, branches);

    static vec3 radius;
    static uint32_t label;
    static MString cmd;
    static std::string cmdString;

    MStatus status;
    for (uint32_t iter = 0; iter < branches.size(); iter++)
    {
        label = iter + 1;
        const LSystem::Branch& branch = branches[iter];
        const vec3& start = branch.first;
        const vec3& end = branch.second;

        cmdString = std::format(k_CMD_FORMAT, start[0], start[1], start[2], end[0], end[1], end[2],
                                label, radius[0], radius[1], radius[2]);

        cmd = cmdString.c_str();
        status = MGlobal::executeCommand(cmd);
        CHECK_MSTATUS_AND_RETURN_IT_VERBOSE(status);
    }

    return status;
}

MStatus LSystemCmd::doIt(const MArgList& args)
{
    MStatus status;

    MArgDatabase argData(getSyntax(), args, &status);
    CHECK_MSTATUS_AND_RETURN_IT_VERBOSE(status);

    static MString tmpGrammar;

    if (!argData.isFlagSet(k_GRAMMAR_LONG))
    {
        MGlobal::displayError("Input grammar is required.");
        return MS::kFailure;
    }

    argData.getFlagArgument(k_GRAMMAR_LONG, 0, tmpGrammar);
    this->mGrammar = tmpGrammar.asChar();  // convert to c string

    if (argData.isFlagSet(k_STEP_SIZE_LONG))
    {
        argData.getFlagArgument(k_STEP_SIZE_LONG, 0, this->mStepSize);
    }
    if (argData.isFlagSet(k_ANGLE_LONG))
    {
        argData.getFlagArgument(k_ANGLE_LONG, 0, this->mAngle);
    }
    if (argData.isFlagSet(k_ITERATIONS_LONG))
    {
        argData.getFlagArgument(k_ITERATIONS_LONG, 0, mIterations);
    }

    return this->createGeometry();
}
