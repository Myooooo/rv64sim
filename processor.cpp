/* ****************************************************************
   RISC-V Instruction Set Simulator
   Processor Simulation
**************************************************************** */

#include <iostream>
#include <iomanip>
#include "processor.h"

// Constructor
processor::processor(memory* main_memory, bool verbose, bool stage2)
{
    // copy input arguments
    this->main_memory = main_memory;
    this->verbose = verbose;
    this->stage2 = stage2;

    // initialise properties
    pc = 0;
    breakpoint = 0;
    bp_enabled = false;
    ins_count = 0;

    // initialise decoder
    decoder = new Decoder(verbose);

    // initialise register values to zero
    for (int i = 0; i < 32; i++)
    {
        registers[i] = 0;
    }

    // initialise stage 2 variables
    prv = 3;            // privilege level default 3
    initCSRs();         // initialise control and status registers
}

// Display PC value
void processor::show_pc()
{
    cout << setw(16) << setfill('0') << hex << pc << endl;
}

// Set PC to new value
void processor::set_pc(uint64_t new_pc)
{
    pc = new_pc;
    if (verbose) cout << "PC set to " << setw(16) << setfill('0') << hex << pc << endl;
}

// Display register value
void processor::show_reg(unsigned int reg_num)
{
    cout << setw(16) << setfill('0') << hex << registers[reg_num] << endl;
}

// Set register to new value
void processor::set_reg(unsigned int reg_num, uint64_t new_value)
{
    // ignore x0
    if (reg_num == 0) return;
    registers[reg_num] = new_value;
}

// Execute a number of instructions
void processor::execute(unsigned int num, bool breakpoint_check)
{
    for (unsigned int i = 0; i < num; i++)
    {
        // check for pc alignment
        if (pc % 4 != 0)
        {
            except(0);
        }
        else
        {
            // check for interrupt, orderred by priority
            // mstatus.mie == 1 or in user mode
            if(((csrs[0x300] >> 3) & 0x1) == 1 || prv == 0)
            {
                if(((csrs[0x344] >> 11) & 0x1) == 1 && ((csrs[0x304] >> 11) & 0x1) == 1)
                {
                    // machine external interrupt
                    // (mip.meip && mie.meie) && mstatus.mie
                    interrupt(11);
                }
                else if(((csrs[0x344] >> 3) & 0x1) == 1 && ((csrs[0x304] >> 3) & 0x1) == 1)
                {
                    // machine software interrupt
                    // (mip.msip && mie.msie) && mstatus.mie
                    interrupt(3);
                }
                else if(((csrs[0x344] >> 7) & 0x1) == 1 && ((csrs[0x304] >> 7) & 0x1) == 1)
                {
                    // machine timer interrupt
                    // (mip.mtip && mie.mtie) && mstatus.mie
                    interrupt(7);
                }
                else if(((csrs[0x344] >> 8) & 0x1) == 1 && ((csrs[0x304] >> 8) & 0x1) == 1)
                {
                    // user external interrupt
                    // (mip.ueip && mie.ueie) && mstatus.mie
                    interrupt(8);
                }
                else if((csrs[0x344] & 0x1) == 1 && (csrs[0x304] & 0x1) == 1)
                {
                    // user software interrupt
                    // (mip.usip && mie.usie) && mstatus.mie
                    interrupt(0);
                }  
                else if(((csrs[0x344] >> 4) & 0x1) == 1 && ((csrs[0x304] >> 4) & 0x1) == 1)
                {
                    // user timer interrupt
                    // (mip.utip && mie.utie) && mstatus.mie
                    interrupt(4);
                }
            }

            // fetch instruction from memory
            uint64_t data = main_memory->read_doubleword(pc);
            uint32_t ins;
            if (pc % 8 != 0)
            {
                // first half of data
                ins = (data >> 32) & 0xffffffff;
            }
            else
            {
                // second half of data
                ins = data & 0xffffffff;
            }

            if (verbose)
            {
                cout << "Fetch: pc = " << setw(16) << setfill('0') << hex << pc;
                cout << ", ins = " << setw(8) << setfill('0') << hex << ins << endl;
            }
            
            // decode and execute instuction
            if (breakpoint_check && (pc == breakpoint) && bp_enabled)
            {
                cout << "Breakpoint reached at " << setw(16) << setfill('0') << hex << breakpoint << endl;
                break;
            }
            else
            {
                // decode
                decoder->decodeIns(ins);

                // execute
                executeIns();

                // increment instruction count
                ins_count ++;
            }
            
            // cout << "x5: " << setw(16) << setfill('0') << registers[5];
            // cout << ", x6: " << setw(16) << setfill('0') << registers[6];
            // cout << ", x8: " << setw(16) << setfill('0') << registers[8];
            // cout << ", x15: " << setw(16) << setfill('0') << registers[15] << endl;
        }
    }
}

