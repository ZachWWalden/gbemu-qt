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
GbInstruction(AddressingMode::NONE_NONE, CpuOperation::NOP, GbRegister::A, GbRegister::A, Execute::nop),						//NOP: 1B, 4C, ----, TODO
GbInstruction(AddressingMode::Reg16_Imm16, CpuOperation::LD, GbRegister::BC, GbRegister::NONE, Execute::load),					//LD BC ,d16: 3B, 12C, ----, TODO
GbInstruction(AddressingMode::MemReg16_Reg, CpuOperation::LD, GbRegister::BC, GbRegister::A, Execute::load),					//LD (BC) ,A: 1B, 8C, ----, TODO
GbInstruction(AddressingMode::Reg16_None, CpuOperation::INC, GbRegister::BC, GbRegister::NONE, Execute::inc),					//INC BC: 1B, 8C, ----
GbInstruction(AddressingMode::Reg_None, CpuOperation::INC, GbRegister::B, GbRegister::NONE, Execute::inc),						//INC B: 1B, 4C, Z0H-
GbInstruction(AddressingMode::Reg_None, CpuOperation::DEC, GbRegister::B, GbRegister::NONE, Execute::dec),						//DEC B: 1B, 4C, Z1H-
GbInstruction(AddressingMode::Reg_Imm8, CpuOperation::LD, GbRegister::B, GbRegister::NONE, Execute::load),						//LD B d8: 2B, 8C, ----, TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RLCA, GbRegister::A, GbRegister::NONE, Execute::rotate),					//RLCA: 1B, 4C, 000C, TODO
GbInstruction(AddressingMode::MemImm16_Reg16, CpuOperation::LD, GbRegister::NONE, GbRegister::SP, Execute::load),				//LD (a16), SP: 3B 20C, ----, TODO
GbInstruction(AddressingMode::Reg16_Reg16, CpuOperation::ADD, GbRegister::HL, GbRegister::BC, Execute::add),					//ADD HL, BC: 1B 8C, -0HC
GbInstruction(AddressingMode::Reg_MemReg16, CpuOperation::LD, GbRegister::A, GbRegister::BC, Execute::load),					//LD A, (BC): 1B 8C, ----, TODO
GbInstruction(AddressingMode::Reg16_NONE, CpuOperation::DEC, GbRegister::BC, GbRegister::NONE, Execute::dec),					//DEC BC: 1B 8C, ----
GbInstruction(AddressingMode::Reg_None, CpuOperation::INC, GbRegister::C, GbRegister::NONE, Execute::inc),						//INC C: 1B, 4C, Z0H-
GbInstruction(AddressingMode::Reg_None, CpuOperation::DEC, GbRegister::C, GbRegister::NONE, Execute::dec),						//DEC C: 1B, 4C, Z1H-
GbInstruction(AddressingMode::Reg_Imm8, CpuOperation::LD, GbRegister::C, GbRegister::NONE, Execute::load),						//LD C, d8: 2B, 8C, ----, TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RRCA, GbRegister::A, GbRegister::NONE, Execute::rotate),					//RRCA: 1B 4C, 000C, TODO
		//$1x
