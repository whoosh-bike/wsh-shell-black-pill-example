#include "main.h"
#include "wsh_shell.h"

static void WatchdogInit(void) {
    static IWDG_HandleTypeDef hiwdg = {
        .Instance       = IWDG,
        .Init.Prescaler = IWDG_PRESCALER_64,
        .Init.Reload    = 3000,
    };

    if (HAL_IWDG_Init(&hiwdg) != HAL_OK) {
        Error_Handler();
    }
}

/* clang-format off */
#define CMD_RESET_OPT_TABLE() \
X_CMD_ENTRY(CMD_RESET_OPT_DEF, WSH_SHELL_OPT_NO(WSH_SHELL_OPT_ACCESS_EXECUTE, "Soft reset")) \
X_CMD_ENTRY(CMD_RESET_OPT_HELP, WSH_SHELL_OPT_HELP()) \
X_CMD_ENTRY(CMD_RESET_OPT_HF, WSH_SHELL_OPT_WO_PARAM(WSH_SHELL_OPT_ACCESS_EXECUTE, "-f", "--hard-fault", "Reset with hard fault")) \
X_CMD_ENTRY(CMD_RESET_OPT_END, WSH_SHELL_OPT_END())
/* clang-format on */

#define X_CMD_ENTRY(en, m) en,
typedef enum { CMD_RESET_OPT_TABLE() CMD_RESET_OPT_ENUM_SIZE } CMD_RESET_OPT_t;
#undef X_CMD_ENTRY

#define X_CMD_ENTRY(enum, opt) {enum, opt},
WshShellOption_t ResetOptArr[] = {CMD_RESET_OPT_TABLE()};
#undef X_CMD_ENTRY

static WSH_SHELL_RET_STATE_t shell_cmd_reset(const WshShellCmd_t* pcCmd, WshShell_Size_t argc,
                                             const WshShell_Char_t* pArgv[], void* pShellCtx) {
    if ((argc > 0 && !pArgv) || !pcCmd)
        return WSH_SHELL_RET_STATE_ERROR;

    WshShell_t* pParentShell = (WshShell_t*)pShellCtx;

    for (WshShell_Size_t tokenPos = 0; tokenPos < argc;) {
        WshShellOption_Ctx_t optCtx =
            WshShellCmd_ParseOpt(pcCmd, argc, pArgv, pParentShell->CurrUser->Rights, &tokenPos);
        if (!optCtx.Option)
            return WSH_SHELL_RET_STATE_ERR_EMPTY;

        switch (optCtx.Option->ID) {
            case CMD_RESET_OPT_DEF:
                NVIC_SystemReset();
                break;

            case CMD_RESET_OPT_HELP:
                WshShellCmd_PrintOptionsOverview(pcCmd);
                break;

            case CMD_RESET_OPT_HF:
                WatchdogInit();
                WSH_SHELL_PRINT_INFO("Create HF exception and restart\r\n");
                ((void (*)(void))0x01)();
                break;

            default:
                return WSH_SHELL_RET_STATE_ERROR;
        }
    }

    return WSH_SHELL_RET_STATE_SUCCESS;
}

const WshShellCmd_t Shell_ResetCmd = {
    .Groups  = WSH_SHELL_CMD_GROUP_HARDWARE,
    .Name    = "rst",
    .Descr   = "Reset MCU",
    .Options = ResetOptArr,
    .OptNum  = CMD_RESET_OPT_ENUM_SIZE,
    .Handler = shell_cmd_reset,
};