// Clear breakpoint
void processor::clear_breakpoint()
{
    breakpoint = 0;
    bp_enabled = false;
    if (verbose) cout << "Breakpoint cleared" << endl;
}

// Set breakpoint at an address
void processor::set_breakpoint(uint64_t address)
{
    breakpoint = address - (address % 4);
    bp_enabled = true;
    if (verbose) cout << "Breakpoint set at " << setw(16) << setfill('0') << hex << breakpoint << endl;
}

// Show privilege level
// Empty implementation for stage 1, required for stage 2
void processor::show_prv()
{
    string prv_str;

    switch(prv)
    {
        case 0:
            prv_str = "user";
            break;
        case 3:
            prv_str = "machine";
            break;
        default:
            prv_str = "machine";
    }

    cout << prv << " (" << prv_str << ")" << endl;
}

// Set privilege level
// Empty implementation for stage 1, required for stage 2
void processor::set_prv(unsigned int prv_num)
{
    prv = prv_num;
}

// Display CSR value
// Empty implementation for stage 1, required for stage 2
void processor::show_csr(unsigned int csr_num)
{
    if(csrs.find(csr_num) == csrs.end())
    {
        // invalid csr
        cout << "Illegal CSR number" << endl;
    }
    else
    {
        // valid csr
        cout << setw(16) << setfill('0') << hex << csrs[csr_num] << endl;
    }
}

// Set CSR to new value
// Empty implementation for stage 1, required for stage 2
void processor::set_csr(unsigned int csr_num, uint64_t new_value)
{
    // invalid csr number
    if(csrs.find(csr_num) == csrs.end()) return;

    // read-only csrs
    if(csr_num == 0xf11 || csr_num == 0xf12 || csr_num == 0xf13 || csr_num == 0xf14)
    {
        cout<<"Illegal write to read-only CSR"<<endl;
        return;
    }
    
    // writable csrs
    switch(csr_num)
    {
        case 0x300:
            // mstatus: only mie, mpie, mpp implemented
            new_value &= 0x1888;
            new_value |= 0x200000000;
            break;
        case 0x301:
            // misa: all bits fixed
            new_value = 0x8000000000100100;
            break;
        case 0x304:
            // mie: only usie, msie, utie, mtie, ueie, meie implemented
            new_value &= 0x999;
            break;
        case 0x305:
            // mtvec: bit 1 fixed at 0, if vectored, bits 7:2 also fixed at 0
            if((new_value & 0x1) == 0)
            {
                // direct Mode
                new_value &= 0xfffffffffffffffc;
            }
            else
            {
                // vectored Mode
                new_value &= 0xffffffffffffff01;
            }
            break;
        case 0x340:
            // mscratch: all bits writable
            break;
        case 0x341:
            // mepc: bit 1:0 fixed at 0
            new_value &= 0xfffffffffffffffc;
            break;
        case 0x342:
            // mcause: only Interrupt bit and 4-bit cause
            new_value &= 0x800000000000000f;
            break;
        case 0x343:
            // mtval: all bits writable
            break;
        case 0x344:
            // mip: only usip, msip, utip, mtip, ueip, meip implemented
            new_value &= 0x999;
            break;
        default:
            break;
    }

    csrs[csr_num] = new_value;
}

// returns the number of executed instructions
uint64_t processor::get_instruction_count()
{
    return ins_count;
}

// Used for Postgraduate assignment. Undergraduate assignment can return 0.
uint64_t processor::get_cycle_count()
{
    return 0;
}