GbInstruction(AddressingMode::NONE_NONE, CpuOperation::STOP,ZwReg::NONE,ZwReg::NONE, &Execute::stop),							//STOP 0: 2B 4C, ----, TODO
GbInstruction(AddressingMode::Reg16_Imm16, CpuOperation::LD, GbRegister::DE, GbRegister::NONE, Execute::load),					//LD BC ,d16: 3B, 12C, ----, TODO
GbInstruction(AddressingMode::MemReg16_Reg, CpuOperation::LD, GbRegister::DE, GbRegister::A, Execute::load),					//LD (DE), A: 1B 8C, ----, TODO
GbInstruction(AddressingMode::Reg16_None, CpuOperation::INC, GbRegister::DE, GbRegister::NONE, Execute::inc),					//INC DE, 1B, 8C, ----, TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::INC, GbRegister::D, GbRegister::NONE, Execute::inc),						//INC D: 1B, 4C, Z0H-
GbInstruction(AddressingMode::Reg_None, CpuOperation::DEC, GbRegister::D, GbRegister::NONE, Execute::dec),						//DEC D: 1B, 4C, Z1H-
GbInstruction(AddressingMode::Reg_Imm8, CpuOperation::LD, GbRegister::D, GbRegister::NONE, Execute::load),						//LD D d8: 2B, 8C, ----, TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RLA, GbRegister::A, GbRegister::NONE, Execute::rotate),					//RLA: 1B 4C, 000C, TODO
GbInstruction(AddressingMode::Reg16_Simm8, CpuOperation::JR, GbRegister::PC, GbRegister::NONE, Execute::jr, GbFlag::T),			//JR r8: 2B 12C, ----, TODO
GbInstruction(AddressingMode::Reg16_Reg16, CpuOperation::ADD, GbRegister::HL, GbRegister::DE, Execute::add),					//ADD HL, DE: 1B 8C, -0HC
GbInstruction(AddressingMode::Reg_MemReg16, CpuOperation::LD, GbRegister::A, GbRegister::DE, Execute::load),					//LD A, (DE): 1B 8C, ----, TODO
GbInstruction(AddressingMode::Reg16_NONE, CpuOperation::DEC, GbRegister::DE, GbRegister::NONE, Execute::dec),					//DEC DE: 1B 8C, ----
GbInstruction(AddressingMode::Reg_None, CpuOperation::INC, GbRegister::E, GbRegister::NONE, Execute::inc),						//INC E: 1B, 4C, Z0H-
GbInstruction(AddressingMode::Reg_None, CpuOperation::DEC, GbRegister::E, GbRegister::NONE, Execute::dec),						//DEC E: 1B, 4C, Z1H-
GbInstruction(AddressingMode::Reg_Imm8, CpuOperation::LD, GbRegister::E, GbRegister::NONE, Execute::load),						//LD E, d8: 2B, 8C, ----, TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::RRA, GbRegister::A, GbRegister::NONE, Execute::rotate),					//RRA: 1B 4C, 000C, TODO

		//$2x
GbInstruction(AddressingMode::Reg16_Simm8, CpuOperation::JR, GbRegister::PC, GbRegister::NONE, Execute::jr, GbFlag::NZ),		//JR NZ, r8: 2B 12/8C, ----, TODO
GbInstruction(AddressingMode::Reg16_Imm16, CpuOperation::LD, GbRegister::HL, GbRegister::NONE, Execute::load),					//LD HL ,d16: 3B, 12C, ----, TODO
GbInstruction(AddressingMode::MemReg16_Reg, CpuOperation::LD, GbRegister::HL, GbRegister::A, Execute::load, GbFlag::PoI),		//LD (HL+),A: 1B 8C, ----, TODO
GbInstruction(AddressingMode::Reg16_None, CpuOperation::INC, GbRegister::HL, GbRegister::NONE, Execute::inc),					//INC HL, 1B, 8C, ----, TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::INC, GbRegister::H, GbRegister::NONE, Execute::inc),						//INC H: 1B, 4C, Z0H-
GbInstruction(AddressingMode::Reg_None, CpuOperation::DEC, GbRegister::H, GbRegister::NONE, Execute::dec),						//DEC H: 1B, 4C, Z1H-
GbInstruction(AddressingMode::Reg_Imm8, CpuOperation::LD, GbRegister::H, GbRegister::NONE, Execute::load),						//LD H d8: 2B, 8C, ----, TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::DAA, GbRegister::A, GbRegister::NONE, Execute::weird),					//DAA: 1B 4C, Z-0C, TODO
GbInstruction(AddressingMode::Reg16_Simm8, CpuOperation::JR, GbRegister::PC, GbRegister::NONE, Execute::jr, GbFlag::Z),			//JR Z, r8: 2B 12/8C, ----, TODO
GbInstruction(AddressingMode::Reg16_Reg16, CpuOperation::ADD, GbRegister::HL, GbRegister::HL, Execute::add),					//ADD HL, HL: 1B 8C, -0HC
GbInstruction(AddressingMode::Reg_MemReg16, CpuOperation::LD, GbRegister::A, GbRegister::HL, Execute::load, GbFlag::PoI),		//LD A, (HL+): 1B 8C, ----, TODO
GbInstruction(AddressingMode::Reg16_NONE, CpuOperation::DEC, GbRegister::HL, GbRegister::NONE, Execute::dec),					//DEC HL: 1B 8C, ----
GbInstruction(AddressingMode::Reg_None, CpuOperation::INC, GbRegister::L, GbRegister::NONE, Execute::dec),						//DEC L: 1B, 4C, Z0H-
GbInstruction(AddressingMode::Reg_None, CpuOperation::DEC, GbRegister::L, GbRegister::NONE, Execute::dec),						//DEC L: 1B, 4C, Z1H-
GbInstruction(AddressingMode::Reg_Imm8, CpuOperation::LD, GbRegister::L, GbRegister::NONE, Execute::load),						//LD L, d8: 2B, 8C, ----, TODO
GbInstruction(AddressingMode::Reg_None, CpuOperation::CPL, GbRegister::A, GbRegister::NONE, Execute::weird),					//CPL, 1B, 4C, -11-, TODO

		//$3x
