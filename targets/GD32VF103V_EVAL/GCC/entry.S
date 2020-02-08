// See LICENSE for license details

#ifndef ENTRY_S
#define ENTRY_S

#include "riscv_encoding.h"
#include "riscv_bits.h"
#include "n200_eclic.h"
#include "n200_timer.h"

###############################################
###############################################
# Disable Interrupt
#
.macro DISABLE_MIE
  csrc CSR_MSTATUS, MSTATUS_MIE  
.endm

.macro push_reg
    addi sp, sp, -32*4

    sw x1, 0 * 4(sp)
    sw x2, 1 * 4(sp)
    sw x3, 2 * 4(sp)
    sw x4, 3 * 4(sp)
    sw x5, 4 * 4(sp)
    sw x6, 5 * 4(sp)
    sw x7, 6 * 4(sp)
    sw x8, 7 * 4(sp)
    sw x9, 8 * 4(sp)
    sw x10, 9 * 4(sp)
    sw x11, 10 * 4(sp)
    sw x12, 11 * 4(sp)
    sw x13, 12 * 4(sp)
    sw x14, 13 * 4(sp)
    sw x15, 14 * 4(sp)
    sw x16, 15 * 4(sp)
    sw x17, 16 * 4(sp)
    sw x18, 17 * 4(sp)
    sw x19, 18 * 4(sp)
    sw x20, 19 * 4(sp)
    sw x21, 20 * 4(sp)
    sw x22, 21 * 4(sp)
    sw x23, 22 * 4(sp)
    sw x24, 23 * 4(sp)
    sw x25, 24 * 4(sp)
    sw x26, 25 * 4(sp)
    sw x27, 26 * 4(sp)
    sw x28, 27 * 4(sp)
    sw x29, 28 * 4(sp)
    sw x30, 29 * 4(sp)
    sw x31, 30 * 4(sp)

.endm

.macro pop_reg

    lw x1, 0 * 4(sp)

    lw x5, 4 * 4(sp)
    lw x6, 5 * 4(sp)
    lw x7, 6 * 4(sp)
    lw x8, 7 * 4(sp)
    lw x9, 8 * 4(sp)
    lw x10, 9 * 4(sp)
    lw x11, 10 * 4(sp)
    lw x12, 11 * 4(sp)
    lw x13, 12 * 4(sp)
    lw x14, 13 * 4(sp)
    lw x15, 14 * 4(sp)
    lw x16, 15 * 4(sp)
    lw x17, 16 * 4(sp)
    lw x18, 17 * 4(sp)
    lw x19, 18 * 4(sp)
    lw x20, 19 * 4(sp)
    lw x21, 20 * 4(sp)
    lw x22, 21 * 4(sp)
    lw x23, 22 * 4(sp)
    lw x24, 23 * 4(sp)
    lw x25, 24 * 4(sp)
    lw x26, 25 * 4(sp)
    lw x27, 26 * 4(sp)
    lw x28, 27 * 4(sp)
    lw x29, 28 * 4(sp)
    lw x30, 29 * 4(sp)
    lw x31, 30 * 4(sp)

    addi sp, sp, 35*4

.endm

###############################################
###############################################
#Save caller registers
.macro SAVE_CONTEXT

#ifdef __riscv_flen
  #if (__riscv_flen==64 )
    addi sp, sp, -20*REGBYTES - 20*FPREGBYTES
  #else
    addi sp, sp, -20*REGBYTES
  #endif
#else
  addi sp, sp, -20*REGBYTES
#endif
  STORE x1, 0*REGBYTES(sp)
	STORE x4, 1*REGBYTES(sp)
	STORE x5, 2*REGBYTES(sp)
	STORE x6, 3*REGBYTES(sp)
	STORE x7, 4*REGBYTES(sp)
	STORE x10, 5*REGBYTES(sp)
	STORE x11, 6*REGBYTES(sp)
	STORE x12, 7*REGBYTES(sp)
	STORE x13, 8*REGBYTES(sp)
	STORE x14, 9*REGBYTES(sp)
	STORE x15, 10*REGBYTES(sp)
#ifndef __riscv_32e
  STORE x16, 11*REGBYTES(sp)
	STORE x17, 12*REGBYTES(sp)
	STORE x28, 13*REGBYTES(sp)
	STORE x29, 14*REGBYTES(sp)
	STORE x30, 15*REGBYTES(sp)
	STORE x31, 16*REGBYTES(sp)
#endif