// execute current instruction
void processor::executeIns()
{
    Ins insCode = decoder->getInsCode();
    uint64_t tmp = 0;
    uint64_t mask = 0;
    unsigned int csr_num;

    switch(insCode)
    {
        case ins_lui:
            set_reg(decoder->getRd(),sext_32_64(decoder->getImm() << 12));
            break;
        case ins_auipc:
            set_reg(decoder->getRd(),pc + sext_32_64(decoder->getImm() << 12));
            break;
        case ins_jal:
            set_reg(decoder->getRd(),pc + 4);
            pc += sext_32_64(sext_20_32(decoder->getImm()) << 1);
            if(pc % 2 != 0) pc -= (pc % 2);
            return;
        case ins_jalr:
            tmp = pc + 4;
            pc = sext_32_64(registers[decoder->getRs1()] + sext_12_32(decoder->getImm()));
            set_reg(decoder->getRd(),tmp);
            if(pc % 2 != 0) pc -= (pc % 2);
            return;
        case ins_beq:
            if(registers[decoder->getRs1()] == registers[decoder->getRs2()])
            {
                pc += sext_32_64(sext_12_32(decoder->getImm()) << 1);
                return;
            }
            break;
        case ins_bne:
            if(registers[decoder->getRs1()] != registers[decoder->getRs2()])
            {   
                pc += sext_32_64(sext_12_32(decoder->getImm()) << 1);
                return;
            }
            break;
        case ins_blt:
            if(signedComp(registers[decoder->getRs1()],registers[decoder->getRs2()]))
            {   
                pc += sext_32_64(sext_12_32(decoder->getImm()) << 1);
                return;
            }
            break;
        case ins_bge:
            if(!signedComp(registers[decoder->getRs1()],registers[decoder->getRs2()]))
            {   
                pc += sext_32_64(sext_12_32(decoder->getImm()) << 1);
                return;
            }
            break;
        case ins_bltu:
            if(registers[decoder->getRs1()] < registers[decoder->getRs2()])
            {   
                pc += sext_32_64(sext_12_32(decoder->getImm()) << 1);
                return;
            }
            break;
        case ins_bgeu:
            if(registers[decoder->getRs1()] >= registers[decoder->getRs2()])
            {   
                pc += sext_32_64(sext_12_32(decoder->getImm()) << 1);
                return;
            }
            break;
        case ins_lb:
            tmp = registers[decoder->getRs1()] + sext_32_64(sext_12_32(decoder->getImm()));
            set_reg(decoder->getRd(),sext_8_64(main_memory->read_doubleword(tmp) >> (tmp % 8 * 8)));
            break;
        case ins_lh:
            tmp = registers[decoder->getRs1()] + sext_32_64(sext_12_32(decoder->getImm()));
            if (tmp % 2 == 0)
            {
                set_reg(decoder->getRd(),sext_16_64(main_memory->read_doubleword(tmp) >> (tmp % 8 * 8)));
            }
            else
            {
                except(4);
            }
            break;
        case ins_lw:
            tmp = registers[decoder->getRs1()] + sext_32_64(sext_12_32(decoder->getImm()));
            if (tmp % 4 == 0)
            {
                set_reg(decoder->getRd(),sext_32_64(main_memory->read_doubleword(tmp) >> (tmp % 8 * 8)));
            }
            else
            {
                except(4);
            }
            break;
        case ins_lbu:
            tmp = registers[decoder->getRs1()] + sext_32_64(sext_12_32(decoder->getImm()));
            set_reg(decoder->getRd(),main_memory->read_doubleword(tmp) >> (tmp % 8 * 8) & 0xff);
            break;
        case ins_lhu:
            tmp = registers[decoder->getRs1()] + sext_32_64(sext_12_32(decoder->getImm()));
            if (tmp % 2 == 0)
            {
                set_reg(decoder->getRd(),main_memory->read_doubleword(tmp) >> (tmp % 8 * 8) & 0xffff);
            }
            else
            {
                except(4);
            }
            break;
        case ins_sb:
            tmp = registers[decoder->getRs1()] + sext_32_64(sext_12_32(decoder->getImm()));
            mask = 0xff;
            mask <<= (tmp % 8 * 8);
            main_memory->write_doubleword(tmp,registers[decoder->getRs2()] << (tmp % 8 * 8),mask);
            break;
        case ins_sh:
            tmp = registers[decoder->getRs1()] + sext_32_64(sext_12_32(decoder->getImm()));
            if (tmp % 2 == 0)
            {
                mask = 0xffff;
                mask <<= (tmp % 8 * 8);
                main_memory->write_doubleword(tmp,registers[decoder->getRs2()] << (tmp % 8 * 8),mask);
            }
            else
            {
                except(6);
            }
            break;
        case ins_sw:
            tmp = registers[decoder->getRs1()] + sext_32_64(sext_12_32(decoder->getImm()));
            if (tmp % 4 == 0)
            {
                mask = 0xffffffff;
                mask <<= (tmp % 8 * 8);
                main_memory->write_doubleword(tmp,registers[decoder->getRs2()] << (tmp % 8 * 8),mask);
            }
            else
            {
                except(6);
            }
            break;
        case ins_addi:
            set_reg(decoder->getRd(),registers[decoder->getRs1()] + sext_32_64(sext_12_32(decoder->getImm())));
            break;
        case ins_slti:
            if (signedComp(registers[decoder->getRs1()],sext_32_64(sext_12_32(decoder->getImm()))))
            {
                set_reg(decoder->getRd(),0x1);
            }
            else
            {
                set_reg(decoder->getRd(),0x0);
            }
            break;
        case ins_sltiu:
            if (registers[decoder->getRs1()] < sext_32_64(sext_12_32(decoder->getImm())))
            {
                set_reg(decoder->getRd(),0x1);
            }
            else
            {
                set_reg(decoder->getRd(),0x0);
            }
            break;
        case ins_xori:
            set_reg(decoder->getRd(),registers[decoder->getRs1()] ^ sext_32_64(sext_12_32(decoder->getImm())));
            break;
        case ins_ori:
            set_reg(decoder->getRd(),registers[decoder->getRs1()] | sext_32_64(sext_12_32(decoder->getImm())));
            break;
        case ins_andi:
            set_reg(decoder->getRd(),registers[decoder->getRs1()] & sext_32_64(sext_12_32(decoder->getImm())));
            break;
        case ins_slli:
            set_reg(decoder->getRd(),registers[decoder->getRs1()] << (((decoder->getFunct7() & 0x1) << 5) + decoder->getRs2()));
            break;
        case ins_srli:
            set_reg(decoder->getRd(),registers[decoder->getRs1()] >> (((decoder->getFunct7() & 0x1) << 5) + decoder->getRs2()));
            break;
        case ins_srai:
            mask = ((decoder->getFunct7() & 0x1) << 5) + decoder->getRs2();
            if ((registers[decoder->getRs1()] >> 63 == 1) && mask != 0)
            {
                tmp = 0xffffffffffffffff;
                tmp <<= (64 - mask);
            }
            else
            {
                tmp = 0x0;
            }
            set_reg(decoder->getRd(),(registers[decoder->getRs1()] >> mask) + tmp);
            break;
        case ins_add:
            set_reg(decoder->getRd(),registers[decoder->getRs1()] + registers[decoder->getRs2()]);
            break;
        case ins_sub:
            tmp = registers[decoder->getRs1()] - registers[decoder->getRs2()];
            set_reg(decoder->getRd(),tmp);
            break;
        case ins_sll:
            set_reg(decoder->getRd(),(registers[decoder->getRs1()] << (registers[decoder->getRs2()] & 0x3f)));
            break;
        case ins_slt:
            if(signedComp(registers[decoder->getRs1()],registers[decoder->getRs2()]))
            {
                set_reg(decoder->getRd(),0x1);
            }
            else
            {
                set_reg(decoder->getRd(),0x0);
            }
            break;
        case ins_sltu:
            if(registers[decoder->getRs1()] < registers[decoder->getRs2()])
            {
                set_reg(decoder->getRd(),0x1);
            }
            else
            {
                set_reg(decoder->getRd(),0x0);
            }
            break;
        case ins_xor:
            set_reg(decoder->getRd(),registers[decoder->getRs1()] ^ (registers[decoder->getRs2()]));
            break;
        case ins_srl:
            set_reg(decoder->getRd(),(registers[decoder->getRs1()] >> (registers[decoder->getRs2()] & 0x3f)));
            break;
        case ins_sra:
            mask = registers[decoder->getRs2()] & 0x3f;
            if ((registers[decoder->getRs1()] >> 63 == 1) && mask != 0)
            {
                tmp = 0xffffffffffffffff;
                tmp <<= (64 - mask);
            }
            else
            {
                tmp = 0x0;
            }
            set_reg(decoder->getRd(),(registers[decoder->getRs1()] >> mask) + tmp);
            break;
        case ins_or:
            set_reg(decoder->getRd(),registers[decoder->getRs1()] | (registers[decoder->getRs2()]));
            break;
        case ins_and:
            set_reg(decoder->getRd(),registers[decoder->getRs1()] & (registers[decoder->getRs2()]));
            break;
        case ins_fence:
            // no action
            break;
        case ins_ecall:
            if(prv == 0)
            {
                except(8);
            }
            else if(prv == 3)
            {
                except(11);
            }
            break;
        case ins_ebreak:
            if(verbose)
            {
                cout << "ebreak" << endl;
                cout << "Exception raised: cause = 3"
                    << ", pc = " << setw(16) << setfill('0') << hex << pc 
                    << ", val = " << setw(16) << setfill('0') << hex << decoder->getIns() << endl;
            }

            // store current pc into mepc
            set_csr(0x341,pc);

            // set pc to mtvec
            if((csrs[0x305] & 0x1) == 0)
            {
                // direct mode, all exceptions set pc to BASE
                pc = (csrs[0x305] & 0xfffffffffffffffc);
            }
            else
            {
                // vector mode, asynchronous interrupts set pc to BASE+4×cause
                pc = (csrs[0x305] & 0xfffffffffffffffc) + (4 * (csrs[0x342] & 0x0));
            }

            // set mpp
            if(prv == 3)
            {
                // machine
                // mpp = 3
                csrs[0x300] |= 0x1800;
            }
            else if(prv == 0)
            {
                // user
                // mpp = 0
                csrs[0x300] &= 0xffffffffffffe7ff;
            }

            // set mpie
            if(((csrs[0x300] >> 3) & 0x1) == 1)
            {
                // mpie = 1
                csrs[0x300] |= 0x80;
            }
            else
            {
                // mpie = 0
                csrs[0x300] &= 0xffffffffffffff7f;
            }

            // set mie = 0
            csrs[0x300] &= 0xfffffffffffffff7;

            // set mcause to 3
            set_csr(0x342,3);

            // set priviledge to machine
            prv = 3;
            
            // decrement instruction count
            ins_count --;

            // decrement pc
            pc -= 4;
            break;
        case ins_lwu:
            tmp = registers[decoder->getRs1()] + sext_32_64(sext_12_32(decoder->getImm()));
            if (tmp % 4 == 0)
            {
                set_reg(decoder->getRd(),main_memory->read_doubleword(tmp) >> (tmp % 8 * 8) & 0xffffffff);
            }
            else
            {
                except(4);
            }
            break;
        case ins_ld:
            tmp = registers[decoder->getRs1()] + sext_32_64(sext_12_32(decoder->getImm()));
            if (tmp % 8 == 0)
            {
                set_reg(decoder->getRd(),main_memory->read_doubleword(tmp));
            }
            else
            {
                except(4);
            }
            break;
        case ins_sd:
            tmp = registers[decoder->getRs1()] + sext_32_64(sext_12_32(decoder->getImm()));
            if (tmp % 8 == 0)
            {
                main_memory->write_doubleword(tmp,registers[decoder->getRs2()],0xffffffffffffffff);
            }
            else
            {
                except(6);
            }
            break;
        case ins_addiw:
            set_reg(decoder->getRd(),sext_32_64(registers[decoder->getRs1()] + sext_12_32(decoder->getImm())));
            break;
        case ins_slliw:
            set_reg(decoder->getRd(),sext_32_64(registers[decoder->getRs1()] << decoder->getRs2()));
            break;
        case ins_srliw:
            set_reg(decoder->getRd(),sext_32_64((registers[decoder->getRs1()] & 0xffffffff) >> decoder->getRs2()));
            break;
        case ins_sraiw:
            mask = decoder->getRs2();
            if ((((registers[decoder->getRs1()] >> 31) & 0x1) == 1) && mask != 0)
            {
                tmp = 0xffffffffffffffff;
                tmp <<= (64 - mask);
            }
            else
            {
                tmp = 0x0;
            }
            set_reg(decoder->getRd(),(sext_32_64(registers[decoder->getRs1()]) >> mask) + tmp);
            break;
        case ins_addw:
            set_reg(decoder->getRd(),sext_32_64(registers[decoder->getRs1()] + registers[decoder->getRs2()]));
            break;
        case ins_subw:
            set_reg(decoder->getRd(),sext_32_64(registers[decoder->getRs1()] - registers[decoder->getRs2()]));
            break;
        case ins_sllw:
            set_reg(decoder->getRd(),sext_32_64(registers[decoder->getRs1()] << (registers[decoder->getRs2()] & 0x1f)));
            break;
        case ins_srlw:
            set_reg(decoder->getRd(),sext_32_64((registers[decoder->getRs1()] & 0xffffffff) >> (registers[decoder->getRs2()] & 0x1f)));
            break;
        case ins_sraw:
            mask = registers[decoder->getRs2()] & 0x1f;
            if ((((registers[decoder->getRs1()] >> 31) & 0x1) == 1) && mask != 0)
            {
                tmp = 0xffffffffffffffff;
                tmp <<= (64 - mask);
            }
            else
            {
                tmp = 0x0;
            }
            set_reg(decoder->getRd(),(sext_32_64(registers[decoder->getRs1()]) >> mask) + tmp);
            break;
        case ins_mret:
            if(verbose) cout << "mret" << endl;
            if(prv == 0)
            {
                except(2);
            }
            else
            {
                // set pc to mepc
                pc = csrs[0x341] - 4;

                // set priviledge by mpp
                if(((csrs[0x300] >> 11) & 0x3) == 3)
                {
                    prv = 3;
                }
                else
                {
                    prv = 0;
                }

                // set mpp = 0
                csrs[0x300] &= 0xffffffffffffe7ff;

                // set mie to mpie
                if(((csrs[0x300] >> 7) & 0x1) == 1)
                {
                    // mie = 1
                    csrs[0x300] |= 0x8;
                }
                else
                {
                    // mie = 0
                    csrs[0x300] &= 0xfffffffffffffff7;
                }

                // set mpie = 0
                csrs[0x300] |= 0x80;
            }
            break;
        case ins_csrrw:
            csr_num = decoder->getImm();
            if(prv == 0 || csrs.find(csr_num) == csrs.end() || 
                (csr_num == 0xf11 && decoder->getRs1() != 0) || 
                (csr_num == 0xf12 && decoder->getRs1() != 0) || 
                (csr_num == 0xf13 && decoder->getRs1() != 0) || 
                (csr_num == 0xf14 && decoder->getRs1() != 0))
            {
                except(2);
            }
            else
            {
                tmp = registers[decoder->getRs1()];
                if(csr_num == 0x344) tmp &= 0x111;

                set_reg(decoder->getRd(),csrs[csr_num]);
                set_csr(csr_num,tmp);
            }
            break;
        case ins_csrrs:
            csr_num = decoder->getImm();
            if(prv == 0 || csrs.find(csr_num) == csrs.end() || 
                (csr_num == 0xf11 && decoder->getRs1() != 0) || 
                (csr_num == 0xf12 && decoder->getRs1() != 0) || 
                (csr_num == 0xf13 && decoder->getRs1() != 0) || 
                (csr_num == 0xf14 && decoder->getRs1() != 0))
            {
                except(2);
            }
            else
            {
                tmp = csrs[csr_num] | registers[decoder->getRs1()];
                if(csr_num == 0x344) tmp &= 0x111;

                set_reg(decoder->getRd(),csrs[csr_num]);
                if(decoder->getRs1() != 0) set_csr(csr_num,tmp);
            }
            break;
        case ins_csrrc:
            csr_num = decoder->getImm();
            if(prv == 0 || csrs.find(csr_num) == csrs.end() || 
                (csr_num == 0xf11 && decoder->getRs1() != 0) || 
                (csr_num == 0xf12 && decoder->getRs1() != 0) || 
                (csr_num == 0xf13 && decoder->getRs1() != 0) || 
                (csr_num == 0xf14 && decoder->getRs1() != 0))
            {
                except(2);
            }
            else
            {
                tmp = csrs[csr_num] & (~registers[decoder->getRs1()]);
                if(csr_num == 0x344) tmp &= 0x111;

                set_reg(decoder->getRd(),csrs[csr_num]);
                if(decoder->getRs1() != 0) set_csr(csr_num,tmp);
            }
            break;
        case ins_csrrwi:
            csr_num = decoder->getImm();
            if(prv == 0 || csrs.find(csr_num) == csrs.end() || 
                (csr_num == 0xf11 && decoder->getRs1() != 0) || 
                (csr_num == 0xf12 && decoder->getRs1() != 0) || 
                (csr_num == 0xf13 && decoder->getRs1() != 0) || 
                (csr_num == 0xf14 && decoder->getRs1() != 0))
            {
                except(2);
            }
            else
            {
                tmp = decoder->getRs1();
                if(csr_num == 0x344) tmp &= 0x111;

                set_reg(decoder->getRd(),csrs[csr_num]);
                set_csr(csr_num,tmp);
            }
            break;
        case ins_csrrsi:
            csr_num = decoder->getImm();
            if(prv == 0 || csrs.find(csr_num) == csrs.end() || 
                (csr_num == 0xf11 && decoder->getRs1() != 0) || 
                (csr_num == 0xf12 && decoder->getRs1() != 0) || 
                (csr_num == 0xf13 && decoder->getRs1() != 0) || 
                (csr_num == 0xf14 && decoder->getRs1() != 0))
            {
                except(2);
            }
            else
            {
                tmp = csrs[csr_num] | decoder->getRs1();
                if(csr_num == 0x344) tmp &= 0x111;

                set_reg(decoder->getRd(),csrs[csr_num]);
                if(decoder->getRs1() != 0) set_csr(csr_num,tmp);
            }
            break;
        case ins_csrrci:
            csr_num = decoder->getImm();
            if(prv == 0 || csrs.find(csr_num) == csrs.end() || 
                (csr_num == 0xf11 && decoder->getRs1() != 0) || 
                (csr_num == 0xf12 && decoder->getRs1() != 0) || 
                (csr_num == 0xf13 && decoder->getRs1() != 0) || 
                (csr_num == 0xf14 && decoder->getRs1() != 0))
            {
                except(2);
            }
            else
            {
                tmp = csrs[csr_num] & (~decoder->getRs1());
                if(csr_num == 0x344) tmp &= 0x111;

                set_reg(decoder->getRd(),csrs[csr_num]);
                if(decoder->getRs1() != 0) set_csr(csr_num,tmp);
            }
            break;
        default:
            break;
    }
    
    // increment program counter
    pc += 4;
}

