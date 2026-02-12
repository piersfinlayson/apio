// MIT License
//
// Copyright (c) 2026 Piers Finlayson <piers@piers.rocks>
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

// apio - Runtime RP2350 PIO Assembler and Disassembler
//
// Macros to construct PIO programs for RP2350 PIO state machines.

#ifndef APIO_H
#define APIO_H

//
// Macros to build PIO programs
//

// Instructions:
//
// You MUST build all SMs for a single PIO block and write them using
// `APIO_END_BLOCK()` before moving onto the next PIO block, as a single
// stack based scratch buffer is used by these macros.
//
// 1.  At the beginning of your PIO building function, call `APIO_ASM_INIT()`
//     to declare and initialise the necessary variables.
//
// 2.  (Optional) Clear all PIO IRQs using `APIO_CLEAR_ALL_IRQS()`.
//
// 3.  Start the first block with `APIO_SET_BLOCK(BLOCK_NUM)` where `BLOCK_NUM`
//     is 0, 1 or 2.
//
// 4.  Starting with SM 0, start the first program with `APIO_SET_SM(SM_NUM)`
//     where `SM_NUM` is 0, 1, 2 or 3.
//
// 5.  (Optional) Create any labels required before the next instruction using
//     `APIO_LABEL_NEW(LABEL_NAME)`, where `LABEL_NAME` must be unique within
//     your function.  These labels are used as destinations for JMP
//     instructions.
//
// 6.  (Optional) Use `APIO_START()`, `APIO_WRAP_BOTTOM()`, `APIO_WRAP_TOP()` and
//     `APIO_END()` before the instruction, to mark the start and wrap points of
//     your program.
// 
//     `APIO_WRAP_TOP()` must be called _before_ adding the instruction that is
//     to be the wrap top.  You do not need to call these macros if the
//     .start, .wrap_bottom or .wrap_top are to be at instruction 0.
//
//     `APIO_END()` is only required if the program ends beyond .wrap, and, if
//     used, must be called _after_ `APIO_WRAP_TOP()`, but before the final
//     instruction of the program.
//
// 7.  Add PIO instructions using `APIO_ADD_INSTR(INSTRUCTION)`.
//
// 8.  Repeat steps 5 to 7 for this SM's program.
//
// 9.  Call 'APIO_SM_CLKDIV_SET(INT, FRAC)` to set the SM's clock divider.
//
// 10. Call `APIO_SM_EXECCTRL_SET(EXECCTRL)` to set the SM's EXECCTRL register.
//     There is no need to encode the wrap top and bottom here, as they are
//     handled automatically.
//
// 11. Call `APIO_SM_SHIFTCTRL_SET(SHIFTCTRL)` to set the SM's SHIFTCTRL
//     register.
//
// 12. Call `APIO_SM_PINCTRL_SET(PINCTRL)` to set the SM's PINCTRL register.
//
// 13. (Optional) Use `APIO_SM_INSTR_SET(INSTRUCTION)` to execute discrete
//     instructions on this SM immediately after configuration.
//
// 14. Call `APIO_SM_JMP_TO_START()` to set the SM to jump to the start of the
//     program after configuration.
//
// 15. (Optional) Call `APIO_LOG_SM("SM NAME")` to log the SM program details
//     for debugging.
//
// 16. (Optional) Repeat steps 4 to 15 for each additional SM in this PIO block.
//
// 17. Call `APIO_END_BLOCK()` to write all constructed programs to the PIO
//     instruction memory.
//
// 18. Repeat steps 3 to 17 for each additional PIO block.

#include <stdint.h>
#include <apio_reg.h>

#define APIO_MAX_PIO_INSTRS      32
#define APIO_MAX_SMS_PER_BLOCK   4
#define APIO_MAX_PIO_BLOCKS      3
#define APIO_MAX_FIFO_DEPTH      4

