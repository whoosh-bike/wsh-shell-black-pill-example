#include "berry_lang.h"
#include "rtos_tasks_stack_and_prio.h"

#if BERRY_LANG

    #include "be_repl.h"
    #include "berry.h"
    #include "wsh_shell.h"

    #if DEBUG_ENABLE
        #define LOCAL_DEBUG_PRINT_ENABLE 0
    #endif /* DEBUG_ENABLE */

    #if LOCAL_DEBUG_PRINT_ENABLE
        #warning LOCAL_DEBUG_PRINT_ENABLE
        #define LOCAL_DEBUG_PRINT DEBUG_PRINT
    #else /* DEBUG_ENABLE */
        #define LOCAL_DEBUG_PRINT(_f_, ...)
    #endif /* DEBUG_ENABLE */

    #define BERRY_MSG_BUFF_SIZE (WSH_SHELL_INTR_BUFF_LEN)
static MessageBufferHandle_t Berry_MessageBuff;
static bvm* BerryVmInteractive_Handle;
static TaskHandle_t BerryProcessIteractive_Handle;

static char* BerryLangInteractive_GetLine(const char* pPrompt) {
    (void)(pPrompt);

    static char buf[BERRY_MSG_BUFF_SIZE];
    uint32_t len = xMessageBufferReceive(Berry_MessageBuff, buf, sizeof(buf), portMAX_DELAY);
    if (len > 0)
        return buf;

    return NULL;
}

static void BerryLangInteractive_FreeLine(char* pPtr) {
    (void)pPtr;
}

static bool BerryLangInteractive_GetState(void) {
    return (bool)(BerryVmInteractive_Handle && Berry_MessageBuff && BerryProcessIteractive_Handle);
}

static void BerryLangInteractive_DeleteAll(void) {
    if (Berry_MessageBuff) {
        vMessageBufferDelete(Berry_MessageBuff);
        Berry_MessageBuff = NULL;
    }

    if (BerryVmInteractive_Handle) {
        be_vm_delete(BerryVmInteractive_Handle);
        BerryVmInteractive_Handle = NULL;
    }

    if (BerryProcessIteractive_Handle)
        vTaskDelete(BerryProcessIteractive_Handle);
}

static void vTask_BerryLangInteractiveProcess(void* pvParameters) {
    int res = be_repl(BerryVmInteractive_Handle, BerryLangInteractive_GetLine,
                      BerryLangInteractive_FreeLine);
    (void)(res);

    BerryLangInteractive_DeleteAll();

    for (;;) {
        vTaskDelay(RTOS_MIN_TIMEOUT_MS);
    }
}

/* clang-format off */
#define BERRY_OPT_TABLE() \
    X_CMD_ENTRY(BERRY_OPT_HELP, WSH_SHELL_OPT_HELP()) \
	X_CMD_ENTRY(BERRY_OPT_VM_CREATE, WSH_SHELL_OPT_WO_PARAM(WSH_SHELL_OPT_ACCESS_EXECUTE, "-c", "--create", "Create VM")) \
	X_CMD_ENTRY(BERRY_OPT_VM_STATE, WSH_SHELL_OPT_WO_PARAM(WSH_SHELL_OPT_ACCESS_READ, "-s", "--state", "State of VM")) \
    X_CMD_ENTRY(BERRY_OPT_INTERACT, WSH_SHELL_OPT_INTERACT(WSH_SHELL_OPT_ACCESS_ANY)) \
	X_CMD_ENTRY(BERRY_OPT_VM_DELETE, WSH_SHELL_OPT_WO_PARAM(WSH_SHELL_OPT_ACCESS_EXECUTE, "-d", "--delete", "Delete VM")) \
    X_CMD_ENTRY(BERRY_OPT_END, WSH_SHELL_OPT_END())
/* clang-format on */

typedef enum {
    #define X_CMD_ENTRY(en, m) en,
    BERRY_OPT_TABLE() BERRY_OPT_ENUM_SIZE
    #undef X_CMD_ENTRY
} BERRY_OPT_t;

    #define X_CMD_ENTRY(enum, opt) {enum, opt},
static const WshShellOption_t WshShell_OptArr[] = {BERRY_OPT_TABLE()};
    #undef X_CMD_ENTRY