#ifdef __riscv_flen
  #if (__riscv_flen == 64)
  FPSTORE f0, (20*REGBYTES + 0*FPREGBYTES)(sp)
	FPSTORE f1, (20*REGBYTES + 1*FPREGBYTES)(sp)
	FPSTORE f2, (20*REGBYTES + 2*FPREGBYTES)(sp)
	FPSTORE f3, (20*REGBYTES + 3*FPREGBYTES)(sp)
	FPSTORE f4, (20*REGBYTES + 4*FPREGBYTES)(sp)
	FPSTORE f5, (20*REGBYTES + 5*FPREGBYTES)(sp)
	FPSTORE f6, (20*REGBYTES + 6*FPREGBYTES)(sp)
	FPSTORE f7, (20*REGBYTES + 7*FPREGBYTES)(sp)
	FPSTORE f10, (20*REGBYTES + 8*FPREGBYTES)(sp)
	FPSTORE f11, (20*REGBYTES + 9*FPREGBYTES)(sp)
	FPSTORE f12, (20*REGBYTES + 10*FPREGBYTES)(sp)
	FPSTORE f13, (20*REGBYTES + 11*FPREGBYTES)(sp)
	FPSTORE f14, (20*REGBYTES + 12*FPREGBYTES)(sp)
	FPSTORE f15, (20*REGBYTES + 13*FPREGBYTES)(sp)
	FPSTORE f16, (20*REGBYTES + 14*FPREGBYTES)(sp)
	FPSTORE f17, (20*REGBYTES + 15*FPREGBYTES)(sp)
	FPSTORE f28, (20*REGBYTES + 16*FPREGBYTES)(sp)
	FPSTORE f29, (20*REGBYTES + 17*FPREGBYTES)(sp)
	FPSTORE f30, (20*REGBYTES + 18*FPREGBYTES)(sp)
	FPSTORE f31, (20*REGBYTES + 19*FPREGBYTES)(sp)   
  #endif    
#endif


.endm


###############################################
###############################################
#restore caller registers
.macro RESTORE_CONTEXT
  LOAD x1, 0*REGBYTES(sp)
	LOAD x4, 1*REGBYTES(sp)
	LOAD x5, 2*REGBYTES(sp)
	LOAD x6, 3*REGBYTES(sp)
	LOAD x7, 4*REGBYTES(sp)
	LOAD x10, 5*REGBYTES(sp)
	LOAD x11, 6*REGBYTES(sp)
	LOAD x12, 7*REGBYTES(sp)
	LOAD x13, 8*REGBYTES(sp)
	LOAD x14, 9*REGBYTES(sp)
	LOAD x15, 10*REGBYTES(sp)
#ifndef __riscv_32e
  LOAD x16, 11*REGBYTES(sp)
	LOAD x17, 12*REGBYTES(sp)
	LOAD x28, 13*REGBYTES(sp)
	LOAD x29, 14*REGBYTES(sp)
	LOAD x30, 15*REGBYTES(sp)
	LOAD x31, 16*REGBYTES(sp)
#endif


#ifdef __riscv_flen
  #if (__riscv_flen==64)
/* Restore fp caller registers */
	FPLOAD f0, (20*REGBYTES + 0*FPREGBYTES)(sp)
	FPLOAD f1, (20*REGBYTES + 1*FPREGBYTES)(sp)
	FPLOAD f2, (20*REGBYTES + 2*FPREGBYTES)(sp)
	FPLOAD f3, (20*REGBYTES + 3*FPREGBYTES)(sp)
	FPLOAD f4, (20*REGBYTES + 4*FPREGBYTES)(sp)
	FPLOAD f5, (20*REGBYTES + 5*FPREGBYTES)(sp)
	FPLOAD f6, (20*REGBYTES + 6*FPREGBYTES)(sp)
	FPLOAD f7, (20*REGBYTES + 7*FPREGBYTES)(sp)
	FPLOAD f10, (20*REGBYTES + 8*FPREGBYTES)(sp)
	FPLOAD f11, (20*REGBYTES + 9*FPREGBYTES)(sp)
	FPLOAD f12, (20*REGBYTES + 10*FPREGBYTES)(sp)
	FPLOAD f13, (20*REGBYTES + 11*FPREGBYTES)(sp)
	FPLOAD f14, (20*REGBYTES + 12*FPREGBYTES)(sp)
	FPLOAD f15, (20*REGBYTES + 13*FPREGBYTES)(sp)
	FPLOAD f16, (20*REGBYTES + 14*FPREGBYTES)(sp)
	FPLOAD f17, (20*REGBYTES + 15*FPREGBYTES)(sp)
	FPLOAD f28, (20*REGBYTES + 16*FPREGBYTES)(sp)
	FPLOAD f29, (20*REGBYTES + 17*FPREGBYTES)(sp)
	FPLOAD f30, (20*REGBYTES + 18*FPREGBYTES)(sp)
	FPLOAD f31, (20*REGBYTES + 19*FPREGBYTES)(sp)
  #endif
#endif


#ifdef __riscv_flen
  #if(__riscv_flen == 64 )
    addi sp, sp, 20*REGBYTES  + 20*FPREGBYTES
  #else
    addi sp, sp, 20*REGBYTES
  #endif
#else
// De-allocate the stack space
 addi sp, sp, 20*REGBYTES
#endif
.endm

