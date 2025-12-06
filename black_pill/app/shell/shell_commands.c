#include "shell_commands.h"

extern const WshShellCmd_t Shell_LedCmd;
extern const WshShellCmd_t Shell_ResetCmd;

#if BERRY_LANG
extern const WshShellCmd_t Shell_BerryLangCmd;
#endif /* BERRY_LANG */

static const WshShellCmd_t* Shell_CmdTable[] = {
    &Shell_LedCmd,
    &Shell_ResetCmd,
#if BERRY_LANG
    &Shell_BerryLangCmd,
#endif /* BERRY_LANG */
};

bool Shell_Commands_Init(WshShell_t* pShell) {
    return WshShellRetState_TranslateToProject(
        WshShellCmd_Attach(&(pShell->Commands), Shell_CmdTable, WSH_SHELL_ARR_LEN(Shell_CmdTable)));
}
