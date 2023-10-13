/*==================================================================================
 *Class - Execute
 *Author - Zach Walden
 *Created - 7/22/22
 *Last Changed - 2/18/23
 *Description - CPU Execution Stage. Decodes Instruction using a Function Pointer lookup table, Reads Operands, and Executes the instructions.
====================================================================================*/

/*
 * This program source code file is part of PROJECT_NAME
 *
 * Copyright (C) 2022 Zachary Walden zachary.walden@eagles.oc.edu
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you may find one here:
 * http://www.gnu.org/licenses/lgpl-3.0.en.html
 * or you may search the http://www.gnu.org website for the version 2 license,
 * or you may write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

#pragma once

#include "RegisterFile/RegisterFile.hpp"
#include "../ProgramCounter/ProgramCounter.hpp"
#include "../MMU/MMU.hpp"
#include <cstdint>

#define NUM_INSTRUCTIONS 256

enum AddressingMode
{
	Reg_Reg, Reg_Imm8, Reg_Simm8, Reg16_Simm8, Reg_Imm16, Reg_Mem, Reg_Reg16, Reg16_Reg16,Reg16Reg16simm8, Reg_None, Reg16_None, Mem_None, Mem_Reg, Reg16_Imm16, NONE_NONE
};

enum CpuOperation
{
	LD, ADD, ADC, SUB, SBC, CP, OR, XOR, AND, INC, DEC, RLA, RLCA, RRA, RRCA, RLC, RRC, RL, RR, SLA, SRA, SRL, SWAP, BIT, SET, RES, JR, JP, RET, PUSH, POP, DI, EI, CPL, CCF, DAA, SCF, HALT, NOP, STOP, ILLEGAL
};

struct GbInstruction
{
	GbInstruction(AddressingMode newMode, CpuOperation newOp, GbRegister opOne, GbRegister opTwo, bool (*func)(void*, GbInstruction, uint8_t*, uint8_t&))
	{
		mode = newMode;
		op = newOp;
		operandOne = opOne;
		operandTwo = opTwo;
		execFunction = func;
	}
	GbInstruction(AddressingMode newMode, CpuOperation newOp, GbRegister opOne, GbRegister opTwo, bool (*func)(void*, GbInstruction, uint8_t*, uint8_t&), GbFlag newCondition)
	{
		mode = newMode;
		op = newOp;
		operandOne = opOne;
		operandTwo = opTwo;
		condition = newCondition;
		execFunction = func;
	}
	AddressingMode mode;
	CpuOperation op;
	GbRegister operandOne,operandTwo;
	GbFlag condition;
	bool (*execFunction)(void*, GbInstruction, uint8_t*, uint8_t&);
};

class Execute
{
	//Attributes
public:

private:
	GbInstruction instDec[NUM_INSTRUCTIONS] = {
		//$0x
GbInstruction(AddressingMode::NONE_NONE, CpuOperation::NOP, GbRegister::A, GbRegister::A, Execute::nop),								//NOP: 1B, 4C, ----, TODO
GbInstruction(AddressingMode::Reg16_Imm16, CpuOperation::LD, GbRegister::BC, GbRegister::NONE, Execute::load),							//LD BC ,d16: 3B, 12C, ----, TODO
GbInstruction(AddressingMode::MemReg16_Reg, CpuOperation::LD, GbRegister::BC, GbRegister::A, Execute::load),							//LD (BC) ,A: 1B, 8C, ----, TODO
GbInstruction(AddressingMode::Reg16_None, CpuOperation::INC, GbRegister::BC, GbRegister::NONE, Execute::inc),							//INC BC: 1B, 8C, ----
GbInstruction(AddressingMode::Reg_None, CpuOperation::INC, GbRegister::B, GbRegister::NONE, Execute::inc),								//INC B: 1B, 4C, Z0H-
GbInstruction(AddressingMode::Reg_None, CpuOperation::DEC, GbRegister::B, GbRegister::NONE, Execute::dec),								//DEC B: 1B, 4C, Z1H-
GbInstruction(AddressingMode::Reg_Imm8, CpuOperation::LD, GbRegister::B, GbRegister::NONE, Execute::load),								//LD B d8: 2B, 8C, ----, TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RLCA, GbRegister::A, GbRegister::NONE, Execute::rotate),							//RLCA: 1B, 4C, 000C, TODO
GbInstruction(AddressingMode::MemImm16_Reg16, CpuOperation::LD, GbRegister::NONE, GbRegister::SP, Execute::load),						//LD (a16), SP: 3B 20C, ----, TODO
GbInstruction(AddressingMode::Reg16_Reg16, CpuOperation::ADD, GbRegister::HL, GbRegister::BC, Execute::add),							//ADD HL, BC: 1B 8C, -0HC
GbInstruction(AddressingMode::Reg_MemReg16, CpuOperation::LD, GbRegister::A, GbRegister::BC, Execute::load),							//LD A, (BC): 1B 8C, ----, TODO
GbInstruction(AddressingMode::Reg16_NONE, CpuOperation::DEC, GbRegister::BC, GbRegister::NONE, Execute::dec),							//DEC BC: 1B 8C, ----
GbInstruction(AddressingMode::Reg_None, CpuOperation::INC, GbRegister::C, GbRegister::NONE, Execute::inc),								//INC C: 1B, 4C, Z0H-
GbInstruction(AddressingMode::Reg_None, CpuOperation::DEC, GbRegister::C, GbRegister::NONE, Execute::dec),								//DEC C: 1B, 4C, Z1H-
GbInstruction(AddressingMode::Reg_Imm8, CpuOperation::LD, GbRegister::C, GbRegister::NONE, Execute::load),								//LD C, d8: 2B, 8C, ----, TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RRCA, GbRegister::A, GbRegister::NONE, Execute::rotate),							//RRCA: 1B 4C, 000C, TODO
		//$1x
GbInstruction(AddressingMode::NONE_NONE, CpuOperation::STOP,ZwReg::NONE,ZwReg::NONE, &Execute::stop),									//STOP 0: 2B 4C, ----, TODO
GbInstruction(AddressingMode::Reg16_Imm16, CpuOperation::LD, GbRegister::DE, GbRegister::NONE, Execute::load),							//LD BC ,d16: 3B, 12C, ----, TODO
GbInstruction(AddressingMode::MemReg16_Reg, CpuOperation::LD, GbRegister::DE, GbRegister::A, Execute::load),							//LD (DE), A: 1B 8C, ----, TODO
GbInstruction(AddressingMode::Reg16_None, CpuOperation::INC, GbRegister::DE, GbRegister::NONE, Execute::inc),							//INC DE, 1B, 8C, ----, TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::INC, GbRegister::D, GbRegister::NONE, Execute::inc),								//INC D: 1B, 4C, Z0H-
GbInstruction(AddressingMode::Reg_None, CpuOperation::DEC, GbRegister::D, GbRegister::NONE, Execute::dec),								//DEC D: 1B, 4C, Z1H-
GbInstruction(AddressingMode::Reg_Imm8, CpuOperation::LD, GbRegister::D, GbRegister::NONE, Execute::load),								//LD D d8: 2B, 8C, ----, TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RLA, GbRegister::A, GbRegister::NONE, Execute::rotate),							//RLA: 1B 4C, 000C, TODO
GbInstruction(AddressingMode::Reg16_Simm8, CpuOperation::JR, GbRegister::PC, GbRegister::NONE, Execute::jr, GbFlag::T),					//JR r8: 2B 12C, ----, TODO
GbInstruction(AddressingMode::Reg16_Reg16, CpuOperation::ADD, GbRegister::HL, GbRegister::DE, Execute::add),							//ADD HL, DE: 1B 8C, -0HC
GbInstruction(AddressingMode::Reg_MemReg16, CpuOperation::LD, GbRegister::A, GbRegister::DE, Execute::load),							//LD A, (DE): 1B 8C, ----, TODO
GbInstruction(AddressingMode::Reg16_NONE, CpuOperation::DEC, GbRegister::DE, GbRegister::NONE, Execute::dec),							//DEC DE: 1B 8C, ----
GbInstruction(AddressingMode::Reg_None, CpuOperation::INC, GbRegister::E, GbRegister::NONE, Execute::inc),								//INC E: 1B, 4C, Z0H-
GbInstruction(AddressingMode::Reg_None, CpuOperation::DEC, GbRegister::E, GbRegister::NONE, Execute::dec),								//DEC E: 1B, 4C, Z1H-
GbInstruction(AddressingMode::Reg_Imm8, CpuOperation::LD, GbRegister::E, GbRegister::NONE, Execute::load),								//LD E, d8: 2B, 8C, ----, TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RRA, GbRegister::A, GbRegister::NONE, Execute::rotate),							//RRA: 1B 4C, 000C, TODO

		//$2x
GbInstruction(AddressingMode::Reg16_Simm8, CpuOperation::JR, GbRegister::PC, GbRegister::NONE, Execute::jr, GbFlag::NZ),				//JR NZ, r8: 2B 12/8C, ----, TODO
GbInstruction(AddressingMode::Reg16_Imm16, CpuOperation::LD, GbRegister::HL, GbRegister::NONE, Execute::load),							//LD HL ,d16: 3B, 12C, ----, TODO
GbInstruction(AddressingMode::MemReg16_Reg, CpuOperation::LD, GbRegister::HL, GbRegister::A, Execute::load, GbFlag::PoI),				//LD (HL+),A: 1B 8C, ----, TODO
GbInstruction(AddressingMode::Reg16_None, CpuOperation::INC, GbRegister::HL, GbRegister::NONE, Execute::inc),							//INC HL, 1B, 8C, ----, TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::INC, GbRegister::H, GbRegister::NONE, Execute::inc),								//INC H: 1B, 4C, Z0H-
GbInstruction(AddressingMode::Reg_None, CpuOperation::DEC, GbRegister::H, GbRegister::NONE, Execute::dec),								//DEC H: 1B, 4C, Z1H-
GbInstruction(AddressingMode::Reg_Imm8, CpuOperation::LD, GbRegister::H, GbRegister::NONE, Execute::load),								//LD H d8: 2B, 8C, ----, TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::DAA, GbRegister::A, GbRegister::NONE, Execute::weird),							//DAA: 1B 4C, Z-0C, TODO
GbInstruction(AddressingMode::Reg16_Simm8, CpuOperation::JR, GbRegister::PC, GbRegister::NONE, Execute::jr, GbFlag::Z),					//JR Z, r8: 2B 12/8C, ----, TODO
GbInstruction(AddressingMode::Reg16_Reg16, CpuOperation::ADD, GbRegister::HL, GbRegister::HL, Execute::add),							//ADD HL, HL: 1B 8C, -0HC
GbInstruction(AddressingMode::Reg_MemReg16, CpuOperation::LD, GbRegister::A, GbRegister::HL, Execute::load, GbFlag::PoI),				//LD A, (HL+): 1B 8C, ----, TODO
GbInstruction(AddressingMode::Reg16_NONE, CpuOperation::DEC, GbRegister::HL, GbRegister::NONE, Execute::dec),							//DEC HL: 1B 8C, ----
GbInstruction(AddressingMode::Reg_None, CpuOperation::INC, GbRegister::L, GbRegister::NONE, Execute::dec),								//DEC L: 1B, 4C, Z0H-
GbInstruction(AddressingMode::Reg_None, CpuOperation::DEC, GbRegister::L, GbRegister::NONE, Execute::dec),								//DEC L: 1B, 4C, Z1H-
GbInstruction(AddressingMode::Reg_Imm8, CpuOperation::LD, GbRegister::L, GbRegister::NONE, Execute::load),								//LD L, d8: 2B, 8C, ----, TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::CPL, GbRegister::A, GbRegister::NONE, Execute::weird),							//CPL, 1B, 4C, -11-, TODO

		//$3x
GbInstruction(AddressingMode::Reg16_Simm8, CpuOperation::JR, GbRegister::PC, GbRegister::NONE, Execute::jr, GbFlag::NC),				//JR NC, r8: 2B 12/8C, ----, TODO
GbInstruction(AddressingMode::Reg16_Imm16, CpuOperation::LD, GbRegister::SP, GbRegister::NONE, Execute::load),							//LD SP ,d16: 3B, 12C, ----, TODO
GbInstruction(AddressingMode::MemReg16_Reg, CpuOperation::LD, GbRegister::HL, GbRegister::A, Execute::load, GbFlag::PoD),				//LD (HL-),A: 1B 8C, ----, TODO
GbInstruction(AddressingMode::Reg16_None, CpuOperation::INC, GbRegister::SP, GbRegister::NONE, Execute::inc),							//INC SP, 1B, 8C, ----, TODO
GbInstruction(AddressingMode::MemReg16_None, CpuOperation::INC, GbRegister::HL, GbRegister::NONE, Execute::inc),						//INC (HL): 1B, 12C, Z0H-
GbInstruction(AddressingMode::MemReg16_None, CpuOperation::DEC, GbRegister::HL, GbRegister::NONE, Execute::dec),						//DEC (HL): 1B, 12C, Z1H-
GbInstruction(AddressingMode::MemReg16_Imm8, CpuOperation::LD, GbRegister::HL, GbRegister::NONE, Execute::load),						//LD (HL) d8: 2B, 8C, ----, TODO
GbInstruction(AddressingMode::NONE_NONE, CpuOperation::SCF, GbRegister::NONE, GbRegister::NONE, Execute::weird),						//SCF: 1B 4C, -001, TODO
GbInstruction(AddressingMode::Reg16_Simm8, CpuOperation::JR, GbRegister::PC, GbRegister::NONE, Execute::jr, GbFlag::C),					//JR C, r8: 2B 12/8C, ----, TODO
GbInstruction(AddressingMode::Reg16_Reg16, CpuOperation::ADD, GbRegister::HL, GbRegister::SP, Execute::add),							//ADD HL, SP: 1B 8C, -0HC
GbInstruction(AddressingMode::Reg_MemReg16, CpuOperation::LD, GbRegister::A, GbRegister::HL, Execute::load, GbFlag::PoD),				//LD A, (HL-): 1B 8C, ----, TODO
GbInstruction(AddressingMode::Reg16_NONE, CpuOperation::DEC, GbRegister::SP, GbRegister::NONE, Execute::dec),							//DEC SP: 1B 8C, ----
GbInstruction(AddressingMode::Reg_None, CpuOperation::INC, GbRegister::A, GbRegister::NONE, Execute::inc),								//INC A: 1B, 4C, Z0H-
GbInstruction(AddressingMode::Reg_None, CpuOperation::DEC, GbRegister::A, GbRegister::NONE, Execute::dec),								//DEC A: 1B, 4C, Z1H-
GbInstruction(AddressingMode::Reg_Imm8, CpuOperation::LD, GbRegister::A, GbRegister::NONE, Execute::load),								//LD A, d8: 2B, 8C, ----, TODO
GbInstruction(AddressingMode::NONE_NONE, CpuOperation::CCF, GbRegister::NONE, GbRegister::NONE, Execute::weird),						//CCF: 1B 4C, -001, TODO

		//$4x
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::B, GbRegister::B, Execute::load),									//LD B, B: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::B, GbRegister::C, Execute::load),									//LD B, C: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::B, GbRegister::D, Execute::load),									//LD B, D: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::B, GbRegister::E, Execute::load),									//LD B, E: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::B, GbRegister::H, Execute::load),									//LD B, H: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::B, GbRegister::L, Execute::load),									//LD B, L: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_MemReg16, CpuOperation::LD, GbRegister::B, GbRegister::HL, Execute::load, GbFlag::NONE),				//LD B, (HL): 1B 8C, ----, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::B, GbRegister::A, Execute::load),									//LD B, A: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::C, GbRegister::B, Execute::load),									//LD C, B: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::C, GbRegister::C, Execute::load),									//LD C, C: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::C, GbRegister::D, Execute::load),									//LD C, D: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::C, GbRegister::E, Execute::load),									//LD C, E: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::C, GbRegister::H, Execute::load),									//LD C, H: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::C, GbRegister::L, Execute::load),									//LD C, L: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_MemReg16, CpuOperation::LD, GbRegister::C, GbRegister::HL, Execute::load, GbFlag::NONE),				//LD C, (HL): 1B 8C, ----, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::C, GbRegister::A, Execute::load),									//LD C, A: 1B 4C, Z0HC, TODO

		//$5x
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::D, GbRegister::B, Execute::load),									//LD D, B: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::D, GbRegister::C, Execute::load),									//LD D, C: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::D, GbRegister::D, Execute::load),									//LD D, D: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::D, GbRegister::E, Execute::load),									//LD D, E: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::D, GbRegister::H, Execute::load),									//LD D, H: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::D, GbRegister::L, Execute::load),									//LD D, L: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_MemReg16, CpuOperation::LD, GbRegister::D, GbRegister::HL, Execute::load, GbFlag::NONE),				//LD D, (HL): 1B 8C, ----, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::D, GbRegister::A, Execute::load),									//LD D, A: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::E, GbRegister::B, Execute::load),									//LD E, B: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::E, GbRegister::C, Execute::load),									//LD E, C: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::E, GbRegister::D, Execute::load),									//LD E, D: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::E, GbRegister::E, Execute::load),									//LD E, E: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::E, GbRegister::H, Execute::load),									//LD E, H: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::E, GbRegister::L, Execute::load),									//LD E, L: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_MemReg16, CpuOperation::LD, GbRegister::E, GbRegister::HL, Execute::load, GbFlag::NONE),				//LD E, (HL): 1B 8C, ----, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::E, GbRegister::A, Execute::load),									//LD E, A: 1B 4C, Z0HC, TODO

		//$6x
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::H, GbRegister::B, Execute::load),									//LD H, B: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::H, GbRegister::C, Execute::load),									//LD H, C: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::H, GbRegister::D, Execute::load),									//LD H, D: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::H, GbRegister::E, Execute::load),									//LD H, E: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::H, GbRegister::H, Execute::load),									//LD H, H: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::H, GbRegister::L, Execute::load),									//LD H, L: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_MemReg16, CpuOperation::LD, GbRegister::H, GbRegister::HL, Execute::load, GbFlag::NONE),				//LD H, (HL): 1B 8C, ----, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::H, GbRegister::A, Execute::load),									//LD H, A: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::L, GbRegister::B, Execute::load),									//LD L, B: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::L, GbRegister::C, Execute::load),									//LD L, C: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::L, GbRegister::D, Execute::load),									//LD L, D: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::L, GbRegister::E, Execute::load),									//LD L, E: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::L, GbRegister::H, Execute::load),									//LD L, H: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::L, GbRegister::L, Execute::load),									//LD L, L: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_MemReg16, CpuOperation::LD, GbRegister::L, GbRegister::HL, Execute::load, GbFlag::NONE),				//LD L, (HL): 1B 8C, ----, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::L, GbRegister::A, Execute::load),									//LD L, A: 1B 4C, Z0HC, TODO

		//$7x
GbInstruction(AddressingMode::MemReg16_Reg, CpuOperation::LD, GbRegister::HL, B, Execute::load),										//LD (HL), B: 1B 8C, ----, TODO
GbInstruction(AddressingMode::MemReg16_Reg, CpuOperation::LD, GbRegister::HL, C, Execute::load),										//LD (HL), C: 1B 8C, ----, TODO
GbInstruction(AddressingMode::MemReg16_Reg, CpuOperation::LD, GbRegister::HL, D, Execute::load),										//LD (HL), D: 1B 8C, ----, TODO
GbInstruction(AddressingMode::MemReg16_Reg, CpuOperation::LD, GbRegister::HL, E, Execute::load),										//LD (HL), E: 1B 8C, ----, TODO
GbInstruction(AddressingMode::MemReg16_Reg, CpuOperation::LD, GbRegister::HL, H, Execute::load),										//LD (HL), H: 1B 8C, ----, TODO
GbInstruction(AddressingMode::MemReg16_Reg, CpuOperation::LD, GbRegister::HL, L, Execute::load),										//LD (HL), L: 1B 8C, ----, TODO
GbInstruction(AddressingMode::NONE_NONE, CpuOperation::HALT, GbRegister::NONE, GbRegister::NONE, Execute::weird),						//HALT: 1B 4C, ----, TODO
GbInstruction(AddressingMode::MemReg16_Reg, CpuOperation::LD, GbRegister::HL, A, Execute::load),										//LD (HL), A: 1B 8C, ----, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::A, GbRegister::B, Execute::load),									//LD A, B: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::A, GbRegister::C, Execute::load),									//LD A, C: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::A, GbRegister::D, Execute::load),									//LD A, D: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::A, GbRegister::E, Execute::load),									//LD A, E: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::A, GbRegister::H, Execute::load),									//LD A, H: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::A, GbRegister::L, Execute::load),									//LD A, L: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_MemReg16, CpuOperation::LD, GbRegister::A, GbRegister::HL, Execute::load, GbFlag::NONE),				//LD A, (HL): 1B 8C, ----, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::A, GbRegister::A, Execute::load),									//LD A, A: 1B 4C, Z0HC, TODO

		//$8x
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::ADD, GbRegister::A, GbRegister::B, Execute::add),									//ADD A, B: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::ADD, GbRegister::A, GbRegister::C, Execute::add),									//ADD A, C: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::ADD, GbRegister::A, GbRegister::D, Execute::add),									//ADD A, D: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::ADD, GbRegister::A, GbRegister::E, Execute::add),									//ADD A, E: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::ADD, GbRegister::A, GbRegister::H, Execute::add),									//ADD A, H: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::ADD, GbRegister::A, GbRegister::L, Execute::add),									//ADD A, L: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_MemReg16, CpuOperation::ADD, GbRegister::A, GbRegister::HL, Execute::add),							//ADD A, (HL): 1B 8C, ----, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::ADD, GbRegister::A, GbRegister::A, Execute::add),									//ADD A, A: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::ADC, GbRegister::A, GbRegister::B, Execute::adc),									//ADC A, B: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::ADC, GbRegister::A, GbRegister::C, Execute::adc),									//ADC A, C: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::ADC, GbRegister::A, GbRegister::D, Execute::adc),									//ADC A, D: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::ADC, GbRegister::A, GbRegister::E, Execute::adc),									//ADC A, E: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::ADC, GbRegister::A, GbRegister::H, Execute::adc),									//ADC A, H: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::ADC, GbRegister::A, GbRegister::L, Execute::adc),									//ADC A, L: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_MemReg16, CpuOperation::ADC, GbRegister::A, GbRegister::HL, Execute::adc),							//ADC A, (HL): 1B 8C, ----, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::ADC, GbRegister::A, GbRegister::A, Execute::adc),									//ADC A, A: 1B 4C, Z0HC, TODO
		//$9x
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::SUB, GbRegister::A, GbRegister::B, Execute::sub),									//SUB A, B: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::SUB, GbRegister::A, GbRegister::C, Execute::sub),									//SUB A, C: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::SUB, GbRegister::A, GbRegister::D, Execute::sub),									//SUB A, D: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::SUB, GbRegister::A, GbRegister::E, Execute::sub),									//SUB A, E: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::SUB, GbRegister::A, GbRegister::H, Execute::sub),									//SUB A, H: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::SUB, GbRegister::A, GbRegister::L, Execute::sub),									//SUB A, L: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_MemReg16, CpuOperation::SUB, GbRegister::A, GbRegister::HL, Execute::sub),							//SUB A, (HL): 1B 8C, ----, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::SUB, GbRegister::A, GbRegister::A, Execute::sub),									//SUB A, A: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::SBC, GbRegister::A, GbRegister::B, Execute::sbc),									//SBC A, B: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::SBC, GbRegister::A, GbRegister::C, Execute::sbc),									//SBC A, C: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::SBC, GbRegister::A, GbRegister::D, Execute::sbc),									//SBC A, D: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::SBC, GbRegister::A, GbRegister::E, Execute::sbc),									//SBC A, E: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::SBC, GbRegister::A, GbRegister::H, Execute::sbc),									//SBC A, H: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::SBC, GbRegister::A, GbRegister::L, Execute::sbc),									//SBC A, L: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_MemReg16, CpuOperation::SBC, GbRegister::A, GbRegister::HL, Execute::sbc),							//SBC A, (HL): 1B 8C, ----, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::SBC, GbRegister::A, GbRegister::A, Execute::sbc),									//SBC A, A: 1B 4C, Z0HC, TODO

		//$Ax
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::AND, GbRegister::A, GbRegister::B, Execute::bwAnd),								//AND A, B: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::AND, GbRegister::A, GbRegister::C, Execute::bwAnd),								//AND A, C: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::AND, GbRegister::A, GbRegister::D, Execute::bwAnd),								//AND A, D: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::AND, GbRegister::A, GbRegister::E, Execute::bwAnd),								//AND A, E: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::AND, GbRegister::A, GbRegister::H, Execute::bwAnd),								//AND A, H: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::AND, GbRegister::A, GbRegister::L, Execute::bwAnd),								//AND A, L: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_MemReg16, CpuOperation::AND, GbRegister::A, GbRegister::HL, Execute::bwAnd),							//AND A, (HL): 1B 8C, ----, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::AND, GbRegister::A, GbRegister::A, Execute::bwAnd),								//AND A, A: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::XOR, GbRegister::A, GbRegister::B, Execute::bwXor),								//XOR A, B: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::XOR, GbRegister::A, GbRegister::C, Execute::bwXor),								//XOR A, C: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::XOR, GbRegister::A, GbRegister::D, Execute::bwXor),								//XOR A, D: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::XOR, GbRegister::A, GbRegister::E, Execute::bwXor),								//XOR A, E: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::XOR, GbRegister::A, GbRegister::H, Execute::bwXor),								//XOR A, H: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::XOR, GbRegister::A, GbRegister::L, Execute::bwXor),								//XOR A, L: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_MemReg16, CpuOperation::XOR, GbRegister::A, GbRegister::HL, Execute::bwXor),							//XOR A, (HL): 1B 8C, ----, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::XOR, GbRegister::A, GbRegister::A, Execute::bwXor),								//XOR A, A: 1B 4C, Z0HC, TODO

		//$Bx
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::OR, GbRegister::A, GbRegister::B, Execute::bwOr),									//OR A, B: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::OR, GbRegister::A, GbRegister::C, Execute::bwOr),									//OR A, C: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::OR, GbRegister::A, GbRegister::D, Execute::bwOr),									//OR A, D: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::OR, GbRegister::A, GbRegister::E, Execute::bwOr),									//OR A, E: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::OR, GbRegister::A, GbRegister::H, Execute::bwOr),									//OR A, H: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::OR, GbRegister::A, GbRegister::L, Execute::bwOr),									//OR A, L: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_MemReg16, CpuOperation::OR, GbRegister::A, GbRegister::HL, Execute::bwOr),							//OR A, (HL): 1B 8C, ----, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::OR, GbRegister::A, GbRegister::A, Execute::bwOr),									//OR A, A: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::CP, GbRegister::A, GbRegister::B, Execute::sub),									//CP A, B: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::CP, GbRegister::A, GbRegister::C, Execute::sub),									//CP A, C: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::CP, GbRegister::A, GbRegister::D, Execute::sub),									//CP A, D: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::CP, GbRegister::A, GbRegister::E, Execute::sub),									//CP A, E: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::CP, GbRegister::A, GbRegister::H, Execute::sub),									//CP A, H: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::CP, GbRegister::A, GbRegister::L, Execute::sub),									//CP A, L: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_MemReg16, CpuOperation::CP, GbRegister::A, GbRegister::HL, Execute::sub),								//CP A, (HL): 1B 8C, ----, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::CP, GbRegister::A, GbRegister::A, Execute::sub),									//CP A, A: 1B 4C, Z0HC, TODO
																																		//
		//$Cx
GbInstruction(AddressingMode::Reg16_MemReg16, CpuOperation::RET, GbRegister::PC, GbRegister::SP, Execute::ret, GbFlag::NZ),				//RET NZ: 1B 20/8C, ----, TODO
GbInstruction(AddressingMode::Reg16_MemReg16, CpuOperation::POP, GbRegister::BC, GbRegister::SP, Execute::load),						//POP BC: 1B 12C, ----, TODO
GbInstruction(AddressingMode::Reg16_Imm16, CpuOperation::JP, GbRegister::PC, GbRegister::NONE, Execute::jp, GbFlag::NZ),				//JP NZ, a16: 3B 16/12C, ----, TODO
GbInstruction(AddressingMode::Reg16_Imm16, CpuOperation::JP, GbRegister::PC, GbRegister::NONE, Execute::jP, GbFlag::T),					//JP a16: 3B 16C, ----, TODO
GbInstruction(AddressingMode::MemImm16_None, CpuOperation::CALL, GbRegister::SP, GbRegister::NONE, Execute::call, GbFlag::NZ),			//CALL NZ, a16: 3B 24/12C, ----, TODO
GbInstruction(AddressingMode::MemReg16_Reg16, CpuOperation::PUSH, GbRegister::SP, GbRegister::BC, Execute::load),						//PUSH BC: 1B 16C, ----, TODO
GbInstruction(AddressingMode::Reg_Imm8, CpuOperation::ADD, GbRegister::A, GbRegister::NONE, Execute::add),								//ADD A, d8: 2B 8C, Z0HC, TODO
GbInstruction(AddressingMode::Reg16_None, CpuOperation::RST, GbRegister::PC, GbRegister::NONE, Execute::rst, GbFlag::H00),				//RST 00H: 1B 16C, ----, TODO
GbInstruction(AddressingMode::Reg16_MemReg16, CpuOperation::RET, GbRegister::PC, GbRegister::SP, Execute::ret, GbFlag::Z),				//RET Z: 1B 20/8C, ----, TODO
GbInstruction(AddressingMode::Reg16_MemReg16, CpuOperation::RET, GbRegister::PC, GbRegister::SP, Execute::ret, GbFlag::T),				//RET : 1B 16C, ----, TODO
GbInstruction(AddressingMode::Reg16_Imm16, CpuOperation::JP, GbRegister::PC, GbRegister::NONE, Execute::jp, GbFlag::Z),					//JP Z, a16: 3B 16/12C, ----, TODO
GbInstruction(AddressingMode::NONE_NONE, CpuOperation::PREFIX, GbRegister::NONE, GbRegister::NONE, Execute::decodePrefixInstruction),	//PREFIX CB: 1B 4C, ----, TODO
GbInstruction(AddressingMode::MemImm16_None, CpuOperation::CALL, GbRegister::SP, GbRegister::NONE, Execute::call, GbFlag::Z),			//CALL Z, a16: 3B 24/12C, ----, TODO
GbInstruction(AddressingMode::MemImm16_None, CpuOperation::CALL, GbRegister::SP, GbRegister::NONE, Execute::call, GbFlag::T),			//CALL a16: 3B 24C, ----, TODO
GbInstruction(AddressingMode::Reg_Imm8, CpuOperation::ADC, GbRegister::A, GbRegister::NONE, Execute::adc),								//ADC A, d8: 2B 8C, Z0HC, TODO
GbInstruction(AddressingMode::Reg16_None, CpuOperation::RST, GbRegister::PC, GbRegister::NONE, Execute::rst, GbFlag::H08),				//RST 08H: 1B 16C, ----, TODO

		//$Dx
GbInstruction(AddressingMode::Reg16_MemReg16, CpuOperation::RET, GbRegister::PC, GbRegister::SP, Execute::ret, GbFlag::NC),				//RET NC: 1B 20/8C, ----, TODO
GbInstruction(AddressingMode::Reg16_MemReg16, CpuOperation::POP, GbRegister::DE, GbRegister::SP, Execute::load),						//POP DE: 1B 12C, ----, TODO
GbInstruction(AddressingMode::Reg16_Imm16, CpuOperation::JP, GbRegister::PC, GbRegister::NONE, Execute::jp, GbFlag::NZ),				//JP NZ, a16: 3B 16/12C, ----, TODO
GbInstruction(AddressingMode::NONE_NONE, CpuOperation::ILLEGAL, GbRegister::NONE, GbRegister::NONE, Execute::illegal),					//ILLEGAL OP CODE (0xD3)
GbInstruction(AddressingMode::MemImm16_None, CpuOperation::CALL, GbRegister::SP, GbRegister::NONE, Execute::call, GbFlag::NC),			//CALL NC, a16: 3B 24/12C, ----, TODO
GbInstruction(AddressingMode::MemReg16_Reg16, CpuOperation::PUSH, GbRegister::SP, GbRegister::DE, Execute::load),						//PUSH DE: 1B 16C, ----, TODO
GbInstruction(AddressingMode::Reg_Imm8, CpuOperation::SUB, GbRegister::A, GbRegister::NONE, Execute::sub),								//SUB A, d8: 2B 8C, Z1HC, TODO
GbInstruction(AddressingMode::Reg16_None, CpuOperation::RST, GbRegister::PC, GbRegister::NONE, Execute::rst, GbFlag::H10),				//RST 10H: 1B 16C, ----, TODO
GbInstruction(AddressingMode::Reg16_MemReg16, CpuOperation::RET, GbRegister::PC, GbRegister::SP, Execute::ret, GbFlag::C),				//RET C: 1B 20/8C, ----, TODO
GbInstruction(AddressingMode::Reg16_MemReg16, CpuOperation::RET, GbRegister::PC, GbRegister::SP, Execute::ret, GbFlag::TI),				//RETI : 1B 16C, ----, TODO
GbInstruction(AddressingMode::Reg16_Imm16, CpuOperation::JP, GbRegister::PC, GbRegister::NONE, Execute::jp, GbFlag::C),					//JP C, a16: 3B 16/12C, ----, TODO
GbInstruction(AddressingMode::NONE_NONE, CpuOperation::ILLEGAL, GbRegister::NONE, GbRegister::NONE, Execute::illegal),					//ILLEGAL OP CODE (0xDB)
GbInstruction(AddressingMode::MemImm16_None, CpuOperation::CALL, GbRegister::SP, GbRegister::NONE, Execute::call, GbFlag::C),			//CALL C, a16: 3B 24/12C, ----, TODO
GbInstruction(AddressingMode::NONE_NONE, CpuOperation::ILLEGAL, GbRegister::NONE, GbRegister::NONE, Execute::illegal),					//ILLEGAL OP CODE (0xDD)
GbInstruction(AddressingMode::Reg_Imm8, CpuOperation::SBC, GbRegister::A, GbRegister::NONE, Execute::sbc),								//SBC A, d8: 2B 8C, Z1HC, TODO
GbInstruction(AddressingMode::Reg16_None, CpuOperation::RST, GbRegister::PC, GbRegister::NONE, Execute::rst, GbFlag::H18),				//RST 18H: 1B 16C, ----, TODO

		//$Ex
GbInstruction(AddressingMode::MemImm8_Reg, CpuOperation::LD, GbRegister::NONE, GbRegister::A, Execute::load),							//LDH (a8), A: 2B 12C, ----, TODO
GbInstruction(AddressingMode::Reg16_MemReg16, CpuOperation::POP, GbRegister::HL, GbRegister::SP, Execute::load),						//POP HL: 1B 12C, ----, TODO
GbInstruction(AddressingMode::MemReg_Reg, CpuOperation::LD, GbRegister::C, GbRegister::A, Execute::load),								//LDH (C), A, 2B 4C, ----, TODO WARNING POTENTIAL BUG (LENGTH)
GbInstruction(AddressingMode::NONE_NONE, CpuOperation::ILLEGAL, GbRegister::NONE, GbRegister::NONE, Execute::illegal),					//ILLEGAL OP CODE (0xE3)
GbInstruction(AddressingMode::NONE_NONE, CpuOperation::ILLEGAL, GbRegister::NONE, GbRegister::NONE, Execute::illegal),					//ILLEGAL OP CODE (0xE4)
GbInstruction(AddressingMode::MemReg16_Reg16, CpuOperation::PUSH, GbRegister::SP, GbRegister::HL, Execute::load),						//PUSH HL: 1B 16C, ----, TODO
GbInstruction(AddressingMode::Reg_Imm8, CpuOperation::AND, GbRegister::A, GbRegister::NONE, Execute::bwAnd),							//AND A, d8: 2B 8C, Z010, TODO
GbInstruction(AddressingMode::Reg16_None, CpuOperation::RST, GbRegister::PC, GbRegister::NONE, Execute::rst, GbFlag::H20),				//RST 20H: 1B 16C, ----, TODO
GbInstruction(AddressingMode::Reg16_Simm8, CpuOperation::ADD, GbRegister:SP, GbRegister::NONE, Execute::add),							//ADD SP, r8, 2B 16C, 00HC
GbInstruction(AddressingMode::Reg16_Reg16, CpuOperation::JP, GbRegister::PC, GbRegister::HL, Execute::jp, GbFlag::T),					//JP (HL): 1B 4C, ----, TODO
GbInstruction(AddressingMode::MemImm16_Reg, CpuOperation::LD, GbRegister::NONE, GbRegister::A, Execute::load),							//LD (a16), A: 3B 16C, ----, TODO
GbInstruction(AddressingMode::NONE_NONE, CpuOperation::ILLEGAL, GbRegister::NONE, GbRegister::NONE, Execute::illegal),					//ILLEGAL OP CODE (0xEB)
GbInstruction(AddressingMode::NONE_NONE, CpuOperation::ILLEGAL, GbRegister::NONE, GbRegister::NONE, Execute::illegal),					//ILLEGAL OP CODE (0xEC)
GbInstruction(AddressingMode::NONE_NONE, CpuOperation::ILLEGAL, GbRegister::NONE, GbRegister::NONE, Execute::illegal),					//ILLEGAL OP CODE (0xED)
GbInstruction(AddressingMode::Reg_Imm8, CpuOperation::XOR, GbRegister::A, GbRegister::NONE, Execute::bwXor),							//XOR A, d8: 2B 8C, Z000, TODO
GbInstruction(AddressingMode::Reg16_None, CpuOperation::RST, GbRegister::PC, GbRegister::NONE, Execute::rst, GbFlag::H28),				//RST 28H: 1B 16C, ----, TODO

		//$Fx
GbInstruction(AddressingMode::Reg_MemImm8, CpuOperation::LD, GbRegister::A, GbRegister::NONE, Execute::load),							//LDH A, (a8): 2B 12C, ----, TODO
GbInstruction(AddressingMode::Reg16_MemReg16, CpuOperation::POP, GbRegister::AF, GbRegister::SP, Execute::load),						//POP AF: 1B 12C, ZNHC, TODO
GbInstruction(AddressingMode::Reg_MemReg, CpuOperation::LD, GbRegister::A, GbRegister::C, Execute::load),								//LDH A, (C): 2B 4C, ----, TODO WARNING POTENTIAL BUG (LENGTH)
GbInstruction(AddressingMode::NONE_NONE, CpuOperation::DI, GbRegister::NONE, GbRegister::NONE, Execute::weird),							//DI: 1B 4C, ----, TODO
GbInstruction(AddressingMode::NONE_NONE, CpuOperation::ILLEGAL, GbRegister::NONE, GbRegister::NONE, Execute::illegal),					//ILLEGAL OP CODE (0xF4)
GbInstruction(AddressingMode::MemReg16_Reg16, CpuOperation::PUSH, GbRegister::SP, GbRegister::AF, Execute::load),						//PUSH AF: 1B 16C, ----, TODO
GbInstruction(AddressingMode::Reg_Imm8, CpuOperation::OR, GbRegister::A, GbRegister::NONE, Execute::bwOr),								//OR A, d8: 2B 8C, Z000, TODO
GbInstruction(AddressingMode::Reg16_None, CpuOperation::RST, GbRegister::PC, GbRegister::NONE, Execute::rst, GbFlag::H30),				//RST 30H: 1B 16C, ----, TODO
GbInstruction(AddressingMode::Reg16_Reg16Sim8, CpuOperation::LD, GbRegister::HL, GbRegister::SP, Execute::load),						//LD HL, SP+r8: 2B 8C, 00HC, TODO
GbInstruction(AddressingMode::Reg16_Reg16, CpuOperation::LD, GbRegister::SP, GbRegister::HL, Execute::load),							//LD SP, HL: 1B 8C, ----, TODO
GbInstruction(AddressingMode::Reg_MemImm16, CpuOperation::LD, GbRegister::A, GbRegister::NONE, Execute::load),							//LD A, (a16): 3B 16C, ----, TODO
GbInstruction(AddressingMode::NONE_NONE, CpuOperation::EI, GbRegister::NONE, GbRegister::NONE, Execute::weird),							//EI: 1B 4C, ----, TODO
GbInstruction(AddressingMode::NONE_NONE, CpuOperation::ILLEGAL, GbRegister::NONE, GbRegister::NONE, Execute::illegal),					//ILLEGAL OP CODE (0xFC)
GbInstruction(AddressingMode::NONE_NONE, CpuOperation::ILLEGAL, GbRegister::NONE, GbRegister::NONE, Execute::illegal),					//ILLEGAL OP CODE (0xFD)
GbInstruction(AddressingMode::Reg_Imm8, CpuOperation::XOR, GbRegister::A, GbRegister::NONE, Execute::bwXor),							//XOR A, d8: 2B 8C, Z000, TODO
GbInstruction(AddressingMode::Reg16_None, CpuOperation::RST, GbRegister::PC, GbRegister::NONE, Execute::rst, GbFlag::H38),				//RST 38H: 1B 16C, ----, TODO



}

GbInstruction prefixInstDec[NUM_INSTRUCTIONS] = {
		//$0x
GbInstruction(AddressingMode::Reg_None, CpuOperation::RLC, GbRegister::B, GbRegister::B, Execute::rotate),								//RLC B: 2B 8C, Z00C. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RLC, GbRegister::C, GbRegister::C, Execute::rotate),								//RLC C: 2B 8C, Z00C. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RLC, GbRegister::D, GbRegister::D, Execute::rotate),								//RLC D: 2B 8C, Z00C. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RLC, GbRegister::E, GbRegister::E, Execute::rotate),								//RLC E: 2B 8C, Z00C. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RLC, GbRegister::H, GbRegister::H, Execute::rotate),								//RLC H: 2B 8C, Z00C. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RLC, GbRegister::L, GbRegister::L, Execute::rotate),								//RLC L: 2B 8C, Z00C. TODO
GbInstruction(AddressingMode::MemReg16_None, CpuOperation::RLC, GbRegister::HL, GbRegister::HL, Execute::rotate),						//RLC (HL): 2B 16C, Z00C, TODD
GbInstruction(AddressingMode::Reg_None, CpuOperation::RLC, GbRegister::A, GbRegister::A, Execute::rotate),								//RLC A: 2B 8C, Z00C. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RRC, GbRegister::B, GbRegister::B, Execute::rotate),								//RRC B: 2B 8C, Z00C. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RRC, GbRegister::C, GbRegister::C, Execute::rotate),								//RRC C: 2B 8C, Z00C. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RRC, GbRegister::D, GbRegister::D, Execute::rotate),								//RRC D: 2B 8C, Z00C. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RRC, GbRegister::E, GbRegister::E, Execute::rotate),								//RRC E: 2B 8C, Z00C. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RRC, GbRegister::H, GbRegister::H, Execute::rotate),								//RRC H: 2B 8C, Z00C. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RRC, GbRegister::L, GbRegister::L, Execute::rotate),								//RRC L: 2B 8C, Z00C. TODO
GbInstruction(AddressingMode::MemReg16_None, CpuOperation::RRC, GbRegister::HL, GbRegister::HL, Execute::rotate),						//RRC (HL): 2B 16C, Z00C, TODD
GbInstruction(AddressingMode::Reg_None, CpuOperation::RRC, GbRegister::A, GbRegister::A, Execute::rotate),								//RRC A: 2B 8C, Z00C. TODO

		//$1x
GbInstruction(AddressingMode::Reg_None, CpuOperation::RL, GbRegister::B, GbRegister::B, Execute::rotate),								//RL B: 2B 8C, Z00C. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RL, GbRegister::C, GbRegister::C, Execute::rotate),								//RL C: 2B 8C, Z00C. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RL, GbRegister::D, GbRegister::D, Execute::rotate),								//RL D: 2B 8C, Z00C. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RL, GbRegister::E, GbRegister::E, Execute::rotate),								//RL E: 2B 8C, Z00C. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RL, GbRegister::H, GbRegister::H, Execute::rotate),								//RL H: 2B 8C, Z00C. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RL, GbRegister::L, GbRegister::L, Execute::rotate),								//RL L: 2B 8C, Z00C. TODO
GbInstruction(AddressingMode::MemReg16_None, CpuOperation::RL, GbRegister::HL, GbRegister::HL, Execute::rotate),						//RL (HL): 2B 16C, Z00C, TODD
GbInstruction(AddressingMode::Reg_None, CpuOperation::RL, GbRegister::A, GbRegister::A, Execute::rotate),								//RL A: 2B 8C, Z00C. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RR, GbRegister::B, GbRegister::B, Execute::rotate),								//RR B: 2B 8C, Z00C. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RR, GbRegister::C, GbRegister::C, Execute::rotate),								//RR C: 2B 8C, Z00C. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RR, GbRegister::D, GbRegister::D, Execute::rotate),								//RR D: 2B 8C, Z00C. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RR, GbRegister::E, GbRegister::E, Execute::rotate),								//RR E: 2B 8C, Z00C. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RR, GbRegister::H, GbRegister::H, Execute::rotate),								//RR H: 2B 8C, Z00C. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RR, GbRegister::L, GbRegister::L, Execute::rotate),								//RR L: 2B 8C, Z00C. TODO
GbInstruction(AddressingMode::MemReg16_None, CpuOperation::RR, GbRegister::HL, GbRegister::HL, Execute::rotate),						//RR (HL): 2B 16C, Z00C, TODD
GbInstruction(AddressingMode::Reg_None, CpuOperation::RR, GbRegister::A, GbRegister::A, Execute::rotate),								//RR A: 2B 8C, Z00C. TODO

		//$2x
GbInstruction(AddressingMode::Reg_None, CpuOperation::SLA, GbRegister::B, GbRegister::B, Execute::shift),								//SLA B: 2B 8C, Z00C. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::SLA, GbRegister::C, GbRegister::C, Execute::shift),								//SLA C: 2B 8C, Z00C. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::SLA, GbRegister::D, GbRegister::D, Execute::shift),								//SLA D: 2B 8C, Z00C. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::SLA, GbRegister::E, GbRegister::E, Execute::shift),								//SLA E: 2B 8C, Z00C. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::SLA, GbRegister::H, GbRegister::H, Execute::shift),								//SLA H: 2B 8C, Z00C. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::SLA, GbRegister::L, GbRegister::L, Execute::shift),								//SLA L: 2B 8C, Z00C. TODO
GbInstruction(AddressingMode::MemReg16_None, CpuOperation::SLA, GbRegister::HL, GbRegister::HL, Execute::shift),						//SLA (HL): 2B 16C, Z00C, TODD
GbInstruction(AddressingMode::Reg_None, CpuOperation::SLA, GbRegister::A, GbRegister::A, Execute::shift),								//SLA A: 2B 8C, Z00C. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::SRA, GbRegister::B, GbRegister::B, Execute::shift),								//SRA B: 2B 8C, Z000. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::SRA, GbRegister::C, GbRegister::C, Execute::shift),								//SRA C: 2B 8C, Z000. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::SRA, GbRegister::D, GbRegister::D, Execute::shift),								//SRA D: 2B 8C, Z000. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::SRA, GbRegister::E, GbRegister::E, Execute::shift),								//SRA E: 2B 8C, Z000. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::SRA, GbRegister::H, GbRegister::H, Execute::shift),								//SRA H: 2B 8C, Z000. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::SRA, GbRegister::L, GbRegister::L, Execute::shift),								//SRA L: 2B 8C, Z000. TODO
GbInstruction(AddressingMode::MemReg16_None, CpuOperation::SRA, GbRegister::HL, GbRegister::HL, Execute::shift),						//SRA (HL): 2B 16C, Z000, TODD
GbInstruction(AddressingMode::Reg_None, CpuOperation::SRA, GbRegister::A, GbRegister::A, Execute::shift),								//SRA A: 2B 8C, Z000. TODO

		//$3x
GbInstruction(AddressingMode::Reg_None, CpuOperation::SWAP, GbRegister::B, GbRegister::B, Execute::swap),								//SWAP B: 2B 8C, Z000. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::SWAP, GbRegister::C, GbRegister::C, Execute::swap),								//SWAP C: 2B 8C, Z000. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::SWAP, GbRegister::D, GbRegister::D, Execute::swap),								//SWAP D: 2B 8C, Z000. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::SWAP, GbRegister::E, GbRegister::E, Execute::swap),								//SWAP E: 2B 8C, Z000. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::SWAP, GbRegister::H, GbRegister::H, Execute::swap),								//SWAP H: 2B 8C, Z000. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::SWAP, GbRegister::L, GbRegister::L, Execute::swap),								//SWAP L: 2B 8C, Z000. TODO
GbInstruction(AddressingMode::MemReg16_None, CpuOperation::SWAP, GbRegister::HL, GbRegister::HL, Execute::swap),						//SWAP (HL): 2B 16C, Z000, TODD
GbInstruction(AddressingMode::Reg_None, CpuOperation::SWAP, GbRegister::A, GbRegister::A, Execute::swap),								//SWAP A: 2B 8C, Z000. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::SRL, GbRegister::B, GbRegister::B, Execute::shift),								//SRL B: 2B 8C, Z00C. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::SRL, GbRegister::C, GbRegister::C, Execute::shift),								//SRL C: 2B 8C, Z00C. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::SRL, GbRegister::D, GbRegister::D, Execute::shift),								//SRL D: 2B 8C, Z00C. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::SRL, GbRegister::E, GbRegister::E, Execute::shift),								//SRL E: 2B 8C, Z00C. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::SRL, GbRegister::H, GbRegister::H, Execute::shift),								//SRL H: 2B 8C, Z00C. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::SRL, GbRegister::L, GbRegister::L, Execute::shift),								//SRL L: 2B 8C, Z00C. TODO
GbInstruction(AddressingMode::MemReg16_None, CpuOperation::SRL, GbRegister::HL, GbRegister::HL, Execute::shift),						//SRL (HL): 2B 16C, Z00C, TODD
GbInstruction(AddressingMode::Reg_None, CpuOperation::SRL, GbRegister::A, GbRegister::A, Execute::shift),								//SRL A: 2B 8C, Z00C. TODO

		//$4x
GbInstruction(AddressingMode::Reg_None, CpuOperation::BIT, GbRegister::B, GbRegister::B, Execute::bit, GbFlag::B0),						//BIT 0, B: 2B 8C, Z01-. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::BIT, GbRegister::C, GbRegister::C, Execute::bit, GbFlag::B0),						//BIT 0, C: 2B 8C, Z01-. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::BIT, GbRegister::D, GbRegister::D, Execute::bit, GbFlag::B0),						//BIT 0, D: 2B 8C, Z01-. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::BIT, GbRegister::E, GbRegister::E, Execute::bit, GbFlag::B0),						//BIT 0, E: 2B 8C, Z01-. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::BIT, GbRegister::H, GbRegister::H, Execute::bit, GbFlag::B0),						//BIT 0, H: 2B 8C, Z01-. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::BIT, GbRegister::L, GbRegister::L, Execute::bit, GbFlag::B0),						//BIT 0, L: 2B 8C, Z01-. TODO
GbInstruction(AddressingMode::MemReg16_None, CpuOperation::BIT, GbRegister::HL, GbRegister::HL, Execute::bit, GbFlag::B0),				//BIT 0, (HL): 2B 16C, Z01-, TODD
GbInstruction(AddressingMode::Reg_None, CpuOperation::BIT, GbRegister::A, GbRegister::A, Execute::bit, GbFlag::B0),						//BIT 0, A: 2B 8C, Z01-. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::BIT, GbRegister::B, GbRegister::B, Execute::bit, GbFlag::B1),						//BIT 1, B: 2B 8C, Z01-. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::BIT, GbRegister::C, GbRegister::C, Execute::bit, GbFlag::B1),						//BIT 1, C: 2B 8C, Z01-. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::BIT, GbRegister::D, GbRegister::D, Execute::bit, GbFlag::B1),						//BIT 1, D: 2B 8C, Z01-. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::BIT, GbRegister::E, GbRegister::E, Execute::bit, GbFlag::B1),						//BIT 1, E: 2B 8C, Z01-. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::BIT, GbRegister::H, GbRegister::H, Execute::bit, GbFlag::B1),						//BIT 1, H: 2B 8C, Z01-. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::BIT, GbRegister::L, GbRegister::L, Execute::bit, GbFlag::B1),						//BIT 1, L: 2B 8C, Z01-. TODO
GbInstruction(AddressingMode::MemReg16_None, CpuOperation::BIT, GbRegister::HL, GbRegister::HL, Execute::bit, GbFlag::B1),				//BIT 1, (HL): 2B 16C, Z01-, TODD
GbInstruction(AddressingMode::Reg_None, CpuOperation::BIT, GbRegister::A, GbRegister::A, Execute::bit, GbFlag::B1),						//BIT 1, A: 2B 8C, Z01-. TODO

		//$5x
GbInstruction(AddressingMode::Reg_None, CpuOperation::BIT, GbRegister::B, GbRegister::B, Execute::bit, GbFlag::B2),						//BIT 2, B: 2B 8C, Z01-. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::BIT, GbRegister::C, GbRegister::C, Execute::bit, GbFlag::B2),						//BIT 2, C: 2B 8C, Z01-. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::BIT, GbRegister::D, GbRegister::D, Execute::bit, GbFlag::B2),						//BIT 2, D: 2B 8C, Z01-. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::BIT, GbRegister::E, GbRegister::E, Execute::bit, GbFlag::B2),						//BIT 2, E: 2B 8C, Z01-. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::BIT, GbRegister::H, GbRegister::H, Execute::bit, GbFlag::B2),						//BIT 2, H: 2B 8C, Z01-. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::BIT, GbRegister::L, GbRegister::L, Execute::bit, GbFlag::B2),						//BIT 2, L: 2B 8C, Z01-. TODO
GbInstruction(AddressingMode::MemReg16_None, CpuOperation::BIT, GbRegister::HL, GbRegister::HL, Execute::bit, GbFlag::B2),				//BIT 2, (HL): 2B 16C, Z01-, TODD
GbInstruction(AddressingMode::Reg_None, CpuOperation::BIT, GbRegister::A, GbRegister::A, Execute::bit, GbFlag::B2),						//BIT 2, A: 2B 8C, Z01-. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::BIT, GbRegister::B, GbRegister::B, Execute::bit, GbFlag::B3),						//BIT 3, B: 2B 8C, Z01-. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::BIT, GbRegister::C, GbRegister::C, Execute::bit, GbFlag::B3),						//BIT 3, C: 2B 8C, Z01-. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::BIT, GbRegister::D, GbRegister::D, Execute::bit, GbFlag::B3),						//BIT 3, D: 2B 8C, Z01-. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::BIT, GbRegister::E, GbRegister::E, Execute::bit, GbFlag::B3),						//BIT 3, E: 2B 8C, Z01-. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::BIT, GbRegister::H, GbRegister::H, Execute::bit, GbFlag::B3),						//BIT 3, H: 2B 8C, Z01-. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::BIT, GbRegister::L, GbRegister::L, Execute::bit, GbFlag::B3),						//BIT 3, L: 2B 8C, Z01-. TODO
GbInstruction(AddressingMode::MemReg16_None, CpuOperation::BIT, GbRegister::HL, GbRegister::HL, Execute::bit, GbFlag::B3),				//BIT 3, (HL): 2B 16C, Z01-, TODD
GbInstruction(AddressingMode::Reg_None, CpuOperation::BIT, GbRegister::A, GbRegister::A, Execute::bit, GbFlag::B3),						//BIT 3, A: 2B 8C, Z01-. TODO

		//$6x
GbInstruction(AddressingMode::Reg_None, CpuOperation::BIT, GbRegister::B, GbRegister::B, Execute::bit, GbFlag::B4),						//BIT 4, B: 2B 8C, Z01-. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::BIT, GbRegister::C, GbRegister::C, Execute::bit, GbFlag::B4),						//BIT 4, C: 2B 8C, Z01-. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::BIT, GbRegister::D, GbRegister::D, Execute::bit, GbFlag::B4),						//BIT 4, D: 2B 8C, Z01-. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::BIT, GbRegister::E, GbRegister::E, Execute::bit, GbFlag::B4),						//BIT 4, E: 2B 8C, Z01-. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::BIT, GbRegister::H, GbRegister::H, Execute::bit, GbFlag::B4),						//BIT 4, H: 2B 8C, Z01-. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::BIT, GbRegister::L, GbRegister::L, Execute::bit, GbFlag::B4),						//BIT 4, L: 2B 8C, Z01-. TODO
GbInstruction(AddressingMode::MemReg16_None, CpuOperation::BIT, GbRegister::HL, GbRegister::HL, Execute::bit, GbFlag::B4),				//BIT 4, (HL): 2B 16C, Z01-, TODD
GbInstruction(AddressingMode::Reg_None, CpuOperation::BIT, GbRegister::A, GbRegister::A, Execute::bit, GbFlag::B4),						//BIT 4, A: 2B 8C, Z01-. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::BIT, GbRegister::B, GbRegister::B, Execute::bit, GbFlag::B5),						//BIT 5, B: 2B 8C, Z01-. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::BIT, GbRegister::C, GbRegister::C, Execute::bit, GbFlag::B5),						//BIT 5, C: 2B 8C, Z01-. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::BIT, GbRegister::D, GbRegister::D, Execute::bit, GbFlag::B5),						//BIT 5, D: 2B 8C, Z01-. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::BIT, GbRegister::E, GbRegister::E, Execute::bit, GbFlag::B5),						//BIT 5, E: 2B 8C, Z01-. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::BIT, GbRegister::H, GbRegister::H, Execute::bit, GbFlag::B5),						//BIT 5, H: 2B 8C, Z01-. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::BIT, GbRegister::L, GbRegister::L, Execute::bit, GbFlag::B5),						//BIT 5, L: 2B 8C, Z01-. TODO
GbInstruction(AddressingMode::MemReg16_None, CpuOperation::BIT, GbRegister::HL, GbRegister::HL, Execute::bit, GbFlag::B5),				//BIT 5, (HL): 2B 16C, Z01-, TODD
GbInstruction(AddressingMode::Reg_None, CpuOperation::BIT, GbRegister::A, GbRegister::A, Execute::bit, GbFlag::B5),						//BIT 5, A: 2B 8C, Z01-. TODO

		//$7x
GbInstruction(AddressingMode::Reg_None, CpuOperation::BIT, GbRegister::B, GbRegister::B, Execute::bit, GbFlag::B6),						//BIT 6, B: 2B 8C, Z01-. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::BIT, GbRegister::C, GbRegister::C, Execute::bit, GbFlag::B6),						//BIT 6, C: 2B 8C, Z01-. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::BIT, GbRegister::D, GbRegister::D, Execute::bit, GbFlag::B6),						//BIT 6, D: 2B 8C, Z01-. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::BIT, GbRegister::E, GbRegister::E, Execute::bit, GbFlag::B6),						//BIT 6, E: 2B 8C, Z01-. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::BIT, GbRegister::H, GbRegister::H, Execute::bit, GbFlag::B6),						//BIT 6, H: 2B 8C, Z01-. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::BIT, GbRegister::L, GbRegister::L, Execute::bit, GbFlag::B6),						//BIT 6, L: 2B 8C, Z01-. TODO
GbInstruction(AddressingMode::MemReg16_None, CpuOperation::BIT, GbRegister::HL, GbRegister::HL, Execute::bit, GbFlag::B6),				//BIT 6, (HL): 2B 16C, Z01-, TODD
GbInstruction(AddressingMode::Reg_None, CpuOperation::BIT, GbRegister::A, GbRegister::A, Execute::bit, GbFlag::B6),						//BIT 6, A: 2B 8C, Z01-. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::BIT, GbRegister::B, GbRegister::B, Execute::bit, GbFlag::B7),						//BIT 7, B: 2B 8C, Z01-. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::BIT, GbRegister::C, GbRegister::C, Execute::bit, GbFlag::B7),						//BIT 7, C: 2B 8C, Z01-. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::BIT, GbRegister::D, GbRegister::D, Execute::bit, GbFlag::B7),						//BIT 7, D: 2B 8C, Z01-. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::BIT, GbRegister::E, GbRegister::E, Execute::bit, GbFlag::B7),						//BIT 7, E: 2B 8C, Z01-. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::BIT, GbRegister::H, GbRegister::H, Execute::bit, GbFlag::B7),						//BIT 7, H: 2B 8C, Z01-. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::BIT, GbRegister::L, GbRegister::L, Execute::bit, GbFlag::B7),						//BIT 7, L: 2B 8C, Z01-. TODO
GbInstruction(AddressingMode::MemReg16_None, CpuOperation::BIT, GbRegister::HL, GbRegister::HL, Execute::bit, GbFlag::B7),				//BIT 7, (HL): 2B 16C, Z01-, TODD
GbInstruction(AddressingMode::Reg_None, CpuOperation::BIT, GbRegister::A, GbRegister::A, Execute::bit, GbFlag::B7),						//BIT 7, A: 2B 8C, Z01-. TODO

		//$8x
GbInstruction(AddressingMode::Reg_None, CpuOperation::RES, GbRegister::B, GbRegister::B, Execute::res, GbFlag::B0),						//RES 0, B: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RES, GbRegister::C, GbRegister::C, Execute::res, GbFlag::B0),						//RES 0, C: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RES, GbRegister::D, GbRegister::D, Execute::res, GbFlag::B0),						//RES 0, D: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RES, GbRegister::E, GbRegister::E, Execute::res, GbFlag::B0),						//RES 0, E: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RES, GbRegister::H, GbRegister::H, Execute::res, GbFlag::B0),						//RES 0, H: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RES, GbRegister::L, GbRegister::L, Execute::res, GbFlag::B0),						//RES 0, L: 2B 8C, ----. TODO
GbInstruction(AddressingMode::MemReg16_None, CpuOperation::RES, GbRegister::HL, GbRegister::HL, Execute::res, GbFlag::B0),				//RES 0, (HL): 2B 16C, ----, TODD
GbInstruction(AddressingMode::Reg_None, CpuOperation::RES, GbRegister::A, GbRegister::A, Execute::res, GbFlag::B0),						//RES 0, A: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RES, GbRegister::B, GbRegister::B, Execute::res, GbFlag::B1),						//RES 1, B: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RES, GbRegister::C, GbRegister::C, Execute::res, GbFlag::B1),						//RES 1, C: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RES, GbRegister::D, GbRegister::D, Execute::res, GbFlag::B1),						//RES 1, D: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RES, GbRegister::E, GbRegister::E, Execute::res, GbFlag::B1),						//RES 1, E: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RES, GbRegister::H, GbRegister::H, Execute::res, GbFlag::B1),						//RES 1, H: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RES, GbRegister::L, GbRegister::L, Execute::res, GbFlag::B1),						//RES 1, L: 2B 8C, ----. TODO
GbInstruction(AddressingMode::MemReg16_None, CpuOperation::RES, GbRegister::HL, GbRegister::HL, Execute::res, GbFlag::B1),				//RES 1, (HL): 2B 16C, ----, TODD
GbInstruction(AddressingMode::Reg_None, CpuOperation::RES, GbRegister::A, GbRegister::A, Execute::res, GbFlag::B1),						//RES 1, A: 2B 8C, ----. TODO

		//$9x
GbInstruction(AddressingMode::Reg_None, CpuOperation::RES, GbRegister::B, GbRegister::B, Execute::res, GbFlag::B2),						//RES 2, B: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RES, GbRegister::C, GbRegister::C, Execute::res, GbFlag::B2),						//RES 2, C: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RES, GbRegister::D, GbRegister::D, Execute::res, GbFlag::B2),						//RES 2, D: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RES, GbRegister::E, GbRegister::E, Execute::res, GbFlag::B2),						//RES 2, E: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RES, GbRegister::H, GbRegister::H, Execute::res, GbFlag::B2),						//RES 2, H: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RES, GbRegister::L, GbRegister::L, Execute::res, GbFlag::B2),						//RES 2, L: 2B 8C, ----. TODO
GbInstruction(AddressingMode::MemReg16_None, CpuOperation::RES, GbRegister::HL, GbRegister::HL, Execute::res, GbFlag::B2),				//RES 2, (HL): 2B 16C, ----, TODD
GbInstruction(AddressingMode::Reg_None, CpuOperation::RES, GbRegister::A, GbRegister::A, Execute::res, GbFlag::B2),						//RES 2, A: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RES, GbRegister::B, GbRegister::B, Execute::res, GbFlag::B3),						//RES 3, B: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RES, GbRegister::C, GbRegister::C, Execute::res, GbFlag::B3),						//RES 3, C: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RES, GbRegister::D, GbRegister::D, Execute::res, GbFlag::B3),						//RES 3, D: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RES, GbRegister::E, GbRegister::E, Execute::res, GbFlag::B3),						//RES 3, E: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RES, GbRegister::H, GbRegister::H, Execute::res, GbFlag::B3),						//RES 3, H: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RES, GbRegister::L, GbRegister::L, Execute::res, GbFlag::B3),						//RES 3, L: 2B 8C, ----. TODO
GbInstruction(AddressingMode::MemReg16_None, CpuOperation::RES, GbRegister::HL, GbRegister::HL, Execute::res, GbFlag::B3),				//RES 3, (HL): 2B 16C, ----, TODD
GbInstruction(AddressingMode::Reg_None, CpuOperation::RES, GbRegister::A, GbRegister::A, Execute::res, GbFlag::B3),						//RES 3, A: 2B 8C, ----. TODO

		//$Ax
GbInstruction(AddressingMode::Reg_None, CpuOperation::RES, GbRegister::B, GbRegister::B, Execute::res, GbFlag::B4),						//RES 4, B: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RES, GbRegister::C, GbRegister::C, Execute::res, GbFlag::B4),						//RES 4, C: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RES, GbRegister::D, GbRegister::D, Execute::res, GbFlag::B4),						//RES 4, D: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RES, GbRegister::E, GbRegister::E, Execute::res, GbFlag::B4),						//RES 4, E: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RES, GbRegister::H, GbRegister::H, Execute::res, GbFlag::B4),						//RES 4, H: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RES, GbRegister::L, GbRegister::L, Execute::res, GbFlag::B4),						//RES 4, L: 2B 8C, ----. TODO
GbInstruction(AddressingMode::MemReg16_None, CpuOperation::RES, GbRegister::HL, GbRegister::HL, Execute::res, GbFlag::B4),				//RES 4, (HL): 2B 16C, ----, TODD
GbInstruction(AddressingMode::Reg_None, CpuOperation::RES, GbRegister::A, GbRegister::A, Execute::res, GbFlag::B4),						//RES 4, A: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RES, GbRegister::B, GbRegister::B, Execute::res, GbFlag::B5),						//RES 5, B: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RES, GbRegister::C, GbRegister::C, Execute::res, GbFlag::B5),						//RES 5, C: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RES, GbRegister::D, GbRegister::D, Execute::res, GbFlag::B5),						//RES 5, D: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RES, GbRegister::E, GbRegister::E, Execute::res, GbFlag::B5),						//RES 5, E: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RES, GbRegister::H, GbRegister::H, Execute::res, GbFlag::B5),						//RES 5, H: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RES, GbRegister::L, GbRegister::L, Execute::res, GbFlag::B5),						//RES 5, L: 2B 8C, ----. TODO
GbInstruction(AddressingMode::MemReg16_None, CpuOperation::RES, GbRegister::HL, GbRegister::HL, Execute::res, GbFlag::B5),				//RES 5, (HL): 2B 16C, ----, TODD
GbInstruction(AddressingMode::Reg_None, CpuOperation::RES, GbRegister::A, GbRegister::A, Execute::res, GbFlag::B5),						//RES 5, A: 2B 8C, ----. TODO

		//$Bx
GbInstruction(AddressingMode::Reg_None, CpuOperation::RES, GbRegister::B, GbRegister::B, Execute::res, GbFlag::B4),						//RES 6, B: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RES, GbRegister::C, GbRegister::C, Execute::res, GbFlag::B4),						//RES 6, C: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RES, GbRegister::D, GbRegister::D, Execute::res, GbFlag::B4),						//RES 6, D: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RES, GbRegister::E, GbRegister::E, Execute::res, GbFlag::B4),						//RES 6, E: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RES, GbRegister::H, GbRegister::H, Execute::res, GbFlag::B4),						//RES 6, H: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RES, GbRegister::L, GbRegister::L, Execute::res, GbFlag::B4),						//RES 6, L: 2B 8C, ----. TODO
GbInstruction(AddressingMode::MemReg16_None, CpuOperation::RES, GbRegister::HL, GbRegister::HL, Execute::res, GbFlag::B4),				//RES 6, (HL): 2B 16C, ----, TODD
GbInstruction(AddressingMode::Reg_None, CpuOperation::RES, GbRegister::A, GbRegister::A, Execute::res, GbFlag::B4),						//RES 6, A: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RES, GbRegister::B, GbRegister::B, Execute::res, GbFlag::B7),						//RES 7, B: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RES, GbRegister::C, GbRegister::C, Execute::res, GbFlag::B7),						//RES 7, C: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RES, GbRegister::D, GbRegister::D, Execute::res, GbFlag::B7),						//RES 7, D: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RES, GbRegister::E, GbRegister::E, Execute::res, GbFlag::B7),						//RES 7, E: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RES, GbRegister::H, GbRegister::H, Execute::res, GbFlag::B7),						//RES 7, H: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RES, GbRegister::L, GbRegister::L, Execute::res, GbFlag::B7),						//RES 7, L: 2B 8C, ----. TODO
GbInstruction(AddressingMode::MemReg16_None, CpuOperation::RES, GbRegister::HL, GbRegister::HL, Execute::res, GbFlag::B7),				//RES 7, (HL): 2B 16C, ----, TODD
GbInstruction(AddressingMode::Reg_None, CpuOperation::RES, GbRegister::A, GbRegister::A, Execute::res, GbFlag::B7),						//RES 7, A: 2B 8C, ----. TODO

		//$Cx
GbInstruction(AddressingMode::Reg_None, CpuOperation::SET, GbRegister::B, GbRegister::B, Execute::set, GbFlag::B0),						//SET 0, B: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::SET, GbRegister::C, GbRegister::C, Execute::set, GbFlag::B0),						//SET 0, C: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::SET, GbRegister::D, GbRegister::D, Execute::set, GbFlag::B0),						//SET 0, D: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::SET, GbRegister::E, GbRegister::E, Execute::set, GbFlag::B0),						//SET 0, E: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::SET, GbRegister::H, GbRegister::H, Execute::set, GbFlag::B0),						//SET 0, H: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::SET, GbRegister::L, GbRegister::L, Execute::set, GbFlag::B0),						//SET 0, L: 2B 8C, ----. TODO
GbInstruction(AddressingMode::MemReg16_None, CpuOperation::SET, GbRegister::HL, GbRegister::HL, Execute::set, GbFlag::B0),				//SET 0, (HL): 2B 16C, ----, TODD
GbInstruction(AddressingMode::Reg_None, CpuOperation::SET, GbRegister::A, GbRegister::A, Execute::set, GbFlag::B0),						//SET 0, A: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::SET, GbRegister::B, GbRegister::B, Execute::set, GbFlag::B1),						//SET 1, B: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::SET, GbRegister::C, GbRegister::C, Execute::set, GbFlag::B1),						//SET 1, C: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::SET, GbRegister::D, GbRegister::D, Execute::set, GbFlag::B1),						//SET 1, D: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::SET, GbRegister::E, GbRegister::E, Execute::set, GbFlag::B1),						//SET 1, E: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::SET, GbRegister::H, GbRegister::H, Execute::set, GbFlag::B1),						//SET 1, H: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::SET, GbRegister::L, GbRegister::L, Execute::set, GbFlag::B1),						//SET 1, L: 2B 8C, ----. TODO
GbInstruction(AddressingMode::MemReg16_None, CpuOperation::SET, GbRegister::HL, GbRegister::HL, Execute::set, GbFlag::B1),				//SET 1, (HL): 2B 16C, ----, TODD
GbInstruction(AddressingMode::Reg_None, CpuOperation::SET, GbRegister::A, GbRegister::A, Execute::set, GbFlag::B1),						//SET 1, A: 2B 8C, ----. TODO

		//$Dx
GbInstruction(AddressingMode::Reg_None, CpuOperation::SET, GbRegister::B, GbRegister::B, Execute::set, GbFlag::B2),						//SET 2, B: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::SET, GbRegister::C, GbRegister::C, Execute::set, GbFlag::B2),						//SET 2, C: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::SET, GbRegister::D, GbRegister::D, Execute::set, GbFlag::B2),						//SET 2, D: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::SET, GbRegister::E, GbRegister::E, Execute::set, GbFlag::B2),						//SET 2, E: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::SET, GbRegister::H, GbRegister::H, Execute::set, GbFlag::B2),						//SET 2, H: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::SET, GbRegister::L, GbRegister::L, Execute::set, GbFlag::B2),						//SET 2, L: 2B 8C, ----. TODO
GbInstruction(AddressingMode::MemReg16_None, CpuOperation::SET, GbRegister::HL, GbRegister::HL, Execute::set, GbFlag::B2),				//SET 2, (HL): 2B 16C, ----, TODD
GbInstruction(AddressingMode::Reg_None, CpuOperation::SET, GbRegister::A, GbRegister::A, Execute::set, GbFlag::B2),						//SET 2, A: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::SET, GbRegister::B, GbRegister::B, Execute::set, GbFlag::B3),						//SET 3, B: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::SET, GbRegister::C, GbRegister::C, Execute::set, GbFlag::B3),						//SET 3, C: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::SET, GbRegister::D, GbRegister::D, Execute::set, GbFlag::B3),						//SET 3, D: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::SET, GbRegister::E, GbRegister::E, Execute::set, GbFlag::B3),						//SET 3, E: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::SET, GbRegister::H, GbRegister::H, Execute::set, GbFlag::B3),						//SET 3, H: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::SET, GbRegister::L, GbRegister::L, Execute::set, GbFlag::B3),						//SET 3, L: 2B 8C, ----. TODO
GbInstruction(AddressingMode::MemReg16_None, CpuOperation::SET, GbRegister::HL, GbRegister::HL, Execute::set, GbFlag::B3),				//SET 3, (HL): 2B 16C, ----, TODD
GbInstruction(AddressingMode::Reg_None, CpuOperation::SET, GbRegister::A, GbRegister::A, Execute::set, GbFlag::B3),						//SET 3, A: 2B 8C, ----. TODO

		//$Ex
GbInstruction(AddressingMode::Reg_None, CpuOperation::SET, GbRegister::B, GbRegister::B, Execute::set, GbFlag::B4),						//SET 4, B: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::SET, GbRegister::C, GbRegister::C, Execute::set, GbFlag::B4),						//SET 4, C: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::SET, GbRegister::D, GbRegister::D, Execute::set, GbFlag::B4),						//SET 4, D: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::SET, GbRegister::E, GbRegister::E, Execute::set, GbFlag::B4),						//SET 4, E: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::SET, GbRegister::H, GbRegister::H, Execute::set, GbFlag::B4),						//SET 4, H: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::SET, GbRegister::L, GbRegister::L, Execute::set, GbFlag::B4),						//SET 4, L: 2B 8C, ----. TODO
GbInstruction(AddressingMode::MemReg16_None, CpuOperation::SET, GbRegister::HL, GbRegister::HL, Execute::set, GbFlag::B4),				//SET 4, (HL): 2B 16C, ----, TODD
GbInstruction(AddressingMode::Reg_None, CpuOperation::SET, GbRegister::A, GbRegister::A, Execute::set, GbFlag::B4),						//SET 4, A: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::SET, GbRegister::B, GbRegister::B, Execute::set, GbFlag::B5),						//SET 5, B: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::SET, GbRegister::C, GbRegister::C, Execute::set, GbFlag::B5),						//SET 5, C: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::SET, GbRegister::D, GbRegister::D, Execute::set, GbFlag::B5),						//SET 5, D: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::SET, GbRegister::E, GbRegister::E, Execute::set, GbFlag::B5),						//SET 5, E: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::SET, GbRegister::H, GbRegister::H, Execute::set, GbFlag::B5),						//SET 5, H: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::SET, GbRegister::L, GbRegister::L, Execute::set, GbFlag::B5),						//SET 5, L: 2B 8C, ----. TODO
GbInstruction(AddressingMode::MemReg16_None, CpuOperation::SET, GbRegister::HL, GbRegister::HL, Execute::set, GbFlag::B5),				//SET 5, (HL): 2B 16C, ----, TODD
GbInstruction(AddressingMode::Reg_None, CpuOperation::SET, GbRegister::A, GbRegister::A, Execute::set, GbFlag::B5),						//SET 5, A: 2B 8C, ----. TODO

		//$Fx
GbInstruction(AddressingMode::Reg_None, CpuOperation::SET, GbRegister::B, GbRegister::B, Execute::set, GbFlag::B6),						//SET 6, B: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::SET, GbRegister::C, GbRegister::C, Execute::set, GbFlag::B6),						//SET 6, C: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::SET, GbRegister::D, GbRegister::D, Execute::set, GbFlag::B6),						//SET 6, D: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::SET, GbRegister::E, GbRegister::E, Execute::set, GbFlag::B6),						//SET 6, E: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::SET, GbRegister::H, GbRegister::H, Execute::set, GbFlag::B6),						//SET 6, H: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::SET, GbRegister::L, GbRegister::L, Execute::set, GbFlag::B6),						//SET 6, L: 2B 8C, ----. TODO
GbInstruction(AddressingMode::MemReg16_None, CpuOperation::SET, GbRegister::HL, GbRegister::HL, Execute::set, GbFlag::B6),				//SET 6, (HL): 2B 16C, ----, TODD
GbInstruction(AddressingMode::Reg_None, CpuOperation::SET, GbRegister::A, GbRegister::A, Execute::set, GbFlag::B6),						//SET 6, A: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::SET, GbRegister::B, GbRegister::B, Execute::set, GbFlag::B7),						//SET 7, B: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::SET, GbRegister::C, GbRegister::C, Execute::set, GbFlag::B7),						//SET 7, C: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::SET, GbRegister::D, GbRegister::D, Execute::set, GbFlag::B7),						//SET 7, D: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::SET, GbRegister::E, GbRegister::E, Execute::set, GbFlag::B7),						//SET 7, E: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::SET, GbRegister::H, GbRegister::H, Execute::set, GbFlag::B7),						//SET 7, H: 2B 8C, ----. TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::SET, GbRegister::L, GbRegister::L, Execute::set, GbFlag::B7),						//SET 7, L: 2B 8C, ----. TODO
GbInstruction(AddressingMode::MemReg16_None, CpuOperation::SET, GbRegister::HL, GbRegister::HL, Execute::set, GbFlag::B7),				//SET 7, (HL): 2B 16C, ----, TODD
GbInstruction(AddressingMode::Reg_None, CpuOperation::SET, GbRegister::A, GbRegister::A, Execute::set, GbFlag::B7),						//SET 7, A: 2B 8C, ----. TODO

};

	//Objects and Object Handles.
	RegisterFile regFile;
	MMU* mem;
	//Methods
public:
	Execute();
	~Execute();
	uint8_t executeInstruction(uint8_t* instructionBytes, uint8_t &pcInc);
private:
	GbInstruction decodeInstruction(uint8_t* instructionBytes);
	static bool decodePrefixInstruction(void* instance, GbInstruction inst, uint8_t* instBytes, uint8_t &pcInc);
	//Functions to execute each Instruction.

	static bool load(void* instance, GbInstruction inst, uint8_t* instBytes, uint8_t &pcInc);
	static bool inc(void* instance, GbInstruction inst, uint8_t* instBytes, uint8_t &pcInc);
	static bool dec(void* instance, GbInstruction inst, uint8_t* instBytes, uint8_t &pcInc);
	static bool add(void* instance, GbInstruction inst, uint8_t* instBytes, uint8_t &pcInc);
	static bool adc(void* instance, GbInstruction inst, uint8_t* instBytes, uint8_t &pcInc);
	static bool sub(void* instance, GbInstruction inst, uint8_t* instBytes, uint8_t &pcInc);
	static bool sbc(void* instance, GbInstruction inst, uint8_t* instBytes, uint8_t &pcInc);
	static bool bwAnd(void* instance, GbInstruction inst, uint8_t* instBytes, uint8_t &pcInc);
	static bool bwXor(void* instance, GbInstruction inst, uint8_t* instBytes, uint8_t &pcInc);
	static bool bwOr(void* instance, GbInstruction inst, uint8_t* instBytes, uint8_t &pcInc);
	static bool bit(void* instance, GbInstruction inst, uint8_t* instBytes, uint8_t &pcInc);
	static bool res(void* instance, GbInstruction inst, uint8_t* instBytes, uint8_t &pcInc);
	static bool set(void* instance, GbInstruction inst, uint8_t* instBytes, uint8_t &pcInc);
};
