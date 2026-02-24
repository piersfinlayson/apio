# Changelog

## 2026-02-24

Renamed `APIO_GPIO_INVERT` to `APIO_GPIO_INPUT_INVERT`, and added two new macros for forcing input pins to a specific value: `APIO_GPIO_FORCE_INPUT_LOW` and `APIO_GPIO_FORCE_INPUT_HIGH`.  Each macro clears the other settings.