#if defined(APIO_EMULATION)
#define MAX_PRE_INSTRS   16
typedef struct {
    uint32_t irq[APIO_MAX_PIO_BLOCKS];
    uint8_t first_instr[APIO_MAX_PIO_BLOCKS][APIO_MAX_SMS_PER_BLOCK];
    uint8_t start[APIO_MAX_PIO_BLOCKS][APIO_MAX_SMS_PER_BLOCK];
    uint8_t end[APIO_MAX_PIO_BLOCKS][APIO_MAX_SMS_PER_BLOCK];
    uint8_t wrap_bottom[APIO_MAX_PIO_BLOCKS][APIO_MAX_SMS_PER_BLOCK];
    uint8_t wrap_top[APIO_MAX_PIO_BLOCKS][APIO_MAX_SMS_PER_BLOCK];
    pio_sm_reg_t pio_sm_reg[APIO_MAX_PIO_BLOCKS][APIO_MAX_SMS_PER_BLOCK];
    uint16_t instr[APIO_MAX_PIO_BLOCKS][APIO_MAX_PIO_INSTRS];
    uint16_t pre_instr[APIO_MAX_PIO_BLOCKS][APIO_MAX_SMS_PER_BLOCK][MAX_PRE_INSTRS];
    uint8_t pre_instr_count[APIO_MAX_PIO_BLOCKS][APIO_MAX_SMS_PER_BLOCK];
    uint32_t tx_fifos[APIO_MAX_PIO_BLOCKS][APIO_MAX_SMS_PER_BLOCK][APIO_MAX_FIFO_DEPTH];
    uint8_t tx_fifo_count[APIO_MAX_PIO_BLOCKS][APIO_MAX_SMS_PER_BLOCK];
    uint32_t rx_fifos[APIO_MAX_PIO_BLOCKS][APIO_MAX_SMS_PER_BLOCK][APIO_MAX_FIFO_DEPTH];
    uint8_t rx_fifo_count[APIO_MAX_PIO_BLOCKS][APIO_MAX_SMS_PER_BLOCK];
    uint8_t offset[APIO_MAX_PIO_BLOCKS];
    uint8_t max_offset[APIO_MAX_PIO_BLOCKS];
    uint8_t enabled_sms[APIO_MAX_PIO_BLOCKS];
    uint8_t block;
    uint8_t sm;
    uint8_t block_ended[APIO_MAX_PIO_BLOCKS];
    uint8_t pios_enabled;
    uint32_t gpio_base[APIO_MAX_PIO_BLOCKS];
} _apio_emulated_pio_t;
extern _apio_emulated_pio_t _apio_emulated_pio;
#define __blk  _apio_emulated_pio.block
#define __sm   _apio_emulated_pio.sm
#define __pio_start   _apio_emulated_pio.start
#define __pio_wrap_bottom   _apio_emulated_pio.wrap_bottom
#define __pio_wrap_top   _apio_emulated_pio.wrap_top
#define __pio_end   _apio_emulated_pio.end
#define __pio_offset _apio_emulated_pio.offset
#define __pio_first_instr _apio_emulated_pio.first_instr
#undef APIO0_SM_REG
#define APIO0_SM_REG(SM)  (&_apio_emulated_pio.pio_sm_reg[__blk][SM])
#undef APIO1_SM_REG
#define APIO1_SM_REG(SM)  (&_apio_emulated_pio.pio_sm_reg[__blk][SM])
#undef APIO2_SM_REG
#define APIO2_SM_REG(SM)  (&_apio_emulated_pio.pio_sm_reg[__blk][SM])
#if defined(APIO_EMU_IMPL)
_apio_emulated_pio_t _apio_emulated_pio = {
    .irq = {0xFFFFFFFF},
    .first_instr = {{0xFF}},
    .start = {{0xFF}},
    .end = {{0xFF}},
    .wrap_bottom = {{0xFF}},
    .wrap_top = {{0xFF}},
    .pio_sm_reg = {{{
        .clkdiv = 0xFFFFFFFF,
        .execctrl = 0xFFFFFFFF,
        .shiftctrl = 0xFFFFFFFF,
        .pinctrl = 0xFFFFFFFF
    }}},
    .instr = {{0xFFFF}},
    .pre_instr = {{{0xFFFF}}},
    .pre_instr_count = {{0xFF}},
    .tx_fifos = {{{0xFFFFFFFF}}},
    .tx_fifo_count = {{0xFF}},
    .rx_fifos = {{{0xFFFFFFFF}}},
    .rx_fifo_count = {{0xFF}},
    .offset = {0xFF},
    .max_offset = {0xFF},
    .enabled_sms = {0xFF},
    .block = 0xFF,
    .sm = 0xFF,
    .block_ended = {0xFF},
    .pios_enabled = 0xFF
};
#endif // APIO_EMU_NO_IMPL
#endif // APIO_EMULATION