// sign extend 12-bit to 32-bit
uint32_t processor::sext_12_32(uint32_t val)
{
    // clear upper bits
    val &= 0xfff;

    if ((val & 0x800) == 0x800)
    {
        return (val + 0xfffff000);
    }

    return val;
}

// sign extend 20-bit to 32-bit
uint32_t processor::sext_20_32(uint32_t val)
{
    // clear upper bits
    val &= 0xfffff;

    if ((val & 0x80000) == 0x80000)
    {
        return (val + 0xfff00000);
    }

    return val;
}

// sign extend 8-bit to 64-bit
uint64_t processor::sext_8_64(uint64_t val)
{
    // clear upper bits
    val &= 0xff;

    if ((val & 0x80) == 0x80)
    {
        return (val + 0xffffffffffffff00);
    }

    return val;
}

// sign extend 16-bit to 64-bit
uint64_t processor::sext_16_64(uint64_t val)
{
    // clear upper bits
    val &= 0xffff;

    if ((val & 0x8000) == 0x8000)
    {
        return (val + 0xffffffffffff0000);
    }

    return val;
}

// sign extend 32-bit to 64-bit
uint64_t processor::sext_32_64(uint64_t val)
{
    // clear upper bits
    val &= 0xffffffff;

    if ((val & 0x80000000) == 0x80000000)
    {
        return (val += 0xffffffff00000000);
    }

    return val;
}