GbInstruction(AddressingMode::Reg16_Simm8, CpuOperation::JR, GbRegister::PC, GbRegister::NONE, Execute::jr, GbFlag::NC),		//JR NC, r8: 2B 12/8C, ----, TODO
GbInstruction(AddressingMode::Reg16_Imm16, CpuOperation::LD, GbRegister::SP, GbRegister::NONE, Execute::load),					//LD SP ,d16: 3B, 12C, ----, TODO
GbInstruction(AddressingMode::MemReg16_Reg, CpuOperation::LD, GbRegister::HL, GbRegister::A, Execute::load, GbFlag::PoD),		//LD (HL-),A: 1B 8C, ----, TODO
GbInstruction(AddressingMode::Reg16_None, CpuOperation::INC, GbRegister::SP, GbRegister::NONE, Execute::inc),					//INC SP, 1B, 8C, ----, TODO
GbInstruction(AddressingMode::MemReg16_None, CpuOperation::INC, GbRegister::HL, GbRegister::NONE, Execute::inc),				//INC (HL): 1B, 12C, Z0H-
GbInstruction(AddressingMode::MemReg16_None, CpuOperation::DEC, GbRegister::HL, GbRegister::NONE, Execute::dec),				//DEC (HL): 1B, 12C, Z1H-
GbInstruction(AddressingMode::MemReg16_Imm8, CpuOperation::LD, GbRegister::HL, GbRegister::NONE, Execute::load),				//LD (HL) d8: 2B, 8C, ----, TODO
GbInstruction(AddressingMode::NONE_NONE, CpuOperation::SCF, GbRegister::NONE, GbRegister::NONE, Execute::weird),				//SCF: 1B 4C, -001, TODO
GbInstruction(AddressingMode::Reg16_Simm8, CpuOperation::JR, GbRegister::PC, GbRegister::NONE, Execute::jr, GbFlag::C),			//JR C, r8: 2B 12/8C, ----, TODO
GbInstruction(AddressingMode::Reg16_Reg16, CpuOperation::ADD, GbRegister::HL, GbRegister::SP, Execute::add),					//ADD HL, SP: 1B 8C, -0HC
GbInstruction(AddressingMode::Reg_MemReg16, CpuOperation::LD, GbRegister::A, GbRegister::HL, Execute::load, GbFlag::PoD),		//LD A, (HL-): 1B 8C, ----, TODO
GbInstruction(AddressingMode::Reg16_NONE, CpuOperation::DEC, GbRegister::SP, GbRegister::NONE, Execute::dec),					//DEC SP: 1B 8C, ----
GbInstruction(AddressingMode::Reg_None, CpuOperation::INC, GbRegister::A, GbRegister::NONE, Execute::inc),						//INC A: 1B, 4C, Z0H-
GbInstruction(AddressingMode::Reg_None, CpuOperation::DEC, GbRegister::A, GbRegister::NONE, Execute::dec),						//DEC A: 1B, 4C, Z1H-
GbInstruction(AddressingMode::Reg_Imm8, CpuOperation::LD, GbRegister::A, GbRegister::NONE, Execute::load),						//LD A, d8: 2B, 8C, ----, TODO
GbInstruction(AddressingMode::NONE_NONE, CpuOperation::CCF, GbRegister::NONE, GbRegister::NONE, Execute::weird),				//CCF: 1B 4C, -001, TODO

		//$4x
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::B, GbRegister::B, Execute::load),							//LD B, B: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::B, GbRegister::C, Execute::load),							//LD B, C: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::B, GbRegister::D, Execute::load),							//LD B, D: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::B, GbRegister::E, Execute::load),							//LD B, E: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::B, GbRegister::H, Execute::load),							//LD B, H: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::B, GbRegister::L, Execute::load),							//LD B, L: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_MemReg16, CpuOperation::LD, GbRegister::B, GbRegister::HL, Execute::load, GbFlag::NONE),		//LD B, (HL): 1B 8C, ----, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::B, GbRegister::A, Execute::load),							//LD B, A: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::C, GbRegister::B, Execute::load),							//LD C, B: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::C, GbRegister::C, Execute::load),							//LD C, C: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::C, GbRegister::D, Execute::load),							//LD C, D: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::C, GbRegister::E, Execute::load),							//LD C, E: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::C, GbRegister::H, Execute::load),							//LD C, H: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::C, GbRegister::L, Execute::load),							//LD C, L: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_MemReg16, CpuOperation::LD, GbRegister::C, GbRegister::HL, Execute::load, GbFlag::NONE),		//LD C, (HL): 1B 8C, ----, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::C, GbRegister::A, Execute::load),							//LD C, A: 1B 4C, Z0HC, TODO

		//$5x
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::D, GbRegister::B, Execute::load),							//LD D, B: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::D, GbRegister::C, Execute::load),							//LD D, C: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::D, GbRegister::D, Execute::load),							//LD D, D: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::D, GbRegister::E, Execute::load),							//LD D, E: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::D, GbRegister::H, Execute::load),							//LD D, H: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::D, GbRegister::L, Execute::load),							//LD D, L: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_MemReg16, CpuOperation::LD, GbRegister::D, GbRegister::HL, Execute::load, GbFlag::NONE),		//LD D, (HL): 1B 8C, ----, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::D, GbRegister::A, Execute::load),							//LD D, A: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::E, GbRegister::B, Execute::load),							//LD E, B: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::E, GbRegister::C, Execute::load),							//LD E, C: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::E, GbRegister::D, Execute::load),							//LD E, D: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::E, GbRegister::E, Execute::load),							//LD E, E: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::E, GbRegister::H, Execute::load),							//LD E, H: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::E, GbRegister::L, Execute::load),							//LD E, L: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_MemReg16, CpuOperation::LD, GbRegister::E, GbRegister::HL, Execute::load, GbFlag::NONE),		//LD E, (HL): 1B 8C, ----, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::E, GbRegister::A, Execute::load),							//LD E, A: 1B 4C, Z0HC, TODO

		//$6x
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::H, GbRegister::B, Execute::load),							//LD H, B: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::H, GbRegister::C, Execute::load),							//LD H, C: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::H, GbRegister::D, Execute::load),							//LD H, D: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::H, GbRegister::E, Execute::load),							//LD H, E: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::H, GbRegister::H, Execute::load),							//LD H, H: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::H, GbRegister::L, Execute::load),							//LD H, L: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_MemReg16, CpuOperation::LD, GbRegister::H, GbRegister::HL, Execute::load, GbFlag::NONE),		//LD H, (HL): 1B 8C, ----, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::H, GbRegister::A, Execute::load),							//LD H, A: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::L, GbRegister::B, Execute::load),							//LD L, B: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::L, GbRegister::C, Execute::load),							//LD L, C: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::L, GbRegister::D, Execute::load),							//LD L, D: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::L, GbRegister::E, Execute::load),							//LD L, E: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::L, GbRegister::H, Execute::load),							//LD L, H: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::L, GbRegister::L, Execute::load),							//LD L, L: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_MemReg16, CpuOperation::LD, GbRegister::L, GbRegister::HL, Execute::load, GbFlag::NONE),		//LD L, (HL): 1B 8C, ----, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::L, GbRegister::A, Execute::load),							//LD L, A: 1B 4C, Z0HC, TODO

		//$7x
