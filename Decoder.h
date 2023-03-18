#ifndef DECODER_H
#define DECODER_H

/* ****************************************************************
   RISC-V Instruction Set Simulator
   Class for instruction decoder
**************************************************************** */

#include <cstdint>
#include <vector>
#include <string>
#include "Instruction.h"

using namespace std;
using namespace RV64I;

class Decoder {

    private:

        // verbose
        bool verbose;

        // instruction parts
        uint32_t ins;
        uint8_t opcode;
        uint8_t rd;
        uint8_t rs1;
        uint8_t rs2;
        uint8_t funct3;
        uint8_t funct7;
        uint32_t imm;

        // instruction properties
        Ins code;
        char type;
        vector<string> insNames;

    public:

        // Consructor
        Decoder(bool verbose);

        // decode current instruction and store parts into variables
        void decodeIns(uint32_t ins);

        // decode current instruction according to type
        void decodeRType();
        void decodeIType();
        void decodeSType();
        void decodeBType();
        void decodeUType();
        void decodeJType();

        // reset instructions and parts
        void resetIns();

        // return current instruction
        uint32_t getIns();

        // return current opcode
        uint8_t getOpcode();

        // return current dest register
        uint8_t getRd();

        // return current source register 1
        uint8_t getRs1();

        // return current source register 1
        uint8_t getRs2();

        // return current funct3
        uint8_t getFunct3();

        // return current funct7
        uint8_t getFunct7();

        // return current immediate
        uint32_t getImm();

        // return current instruction code
        Ins getInsCode();

        // return current instruction name string
        string getInsName();

        // return current instruction type (capital letter)
        char getInsType();

        // destructor
        ~Decoder();
};

#endif