// performed signed comparison of two 64-bit values
// a < b true, a >= b false
bool processor::signedComp(uint64_t a, uint64_t b)
{
    // compare sign
    uint64_t a_sign = a >> 63;
    uint64_t b_sign = b >> 63;

    if (a_sign == b_sign)
    {
        return a < b;
    }
    else
    {
        if (a_sign == 1) return true;
        return false;
    }
}

// initialise control and status registers
void processor::initCSRs()
{
    csrs.insert(make_pair(0xf11,0x0000000000000000));   // mvendorid
    csrs.insert(make_pair(0xf12,0x0000000000000000));   // marchid
    csrs.insert(make_pair(0xf13,0x2020020000000000));   // mimpid
    csrs.insert(make_pair(0xf14,0x0000000000000000));   // mhartid
    csrs.insert(make_pair(0x300,0x0000000200000000));   // mstatus
    csrs.insert(make_pair(0x301,0x8000000000100100));   // misa 
    csrs.insert(make_pair(0x304,0x0000000000000000));   // mie
    csrs.insert(make_pair(0x305,0x0000000000000000));   // mtvec
    csrs.insert(make_pair(0x340,0x0000000000000000));   // mscratch  
    csrs.insert(make_pair(0x341,0x0000000000000000));   // mepc
    csrs.insert(make_pair(0x342,0x0000000000000000));   // mcause
    csrs.insert(make_pair(0x343,0x0000000000000000));   // mtval
    csrs.insert(make_pair(0x344,0x0000000000000000));   // mip
}

