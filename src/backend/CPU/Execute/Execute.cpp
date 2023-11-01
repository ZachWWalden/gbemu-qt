/*==================================================================================
 *Class - Execute
 *Author - Zach Walden
 *Created - 7/22/22
 *Last Changed - 10/13/23
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
#include "RegisterFile/RegisterFile.hpp"
#include <cstdint>

/*
TODO 1. Fix 16 bit arithmetic half carries.
*/

GbInstruction Execute::decodeInstruction(uint8_t* instructionBytes)
{
	return this->instDec[instructionBytes[0]];
}
bool Execute::decodePrefixInstruction(void* instance, GbInstruction inst, uint8_t* instBytes, uint8_t &pcInc)
{
	GbInstruction  inststruction = ((Execute*)instance)->prefixInstDec[instBytes[1]];
	if(!(inststruction.execFunction(instance, inststruction, instBytes, pcInc)))
	{
		LOG("EXECUTE: Prefix instruction errored");
		return false;
	}
	return true;
}
//Functions to execute each Instruction.
bool Execute::load(void* instance, GbInstruction inst, uint8_t* instBytes, uint8_t &pcInc)
{
	Execute* thees = (Execute*) instance;
	uint16_t opOne, address, opTwo;

	//Reg Source:MemReg_Reg,  Reg_Reg,  MemImm8_Reg,  MemReg16_Reg
	if(inst.mode == MemReg_Reg || inst.mode == Reg_Reg || inst.mode == MemImm8_Reg || inst.mode == MemReg16_Reg)
	{
		opTwo = thees->regFile.readReg(inst.operandTwo) & 0x00FF;
	}
	//Reg16 source: MemImm16_Reg16  Reg16_Reg16 MemReg16_Reg16
	else if (inst.mode == MemImm16_Reg16 || inst.mode == Reg16_Reg16 || inst.mode == MemReg16_Reg16)
	{
		opTwo = thees->regFile.readRegPair(inst.operandTwo);
	}
	//Imm8 source: Reg_Imm8 MemReg16_Imm8
	else if (inst.mode == Reg_Imm8 || inst.mode == MemReg16_Imm8) {
		opTwo = instBytes[1];
	}
	//Reg16_Reg16Sim8
	else if (inst.mode == Reg16_Reg16Sim8) {
		opTwo = thees->regFile.readRegPair(inst.operandTwo);
		//Sign extend immeadiate
		opOne = ((instBytes[1] & 0x80) == 0x80) ? (instBytes[1]) | 0xFF00 : instBytes[1] & 0x00FF;
	}
	//Imm16 source: Reg16_Imm16
	else if (inst.mode == Reg16_Imm16) {
		opTwo = ((instBytes[2] << 8) & 0xFF00) | (instBytes[1] & 0xFF00);
	}
	//MemReg: Source: Reg_MemReg
	else if (inst.mode == Reg_MemReg) {
		opTwo = thees->mem->read(0xFF00 | thees->regFile.readReg(inst.operandTwo));
	}
	//MemReg16: Source: Reg_MemReg16 Reg16_MemReg16
	else if (inst.mode == Reg_MemReg16 || inst.mode == Reg16_MemReg16) {
		//get source Register
		address = thees->regFile.readRegPair(inst.operandTwo);
		opTwo = thees->mem->read(address);
		//Check to see if there are post increments/decrements
		if(inst.condition == GbFlag::GbFlag::PoI)
		{
			if (inst.op == POP)
			{
				//if a pop read the next byte. low byte, low address, high byte high address.
				opTwo = ((thees->mem->read(address + 1) << 8) & 0xFF00) | opTwo;
				thees->regFile.incSp();
			}
			else {
				thees->regFile.incRegPair(inst.operandTwo);
			}
		}
		else if (inst.condition == GbFlag::GbFlag::PoD)
		{
			thees->regFile.decRegPair(inst.operandTwo);
		}

	}
	else if (inst.mode == Reg_MemImm8) {
		opTwo = thees->mem->read(0xFF00 | instBytes[1]);
	}
	else if (inst.mode == Reg_MemImm16) {
		//Little Endian
		address = ((instBytes[2] << 8) & 0xFF00) | (instBytes[1] & 0xFF00);
		opTwo = thees->mem->read(address);
	}
	else
	{
		return false;
	}
	//store
	//Reg dest:Reg_Reg Reg_Imm8  Reg_MemReg Reg_MemReg16  Reg_MemImm8 Reg_MemImm16
	if(inst.mode == Reg_Reg || inst.mode == Reg_Imm8 || inst.mode == Reg_MemReg || inst.mode == Reg_MemReg16 || inst.mode == Reg_MemImm8 || inst.mode == Reg_MemImm16){
		thees->regFile.writeReg(inst.operandOne, opTwo & 0x00FF);
	}
	//Reg16 dest:Reg16_Reg16 Reg16_Reg16Sim8 Reg16_Imm16 Reg16_MemReg16
	else if (inst.mode == Reg16_Reg16 || inst.mode == Reg16_Reg16Sim8 || inst.mode == Reg16_Imm16 || inst.mode == Reg16_MemReg16) {
		thees->regFile.writeRegPair(inst.operandOne, opTwo);
	}
	// MemReg det:MemReg_Reg
	else if (inst.mode == MemReg_Reg) {
		thees->mem->write(0xFF00 | thees->regFile.readReg(inst.operandOne), opTwo);
	}
	// MemReg16 dest: MemReg16_Reg MemReg16_Reg16 MemReg16_Imm8
	else if (inst.mode == MemReg16_Reg || inst.mode == MemReg16_Reg16 || inst.mode == MemReg16_Imm8) {
		//Check for PoI, PoD, and Push
		address = thees->regFile.readRegPair(inst.operandOne);
		if(inst.condition == GbFlag::GbFlag::PoI)
		{
			thees->mem->write(address, opTwo);
			thees->regFile.incRegPair(inst.operandTwo);
		}
		else if (inst.condition == GbFlag::GbFlag::PoD)
		{
			if (inst.op == PUSH)
			{
				//opTwo is a reg 16 high byte goes up fist.
				thees->mem->write(address, (opTwo >> 8) & 0x00FF);
				thees->mem->write(address - 1, opTwo & 0x00FF);
				thees->regFile.decSp();
			}
			else {
				thees->mem->write(address, opTwo);
				thees->regFile.decRegPair(inst.operandTwo);
			}
		}
		else {

			thees->mem->write(address, opTwo);
		}
	}
	// MemImm8 dest:MemImm8_Reg
	else if (inst.mode == MemImm8_Reg) {
		thees->mem->write(0xFF00 | instBytes[1], opTwo);
	}
	// MemImm16 dest:MemImm16_Reg16
	else if (inst.mode == MemImm16_Reg16) {
		//2 byte store
		address = ((instBytes[2] << 8) & 0xFF00) | instBytes[1] & 0x00FF;
		thees->mem->write(address, opTwo & 0x00FF);
		thees->mem->write(address + 1, (opTwo >> 8) & 0x00FF);
	}
	else {
		return false;
	}

}