// Include disassembler here to allow APIOX_SM_REG to be redefined for
// emulation
#include <apio_dis.h>

// Internal macros - do not use directly
#define _STATIC_BLOCK_ASSERT(BLOCK) _Static_assert((BLOCK) >= 0 && (BLOCK) < APIO_MAX_PIO_BLOCKS, "Invalid PIO block")

// Internal macro - do not use directly
#define _STATIC_SM_ASSERT(SM)       _Static_assert((SM) >= 0 && (SM) < APIO_MAX_SMS_PER_BLOCK, "Invalid PIO state machine")

// Internal macro - do not use directly
#define _OFFSET_ARRAY_INIT       {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}

// Macro to bring JTAG/SWD out of reset, for SWD logging
#if !defined(APIO_EMULATION)
#define APIO_ENABLE_JTAG() do { \
                                APIO_RESET_RESET &= ~APIO_RESET_JTAG; \
                                while (!(APIO_RESET_DONE & APIO_RESET_JTAG)); \
                            } while(0)
#else // APIO_EMULATION
#define APIO_ENABLE_JTAG()
#endif // !APIO_EMULATION


// Macro to bring IOBANK0 and PADS_BANK0 out of reset, allowing GPIO usage.
#if !defined(APIO_EMULATION)
#define APIO_ENABLE_GPIOS() do { \
                                APIO_RESET_RESET &= ~(APIO_RESET_IOBANK0 | APIO_RESET_PADS_BANK0); \
                                while (!(APIO_RESET_DONE & (APIO_RESET_IOBANK0 | APIO_RESET_PADS_BANK0))); \
                            } while(0)
#else // APIO_EMULATION
#define APIO_ENABLE_GPIOS()
#endif // !APIO_EMULATION

// Configure a GPIO for (output) usage by a PIO block.  Also disables the
// pad's isolation setting and any pad output disable.
#if !defined(APIO_EMULATION)
#define APIO_GPIO_OUTPUT(PIN, BLOCK) \
                            do { \
                                _STATIC_BLOCK_ASSERT(BLOCK); \
                                APIO_GPIO_CTRL(PIN) = APIO_GPIO_CTRL_FUNC_PIO0 + (BLOCK); \
                                APIO_GPIO_PAD(PIN) &= ~(APIO_PAD_ISO_BIT | APIO_PAD_OUTPUT_DIS_BIT); \
                            } while(0)
#else // APIO_EMULATION
#define APIO_GPIO_OUTPUT(PIN, BLOCK)
#endif // !APIO_EMULATION

// Clears IRQs for the specified PIO block
#if !defined(APIO_EMULATION)
#define PIO_CLEAR_IRQ(BLOCK)    _STATIC_BLOCK_ASSERT(BLOCK); \
                                if (BLOCK == 0) {           \
                                    APIO0_IRQ = 0xFFFFFFFF;  \
                                } else if (BLOCK == 1) {    \
                                    APIO1_IRQ = 0xFFFFFFFF;  \
                                } else {                    \
                                    APIO2_IRQ = 0xFFFFFFFF;  \
                                }
#else // APIO_EMULATION
#define PIO_CLEAR_IRQ(BLOCK)    _STATIC_BLOCK_ASSERT(BLOCK); \
                                _apio_emulated_pio.irq[BLOCK] = 0xFFFFFFFF
#endif // !APIO_EMULATION

// Clear all PIO IRQs
#if !defined(APIO_EMULATION)
#define APIO_CLEAR_ALL_IRQS()   APIO0_IRQ = 0xFFFFFFFF;  \
                                APIO1_IRQ = 0xFFFFFFFF;  \
                                APIO2_IRQ = 0xFFFFFFFF
