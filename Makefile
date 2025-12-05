# ===== Project Configuration =====
TARGET := black_pill
BUILD ?= Debug

# ===== Toolchain =====
TOOLCHAIN_PREFIX := arm-none-eabi-
CC := $(TOOLCHAIN_PREFIX)gcc
OBJCOPY := $(TOOLCHAIN_PREFIX)objcopy
SIZE := $(TOOLCHAIN_PREFIX)size
MKDIR := mkdir -p
RM := rm -rf

EXAMPLE_DIR := ./black_pill

# ===== Paths =====
INCLUDE_FLAGS_COMMON := -I.
INCLUDE_FLAGS_COMMON += -I$(EXAMPLE_DIR)/app
INCLUDE_FLAGS_COMMON += -I$(EXAMPLE_DIR)/app/shell
INCLUDE_FLAGS_COMMON += -I$(EXAMPLE_DIR)/app/usb
INCLUDE_FLAGS_COMMON += -I$(EXAMPLE_DIR)/platform
INCLUDE_FLAGS_COMMON += -I$(EXAMPLE_DIR)/platform/STM32CubeF4
INCLUDE_FLAGS_COMMON += -I$(EXAMPLE_DIR)/platform/STM32CubeF4/Drivers
INCLUDE_FLAGS_COMMON += -I$(EXAMPLE_DIR)/platform/STM32CubeF4/Drivers/CMSIS/Include
INCLUDE_FLAGS_COMMON += -I$(EXAMPLE_DIR)/platform/STM32CubeF4/Drivers/CMSIS/Core/Include
INCLUDE_FLAGS_COMMON += -I$(EXAMPLE_DIR)/platform/STM32CubeF4/Drivers/CMSIS/Device/ST/STM32F4xx/Include
INCLUDE_FLAGS_COMMON += -I$(EXAMPLE_DIR)/platform/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver
INCLUDE_FLAGS_COMMON += -I$(EXAMPLE_DIR)/platform/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Inc
INCLUDE_FLAGS_COMMON += -I$(EXAMPLE_DIR)/platform/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy
INCLUDE_FLAGS_COMMON += -I$(EXAMPLE_DIR)/platform/STM32CubeF4/Middlewares/ST/STM32_USB_Device_Library/Core/Inc
INCLUDE_FLAGS_COMMON += -I$(EXAMPLE_DIR)/platform/STM32CubeF4/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc
INCLUDE_FLAGS_COMMON += -I$(EXAMPLE_DIR)/rtos
INCLUDE_FLAGS_COMMON += -I$(EXAMPLE_DIR)/rtos/FreeRTOS-KernelV11.2.0/include
INCLUDE_FLAGS_COMMON += -I$(EXAMPLE_DIR)/rtos/FreeRTOS-KernelV11.2.0/portable/GCC/ARM_CM4F

BUILD_DIR := $(EXAMPLE_DIR)/build
OBJ_DIR := $(BUILD_DIR)/obj

