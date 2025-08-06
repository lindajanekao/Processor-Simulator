💻 Processor Simulator: Visualizing 8-Bit CPU Execution

Welcome! Our design provides an intuitive way to understand how an 8-bit processor operates by showing real-time instruction execution — ideal for anyone exploring computer architecture.

💡Key Features:

-Memory: 8-cell array (4 for instructions, 4 for data)

-Registers: Four 8-bit registers (R0–R3) for storing values or memory addresses

-ALU: Performs arithmetic/logical operations with Z/N status flags

-Program Counter (PC): Tracks instruction flow

-Editor & Disassembly Screens: Input instructions via DE1 board switches and watch step-by-step visual execution


🔍Supports 11 Instructions:
Includes ADD, SUB, LOAD, STORE, branching, shifts, and immediate loads. The disassembly view clearly shows how each instruction interacts with memory, registers, the ALU, and the PC.

//user instruction:
/* press ADD, button_pressed=1: Ra,Rb*/

/* press SUB, button_pressed=2: Ra,Rb */

/* press NAND, button_pressed=3: Ra,Rb */

/* press ORi, button_pressed=4: imm */

/* press LOAD, button_pressed=5: Ra,Rb */

/* press STORE, button_pressed=6: Ra,Rb */

/* press BNZ, button_pressed=7:imm */

/* press BPZ, button_pressed=8:imm */

/* press shift, button_pressed=9:Ra, 0/1, imm(<4) */

/* press loadiR, button_pressed=10: Ra, imm */

/* press loadiM, button_pressed=11: memory address, imm */

