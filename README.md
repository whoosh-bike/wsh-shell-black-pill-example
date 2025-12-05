# Wsh-Shell Black Pill Example

Whoosh Shell example for popular stm32f411-based demo board with RTOS

This guide explains how to build, flash, and debug the **Black Pill** firmware using `make`, `OpenOCD`, and VSCode.
This example imports `wsh-shell` project as git submodule.

---

## 1. Setup Python environment

Python is only needed for helper scripts (e.g. ELF size analysis).

```bash
python3 -m venv .venv
source .venv/bin/activate
pip install --upgrade pip
pip install -r requirements.txt
```

Or just run `Setup python env` task

## 2. Build and programm tools

`make`, `openocd` and `arm-none-eabi-xxx` must be installed

```bash
make --version
GNU Make 4.4.1
Built for aarch64-apple-darwin24.0.0
Copyright (C) 1988-2023 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.

openocd --version
Open On-Chip Debugger 0.12.0
Licensed under GNU GPL v2
For bug reports, read
    http://openocd.org/doc/doxygen/bugs.html

arm-none-eabi-gcc --version
arm-none-eabi-gcc (Arm GNU Toolchain 14.2.Rel1 (Build arm-14.52)) 14.2.1 20241119
Copyright (C) 2024 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
```

## 3. Setup `.env`

Create a .env file in the project root with your toolchain and OpenOCD paths.
Example for macOS:

```bash
OPENOCD_PATH="/opt/homebrew/bin/openocd"
GDB_PATH="/Users/katbert/my-utils/arm-gnu-toolchain-14.2.rel1-darwin-arm64-arm-none-eabi/bin/arm-none-eabi-gdb"
```

## 4. Generate VSCode launch.json

Run `generate launch.json` vscode task
This will create .vscode/launch.json for debugging the Black Pill directly from VSCode.

## 5. Build

```bash
make clean
make

make clean
make BUILD=Release
```

Or just run `build black_pill: release` or `build black_pill: debug`. Both options are equivalent to the CLI commands above.

## 6. Flash firmware

```bash
openocd \
-f ./black_pill/stlink.cfg \
-f ./black_pill/stm32f4x.cfg \
-c "program ./black_pill/build/black_pill.elf verify reset exit"
```

Or just launch `Debug black_pill (OpenOCD-STLink)`
Or just run

```bash
make flash
```
