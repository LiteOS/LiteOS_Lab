Cortex-M variants attributes mitrix:

|                                 | M0/M0+/M1 | M3/M4/M7           | M23      | M33/M35P |
| ------------------------------- | --------- | ------------------ | -------- | -------- |
| CBNZ, CBZ (T1)                  | NO        | YES                | YES      | YES      |
| IT (T1)                         | NO        | YES                | NO       | YES      |
| other Thumb-1 instructions      | YES       | YES                | YES      | YES      |
| BL, DMB, DSB ISB, MRS, MSR (T2) | YES       | YES                | YES      | YES      |
| other Thumb-2 instructions      | NO        | YES                | NO       | YES      |
| HardFault                       | YES       | YES                | YES      | YES      |
| MemManage fault                 | NO        | YES                | NO       | YES      |
| BusFault                        | NO        | YES                | NO       | YES      |
| UsageFault                      | NO        | YES                | NO       | YES      |
| SecureFault                     | NO        | YES                | NO       | YES      |
| DebugMonitor                    | NO        | YES                | NO       | YES      |
| FPU                             | NO        | optinal for M4, M7 | NO       | YES      |
| ARCH VERSION                    | ARMV6M    | ARMV7M             | ARMV8M   | ARMV8M   |
| can be support as               | baseline  | mainline           | baseline | mainline |

From "A1.4 ARMv8-M variants" in DDI0553A_armv8m_arm.pdf:

• A PE with the Main Extension is also referred to as a Mainline implementation.
• A PE without the Main Extension is also referred to as a Baseline implementation. A Baseline
implementation has a subset of the instructions, registers, and features, of a Mainline implementation.
• ARMv7-M compatibility requires the Main Extension.
• ARMv6-M compatibility is provided by all ARMv8-M implementations. 

These differences will effect:

1. the .s code (instructions)
2. exception handler

Other differences are not considered by arch code.

So we can just divide these variants into two different parts (according to ARMv8 manual) even for ARMv6M and ARMv7M:
1. baseline, including ARMv6M, ARMv8M-baseline, compile with config `CONFIG_AARCH_M_BASELINE` selected
2. mainline, including ARMv7M, ARMv8M-mainline, compile with macro `CONFIG_AARCH_M_MAINLINE` selected

Note: ARMv8M-baseline have `cbz`, `cbnz` instructions supported, for all sub-arches with these two instructions, `CONFIG_AARCH_M_CBNZ_CBZ` is selected

