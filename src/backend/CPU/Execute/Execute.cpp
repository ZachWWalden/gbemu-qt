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

#include "Execute.hpp"

GbInstruction Execute::decodeInstruction(uint8_t* instructionBytes)
{

}
GbInstruction Execute::decodePrefixInstruction(uint8_t* instructionBytes)
{

}
//Functions to execute each Instruction.

uint8_t Execute::bit(GbInstruction inst, uint8_t* instBytes)
{
	//Operand One is the Bit operand. operand two is the register operand.
	uint8_t operand;
	//There are two addressing mode for this Instruction. RegNone, and MemNone.
	if(inst.mode == RegNone)
	{
		//Fetch Register.
		operand = this->regFile.readReg(inst.operandTwo);
	}
	else if(inst.mode == MemNone)
	{
		//Fetch Operand from memory.
		//Read Reg Pair
		uint16_t address = this->regFile.readRegPair(inst.operandTwo);
		operand = this->mmm->read(address);
	}
	else
	{
		//TODO Trow Exception.
	}
}
uint8_t Execute::res(GbInstruction inst, uint8_t* instBytes)
{

}
uint8_t Execute::set(GbInstruction inst, uint8_t* instBytes)
{

}

/*
<++> Execute::<++>()
{

}
*/
