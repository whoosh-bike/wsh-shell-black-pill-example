#include "shell_commands.h"

extern const WshShellCmd_t Shell_LedCmd;
extern const WshShellCmd_t Shell_ResetCmd;

static const WshShellCmd_t* Shell_CmdTable[] = {
    &Shell_LedCmd,
    &Shell_ResetCmd,
};

bool Shell_Commands_Init(WshShell_t* pShell) {
    return WshShellRetState_TranslateToProject(
        WshShellCmd_Attach(&(pShell->Commands), Shell_CmdTable, WSH_SHELL_ARR_LEN(Shell_CmdTable)));
}