#else // APIO_EMULATION
#define APIO_CLEAR_ALL_IRQS()   for (int __i = 0; __i < APIO_MAX_PIO_BLOCKS; __i++) { \
                                    _apio_emulated_pio.irq[__i] = 0xFFFFFFFF; \
                                }
#endif // !APIO_EMULATION

// Call before creating PIO programs
//
// Uses around 128 bytes of stack space in RP2350 hardware case.
#if !defined(APIO_EMULATION)
#define APIO_ASM_INIT()  \
    uint16_t instr_scratch[APIO_MAX_PIO_INSTRS]; \
    uint8_t __attribute__((unused)) __pio_first_instr[APIO_MAX_PIO_BLOCKS][APIO_MAX_SMS_PER_BLOCK] = _OFFSET_ARRAY_INIT; \
    uint8_t __pio_start[APIO_MAX_PIO_BLOCKS][APIO_MAX_SMS_PER_BLOCK] = _OFFSET_ARRAY_INIT; \
    uint8_t __pio_wrap_bottom[APIO_MAX_PIO_BLOCKS][APIO_MAX_SMS_PER_BLOCK] = _OFFSET_ARRAY_INIT; \
    uint8_t __pio_wrap_top[APIO_MAX_PIO_BLOCKS][APIO_MAX_SMS_PER_BLOCK] = _OFFSET_ARRAY_INIT; \
    uint8_t __attribute__((unused)) __pio_end[APIO_MAX_PIO_BLOCKS][APIO_MAX_SMS_PER_BLOCK] = _OFFSET_ARRAY_INIT; \
    uint8_t __pio_offset[APIO_MAX_PIO_BLOCKS] = {0, 0, 0}; \
    uint8_t __blk = 0; \
    uint8_t __sm = 0
#else // APIO_EMULATION
#define APIO_ASM_INIT() uint8_t _pios_enabled = _apio_emulated_pio.pios_enabled; \
                        _apio_emulated_pio = (_apio_emulated_pio_t){0};                    \
                        _apio_emulated_pio.pios_enabled = _pios_enabled
#endif // !APIO_EMULATION

// Assert these, as if they change, the above stack space calculation must be updated.
_Static_assert((APIO_MAX_PIO_BLOCKS == 3), "APIO_MAX_PIO_BLOCKS must be 3");
_Static_assert((APIO_MAX_SMS_PER_BLOCK == 4), "APIO_MAX_SMS_PER_BLOCK must be 4");
_Static_assert((APIO_MAX_PIO_INSTRS == 32), "APIO_MAX_PIO_INSTRS must be 32");

// Bring PIO blocks out of reset
#if !defined(APIO_EMULATION)
#define APIO_ENABLE_PIOS()  do { \
                                APIO_RESET_RESET &= ~(APIO_RESET_PIO0 | APIO_RESET_PIO1 | APIO_RESET_PIO2 ); \
                                while (!(APIO_RESET_DONE & (APIO_RESET_PIO0 | APIO_RESET_PIO1 | APIO_RESET_PIO2))); \
                            } while(0)
#else // APIO_EMULATION
#define APIO_ENABLE_PIOS()    _apio_emulated_pio.pios_enabled = 1
#endif // !APIO_EMULATION

#if !defined(APIO_EMULATION)
#define APIO_ENABLE_SMS(BLOCK, SMS_MASK)  \
                                _STATIC_BLOCK_ASSERT(BLOCK); \
                                _Static_assert((SMS_MASK) > 0 && (SMS_MASK) < (1 << APIO_MAX_SMS_PER_BLOCK), "Invalid SMS_MASK"); \
                                if (BLOCK == 0) {           \
                                    APIO0_CTRL_SM_ENABLE(SMS_MASK);  \
                                } else if (BLOCK == 1) {    \
                                    APIO1_CTRL_SM_ENABLE(SMS_MASK);  \
                                } else {                    \
                                    APIO2_CTRL_SM_ENABLE(SMS_MASK);  \
                                }
#else // APIO_EMULATION
#define APIO_ENABLE_SMS(BLOCK, SMS_MASK)    _apio_emulated_pio.enabled_sms[BLOCK] = SMS_MASK
#endif // !APIO_EMULATION