// return from machine trap
void processor::except(int cause)
{
    if(verbose)
    {
        cout << "Exception raised: cause = " << cause
            << ", pc = " << setw(16) << setfill('0') << hex << pc 
            << ", val = " << setw(16) << setfill('0') << hex << decoder->getIns() << endl;
    }

    uint64_t old_pc = pc;

    // store old pc into mepc
    set_csr(0x341,old_pc);

    // set mcause to cause
    set_csr(0x342,cause);

    // set pc to mtvec
    if((csrs[0x305] & 0x1) == 0)
    {
        // direct mode, all exceptions set pc to BASE
        pc = (csrs[0x305] & 0xfffffffffffffffc);
    }
    else
    {
        // vector mode, asynchronous interrupts set pc to BASE+4×cause
        pc = (csrs[0x305] & 0xfffffffffffffffc) + (4 * (cause & 0x0));
    }

    // set mstatus by priviledge
    if(prv == 0)
    {
        // set mpp = 0
        csrs[0x300] &= 0xffffffffffffe7ff;

        // set mpie
        if(((csrs[0x300] >> 3) & 0x1) == 1)
        {
            // mpie = 1
            csrs[0x300] |= 0x80;
        }
        else
        {
            // mpie = 0
            csrs[0x300] &= 0xffffffffffffff7f;
        }

        // set mie = 0
        csrs[0x300] &= 0xfffffffffffffff7;
    }
    else if(prv == 3)
    {
        // set mpp = 3
        csrs[0x300] |= 0x1800;

        // mpie = 0
        csrs[0x300] &= 0xffffffffffffff7f;
    }

    switch(cause)
    {
        case 0:
            // instruction address misaligned
            ins_count ++;
            pc += 4;
            // set mtval to misaligned pc
            set_csr(0x343,old_pc);
            break;
        case 2:
            // illegal instruction
            // set mtval to instruction
            set_csr(0x343,decoder->getIns());
            break;
        case 4:
            // load address misaligned
            // set mtval to misaligned address
            set_csr(0x343,registers[decoder->getRs1()]);
            break;
        case 6:
            // store address misaligned
            // set mtval to misaligned address
            set_csr(0x343,registers[decoder->getRs1()]);
            break;
        case 8:
            // ecall in user mode
            set_csr(0x343,0);
            set_prv(3);
            break;
        case 11:
            // ecall in machine mode
            set_csr(0x343,0);
            break;
        default:
            break;
    }

    // decrement pc
    pc -= 4;

    // decrement instruction count
    ins_count --;
}