bool Execute::inc(void* instance, GbInstruction inst, uint8_t* instBytes, uint8_t &pcInc)
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
bool dec(void* instance, GbInstruction inst, uint8_t* instBytes, uint8_t &pcInc)bool Execute::add(void* instance, GbInstruction inst, uint8_t* instBytes, uint8_t &pcInc)
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
bool adc(void* instance, GbInstruction inst, uint8_t* instBytes, uint8_t &pcInc)
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
bool sub(void* instance, GbInstruction inst, uint8_t* instBytes, uint8_t &pcInc)
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
bool sbc(void* instance, GbInstruction inst, uint8_t* instBytes, uint8_t &pcInc)
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
bool bwAnd(void* instance, GbInstruction inst, uint8_t* instBytes, uint8_t &pcInc)
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
bool bwXor(void* instance, GbInstruction inst, uint8_t* instBytes, uint8_t &pcInc)
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
bool bwOr(void* instance, GbInstruction inst, uint8_t* instBytes, uint8_t &pcInc)
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
bool Execute::bit(void* instance, GbInstruction inst, uint8_t* instBytes, uint8_t &pcInc)
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
bool Execute::res(void* instance, GbInstruction inst, uint8_t* instBytes, uint8_t &pcInc)
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
bool Execute::set(void* instance, GbInstruction inst, uint8_t* instBytes, uint8_t &pcInc)
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