###############################################
###############################################
#restore caller registers
.macro RESTORE_CONTEXT_EXCPT_X5
  LOAD x1,  0*REGBYTES(sp)
  LOAD x6,  2*REGBYTES(sp)
  LOAD x7,  3*REGBYTES(sp)
  LOAD x10, 4*REGBYTES(sp)
  LOAD x11, 5*REGBYTES(sp)
  LOAD x12, 6*REGBYTES(sp)
  LOAD x13, 7*REGBYTES(sp)
  LOAD x14, 8*REGBYTES(sp)
  LOAD x15, 9*REGBYTES(sp)
#ifndef __riscv_32e
  LOAD x16, 10*REGBYTES(sp)
  LOAD x17, 11*REGBYTES(sp)
  LOAD x28, 12*REGBYTES(sp)
  LOAD x29, 13*REGBYTES(sp)
  LOAD x30, 14*REGBYTES(sp)
  LOAD x31, 15*REGBYTES(sp)
#endif
.endm

###############################################
###############################################
#restore caller registers
.macro RESTORE_CONTEXT_ONLY_X5
  LOAD x5,  1*REGBYTES(sp)
.endm

###############################################
###############################################
# Save the mepc and mstatus
#
.macro SAVE_EPC_STATUS
  csrr x5, CSR_MEPC  
  STORE x5,  32*REGBYTES(sp)
  csrr x5, CSR_MSTATUS  
  STORE x5,  33*REGBYTES(sp)
  csrr x5, CSR_MSUBM  
  STORE x5,  34*REGBYTES(sp)
.endm

###############################################
###############################################
# Restore the mepc and mstatus
#
.macro RESTORE_EPC_STATUS
  LOAD x5,  32*REGBYTES(sp)
  csrw CSR_MEPC, x5  
  LOAD x5,  33*REGBYTES(sp)
  csrw CSR_MSTATUS, x5  
  LOAD x5,  34*REGBYTES(sp)
  csrw CSR_MSUBM, x5  
.endm



###############################################
###############################################
// Trap entry point
//
  .section      .text.trap	
  .align 6// In CLIC mode, the trap entry must be 64bytes aligned
  .global trap_entry
.weak trap_entry
trap_entry:
  // Allocate the stack space
 // addi sp, sp, -19*REGBYTES

  // Save the caller saving registers (context)
  SAVE_CONTEXT
  // Save the MEPC/Mstatus/Msubm reg
  SAVE_EPC_STATUS

     // Set the function argument
  csrr a0, mcause
  mv a1, sp
     // Call the function
  call handle_trap

  // Restore the MEPC/Mstatus/Msubm reg
  RESTORE_EPC_STATUS
  // Restore the caller saving registers (context)
  RESTORE_CONTEXT

  // De-allocate the stack space
 // addi sp, sp, 19*REGBYTES
  // Return to regular code
  mret


###############################################
###############################################
// IRQ entry point
//
  .section      .text.irq	
  .align 2
  .global irq_entry
.weak irq_entry
irq_entry: // -------------> This label will be set to MTVT2 register
  // Allocate the stack space

  SAVE_CONTEXT// Save 16 regs

  //------This special CSR read operation, which is actually use mcause as operand to directly store it to memory
  csrrwi  x0, CSR_PUSHMCAUSE, 17
  //------This special CSR read operation, which is actually use mepc as operand to directly store it to memory
  csrrwi  x0, CSR_PUSHMEPC, 18
  //------This special CSR read operation, which is actually use Msubm as operand to directly store it to memory
  csrrwi  x0, CSR_PUSHMSUBM, 19


  la t0, g_int_cnt
  lw t1, 0(t0)
  addi t1, t1, 1
  sw t1, 0(t0)
  li t2, 1
  bgtu t1,t2,service_loop

  csrw mscratch, sp
  la sp, __irq_stack_top

service_loop:
  //------This special CSR read/write operation, which is actually Claim the CLIC to find its pending highest
  // ID, if the ID is not 0, then automatically enable the mstatus.MIE, and jump to its vector-entry-label, and
  // update the link register 
  csrrw ra, CSR_JALMNXTI, ra 
  
  //RESTORE_CONTEXT_EXCPT_X5

  la t0, g_int_cnt
  lw t1, 0(t0)
  addi t1, t1, -1
  sw t1, 0(t0)
  bnez t1, _rfi

  csrr sp, mscratch

  DISABLE_MIE # Disable interrupts

  LOAD x5,  19*REGBYTES(sp)
  csrw CSR_MSUBM, x5
  LOAD x5,  18*REGBYTES(sp)
  csrw CSR_MEPC, x5
  LOAD x5,  17*REGBYTES(sp)
  csrw CSR_MCAUSE, x5


  la t0, g_usLosTaskLock
  lw t1, 0(t0)
  bnez t1, _rfi

  la      t0, g_stLosTask
  lw      t1, 0x4(t0)
  lw      t2, 0x0(t0)
  beq  t1, t2, _rfi

  RESTORE_CONTEXT

  push_reg
  csrr t0, mepc
  sw t0, 31*4(sp)
  csrw mscratch, sp
  j TaskSwitch

_rfi:
  RESTORE_CONTEXT
  // Return to regular code
  mret


#endif