void processor::interrupt(int cause)
{
    if(verbose)
    {
        cout << "Interrupt taken: cause = " << cause
            << ", pc = " << setw(16) << setfill('0') << hex << pc << endl;
    }

    // set mpie = 1
    csrs[0x300] |= 0x80;

    // store pc into mepc
    set_csr(0x341,pc);

    // set mcause to cause with first bit enabled
    set_csr(0x342,0x8000000000000000 + cause);

    // set pc to mtvec
    if((csrs[0x305] & 0x1) == 0)
    {
        // direct mode, all exceptions set pc to BASE
        pc = (csrs[0x305] & 0xfffffffffffffffc);
    }
    else
    {
        // vector mode, asynchronous interrupts set pc to BASE+4×cause
        pc = (csrs[0x305] & 0xfffffffffffffffc) + (4 * cause);
    }

    if(prv == 0)
    {
        // user mode

        // switch to machine mode
        set_prv(3);

        // mie = 0
        if(((csrs[0x300] >> 3) & 0x1) == 0)
        {
            // set mpie = 0
            csrs[0x300] &= 0xffffffffffffff7f;
        }
    }
    else if(prv == 3)
    {
        // machine mode

        // set mpp = 3
        csrs[0x300] |= 0x1800;
    }

    // set mie = 0
    csrs[0x300] &= 0xfffffffffffffff7;

    switch(cause)
    {
        case 0:
            // User software interrupt
            break;
        case 3:
            // Machine software interrupt
            break;
        case 4:
            // User timer interrupt
            break;
        case 7:
            // Machine timer interrupt
            break;
        case 8:
            // User external interrupt
            break;
        case 11:
            // Machine external interrupt
            break;
        default:
            break;
    }
}

processor::~processor()
{
    delete decoder;
}