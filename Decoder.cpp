/* ****************************************************************
   RISC-V Instruction Set Simulator
   Class for instruction decoder
**************************************************************** */

#include "Decoder.h"
#include <iostream>
#include <iomanip>

// default constructor
Decoder::Decoder(bool verbose)
{
    // verbose
    this->verbose = verbose;

    // instruction parts
    ins = 0;
    opcode = 0;
    rd = 0;
    rs1 = 0;
    rs2 = 0;
    funct3 = 0;
    funct7 = 0;
    imm = 0;

    // instruction properties
    code = ins_default;
    type = '0';
    insNames = 
    {
        "default",
        "lui",
        "auipc",
        "jal",
        "jalr",
        "beq",
        "bne",
        "blt",
        "bge",
        "bltu",
        "bgeu",
        "lb",
        "lh",
        "lw",
        "lbu",
        "lhu",
        "sb",
        "sh",
        "sw",
        "addi",
        "slti",
        "sltiu",
        "xori",
        "ori",
        "andi",
        "slli",
        "srli",
        "srai",
        "add",
        "sub",
        "sll",
        "slt",
        "sltu",
        "xor",
        "srl",
        "sra",
        "or",
        "and",
        "fence",
        "ecall",
        "ebreak",
        "lwu",
        "ld",
        "sd",
        "addiw",
        "slliw",
        "srliw",
        "sraiw",
        "addw",
        "subw",
        "sllw",
        "srlw",
        "sraw",
        "mret",
        "csrrw",
        "csrrs",
        "csrrc",
        "csrrwi",
        "csrrsi",
        "csrrci"
    };
}

