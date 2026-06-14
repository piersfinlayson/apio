# Changelog

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