# ===== Source Files =====
SRCS := 
SRCS += $(wildcard $(EXAMPLE_DIR)/app/*.c)
SRCS += $(wildcard $(EXAMPLE_DIR)/app/shell/*.c)
SRCS += $(wildcard $(EXAMPLE_DIR)/app/usb/*.c)
SRCS += $(wildcard $(EXAMPLE_DIR)/platform/*.c)
SRCS += $(wildcard $(EXAMPLE_DIR)/rtos/FreeRTOS-KernelV11.2.0/*.c)
SRCS += $(wildcard $(EXAMPLE_DIR)/rtos/FreeRTOS-KernelV11.2.0/portable/Common/*.c)
SRCS += $(wildcard $(EXAMPLE_DIR)/rtos/FreeRTOS-KernelV11.2.0/portable/GCC/ARM_CM4F/*.c)
SRCS += $(wildcard $(EXAMPLE_DIR)/rtos/FreeRTOS-KernelV11.2.0/portable/MemMang/*.c)
SRCS += $(EXAMPLE_DIR)/platform/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_usb.c
SRCS += $(EXAMPLE_DIR)/platform/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_tim.c
SRCS += $(EXAMPLE_DIR)/platform/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c
SRCS += $(EXAMPLE_DIR)/platform/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c
SRCS += $(EXAMPLE_DIR)/platform/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_iwdg.c
SRCS += $(EXAMPLE_DIR)/platform/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c
SRCS += $(EXAMPLE_DIR)/platform/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c
SRCS += $(EXAMPLE_DIR)/platform/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c
SRCS += $(EXAMPLE_DIR)/platform/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c
# SRCS += $(EXAMPLE_DIR)/platform/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c
SRCS += $(EXAMPLE_DIR)/platform/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pcd.c
SRCS += $(EXAMPLE_DIR)/platform/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pcd_ex.c
SRCS += $(EXAMPLE_DIR)/platform/STM32CubeF4/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_core.c
SRCS += $(EXAMPLE_DIR)/platform/STM32CubeF4/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.c
SRCS += $(EXAMPLE_DIR)/platform/STM32CubeF4/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ioreq.c
SRCS += $(EXAMPLE_DIR)/platform/STM32CubeF4/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Src/usbd_cdc.c
SRCS += $(EXAMPLE_DIR)/platform/startup_stm32f411xe.s

OBJS := $(SRCS:%=$(OBJ_DIR)/%.o)

# WshShell
WSH_SHELL_DIR := ./thirdparty/wsh-shell/src
WSH_SHELL_SRCS := $(wildcard $(WSH_SHELL_DIR)/*.c)
WSH_SHELL_OBJS := $(WSH_SHELL_SRCS:$(WSH_SHELL_DIR)/%.c=$(OBJ_DIR)/src/%.o)
WSH_SHELL_INCLUDE_FLAGS = -I$(WSH_SHELL_DIR)

ALL_OBJS := $(OBJS) $(WSH_SHELL_OBJS) $(BERRY_LANG_OBJS)
DEPS := $(ALL_OBJS:.o=.d)

# ===== Compiler Flags =====
INCLUDE_FLAGS := $(INCLUDE_FLAGS_COMMON) $(WSH_SHELL_INCLUDE_FLAGS) -MMD -Wall -Wextra -Wpedantic -Wno-unused-parameter -Wno-format
CPU_FLAGS := -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mthumb -DSTM32F411xE -DUSE_FULL_LL_DRIVER -DHSE_VALUE=25000000

LINKER_FLAGS += -Wl,-cref
LINKER_FLAGS += -Wl,-u,Reset_Handler
LINKER_FLAGS += -Wl,-Map=$(BUILD_DIR)/$(TARGET).map
LINKER_FLAGS += -specs=nano.specs
LINKER_FLAGS += -specs=nosys.specs
LINKER_FLAGS += -u _printf_float
LINKER_FLAGS += -Wl,--start-group -lc -lm -Wl,--end-group
LINKER_FLAGS += -Wl,--gc-sections
LINKER_FLAGS += -Wl,--print-memory-usage
LINKER_FLAGS += -Wl,--no-warn-rwx-segments

LINKER_SCRIPT := $(EXAMPLE_DIR)/platform/stm32f411ce.ld

DEBUG_FLAGS := -O0 -g -DWSH_SHELL_ASSERT_ENABLE -DUSE_FULL_ASSERT -DDEBUG_ENABLE
RELEASE_FLAGS := -O2 -DNDEBUG

ifeq ($(BUILD),Debug)
    CFLAGS := $(INCLUDE_FLAGS) $(CPU_FLAGS) $(DEBUG_FLAGS)
else
    CFLAGS := $(INCLUDE_FLAGS) $(CPU_FLAGS) $(RELEASE_FLAGS)
endif

# ===== Targets =====
.PHONY: all clean flash

all: $(BUILD_DIR)/$(TARGET)

$(BUILD_DIR)/$(TARGET): $(ALL_OBJS)
	@echo "[LD] $@"
	@$(MKDIR) $(dir $@)
	@$(CC) -T$(LINKER_SCRIPT) $^ -o $@.elf $(LINKER_FLAGS) $(CPU_FLAGS)
	@echo "[OBJCOPY] $@.elf -> $@.hex"
	@$(OBJCOPY) -O ihex $@.elf $@.hex
	@echo "[OBJCOPY] $@.elf -> $@.bin"
	@$(OBJCOPY) -O binary $@.elf $@.bin
	@$(SIZE) $@.elf

$(OBJ_DIR)/%.c.o: %.c
	@echo "[CC] $<"
	@$(MKDIR) $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.s.o: %.s
	@echo "[AS] $<"
	@$(MKDIR) $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/src/%.o: $(WSH_SHELL_DIR)/%.c
	@echo "[CC] $<"
	@$(MKDIR) $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "[CLEAN] Removing build directory"
	@$(RM) $(BUILD_DIR)

flash:
	@echo "[FLASH] Programming device with OpenOCD"
	@openocd -f $(EXAMPLE_DIR)/stlink.cfg -f $(EXAMPLE_DIR)/stm32f4x.cfg -c "program $(BUILD_DIR)/$(TARGET).elf verify reset exit"

# ===== Include Dependencies =====
-include $(DEPS)
