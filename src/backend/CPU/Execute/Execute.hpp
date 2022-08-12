/*==================================================================================
 *Class - Execute
 *Author - Zach Walden
 *Created - 7/22/22
 *Last Changed - 7/25/22
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

using namespace std;

#define NUM_INSTRUCTIONS 256

enum AddressingMode
{
	RegReg, RegImm8, RegSimm8, Reg16Simm8, RegImm16, RegMem, RegReg16, Reg16Reg16,Reg16Reg16simm8, RegNone, Reg16None, MemNone, NONE
};

enum CpuOperation
{
	ADD, ADC SUB, SBC, OR, XOR, AND, INC, DEC, RLA, RLCA, RRA, RRCA, RLC, RRC, RL, RR, SLA, SRA, SRL, SWAP, BIT, SET, RES, JR, JP, RET, PUSH, POP, DI, EI, CPL, CCF, DAA, SCF, HALT, NOP, STOP
};

struct GbInstruction
{
	GbInstruction(AddressingMode newMode, CpuOperation op, GbRegister opOne, GbRegister opTwo, void* func, GbFlag flag)
	{
		mode = newMode;
		op = newOp;
		operandOne = opOne;
		operandTwo = opTwo;
		condition = flag
		execFunction = func;
	}
	AddressingMode mode;
	CpuOperation op;
	GbRegister operandOne,operandTwo;
	GbFlag condition
	void * execFunction;
};

class Execute
{
	//Attributes
public:

private:
	GbInstruction instDec[NUM_INSTRUCTIONS] = {
GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(),
GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(),
GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(),
GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(),
GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(),
GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(),
GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(),
GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(),
GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(),
GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(),
GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(),
GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(),
GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(),
GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(),
GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(),
GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(), GbInstruction(),
}
	RegisterFile regFile;
	//Methods
public:
	Execute();
	~Execute();
	uint8_t executeInstruction(uint8_t* instructionBytes);
private:
	GbInstruction decodeInstruction(uint8_t* instructionBytes);
	GbInstruction decodePrefixInstruction(uint8_t* instructionBytes);
	//Functions to execute each Instruction.

	uint8_t bit(GbInstruction inst, uint8_t* instBytes);
	uint8_t res(GbInstruction inst, uint8_t* instBytes);
	uint8_t set(GbInstruction inst, uint8_t* instBytes);
};