// Set the current PIO block
#define APIO_SET_BLOCK(BLOCK)   _STATIC_BLOCK_ASSERT(BLOCK); \
                                __blk = BLOCK

// Set the current PIO SM
#define APIO_SET_SM(SM)         _STATIC_SM_ASSERT(SM);                                   \
                                __sm = SM;                                              \
                                __pio_first_instr[__blk][__sm] = __pio_offset[__blk];   \
                                __pio_start[__blk][__sm] = __pio_offset[__blk];         \
                                __pio_wrap_bottom[__blk][__sm] = __pio_offset[__blk];   \
                                __pio_wrap_top[__blk][__sm] = __pio_offset[__blk];      \
                                __pio_end[__blk][__sm] = __pio_offset[__blk]

// Use a label as a destination for JMPs
#define APIO_LABEL(NAME)        __pio_label__##NAME

// Create a label for JMPs
#define APIO_LABEL_NEW(NAME)    uint8_t APIO_LABEL(NAME) = __pio_offset[__blk]

// Create a label for JMPs at a relative offset
#define APIO_LABEL_NEW_OFFSET(NAME, OFFSET) uint8_t APIO_LABEL(NAME) = __pio_offset[__blk] + (OFFSET)

// Set the start offset within a PIO program - call before `APIO_ADD_INSTR()`
// for the start instruction.
#define APIO_START()            __pio_start[__blk][__sm] = __pio_offset[__blk]

// Get a label representing the start of the current PIO program
#define APIO_START_LABEL()      __pio_start[__blk][__sm]

// Set the end offset within a PIO program - call before `APIO_ADD_INSTR()`
// for the last instruction.  Must be called after `APIO_WRAP_TOP()`.  If
// .wrap is the last instruction, this is not required.
#define APIO_END()              __pio_end[__blk][__sm] = __pio_offset[__blk]

// Set the wrap bottom offset within a PIO program - call before
// `APIO_ADD_INSTR()` for the .wrap_target instruction.
#define APIO_WRAP_BOTTOM()      __pio_wrap_bottom[__blk][__sm] = __pio_offset[__blk]

// Set the wrap top offset within a PIO program - call before
// `APIO_ADD_INSTR()` for the .wrap instruction.
#define APIO_WRAP_TOP()         __pio_wrap_top[__blk][__sm] = __pio_offset[__blk];  \
                                APIO_END()

// Add an instruction to the current PIO program.
#if !defined(APIO_EMULATION)
#if defined(DEBUG_LOGGING) && (DEBUG_LOGGING == 1)
#define APIO_ADD_INSTR(INST)    instr_scratch[__pio_offset[__blk]++] = INST
#else // !DEBUG_LOGGING
#define APIO_ADD_INSTR(INST)    instr_scratch[__pio_offset[__blk]++] = INST
#endif // DEBUG_LOGGING
#else // APIO_EMULATION
#define APIO_ADD_INSTR(INST)    _apio_emulated_pio.instr[__blk][_apio_emulated_pio.offset[__blk]++] = INST
#endif // !APIO_EMULATION

// Set the clock divider for the current PIO SM.
#define APIO_SM_CLKDIV_SET(INT, FRAC)   _apio_sm_reg_ptr(__blk, __sm)->clkdiv = APIO_CLKDIV((INT), (FRAC))

// Set the EXECCTRL for the current PIO SM.  Do not include wrap top/bottom.
// Those will be set automatically from the wrap values.
#define APIO_SM_EXECCTRL_SET(EXECCTRL)  _apio_sm_reg_ptr(__blk, __sm)->execctrl =  \
                                            (EXECCTRL) | \
                                            APIO_WRAP_BOTTOM_AS_REG(__pio_wrap_bottom[__blk][__sm]) |  \
                                            APIO_WRAP_TOP_AS_REG(__pio_wrap_top[__blk][__sm])

// Set the SHIFTCTRL for the current PIO SM.
#define APIO_SM_SHIFTCTRL_SET(SHIFTCTRL)    _apio_sm_reg_ptr(__blk, __sm)->shiftctrl = (SHIFTCTRL)

// Set the PINCTRL for the current PIO SM.
#define APIO_SM_PINCTRL_SET(PINCTRL)    _apio_sm_reg_ptr(__blk, __sm)->pinctrl = (PINCTRL)

