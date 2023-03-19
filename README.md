# RISC-V RV64I Instruction Set Simulator

This is an instruction set simulator (ISS) for the RV64I subset of the RISC-V instruction set, including Zicsr extension instructions.
Developed as a project in univeristy.

To build:
```
make all
```

To execute: 
```
./rv64sim
```

`-v` for verbose output, 
`-c` to enable cycle and instruction reporting

Supported CLI inputs: 

|Command|Operation performed|
|---|---|
|xn|Show the content of register xn in hex (n is register number, from 0 to 31). The value is displayed as 16 hex digits with leading 0s.|
|xn = value|Set register xn to value (value in hex).|
|pc|Show content of PC register in hex. The value is displayed as 16 hex digits with leading 0s.|
|pc = address|Set PC register to address (address in hex).|
|m address|Show the content of memory doubleword at address (address in hex, rv64sim rounds it down to nearest doubleword-aligned address). The value is displayed as 16 hex digits with leading 0s.|
|m address = value|Set memory doubleword at address to value (address in hex, rv64sim rounds it down to nearest doubleword-aligned address; value in hex).|
|l "filename"|Load memory from Intel hex format file named filename. If the file includes a start address record, the PC is set to the start address.|
|.|Execute one instruction.|
|. n|Execute n instructions.|
|b address|Set an execution breakpoint at address. If the simulator is executing multiple instructions (. n command), it stops when the PC reaches address without executing that instruction. There is only one execution breakpoint; using the b command with a different address removes any previously set breakpoint.|
|b|Clear the breakpoint.|
|csr num|Show the content of CSR num (num in hex). The value is displayed as 16 hex digits with leading 0s.|
|csr num = value|Set CSR num to value (num and value in hex).|
|prv|Display the current processor privilege level (0 = user, or 3 = machine)|
|prv = value|Set the current processor privilege level to value (0 = user, or 3 = machine)|

Exceptions: 

|Cause code|Exception|Instructions that cause exception|
|---|---|---|
|0|Instruction address misaligned|Any instruction fetch for which the PC is not a multiple of 4.|
|2|Illegal instruction|Any defined instruction that is not implemented; Any undefined instruction; An mret instruction executed in user mode; A csr instruction (not the csr command) that accesses an undefined or unimplemented CSR.|
|3|Breakpoint|ebreak|
|4|Load address misaligned|ld for which the effective address is not a multiple of 8. lw/lwu for which the effective address is not a multiple of 4. lh/lhu or which the effective address is not a multiple of 2.|
|6|Store address misaligned|sd for which the effective address is not a multiple of 8. sw for which the effective address is not a multiple of 4. sh or which the effective address is not a multiple of 2.|
|8|Environment call from U-mode|ecall executed in user mode.|
|11|Environment call from M-mode|ecall executed in machine mode.|

Interrupts: 

|Cause code|Interrupt|Interrupt trigger|
|---|---|---|
|0|User software interrupt|(mip.usip && mie.usie) && mstatus.mie|
|3|Machine software interrupt|(mip.msip && mie.msie) && mstatus.mie|
|4|User timer interrupt|(mip.utip && mie.utie) && mstatus.mie|
|7|Machine timer interrupt|(mip.mtip && mie.mtie) && mstatus.mie|
|8|User external interrupt|(mip.ueip && mie.ueie) && mstatus.mie|
|11|Machine external interrupt|(mip.meip && mie.meie) && mstatus.mie|


Comments that begin with the '#' character and continue until the end of the line can be added after each command. It is allowed to have empty lines or lines that solely contain comments.
At the start, all general-purpose registers of the processor including the PC should hold a value of 0. Additionally, the memory should seem to have all its locations initialized with 0. 