// decode current instruction and store parts into variables
void Decoder::decodeIns(uint32_t ins)
{
    // set current instructio
    this->ins = ins;

    // opcode = ins[6:0]
    opcode = ins & 0x7f;

    // funct3 = ins[14:12]
    funct3 = (ins >> 12) & 0x07;

    // funct7 = ins[31:25]
    funct7 = (ins >> 25) & 0x7f;

    switch(opcode)
    {
        // 0b0000011 => 3
        case 3:
            switch(funct3)
            {
                // 0b000 => 0
                case 0:
                    code = ins_lb;
                    type = 'I';
                    decodeIType();
                    break;
                // 0b001 => 1
                case 1:
                    code = ins_lh;
                    type = 'I';
                    decodeIType();
                    break;
                // 0b010 => 2
                case 2:
                    code = ins_lw;
                    type = 'I';
                    decodeIType();
                    break;
                // 0b011 => 3
                case 3:
                    code = ins_ld;
                    type = 'I';
                    decodeIType();
                    break;
                // 0b100 => 4
                case 4:
                    code = ins_lbu;
                    type = 'I';
                    decodeIType();
                    break;
                // 0b101 => 5
                case 5:
                    code = ins_lhu;
                    type = 'I';
                    decodeIType();
                    break;
                // 0b110 => 6
                case 6:
                    code = ins_lwu;
                    type = 'I';
                    decodeIType();
                    break;
                default:
                    break;
            }
            break;
        // 0b0001111 = 15
        case 15:
            code = ins_fence;
            break;
        // 0b0010011 => 19
        case 19:
            switch(funct3)
            {
                // 0b000 => 0
                case 0:
                    code = ins_addi;
                    type = 'I';
                    decodeIType();
                    break;
                // 0b001 => 1
                case 1:
                    code = ins_slli;
                    type = 'R';
                    decodeRType();
                    break;
                // 0b010 => 2
                case 2:
                    code = ins_slti;
                    type = 'I';
                    decodeIType();
                    break;
                // 0b011 => 3
                case 3:
                    code = ins_sltiu;
                    type = 'I';
                    decodeIType();
                    break;
                // 0b100 => 4
                case 4:
                    code = ins_xori;
                    type = 'I';
                    decodeIType();
                    break;
                // 0b101 => 5
                case 5:
                    code = ins_srai;
                    if ((funct7 >> 1) == 0) code = ins_srli;
                    type = 'R';
                    decodeRType();
                    break;
                // 0b110 => 6
                case 6:
                    code = ins_ori;
                    type = 'I';
                    decodeIType();
                    break;
                // 0b111 => 7
                case 7:
                    code = ins_andi;
                    type = 'I';
                    decodeIType();
                    break;
                default:
                    break;
            }
            break;
        // 0b0010111 = 23
        case 23:
            code = ins_auipc;
            type = 'U';
            decodeUType();
            break;
        // 0b0011011 => 27
        case 27: 
            switch(funct3)
            {
                // 0b000 => 0
                case 0:
                    code = ins_addiw;
                    type = 'I';
                    decodeIType();
                    break;
                // 0b001 => 1
                case 1:
                    code = ins_slliw;
                    type = 'R';
                    decodeRType();
                    break;
                // 0b101 => 5
                case 5:
                    code = ins_sraiw;
                    if (funct7 == 0) code = ins_srliw;
                    type = 'R';
                    decodeRType();
                    break;
                default:
                    break;
            }
            break;
        // 0b0100011 => 35
        case 35: 
            switch(funct3)
            {
                // 0b000 => 0
                case 0:
                    code = ins_sb;
                    type = 'S';
                    decodeSType();
                    break;
                // 0b001 => 1
                case 1:
                    code = ins_sh;
                    type = 'S';
                    decodeSType();
                    break;
                // 0b010 => 2
                case 2:
                    code = ins_sw;
                    type = 'S';
                    decodeSType();
                    break;
                // 0b011 => 3
                case 3:
                    code = ins_sd;
                    type = 'S';
                    decodeSType();
                    break;
                default:
                    break;
            }
            break;
        // 0b0110011 = 51
        case 51:
            switch(funct3)
            {
                // 0b000 => 0
                case 0:
                    code = ins_sub;
                    if (funct7 == 0) code = ins_add;
                    type = 'R';
                    decodeRType();
                    break;
                // 0b001 => 1
                case 1:
                    code = ins_sll;
                    type = 'R';
                    decodeRType();
                    break;
                // 0b010 => 2
                case 2:
                    code = ins_slt;
                    type = 'R';
                    decodeRType();
                    break;
                // 0b011 => 3
                case 3:
                    code = ins_sltu;
                    type = 'R';
                    decodeRType();
                    break;
                // 0b100 => 4
                case 4:
                    code = ins_xor;
                    type = 'R';
                    decodeRType();
                    break;
                // 0b101 => 5
                case 5:
                    code = ins_sra;
                    if (funct7 == 0) code = ins_srl;
                    type = 'R';
                    decodeRType();
                    break;
                // 0b110 => 6
                case 6:
                    code = ins_or;
                    type = 'R';
                    decodeRType();
                    break;
                // 0b111 => 7
                case 7:
                    code = ins_and;
                    type = 'R';
                    decodeRType();
                    break;
                default:
                    break;
            }
            break;
        // 0b0110111 = 55
        case 55:
            code = ins_lui;
            type = 'U';
            decodeUType();
            break;
        // 0b0111011 => 59
        case 59: 
            switch(funct3)
            {
                // 0b000 => 0
                case 0:
                    code = ins_subw;
                    if (funct7 == 0) code = ins_addw;
                    type = 'R';
                    decodeRType();
                    break;
                // 0b001 => 1
                case 1:
                    code = ins_sllw;
                    type = 'R';
                    decodeRType();
                    break;
                // 0b101 => 5
                case 5:
                    code = ins_sraw;
                    if (funct7 == 0) code = ins_srlw;
                    type = 'R';
                    decodeRType();
                    break;
                default:
                    break;
            }
            break;
        // 0b1100011 => 99
        case 99:
            switch(funct3)
            {
                // 0b000 => 0
                case 0:
                    code = ins_beq;
                    type = 'B';
                    decodeBType();
                    break;
                // 0b001 => 1
                case 1:
                    code = ins_bne;
                    type = 'B';
                    decodeBType();
                    break;
                // 0b100 => 4
                case 4:
                    code = ins_blt;
                    type = 'B';
                    decodeBType();
                    break;
                // 0b101 => 5
                case 5:
                    code = ins_bge;
                    type = 'B';
                    decodeBType();
                    break;
                // 0b110 => 6
                case 6:
                    code = ins_bltu;
                    type = 'B';
                    decodeBType();
                    break;
                // 0b111 => 7
                case 7:
                    code = ins_bgeu;
                    type = 'B';
                    decodeBType();
                    break;
                default:
                    break;
            }
            break;
        // 0b1100111 => 103
        case 103:
            code = ins_jalr;
            type = 'I';
            decodeIType();
            break;
        // 0b1101111 => 111
        case 111:
            code = ins_jal;
            type = 'J';
            decodeJType();
            break;
        // 0b01110011 = 115
        case 115:
            switch(funct3)
            {
                // 0b000 => 0
                case 0:
                    code = ins_ebreak;
                    if (ins >> 20 == 0) code = ins_ecall;
                    if (ins >> 20 == 770) code = ins_mret;
                    break;
                // 0b001 => 1
                case 1:
                    code = ins_csrrw;
                    type = 'I';
                    decodeIType();
                    break;
                // 0b010 => 2
                case 2:
                    code = ins_csrrs;
                    type = 'I';
                    decodeIType();
                    break;
                // 0b011 => 3
                case 3:
                    code = ins_csrrc;
                    type = 'I';
                    decodeIType();
                    break;
                // 0b101 => 5
                case 5:
                    code = ins_csrrwi;
                    type = 'I';
                    decodeIType();
                    break;
                // 0b110 => 6
                case 6:
                    code = ins_csrrsi;
                    type = 'I';
                    decodeIType();
                    break;
                // 0b111 => 7
                case 7:
                    code = ins_csrrci;
                    type = 'I';
                    decodeIType();
                    break;
                default:
                    break;
            }
            break;
        default: 
            resetIns();
            break;
    }
}