GbInstruction(AddressingMode::MemReg16_Reg, CpuOperation::LD, GbRegister::HL, B, Execute::load),								//LD (HL), B: 1B 8C, ----, TODO
GbInstruction(AddressingMode::MemReg16_Reg, CpuOperation::LD, GbRegister::HL, C, Execute::load),								//LD (HL), C: 1B 8C, ----, TODO
GbInstruction(AddressingMode::MemReg16_Reg, CpuOperation::LD, GbRegister::HL, D, Execute::load),								//LD (HL), D: 1B 8C, ----, TODO
GbInstruction(AddressingMode::MemReg16_Reg, CpuOperation::LD, GbRegister::HL, E, Execute::load),								//LD (HL), E: 1B 8C, ----, TODO
GbInstruction(AddressingMode::MemReg16_Reg, CpuOperation::LD, GbRegister::HL, H, Execute::load),								//LD (HL), H: 1B 8C, ----, TODO
GbInstruction(AddressingMode::MemReg16_Reg, CpuOperation::LD, GbRegister::HL, L, Execute::load),								//LD (HL), L: 1B 8C, ----, TODO
GbInstruction(AddressingMode::NONE_NONE, CpuOperation::HALT, GbRegister::NONE, GbRegister::NONE, Execute::weird),				//HALT: 1B 4C, ----, TODO
GbInstruction(AddressingMode::MemReg16_Reg, CpuOperation::LD, GbRegister::HL, A, Execute::load),								//LD (HL), A: 1B 8C, ----, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::A, GbRegister::B, Execute::load),							//LD A, B: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::A, GbRegister::C, Execute::load),							//LD A, C: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::A, GbRegister::D, Execute::load),							//LD A, D: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::A, GbRegister::E, Execute::load),							//LD A, E: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::A, GbRegister::H, Execute::load),							//LD A, H: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::A, GbRegister::L, Execute::load),							//LD A, L: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_MemReg16, CpuOperation::LD, GbRegister::A, GbRegister::HL, Execute::load, GbFlag::NONE),		//LD A, (HL): 1B 8C, ----, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::LD, GbRegister::A, GbRegister::A, Execute::load),							//LD A, A: 1B 4C, Z0HC, TODO

		//$8x
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::ADD, GbRegister::A, GbRegister::B, Execute::add),							//ADD A, B: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::ADD, GbRegister::A, GbRegister::C, Execute::add),							//ADD A, C: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::ADD, GbRegister::A, GbRegister::D, Execute::add),							//ADD A, D: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::ADD, GbRegister::A, GbRegister::E, Execute::add),							//ADD A, E: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::ADD, GbRegister::A, GbRegister::H, Execute::add),							//ADD A, H: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::ADD, GbRegister::A, GbRegister::L, Execute::add),							//ADD A, L: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_MemReg16, CpuOperation::ADD, GbRegister::A, GbRegister::HL, Execute::add),					//ADD A, (HL): 1B 8C, ----, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::ADD, GbRegister::A, GbRegister::A, Execute::add),							//ADD A, A: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::ADC, GbRegister::A, GbRegister::B, Execute::adc),							//ADC A, B: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::ADC, GbRegister::A, GbRegister::C, Execute::adc),							//ADC A, C: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::ADC, GbRegister::A, GbRegister::D, Execute::adc),							//ADC A, D: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::ADC, GbRegister::A, GbRegister::E, Execute::adc),							//ADC A, E: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::ADC, GbRegister::A, GbRegister::H, Execute::adc),							//ADC A, H: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::ADC, GbRegister::A, GbRegister::L, Execute::adc),							//ADC A, L: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_MemReg16, CpuOperation::ADC, GbRegister::A, GbRegister::HL, Execute::adc),					//ADC A, (HL): 1B 8C, ----, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::ADC, GbRegister::A, GbRegister::A, Execute::adc),							//ADC A, A: 1B 4C, Z0HC, TODO
		//$9x
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::SUB, GbRegister::A, GbRegister::B, Execute::sub),							//SUB A, B: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::SUB, GbRegister::A, GbRegister::C, Execute::sub),							//SUB A, C: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::SUB, GbRegister::A, GbRegister::D, Execute::sub),							//SUB A, D: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::SUB, GbRegister::A, GbRegister::E, Execute::sub),							//SUB A, E: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::SUB, GbRegister::A, GbRegister::H, Execute::sub),							//SUB A, H: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::SUB, GbRegister::A, GbRegister::L, Execute::sub),							//SUB A, L: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_MemReg16, CpuOperation::SUB, GbRegister::A, GbRegister::HL, Execute::sub),					//SUB A, (HL): 1B 8C, ----, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::SUB, GbRegister::A, GbRegister::A, Execute::sub),							//SUB A, A: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::SBC, GbRegister::A, GbRegister::B, Execute::sbc),							//SBC A, B: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::SBC, GbRegister::A, GbRegister::C, Execute::sbc),							//SBC A, C: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::SBC, GbRegister::A, GbRegister::D, Execute::sbc),							//SBC A, D: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::SBC, GbRegister::A, GbRegister::E, Execute::sbc),							//SBC A, E: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::SBC, GbRegister::A, GbRegister::H, Execute::sbc),							//SBC A, H: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::SBC, GbRegister::A, GbRegister::L, Execute::sbc),							//SBC A, L: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_MemReg16, CpuOperation::SBC, GbRegister::A, GbRegister::HL, Execute::sbc),					//SBC A, (HL): 1B 8C, ----, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::SBC, GbRegister::A, GbRegister::A, Execute::sbc),							//SBC A, A: 1B 4C, Z0HC, TODO

		//$Ax
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::AND, GbRegister::A, GbRegister::B, Execute::bwAnd),							//AND A, B: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::AND, GbRegister::A, GbRegister::C, Execute::bwAnd),							//AND A, C: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::AND, GbRegister::A, GbRegister::D, Execute::bwAnd),							//AND A, D: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::AND, GbRegister::A, GbRegister::E, Execute::bwAnd),							//AND A, E: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::AND, GbRegister::A, GbRegister::H, Execute::bwAnd),							//AND A, H: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::AND, GbRegister::A, GbRegister::L, Execute::bwAnd),							//AND A, L: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_MemReg16, CpuOperation::AND, GbRegister::A, GbRegister::HL, Execute::bwAnd),					//AND A, (HL): 1B 8C, ----, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::AND, GbRegister::A, GbRegister::A, Execute::bwAnd),							//AND A, A: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::XOR, GbRegister::A, GbRegister::B, Execute::bwXor),							//XOR A, B: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::XOR, GbRegister::A, GbRegister::C, Execute::bwXor),							//XOR A, C: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::XOR, GbRegister::A, GbRegister::D, Execute::bwXor),							//XOR A, D: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::XOR, GbRegister::A, GbRegister::E, Execute::bwXor),							//XOR A, E: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::XOR, GbRegister::A, GbRegister::H, Execute::bwXor),							//XOR A, H: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::XOR, GbRegister::A, GbRegister::L, Execute::bwXor),							//XOR A, L: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_MemReg16, CpuOperation::XOR, GbRegister::A, GbRegister::HL, Execute::bwXor),					//XOR A, (HL): 1B 8C, ----, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::XOR, GbRegister::A, GbRegister::A, Execute::bwXor),							//XOR A, A: 1B 4C, Z0HC, TODO

		//$Bx
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::OR, GbRegister::A, GbRegister::B, Execute::bwOr),							//OR A, B: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::OR, GbRegister::A, GbRegister::C, Execute::bwOr),							//OR A, C: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::OR, GbRegister::A, GbRegister::D, Execute::bwOr),							//OR A, D: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::OR, GbRegister::A, GbRegister::E, Execute::bwOr),							//OR A, E: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::OR, GbRegister::A, GbRegister::H, Execute::bwOr),							//OR A, H: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::OR, GbRegister::A, GbRegister::L, Execute::bwOr),							//OR A, L: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_MemReg16, CpuOperation::OR, GbRegister::A, GbRegister::HL, Execute::bwOr),					//OR A, (HL): 1B 8C, ----, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::OR, GbRegister::A, GbRegister::A, Execute::bwOr),							//OR A, A: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::CP, GbRegister::A, GbRegister::B, Execute::sub),							//CP A, B: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::CP, GbRegister::A, GbRegister::C, Execute::sub),							//CP A, C: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::CP, GbRegister::A, GbRegister::D, Execute::sub),							//CP A, D: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::CP, GbRegister::A, GbRegister::E, Execute::sub),							//CP A, E: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::CP, GbRegister::A, GbRegister::H, Execute::sub),							//CP A, H: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::CP, GbRegister::A, GbRegister::L, Execute::sub),							//CP A, L: 1B 4C, Z0HC, TODO
GbInstruction(AddressingMode::Reg_MemReg16, CpuOperation::CP, GbRegister::A, GbRegister::HL, Execute::sub),					    //CP A, (HL): 1B 8C, ----, TODO
GbInstruction(AddressingMode::Reg_Reg, CpuOperation::CP, GbRegister::A, GbRegister::A, Execute::sub),							//CP A, A: 1B 4C, Z0HC, TODO
		//$Cx
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),

		//$Dx
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),

		//$Ex
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),

		//$Fx
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),

}

