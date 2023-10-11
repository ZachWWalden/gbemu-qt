/*==================================================================================
 *Class - Execute
 *Author - Zach Walden
 *Created - 7/22/22
 *Last Changed - 9/10/22
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
#include <cstdint>

/*
TODO 1. Fix 16 bit arithmetic half carries.
*/

GbInstruction Execute::decodeInstruction(uint8_t* instructionBytes)
{

}
GbInstruction Execute::decodePrefixInstruction(uint8_t* instructionBytes)
{

}
//Functions to execute each Instruction.
bool Execute::load(GbInstruction inst, uint8_t* instBytes, uint8_t &pcInc)
{
	//AddressingMode Reg16Imm16, MemReg, MemImm8, RegImm8, MemImm16Reg16, RegMem, RegReg
	if(inst.mode == RegReg)
	{
		//execute Instruction
		this->regFile.writeReg(inst.operandOne, this->regFile.readReg(inst.operandTwo));
		pcInc = 1;
		return true;
	}
	else if(inst.mode == RegMem)
	{
		//Execute Instruction
		this->regFile.writeReg(inst.operandOne, this->mem->read(this->regFile.readRegPair(inst.operandTwo)));
		pcInc = 1;
		return true;
	}
	else if(inst.mode == MemReg)
	{
		//Execute Instruction
		this->mem->write(this->regFile.readRegPair(inst.operandOne),this->regFile.readReg(inst.operandTwo));
		pcInc = 1;
		return true;
	}
	//LD Reg16, d16
	else if(inst.mode == Reg16Imm16)
	{
		uint16_t imm16 = ((instBytes[2] < 8) & 0x0FF00) | (instBytes[1] & 0x0FF);
		this->regFile.wirteRegPair(inst.operandOne, imm16);
		pcInc = 3;
		return true;
	}
	//LD (a16), SP
	else if(inst.mode == MemImm16Reg16)
	{

	}
	else if(inst.mode == RegImm8)
	{
		this->regFile.writeReg(inst.operandOne, instBytes[1]);
		pcInc = 2;

	}

	else
	{
		return false;
	}


}

