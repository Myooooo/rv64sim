#ifndef PROCESSOR_H
#define PROCESSOR_H

/* ****************************************************************
   RISC-V Instruction Set Simulator
   Class for processor
**************************************************************** */

#include "memory.h"
#include "Decoder.h"

using namespace std;

class processor {

 private:

  // TODO: Add private members here

  // input arguments
  memory* main_memory;
  bool verbose;
  bool stage2;

  // processor properties
  uint64_t pc;
  uint64_t breakpoint;
  bool bp_enabled;
  uint64_t ins_count;
  uint64_t registers[32];

  // instruction decoder
  Decoder* decoder;

  // stage 2 variables
  unsigned int prv;
  unordered_map<unsigned int,uint64_t> csrs;

 public:

  // Consructor
  processor(memory* main_memory, bool verbose, bool stage2);

  // Display PC value
  void show_pc();

  // Set PC to new value
  void set_pc(uint64_t new_pc);

  // Display register value
  void show_reg(unsigned int reg_num);

  // Set register to new value
  void set_reg(unsigned int reg_num, uint64_t new_value);

  // Execute a number of instructions
  void execute(unsigned int num, bool breakpoint_check);

  // Clear breakpoint
  void clear_breakpoint();

  // Set breakpoint at an address
  void set_breakpoint(uint64_t address);

  // Show privilege level
  // Empty implementation for stage 1, required for stage 2
  void show_prv();

  // Set privilege level
  // Empty implementation for stage 1, required for stage 2
  void set_prv(unsigned int prv_num);

  // Display CSR value
  // Empty implementation for stage 1, required for stage 2
  void show_csr(unsigned int csr_num);

  // Set CSR to new value
  // Empty implementation for stage 1, required for stage 2
  void set_csr(unsigned int csr_num, uint64_t new_value);

  uint64_t get_instruction_count();

  // Used for Postgraduate assignment. Undergraduate assignment can return 0.
  uint64_t get_cycle_count();

  // execute current instruction
  void executeIns();

  // sign extend 12-bit to 32-bit
  uint32_t sext_12_32(uint32_t val);

  // sign extend 20-bit to 32-bit
  uint32_t sext_20_32(uint32_t val);

  // sign extend 8-bit to 64-bit
  uint64_t sext_8_64(uint64_t val);

  // sign extend 16-bit to 64-bit
  uint64_t sext_16_64(uint64_t val);

  // sign extend 32-bit to 64-bit
  uint64_t sext_32_64(uint64_t val);

  // performed signed comparison of two 64-bit values
  bool signedComp(uint64_t a, uint64_t b);

  // initialise control and status registers
  void initCSRs();

  // return from machine trap
  void except(int cause);

  // interrupt routine
  void interrupt(int cause);

  // destructor
  ~processor();

};

#endif
