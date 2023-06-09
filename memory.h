#ifndef MEMORY_H
#define MEMORY_H

/* ****************************************************************
   RISC-V Instruction Set Simulator
   Class for memory
**************************************************************** */

#include <vector>
#include <unordered_map>

using namespace std;

class memory {

 private:

  // TODO: Add private members here
  bool verbose;
  
  // assign block size for memory blocks
  const int block_size = 1024;

  // memory blocks
  unordered_map<int,unordered_map<uint64_t,uint64_t>*> mem;

 public:

  // Constructor
  memory(bool verbose);

  // Read a doubleword of data from a doubleword-aligned address.
  // If the address is not a multiple of 8, it is rounded down to a multiple of 8.
  uint64_t read_doubleword (uint64_t address);

  // Write a doubleword of data to a doubleword-aligned address.
  // If the address is not a multiple of 8, it is rounded down to a multiple of 8.
  // The mask contains 1s for bytes to be updated and 0s for bytes that are to be unchanged.
  void write_doubleword (uint64_t address, uint64_t data, uint64_t mask);

  // Load a hex image file and provide the start address for execution from the file in start_address.
  // Return true if the file was read without error, or false otherwise.
  bool load_file(string file_name, uint64_t &start_address);

  // destructor
  ~memory();

};

#endif