bool Execute::inc(GbInstruction inst, uint8_t* instBytes, uint8_t &pcInc)
{
	uint16_t opOne;
	uint16_t result;
	pcInc = 1;
	//Fetch Operands
	if(inst.mode == RegNone)
	{
		opOne = 0x00FF & this->regFile.readReg(inst.operandOne);
	}
	else if(inst.mode == MemNone)
	{
		opOne = 0x00FF & this->mem->read(this->regFile.readRegPair(inst.operandTwo));
	}
	else if(inst.mode == Reg16None)
	{
		//Signed immeadiate must be sign extended to at least 16 bits.
		opOne = this->regFile.readRegPair(inst.operandOne);
	}
	else
	{
		return false;
	}
	result = opOne + 0x01;
	//16-bit INC/DEC does not affect the flags.
	if(inst.mode != Reg16None)
	{
		//Carry is unaffected
		this->regFile.modifyFlag(Z, (result & 0x0FF) ==0);
		//Half Carry
		this->regFile.modifyFlag(H, (((opOne & 0x0F) + 1) == 0x10)); //TODO Add namespace to Regesiters and Operating Modes
		{

		}
		//Clear N
		this->regFile.modifyFlag(N, false);
	}
	//Write Back
	if(inst.mode == RegNone)
	{
		this->regFile.writeReg(inst.operandOne, (uint8_t)(result & 0x0FF));
	}
	else if(inst.mode == MemNone)
	{
		this->mem->write(this->regFile.readRegPair(inst.operandTwo), (uint8_t)(result & 0x0FF));
	}
	else
	{
		//Signed immeadiate must be sign extended to at least 16 bits.
		this->regFile.writeRegPair(inst.operandOne, result);
	}
	return true;
}
bool dec(GbInstruction inst, uint8_t* instBytes, uint8_t &pcInc)bool Execute::add(GbInstruction inst, uint8_t* instBytes, uint8_t &pcInc)
{
	uint16_t opOne;
	uint16_t result;
	pcInc = 1;
	//Fetch Operands
	if(inst.mode == RegNone)
	{
		opOne = 0x00FF & this->regFile.readReg(inst.operandOne);
	}
	else if(inst.mode == MemNone)
	{
		opOne = 0x00FF & this->mem->read(this->regFile.readRegPair(inst.operandTwo));
	}
	else if(inst.mode == Reg16None)
	{
		//Signed immeadiate must be sign extended to at least 16 bits.
		opOne = this->regFile.readRegPair(inst.operandOne);
	}
	else
	{
		return false;
	}
	result = opOne - 0x01;
	//16-bit INC/DEC does not affect the flags.
	if(inst.mode != Reg16None)
	{
		//Carry is unaffected
		this->regFile.modifyFlag(Z, (result & 0x0FF) ==0);
		//Half Carry
		this->regFile.modifyFlag(H, (opOne & 0x0F) < 0x01 );
		//Clear N
		this->regFile.modifyFlag(N, true);
	}
	//Write Back
	if(inst.mode == RegNone)
	{
		this->regFile.writeReg(inst.operandOne, (uint8_t)(result & 0x0FF));
	}
	else if(inst.mode == MemNone)
	{
		this->mem->write(this->regFile.readRegPair(inst.operandTwo), (uint8_t)(result & 0x0FF));
	}
	else
	{
		//Signed immeadiate must be sign extended to at least 16 bits.
		this->regFile.writeRegPair(inst.operandOne, result);
	}
	return true;
}
{
	uint16_t opOne;
	uint16_t opTwo;
	uint32_t result;
	//Fetch Operands
	if(inst.mode == RegReg)
	{
		opOne = 0x00FF & this->regFile.readReg(inst.operandOne);
		opTwo = 0x00FF & this->regFile.readReg(inst.operandTwo);
	}
	else if(inst.mode == RegImm8)
	{
		opOne = 0x00FF & this->regFile.readReg(inst.operandOne);
		opTwo = 0x00FF & instBytes[1];
	}
	else if(inst.mode == RegMem)
	{
		opOne = 0x00FF & this->regFile.readReg(inst.operandOne);
		opTwo = 0x00FF & this->mem->read(this->regFile.readRegPair(inst.operandTwo));
	}
	else if(inst.mode == Reg16Simm8)
	{
		//Signed immeadiate must be sign extended to at least 16 bits.
		opOne = 0x00FF & this->regFile.readReg(inst.operandOne);
		opTwo = 0x00FF & (instBytes[1]) | (((instBytes[1] & 0x80) == 0x80) ? 0xFF00 : 0x0000);
		//Set Zero flag to zero. This is due to 16-bit arithmetic being double pumped, other than the INC/DEC which feature 16-bit units with proper flag checks for that width.
		this->regFile.modifyFlag(Z, false);
	}
	else if(inst.mode == Reg16Reg16)
	{
		opOne = 0xFFFF & this->regFile.readRegPair(inst.operandOne);
		opTwo = 0xFFFF & this->regFile.readRegPair(inst.operandTwo);
	}
	else
	{
		return false;
	}
	//Execute Instruction
	result = opOne + opTwo;
	//Set N flag to 0
	this->regFile.modifyFlag(N, false);
	//Write Back Results.
	if(inst.mode == RegReg || inst.mode == RegImm8 || inst.mode == RegMem)
	{
		//Check carry & Half Carry
		this->regFile.modifyFlag(H,(((opOne & 0x0F) + (opTwo & 0x0F)) & 0x10) == 0x10);
		this->regFile.modifyFlag(C,(result & 0x100) == 0x100);
		//set zero flag
		this->regFile(Z, (uint8_t)(result & 0x00FF) == 0x00)
		this->regFile.writeReg(inst.operandOne, (uint8_t)(result & 0x00FF))
	}
	else if(inst.mode == Reg16Simm8 || inst.mode == Reg16Reg16)
	{
		//Check carry and half carry.
		this->regFile.modifyFlag(H,(((opOne & 0x0FFF) + (opTwo & 0x0FFF)) & 0x1000) == 0x1000);
		this->regFile.modifyFlag(C,(result & 0x10000) == 0x10000);
		this->regFile.writeRegPair(inst.operandOne, (uint16_t)(result & 0x0FFFF);
	}
	if(inst.mode == Reg16Simm8 || inst.mode == RegImm8)
	{
		//Increment Program counter by two
		pcInc = 2;
	}
	else
	{
		//Increment Program Counter by One.
		pcInc = 1;
	}
	else
	{
		return false;
	}
	return true;
}
bool adc(GbInstruction inst, uint8_t* instBytes, uint8_t &pcInc)
{
	uint8_t opOne;
	uint8_t opTwo;
	uint16_t result;
	//Fetch Operands
	if(inst.mode == RegReg)
	{
		opOne = 0x00FF & this->regFile.readReg(inst.operandOne);
		opTwo = 0x00FF & this->regFile.readReg(inst.operandTwo);
		pcInc = 1;
	}
	else if(inst.mode == RegImm8)
	{
		opOne = 0x00FF & this->regFile.readReg(inst.operandOne);
		opTwo = 0x00FF & instBytes[1];
		pcInc = 2;
	}
	else if(inst.mode == RegMem)
	{
		opOne = 0x00FF & this->regFile.readReg(inst.operandOne);
		opTwo = 0x00FF & this->mem->read(this->regFile.readRegPair(inst.operandTwo));
		pcInc = 1;
	}
	else
	{
		return false;
	}
	//Get carry bit.
	uint8_t carry = this->regFile.checkFlag(C) ? 0x01 : 0x00;
	//Execute Instruction
	result = opOne + opTwo + carry;
	//Check Zero
	this->regFile.modifyFlag(Z, result == 0x0000);
	//Check Carry
	this->regFile.modifyFlag(C, (result & 0x100) == 0x100);
	//Check Half Carry
	this->regFile.modifyFlag(H,(((opOne & 0x0F) + (opTwo &0x0F) + carry) & 0x10) == 0x10);
	//Write Result.
	this->regFile.writeReg(inst.operandOne, (uint8_t)(result & 0xFF));
	return true;
}
bool sub(GbInstruction inst, uint8_t* instBytes, uint8_t &pcInc)
{
	uint8_t opOne;
	uint8_t opTwo;
	uint16_t result;
	if(inst.mode == RegReg)
	{
		opOne = this->regFile.readReg(inst.operandOne);
		opTwo = this->regFile.readReg(inst.operandTwo);
		pcInc = 1;
	}
	else if(inst.mode == RegImm8)
	{
		opOne = this->regFile.readReg(inst.operandOne);
		opTwo = instBytes[1];
		pcInc = 2;
	}
	else if(inst.mode == RegMem)
	{
		opOne = this->regFile.readReg(inst.operandOne);
		opTwo = this->mem->read(this->regFile.readRegPair(inst.operandTwo));
		pcInc = 1;
	}
	else
	{
		return false;
	}
	//Execute Instruction
	result = opOne - opTwo;
	//Check Half/Full Carry & Zero
	this->regFile.modifyFlag(C, (opOne & 0x0FF) < (opTwo & 0x0FF));
	this->regFile.modifyFlag(H, (opOne & 0x00F) < (opTwo & 0x00F));
	this->regFile.modifyFlag(Z, (result & 0x0FF) == 0x00);
	//Set Negative
	this->regFile.modifyFlag(N, true);
	//Write Result if not the compare instruction.
	if(inst.op != CP)
	{
		this->regFile.writeReg(inst.operandOne, (uint8_t)(result & 0x0FF));
	}
	return true;
}
bool sbc(GbInstruction inst, uint8_t* instBytes, uint8_t &pcInc)
{
	uint8_t opOne;
	uint8_t opTwo;
	uint16_t result;
	if(inst.mode == RegReg)
	{
		opOne = this->regFile.readReg(inst.operandOne);
		opTwo = this->regFile.readReg(inst.operandTwo);
		pcInc = 1;
	}
	else if(inst.mode == RegImm8)
	{
		opOne = this->regFile.readReg(inst.operandOne);
		opTwo = instBytes[1];
		pcInc = 2;
	}
	else if(inst.mode == RegMem)
	{
		opOne = this->regFile.readReg(inst.operandOne);
		opTwo = this->mem->read(this->regFile.readRegPair(inst.operandTwo));
		pcInc = 1;
	}
	else
	{
		return false;
	}
	//Get carry bit.
	uint8_t carry = this->regFile.checkFlag(C) ? 0x01 : 0x00;
	//Execute Instruction
	result = opOne - (opTwo + carry);
	//Check Half/Full Carry & Zero
	this->regFile.modifyFlag(C, (opOne & 0x0FF) < ((opTwo + carry) & 0x0FF));
	this->regFile.modifyFlag(H, (opOne & 0x00F) < ((opTwo + carry) & 0x00F));
	this->regFile.modifyFlag(Z, (result & 0x0FF) == 0x00);
	//Set Negative
	this->regFile.modifyFlag(N, true);
	//Write Result
	this->regFile.writeReg(inst.operandOne, (uint8_t)(result & 0x0FF));
	return true;
}
bool bwAnd(GbInstruction inst, uint8_t* instBytes, uint8_t &pcInc)
{
	uint8_t opOne;
	uint8_t opTwo;
	uint8_t result;
	if(inst.mode == RegReg)
	{
		opOne = this->regFile.readReg(inst.operandOne);
		opTwo = this->regFile.readReg(inst.operandTwo);
		pcInc = 1;
	}
	else if(inst.mode == RegImm8)
	{
		opOne = this->regFile.readReg(inst.operandOne);
		opTwo = instBytes[1];
		pcInc = 2;
	}
	else if(inst.mode == RegMem)
	{
		opOne = this->regFile.readReg(inst.operandOne);
		opTwo = this->mem->read(this->regFile.readRegPair(inst.operandTwo));
		pcInc = 1;
	}
	else
	{
		return false;
	}
	//Execute Instruction
	result = opOne & opTwo;
	//Check Zero Flag
	this->regFile.modifyFlag(Z, result == 0x00);
	//Set flags
	this->regFile.modifyFlag(N, false);
	this->regFile.modifyFlag(H, true);
	this->regFile.modifyFlag(C, false);
	//Write Result
	this->regFile.writeReg(inst.operandOne, result);
	return true;
}
bool bwXor(GbInstruction inst, uint8_t* instBytes, uint8_t &pcInc)
{
	uint8_t opOne;
	uint8_t opTwo;
	uint8_t result;
	if(inst.mode == RegReg)
	{
		opOne = this->regFile.readReg(inst.operandOne);
		opTwo = this->regFile.readReg(inst.operandTwo);
		pcInc = 1;
	}
	else if(inst.mode == RegImm8)
	{
		opOne = this->regFile.readReg(inst.operandOne);
		opTwo = instBytes[1];
		pcInc = 2;
	}
	else if(inst.mode == RegMem)
	{
		opOne = this->regFile.readReg(inst.operandOne);
		opTwo = this->mem->read(this->regFile.readRegPair(inst.operandTwo));
		pcInc = 1;
	}
	else
	{
		return false;
	}
	//Execute Instruction
	result = opOne ^ opTwo;
	//Check Zero Flag
	this->regFile.modifyFlag(Z, result == 0x00);
	//Set flags
	this->regFile.modifyFlag(N, false);
	this->regFile.modifyFlag(H, false);
	this->regFile.modifyFlag(C, false);
	//Write Result
	this->regFile.writeReg(inst.operandOne, result);
	return true;
}
bool bwOr(GbInstruction inst, uint8_t* instBytes, uint8_t &pcInc)
{
	uint8_t opOne;
	uint8_t opTwo;
	uint8_t result;
	if(inst.mode == RegReg)
	{
		opOne = this->regFile.readReg(inst.operandOne);
		opTwo = this->regFile.readReg(inst.operandTwo);
		pcInc = 1;
	}
	else if(inst.mode == RegImm8)
	{
		opOne = this->regFile.readReg(inst.operandOne);
		opTwo = instBytes[1];
		pcInc = 2;
	}
	else if(inst.mode == RegMem)
	{
		opOne = this->regFile.readReg(inst.operandOne);
		opTwo = this->mem->read(this->regFile.readRegPair(inst.operandTwo));
		pcInc = 1;
	}
	else
	{
		return false;
	}
	//Execute Instruction
	result = opOne | opTwo;
	//Check Zero Flag
	this->regFile.modifyFlag(Z, result == 0x00);
	//Set flags
	this->regFile.modifyFlag(N, false);
	this->regFile.modifyFlag(H, false);
	this->regFile.modifyFlag(C, false);
	//Write Result
	this->regFile.writeReg(inst.operandOne, result);
	return true;
}
bool Execute::bit(GbInstruction inst, uint8_t* instBytes, uint8_t &pcInc)
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
	//Increment The Program Counter
	pcInc = 2;
	return true;
}
bool Execute::res(GbInstruction inst, uint8_t* instBytes, uint8_t &pcInc)
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
	//Increment The Program Counter
	pcInc = 2;
	return true;
}
bool Execute::set(GbInstruction inst, uint8_t* instBytes, uint8_t &pcInc)
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
	//Flags SET does not affect any flags.
	//Increment The Program Counter
	pcInc = 2;
	return true;
}

/*
<++> Execute::<++>()
{

}
*/