GbInstruction prefixInstDec[NUM_INSTRUCTIONS] = {
		//$0x
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),

		//$1x
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),

		//$2x
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),

		//$3x
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),

		//$4x
GbInstruction(RegNone, BIT, A, B, NONE, &Execute::bit),
GbInstruction(RegNone, BIT, A, C, NONE, &Execute::bit),
GbInstruction(RegNone, BIT, A, D, NONE, &Execute::bit),
GbInstruction(RegNone, BIT, A, E, NONE, &Execute::bit),
GbInstruction(RegNone, BIT, A, H, NONE, &Execute::bit),
GbInstruction(RegNone, BIT, A, L, NONE, &Execute::bit),
GbInstruction(MemNone, BIT, A, HL, NONE, &Execute::bit),
GbInstruction(RegNone, BIT, A, A, NONE, &Execute::bit),
GbInstruction(RegNone, BIT, F, B, NONE, &Execute::bit),
GbInstruction(RegNone, BIT, F, C, NONE, &Execute::bit),
GbInstruction(RegNone, BIT, F, D, NONE, &Execute::bit),
GbInstruction(RegNone, BIT, F, E, NONE, &Execute::bit),
GbInstruction(RegNone, BIT, F, H, NONE, &Execute::bit),
GbInstruction(RegNone, BIT, F, L, NONE, &Execute::bit),
GbInstruction(MemNone, BIT, F, HL, NONE, &Execute::bit),
GbInstruction(RegNone, BIT, F, A, NONE, &Execute::bit),
		//$5x
