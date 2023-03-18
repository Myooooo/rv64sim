#ifndef INSTRUCTION_H
#define INSTRUCTION_H

/* ****************************************************************
   RISC-V Instruction Set Simulator
   Instruction Enumeration
**************************************************************** */

namespace RV64I
{
    enum Ins
    {
        ins_default,
        ins_lui,
        ins_auipc,
        ins_jal,
        ins_jalr,
        ins_beq,
        ins_bne,
        ins_blt,
        ins_bge,
        ins_bltu,
        ins_bgeu,
        ins_lb,
        ins_lh,
        ins_lw,
        ins_lbu,
        ins_lhu,
        ins_sb,
        ins_sh,
        ins_sw,
        ins_addi,
        ins_slti,
        ins_sltiu,
        ins_xori,
        ins_ori,
        ins_andi,
        ins_slli,
        ins_srli,
        ins_srai,
        ins_add,
        ins_sub,
        ins_sll,
        ins_slt,
        ins_sltu,
        ins_xor,
        ins_srl,
        ins_sra,
        ins_or,
        ins_and,
        ins_fence,
        ins_ecall,
        ins_ebreak,
        ins_lwu,
        ins_ld,
        ins_sd,
        ins_addiw,
        ins_slliw,
        ins_srliw,
        ins_sraiw,
        ins_addw,
        ins_subw,
        ins_sllw,
        ins_srlw,
        ins_sraw,
        ins_mret,
        ins_csrrw,
        ins_csrrs,
        ins_csrrc,
        ins_csrrwi,
        ins_csrrsi,
        ins_csrrci
    };
}

#endif