#ifndef __SHELL_H
#define __SHELL_H

#include "main.h"

bool Shell_Init(const char* pcHostName);
void Shell_SendChar(char ch);
void FreeRTOS_Shell_InitComponents(bool resources, bool tasks);

#endif /* __SHELL_H */
