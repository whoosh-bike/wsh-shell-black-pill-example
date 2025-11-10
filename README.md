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

## 2. Setup `.env`

Create a .env file in the project root with your toolchain and OpenOCD paths.
Example for macOS:

```bash
MAKE_PATH="/opt/homebrew/opt/make/libexec/gnubin/make"
OPENOCD_PATH="/opt/homebrew/bin/openocd"
ARM_NONE_EABI_PATH="/Users/katbert/my-utils/arm-gnu-toolchain-14.2.rel1-darwin-arm64-arm-none-eabi/bin"
GDB_PATH="/Users/katbert/my-utils/arm-gnu-toolchain-14.2.rel1-darwin-arm64-arm-none-eabi/bin/arm-none-eabi-gdb"
NM_PATH="/Users/katbert/my-utils/arm-gnu-toolchain-14.2.rel1-darwin-arm64-arm-none-eabi/bin/arm-none-eabi-nm"
```

## 3. Generate VSCode launch.json

Run `generate launch.json` vscode task
This will create .vscode/launch.json for debugging the Black Pill directly from VSCode.

## 4. Build

```bash
make clean
make

make clean
make BUILD=Release
```

Or just run `build black_pill: release` or `build black_pill: debug`. Both options are equivalent to the CLI commands above.

## 5. Flash firmware

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

## 6. Analyse firmware footprint

```bash
.venv/bin/python3 ./utils/elf-size-analyse.py ./black_pill/build/black_pill.elf
```

Or just run `analyse elf of black_pill`