GbInstruction(RegNone, BIT),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(MemNone, BIT, A, HL),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(MemNone, BIT, A, HL),
GbInstruction(),

		//$6x
GbInstruction(RegNone, BIT),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(MemNone, BIT, B, HL),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(MemNone, BIT, C, HL),
GbInstruction(),

		//$7x
GbInstruction(RegNone, BIT),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(MemNone, BIT, D, HL),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(MemNone, BIT, E, HL),
GbInstruction(),

		//$8x
GbInstruction(RegNone, RES),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(MemNone, RES, H, HL),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(MemNone,RES, L, HL),
GbInstruction(),

		//$9x
GbInstruction(RegNone, RES),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(MemNone, RES, A, HL),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(MemNone,RES, A, HL),
GbInstruction(),

		//$Ax
GbInstruction(RegNone, RES),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(MemNone, RES, A, HL),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(MemNone,RES, A, HL),
GbInstruction(),

		//$Bx
GbInstruction(RegNone, RES),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(MemNone, RES, A, HL),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(MemNone,RES, A, HL),
GbInstruction(),

		//$Cx
GbInstruction(RegNone, SET),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(MemNone, SET, A, HL),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(MemNone, SET, A, HL),
GbInstruction(),

		//$Dx
GbInstruction(RegNone, SET),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(MemNone, SET, A, HL),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(MemNone, SET, A, HL),
GbInstruction(),

		//$Ex
GbInstruction(RegNone, SET),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(MemNone, SET, A, HL),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(MemNone, SET, A, HL),
GbInstruction(),

		//$Fx
GbInstruction(RegNone, SET),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(MemNone, SET, A, HL),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(),
GbInstruction(MemNone, SET, A, HL),
GbInstruction(),

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
	GbInstruction decodePrefixInstruction(uint8_t* instructionBytes);
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