static void ShellCmdBerryInteractive(WshShellIO_CommandLine_t* pInter) {
    if (Berry_MessageBuff) {
        xMessageBufferSend(Berry_MessageBuff, pInter->Buff, strlen(pInter->Buff) + 1,
                           portMAX_DELAY);
        vTaskDelay(RTOS_MIN_TIMEOUT_MS);  //For berry lang task execution
        return;
    }

    WSH_SHELL_PRINT_ERR("Berry VM isn't inited!\r\n");
}

static WSH_SHELL_RET_STATE_t ShellCmdBerry(const WshShellCmd_t* pcCmd, WshShell_Size_t argc,
                                           const WshShell_Char_t* pArgv[], void* pShellCtx) {
    if ((argc > 0 && !pArgv) || !pcCmd)
        return WSH_SHELL_RET_STATE_ERROR;

    WshShell_t* pParentShell       = (WshShell_t*)pShellCtx;
    WSH_SHELL_RET_STATE_t retState = WSH_SHELL_RET_STATE_SUCCESS;

    for (WshShell_Size_t tokenPos = 0; tokenPos < argc;) {
        WshShellOption_Ctx_t optCtx =
            WshShellCmd_ParseOpt(pcCmd, argc, pArgv, pParentShell->CurrUser->Rights, &tokenPos);
        if (!optCtx.Option)
            return WSH_SHELL_RET_STATE_ERR_EMPTY;

        switch (optCtx.Option->ID) {
            case BERRY_OPT_HELP:
                WshShellCmd_PrintOptionsOverview(pcCmd);
                break;

            case BERRY_OPT_VM_CREATE: {
                if (BerryLangInteractive_GetState()) {
                    WSH_SHELL_PRINT_WARN("Berry VM created before\r\n");
                    break;
                }

                BerryVmInteractive_Handle = be_vm_new();
                Berry_MessageBuff         = xMessageBufferCreate(BERRY_MSG_BUFF_SIZE);
                xTaskCreate(vTask_BerryLangInteractiveProcess, "berry-lang-vm",
                            BERRY_LANG_TASK_STACK, NULL, BERRY_LANG_TASK_PRIORITY,
                            &BerryProcessIteractive_Handle);

                bool vmState = BerryLangInteractive_GetState();
                WSH_SHELL_PRINT_LEVEL(vmState ? WSH_SHELL_PRINT_SYS : WSH_SHELL_PRINT_ERR,
                                      "Berry VM create status: %s\r\n",
                                      vmState ? "success" : "fail");

                if (!vmState)
                    return WSH_SHELL_RET_STATE_ERROR;
            } break;

            case BERRY_OPT_VM_STATE: {
                bool vmState = BerryLangInteractive_GetState();
                WSH_SHELL_PRINT_LEVEL(vmState ? WSH_SHELL_PRINT_SYS : WSH_SHELL_PRINT_ERR,
                                      "Berry VM state %sready\r\n", vmState ? "" : "not ");
            } break;

            case BERRY_OPT_INTERACT:
                bool vmState = BerryLangInteractive_GetState();
                WSH_SHELL_PRINT_LEVEL(vmState ? WSH_SHELL_PRINT_SYS : WSH_SHELL_PRINT_ERR,
                                      "Berry VM state %sready\r\n", vmState ? "" : "not ");
                if (!vmState)
                    return WSH_SHELL_RET_STATE_ERROR;

                WshShellInteract_Attach(&(pParentShell->Interact), pcCmd->Name,
                                        ShellCmdBerryInteractive);
                break;

            case BERRY_OPT_VM_DELETE:
                BerryLangInteractive_DeleteAll();
                WSH_SHELL_PRINT_SYS("Berry VM deleted\r\n");
                break;

            default:
                retState = WSH_SHELL_RET_STATE_ERROR;
                break;
        }
    }

    return retState;
}

const WshShellCmd_t Shell_BerryLangCmd = {
    .Groups  = WSH_SHELL_CMD_GROUP_USER,
    .Name    = "berry",
    .Descr   = "The Berry Script Language",
    .Options = WshShell_OptArr,
    .OptNum  = WSH_SHELL_ARR_LEN(WshShell_OptArr),
    .Handler = ShellCmdBerry,
};

#endif /* BERRY_LANG */