static inline volatile pio_sm_reg_t* _apio_sm_reg_ptr(uint8_t block, uint8_t sm) {
    if (block == 0) return APIO0_SM_REG(sm);
    else if (block == 1) return APIO1_SM_REG(sm);
    else return APIO2_SM_REG(sm);
}

// Immediately execute an instruction on the current PIO SM.  Can be called
// before enabling the SM to set initial state.
#if !defined(APIO_EMULATION)
#define APIO_SM_EXEC_INSTR(INSTR) _apio_sm_reg_ptr(__blk, __sm)->instr = INSTR
#else // APIO_EMULATION
#define APIO_SM_EXEC_INSTR(INSTR) _apio_emulated_pio.pre_instr[__blk][__sm][_apio_emulated_pio.pre_instr_count[__blk][__sm]++] = INSTR;
#endif // !APIO_EMULATION

#if !defined(APIO_EMULATION)
static inline volatile uint32_t* _apio_txf_ptr(uint8_t block, uint8_t sm) {
    if (block == 0) return (volatile uint32_t *)((uintptr_t)APIO0_BASE + APIO_TXF_OFFSET + (sm * 0x04));
    else if (block == 1) return (volatile uint32_t *)((uintptr_t)APIO1_BASE + APIO_TXF_OFFSET + (sm * 0x04));
    else return (volatile uint32_t *)((uintptr_t)APIO2_BASE + APIO_TXF_OFFSET + (sm * 0x04));
}
static inline volatile uint32_t* _apio_rxf_ptr(uint8_t block, uint8_t sm) {
    if (block == 0) return (volatile uint32_t *)((uintptr_t)APIO0_BASE + APIO_RXF_OFFSET + (sm * 0x04));
    else if (block == 1) return (volatile uint32_t *)((uintptr_t)APIO1_BASE + APIO_RXF_OFFSET + (sm * 0x04));
    else return (volatile uint32_t *)((uintptr_t)APIO2_BASE + APIO_RXF_OFFSET + (sm * 0x04));
}
#endif // !APIO_EMULATION

// Access the current SM's TX FIFO
#if !defined(APIO_EMULATION)
#define APIO_TXF (*_apio_txf_ptr(__blk, __sm))
#else // APIO_EMULATION
#define APIO_TXF _apio_emulated_pio.tx_fifos[__blk][__sm][_apio_emulated_pio.tx_fifo_count[__blk][__sm]++]
#endif // !APIO_EMULATION

// Access the current SM's RX FIFO
#if !defined(APIO_EMULATION)
#define APIO_RXF (*_apio_rxf_ptr(__blk, __sm))
#else // APIO_EMULATION
#define APIO_RXF _apio_emulated_pio.rx_fifos[__blk][__sm][_apio_emulated_pio.rx_fifo_count[__blk][__sm]++]
#endif // !APIO_EMULATION

// Set the current PIO SM to jump to its start instruction after
// configuration.  The PIO SM will only be started by explicitly enabling.
// This sets the point at which it will start.
#define APIO_SM_JMP_TO_START()  APIO_SM_EXEC_INSTR(APIO_JMP(__pio_start[__blk][__sm]))

#if !defined(APIO_EMULATION)
static inline volatile uint32_t* _apio_instr_mem_ptr(uint8_t block) {
    if (block == 0) return (volatile uint32_t *)(APIO0_BASE + APIO_INSTR_MEM_OFFSET);
    else if (block == 1) return (volatile uint32_t *)(APIO1_BASE + APIO_INSTR_MEM_OFFSET);
    else return (volatile uint32_t *)(APIO2_BASE + APIO_INSTR_MEM_OFFSET);
}
#endif // !APIO_EMULATION

// Write the constructed PIO programs to the PIO instruction memory for the
// current PIO block.  Call after all SMs for this block have been built,
// before enabling.
#if !defined(APIO_EMULATION)
#define APIO_END_BLOCK() do { \
                            volatile uint32_t* ptr = _apio_instr_mem_ptr(__blk);    \
                            for (int ii = 0; ii < __pio_offset[__blk]; ii++) {    \
                                 ptr[ii] = instr_scratch[ii];                        \
                            }                                                       \
                        } while(0)
