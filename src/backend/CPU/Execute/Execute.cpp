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

bool Execute::bit(GbInstruction inst, uint8_t* instBytes)
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
		operand = this->mem->read(address);
	}
	else
	{
		return false;
	}
	//Execute Instruction
	//create operand mask value. shift 1 left by bit nneded to test.
	uint8_t mask = 0x00 | 0x01 << inst.operandOne;
	//Operand, is now wearing a chin diaper.
	operand = operand & mask;
	//I like this line 9 line iof/else into a 1 liner....
	this->regFile.modifyFlag(Z, operand >> inst.operandOne == 0x01)
	//Clear N, Set H.
	this->regFile.modifyFlag(N, false);
	this->regFile.modifyFlag(H, true);
	return true;
}
bool Execute::res(GbInstruction inst, uint8_t* instBytes)
{
	//Operand One is the Bit operand. operand two is the register operand.
	uint8_t operand;
	uint16_t address;
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
		address = this->regFile.readRegPair(inst.operandTwo);
		operand = this->mem->read(address);
	}
	else
	{
		return false;
	}
	//clear bit inst.operandOne in the register or memory location pointed to by operand b.
	uint8_t mask = ~(0x00 | 0x01 << inst.operandOne);
	//WEAR YOU"RE MASK COORECTlY;
	operand = operand & mask;
	//write back operand one.
	if(inst.mode == RegNone)
	{
		//Write Register.
		this->regFile.writeReg(inst.operandTwo, operand);
	}
	else
	{
		//Write Operand to memory.
		//Read Reg Pair
		this->mem->write(address, operand);
	}
	//Flags RES does not affect any flags.
	return true;
}
bool Execute::set(GbInstruction inst, uint8_t* instBytes)
{
	//Operand One is the Bit operand. operand two is the register operand.
	uint8_t operand;
	uint16_t address;
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
		address = this->regFile.readRegPair(inst.operandTwo);
		operand = this->mem->read(address);
	}
	else
	{
		return false;
	}
	//clear bit inst.operandOne in the register or memory location pointed to by operand b.
	uint8_t mask = 0x01 << inst.operandOne;
	//WEAR YOU"RE MASK COORECTlY;
	operand = operand & mask;
	//write back operand one.
	if(inst.mode == RegNone)
	{
		//Write Register.
		this->regFile.writeReg(inst.operandTwo, operand);
	}
	else
	{
		//Write Operand to memory.
		//Read Reg Pair
		this->mem->write(address, operand);
	}
	//Flags RES does not affect any flags.
	return true;
}

/*
<++> Execute::<++>()
{

}
*/
