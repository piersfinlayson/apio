# Changelog

## 2026-06-16

Added support for emulating internal GPIO pulls.

- Renamed `APIO_GPIO_OUTPUT` to `APIO_GPIO_INPUT_OUTPUT` to accurately reflect
  that configuring a pin for PIO output control does not disable its input path.
- Added `APIO_GPIO_INPUT_ONLY(PIN)`: configures a pin as a pure SIO input with
  output driver disabled.  On real hardware, resets GPIO_CTRL to SIO and sets
  PAD_INPUT | PAD_OUTPUT_DIS.  No-op in emulation (tracked via `input_only`
  bitmask; EPIO will assert on any attempt to drive the pin as an output).
- Added `APIO_GPIO_PULL_UP(PIN)`, `APIO_GPIO_PULL_DOWN(PIN)`,
  `APIO_GPIO_PULL_NONE(PIN)`: configure pad pull resistors.  On real hardware,
  manipulate PUE/PDE bits in the pad register.  In emulation, set/clear
  `pull_up` / `pull_down` bitmasks in `_apio_emulated_gpios`.
- Added `APIO_GPIO_DRIVE(PIN, STRENGTH)` with constants `APIO_DRIVE_2MA`,
  `APIO_DRIVE_4MA`, `APIO_DRIVE_8MA`, `APIO_DRIVE_12MA`: configure pad drive
  strength.  On real hardware, sets DRIVE field [5:4] in the pad register.
  In emulation, stored per-pin (no behavioural effect currently).
- Added `APIO_GPIO_SLEW_FAST(PIN)` and `APIO_GPIO_SLEW_SLOW(PIN)`: configure
  pad slew rate.  On real hardware, sets/clears SLEWFAST in the pad register.
  In emulation, stored per-pin (no behavioural effect currently).
- Added fields to `_apio_emulated_gpio_t`: `pull_up` (bitmask), `pull_down`
  (bitmask), `input_only` (bitmask), `drive_strength[APIO_MAX_GPIOS]` (per-pin
  array), `slew_fast` (bitmask).
- `APIO_GPIO_INIT()` now initialises all new fields to their RP2350 hardware
  reset defaults: pull-down enabled on all pins, 4 mA drive strength, slow
  slew.

## 2026-06-14

Added register logging to APIO_SM_LOG

## 2026-06-10

Added APIs:
- APIO_INSTR_COUNT() — returns the number of instructions stored for the current block
- APIO_SET_BLOCK_VAR(BLOCK) — runtime variable equivalent of APIO_SET_BLOCK
- APIO_SET_BLOCK_FROM(BLOCK, OFFSET) — resume a previously committed block from a known offset
- APIO_SET_BLOCK_FROM_VAR(BLOCK, OFFSET) — runtime variable equivalent of APIO_SET_BLOCK_FROM
- APIO_SET_SM_VAR(SM) — runtime variable equivalent of APIO_SET_SM
- APIO_END_BLOCK_FROM(OFFSET) — commits only instructions added since APIO_SET_BLOCK_FROM

## 2026-02-24

Renamed `APIO_GPIO_INVERT` to `APIO_GPIO_INPUT_INVERT`, and added two new macros for forcing input pins to a specific value: `APIO_GPIO_FORCE_INPUT_LOW` and `APIO_GPIO_FORCE_INPUT_HIGH`.  Each macro clears the other settings.
