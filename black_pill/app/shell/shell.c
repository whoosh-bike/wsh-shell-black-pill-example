#include "shell.h"
#include "shell_commands.h"
#include "wsh_shell.h"

#if DEBUG_ENABLE
    #define WSH_SHELL_AUTO_EXIT_TMO (60 * 60 * 1000 * 1)
#else /* DEBUG_ENABLE */
    #define WSH_SHELL_AUTO_EXIT_TMO (60 * 1000 * 3)
#endif /* DEBUG_ENABLE */

static WshShell_t Shell;
static TaskHandle_t ShellProcess_Handle;
static TimerHandle_t ShellExit_Timer;

static const WshShellUser_t Shell_UserTable[] = {
    {
        .Login  = "admin",
        .Salt   = "a0523cb065ee08c1",
        .Hash   = "0632cee0",  //1234
        .Groups = WSH_SHELL_CMD_GROUP_ALL,
        .Rights = WSH_SHELL_OPT_ACCESS_ADMIN,
    },
    {
        .Login  = "user",
        .Salt   = "aa89fee902ab12e4",
        .Hash   = "ff69cdab",  //qwer
        .Groups = WSH_SHELL_CMD_GROUP_USER,
        .Rights = WSH_SHELL_OPT_ACCESS_ANY,
    },
};

static __attribute__((section(".SHELL_HISTORY_DATA"))) WshShellHistory_t Shell_HistoryStorage;

static WshShellHistory_t Shell_HistoryRead(void) {
    return Shell_HistoryStorage;
}

static void Shell_HistoryWrite(WshShellHistory_t history) {
    memcpy((void*)&Shell_HistoryStorage, (void*)&history, sizeof(WshShellHistory_t));
}

static void Shell_AuthClbk(void* pCtx) {
    (void)(pCtx);

    xTimerStart(ShellExit_Timer, 0);
}

static void Shell_DeAuthClbk(void* pCtx) {
    (void)(pCtx);

    xTimerStop(ShellExit_Timer, 0);
}

static void Shell_SymInClbk(void* pCtx) {
    (void)(pCtx);

    if (WshShell_IsAuth(&Shell)) {
        xTimerChangePeriod(ShellExit_Timer, WSH_SHELL_AUTO_EXIT_TMO, 0);
    }
}

static WshShell_ExtCallbacks_t Shell_Callbacks = {
    .Auth     = Shell_AuthClbk,
    .DeAuth   = Shell_DeAuthClbk,
    .SymbolIn = Shell_SymInClbk,
};

bool Shell_Init(const char* pcHostName) {
    if (WshShell_Init(&Shell, pcHostName, NULL, &Shell_Callbacks) != WSH_SHELL_RET_STATE_SUCCESS) {
        return false;
    }

    if (WshShellUser_Attach(&(Shell.Users), Shell_UserTable, WSH_SHELL_ARR_LEN(Shell_UserTable),
                            NULL) != WSH_SHELL_RET_STATE_SUCCESS) {
        return false;
    }

    WshShellHistory_Init(&Shell.HistoryIO, Shell_HistoryRead, Shell_HistoryWrite);

    if (!Shell_Commands_Init(&Shell))
        return false;

    // WshShell_Auth(&Shell, "admin", "1234");  //For quick auth

    return true;
}

void Shell_SendChar(char ch) {
    WshShell_InsertChar(&Shell, ch);
}

static void vTask_Shell_Process(void* pvParameters) {
    vTaskDelay(2000);

    if (!Shell_Init("black-pill"))
        Error_Handler();

#if DEBUG_ENABLE
    WshShell_Auth(&Shell, "admin", "1234");
#endif /* DEBUG_ENABLE */

    for (;;) {
        int symbol = getchar();
        // printf("key: %c / %d / 0x%02X\r\n", symbol, (unsigned char)symbol);

        if (symbol == EOF)
            continue;

        Shell_SendChar((char)symbol);
    }
}

static void ShellRoot_ResetTimerClbk(TimerHandle_t xTimer) {
    WshShell_DeAuth(&Shell, (WshShell_Char_t*)pcTimerGetName(ShellExit_Timer));
}

void FreeRTOS_Shell_InitComponents(bool resources, bool tasks) {
    if (resources) {
        ShellExit_Timer = xTimerCreate("tim-shell-autoexit", WSH_SHELL_AUTO_EXIT_TMO, pdFALSE, NULL,
                                       ShellRoot_ResetTimerClbk);
    }

    if (tasks) {
        xTaskCreate(vTask_Shell_Process, "shell-interface", SHELL_TASK_STACK, NULL,
                    SHELL_TASK_PRIORITY, &ShellProcess_Handle);
    }
}