#else // APIO_EMULATION
#define APIO_END_BLOCK() _apio_emulated_pio.max_offset[__blk] = __pio_offset[__blk]
#endif // !APIO_EMULATION

// Call to enable one or more SMs within a PIO block.  To enable more than SM
// simultaneously, OR the SM numbers together (e.g. to enable SM0 and SM2, use
// 0b00000101 = 5).
#if !defined(APIO_EMULATION)
#define APIO_ENABLE_SM(BLOCK, SM_MASK)  _STATIC_BLOCK_ASSERT(BLOCK);         \
                                        _Static_assert((SM_MASK < 0xF), "Attempt to enable invalid SM"); \
                                        if (BLOCK == 0) {                   \
                                            APIO0_CTRL_SM_ENABLE(SM_MASK);   \
                                        } else if (BLOCK == 1) {            \
                                            APIO1_CTRL_SM_ENABLE(SM_MASK);   \
                                        } else {                            \
                                            APIO2_CTRL_SM_ENABLE(SM_MASK);   \
                                        }
#else // APIO_EMULATION
#define APIO_ENABLE_SM(BLOCK, SM_MASK)  _STATIC_BLOCK_ASSERT(BLOCK);         \
                                        _Static_assert((SM_MASK < 0xF), "Attempt to enable invalid SM"); \
                                        _apio_emulated_pio.enabled_sms[BLOCK] |= SM_MASK
#endif // !APIO_EMULATION

//
// PIO Instruction Macros
//

// Add a side set delay from 0-31 cycles to an instruction
#define APIO_ADD_DELAY(INST, DELAY)  ((INST) | (((DELAY) & 0x1F) << 8))

// Move the pins value to the ISR
#define APIO_IN_PINS(NUM)            (0x4000 | ((NUM) & 0x1F))

// Move X to the ISR
#define APIO_IN_X(NUM)               (0x4020 | ((NUM) & 0x1F))

// Move Y to the ISR
#define APIO_IN_Y(NUM)               (0x4040 | ((NUM) & 0x1F))

// Clear one of this PIO block's IRQs
#define APIO_IRQ_CLEAR(X)            (0xC040 | ((X) & 0x07))

// Clear one of the previous PIO block's IRQs
#define APIO_IRQ_CLEAR_PREV(X)       (0xC048 | ((X) & 0x07))

// Clear one of the next PIO block's IRQs
#define APIO_IRQ_CLEAR_NEXT(X)       (0xC058 | ((X) & 0x07))

// Set one of this PIO block's IRQs to 1
#define APIO_IRQ_SET(X)              (0xC000 | ((X) & 0x07))

// Set one of the previous PIO block's IRQs to 1
#define APIO_IRQ_SET_PREV(X)         (0xC008 | ((X) & 0x07))

// Set one of the next PIO block's IRQs to 1
#define APIO_IRQ_SET_NEXT(X)         (0xC018 | ((X) & 0x07))

// Jump unconditionally to label X within this PIO program 
#define APIO_JMP(X)                  (0x0000 | ((X) & 0x1F))

// Jump to label if X register is zero
#define APIO_JMP_NOT_X(DEST)         (0x0020 | ((DEST) & 0x1F))

// Jump to label if X register is non-zero and then decrement Y after the test
#define APIO_JMP_X_DEC(DEST)         (0x0040 | ((DEST) & 0x1F))

// Jump to label if Y register is zero
#define APIO_JMP_NOT_Y(DEST)         (0x0060 | ((DEST) & 0x1F))

// Jump to label if Y register is non-zero and then decrement X after the test
#define APIO_JMP_Y_DEC(DEST)         (0x0080 | ((DEST) & 0x1F))

// Jump to label if X register is not equal to Y register
#define APIO_JMP_X_NOT_Y(DEST)       (0x00A0 | ((DEST) & 0x1F))

// Jump to label if pin specified as the EXECCTRL JMP_PIN is high
#define APIO_JMP_PIN(X)              (0x00C0 | ((X) & 0x1F))

// Jump to label if the OSR is not empty
#define APIO_JMP_NOT_OSRE            (0x00E0)