// decode R-type instructions
void Decoder::decodeRType()
{
    // rd = ins[11:7]
    rd = (ins >> 7) & 0x1f;

    // rs1 = ins[19:15]
    rs1 = (ins >> 15) & 0x1f;

    // rs2 = ins[24:20]
    rs2 = (ins >> 20) & 0x1f;

    if(verbose)
    {
        cout << insNames[code];
        cout << ": type = " << type;
        cout << ", rd = " << dec << (int) rd;
        cout << ", rs1 = " << dec << (int) rs1;
        cout << ", rs2 = " << dec << (int) rs2 << endl;;
    }
}

// decode I-type instructions
void Decoder::decodeIType()
{
    // rd = ins[11:7]
    rd = (ins >> 7) & 0x1f;

    // rs1 = ins[19:15]
    rs1 = (ins >> 15) & 0x1f;

    // imm = ins[31:20]
    imm = (ins >> 20) & 0xfff;

    if(verbose)
    {
        cout << insNames[code];
        cout << ": type = " << type;
        cout << ", rd = " << dec << (int) rd;
        cout << ", rs1 = " << dec << (int) rs1;
        cout << ", imm = " << setw(16) << setfill('0') << hex << imm << endl;
    }
}

// decode S-type instructions
void Decoder::decodeSType()
{
    // rs1 = ins[19:15]
    rs1 = (ins >> 15) & 0x1f;

    // rs2 = ins[24:20]
    rs2 = (ins >> 20) & 0x1f;

    // imm = ins[31:25,11:7]
    imm = (ins >> 7) & 0x1f;
    imm += ((ins >> 25) & 0x7f) << 5;

    if(verbose)
    {
        cout << insNames[code];
        cout << ": type = " << type;
        cout << ", rs1 = " << dec << (int) rs1;
        cout << ", rs2 = " << dec << (int) rs2;
        cout << ", imm = " << setw(16) << setfill('0') << hex << imm << endl;
    }
}

// decode B-type instructions
void Decoder::decodeBType()
{
    // rs1 = ins[19:15]
    rs1 = (ins >> 15) & 0x1f;

    // rs2 = ins[24:20]
    rs2 = (ins >> 20) & 0x1f;

    // imm = ins[31,7,30:25,11:8]
    imm = (ins >> 8) & 0xf;
    imm += ((ins >> 25) & 0x3f) << 4;
    imm += ((ins >> 7) & 0x1) << 10;
    imm += ((ins >> 31) & 0x1) << 11;

    if(verbose)
    {
        cout << insNames[code];
        cout << ": type = " << type;
        cout << ", rs1 = " << dec << (int) rs1;
        cout << ", rs2 = " << dec << (int) rs2;
        cout << ", imm = " << setw(16) << setfill('0') << hex << imm << endl;
    }
}

// decode U-type instructions
void Decoder::decodeUType()
{
    // rd = ins[11:7]
    rd = (ins >> 7) & 0x1f;

    // imm = ins[31:12]
    imm = (ins >> 12) & 0xfffff;

    if(verbose)
    {
        cout << insNames[code];
        cout << ": type = " << type;
        cout << ", rd = " << dec << (int) rd;
        cout << ", imm = " << setw(16) << setfill('0') << hex << imm << endl;
    }
}

// decode J-type instructions
void Decoder::decodeJType()
{
    // rd = ins[11:7]
    rd = (ins >> 7) & 0x1f;

    // imm = ins[31,19:12,20,30:21]
    imm = (ins >> 21) & 0x3ff;
    imm += ((ins >> 20) & 0x1) << 10;
    imm += ((ins >> 12) & 0xff) << 11;
    imm += ((ins >> 31) & 0x1) << 19;

    if(verbose)
    {
        cout << insNames[code];
        cout << ": type = " << type;
        cout << ", rd = " << dec << (int) rd;
        cout << ", imm = " << setw(16) << setfill('0') << hex << imm << endl;
    }
}

void Decoder::resetIns()
{
    // instruction parts
    ins = 0;
    opcode = 0;
    rd = 0;
    rs1 = 0;
    rs2 = 0;
    funct3 = 0;
    funct7 = 0;
    imm = 0;

    // instruction properties
    code = ins_default;
    type = '0';
}

// return current instruction
uint32_t Decoder::getIns()
{
    return ins;
}

// return current opcode
uint8_t Decoder::getOpcode()
{
    return opcode;
}

// return current dest register
uint8_t Decoder::getRd()
{
    return rd;
}

// return current source register 1
uint8_t Decoder::getRs1()
{
    return rs1;
}

// return current source register 1
uint8_t Decoder::getRs2()
{
    return rs2;
}

// return current funct3
uint8_t Decoder::getFunct3()
{
    return funct3;
}

// return current funct7
uint8_t Decoder::getFunct7()
{
    return funct7;
}

// return current immediate
uint32_t Decoder::getImm()
{
    return imm;
}

// return current instruction code
Ins Decoder::getInsCode()
{
    return code;
}

// return current instruction name string
string Decoder::getInsName()
{
    return insNames[code];
}

// return current instruction type (capital letter)
char Decoder::getInsType()
{
    return type;
}

// destructor
Decoder::~Decoder()
{

}