// Set the output pin values to 0 (low)
#define APIO_MOV_PINS_NULL           0xA003

// Move the pin values to the X register
#define APIO_MOV_X_PINS              0xA020

// Move the OSR into the X register
#define APIO_MOV_X_OSR               0xA027

// Set the output pin directions to 0 (inputs)
#define APIO_MOV_PINDIRS_NULL        0xA063

// Set the output pin directions to 1 (outputs)
#define APIO_MOV_PINDIRS_NOT_NULL    0xA06B

// Move the pin values to the ISR
#define APIO_MOV_ISR_PINS            0xA0C0

// No operation (move Y to Y)
#define APIO_NOP                     0xA042

// Move data from OSR to the output pins
#define APIO_OUT_PINS(NUM)           (0x6000 | ((NUM) & 0x1F))

// Move data from OSR to X register
#define APIO_OUT_X(NUM)              (0x6020 | ((NUM) & 0x1F))

// Move data from OSR to Y register
#define APIO_OUT_Y(NUM)              (0x6040 | ((NUM) & 0x1F))

// Move data from OSR to null (discard)
#define APIO_OUT_NULL(NUM)           (0x6060 | ((NUM) & 0x1F))

// Move data from OSR to pin directions
#define APIO_OUT_PINDIRS(NUM)        (0x6080 | ((NUM) & 0x1F))

// Move data from OSR to PC
#define APIO_OUT_PC(NUM)             (0x60A0 | ((NUM) & 0x1F))

// Move data from OSR to ISR
#define APIO_OUT_ISR(NUM)            (0x60C0 | ((NUM) & 0x1F))

// Move data from OSR to EXEC (execute as instruction)
#define APIO_OUT_EXEC(NUM)           (0x60E0 | ((NUM) & 0x1F))

// Pull data from the TX FIFO into the OSR, blocking if FIFO is empty
#define APIO_PULL_BLOCK              0x80A0

// Push data from the ISR into the RX FIFO, blocking if FIFO is full
#define APIO_PUSH_BLOCK              0x8020

// Set output pins to VALUE (0-31)
#define APIO_SET_PINS(VALUE)         (0xE000 | ((VALUE) & 0x1F))

// Set X register to VALUE (0-31)
#define APIO_SET_X(VALUE)            (0xE020 | ((VALUE) & 0x1F))

// Set Y register to VALUE (0-31)
#define APIO_SET_Y(VALUE)            (0xE040 | ((VALUE) & 0x1F))

// Set output pin directions to VALUE (0-31)
#define APIO_SET_PIN_DIRS(VALUE)     (0xE080 | ((VALUE) & 0x1F))

// Wait for one of this PIO block's IRQs to go high.  Clears the IRQ
// after the instruction (so other PIOs waiting at the same time will also be
// triggered).
#define APIO_WAIT_IRQ_HIGH(X)        (0x20C0 | ((X) & 0x07))

// Wait for one of the previous PIO block's IRQs to go high.
#define APIO_WAIT_IRQ_HIGH_PREV(X)   (0x20C8 | ((X) & 0x07))

// Wait for one of the next PIO block's IRQs to go high.
#define APIO_WAIT_IRQ_HIGH_NEXT(X)   (0x20D8 | ((X) & 0x07))

// Wait for one of this PIO block's IRQs to go low.
#define APIO_WAIT_IRQ_LOW(X)         (0x2040 | ((X) & 0x07))

// Wait for one of the previous PIO block's IRQs to go low.
#define APIO_WAIT_IRQ_LOW_PREV(X)    (0x2048 | ((X) & 0x07))

// Wait for one of the next PIO block's IRQs to go low.
#define APIO_WAIT_IRQ_LOW_NEXT(X)    (0x2058 | ((X) & 0x07))

// Wait for the specified pin to go high
#define APIO_WAIT_PIN_HIGH(X)        (0x20A0 | ((X) & 0x1F))

#if !defined(APIO_EMULATION)
#define APIO_ASM_WFI()               __asm volatile("wfi")
#else // APIO_EMULATION
#define APIO_ASM_WFI()               return 0
#endif // !APIO_EMULATION

#endif // APIO_H