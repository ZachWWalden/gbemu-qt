/*==================================================================================
 *Class - Execute
 *Author - Zach Walden
 *Created - 7/22/22
 *Last Changed - 11/2/23
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
 * along with thees program; if not, you may find one here:
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

void Execute::registerCycleWatchCalback(CycleListener* listener)
{
	this->cycleListeners.push_back(listener);
}
void Execute::emitCycles(uint8_t numCycles)
{
	for (CycleListener* listener : this->cycleListeners)
	{
		listener->cycleListener(numCycles);
	}
}

GbInstruction Execute::decodeInstruction(uint8_t* instructionBytes)
{
	return this->instDec[instructionBytes[0]];
}
bool Execute::decodePrefixInstruction(void* instance, GbInstruction inst, uint8_t* instBytes)
{
	GbInstruction  inststruction = ((Execute*)instance)->prefixInstDec[instBytes[1]];
	if(!(inststruction.execFunction(instance, inststruction, instBytes)))
	{
		LOG("EXECUTE: Prefix instruction errored");
		return false;
	}
	return true;
}
//Functions to execute each Instruction.
bool Execute::load(void* instance, GbInstruction inst, uint8_t* instBytes)
{
	Execute* thees = (Execute*) instance;
	thees->emitCycles(inst.cycles);
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
		opTwo = opOne + opTwo;
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
				thees->mem->write(address - 1, (opTwo >> 8) & 0x00FF);
				thees->mem->write(address - 2, opTwo & 0x00FF);
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

bool Execute::inc(void* instance, GbInstruction inst, uint8_t* instBytes)
{
	Execute* thees = (Execute*)instance;
	thees->emitCycles(inst.cycles);
	uint16_t opOne;
	uint16_t result;
	//Fetch Operands
	if(inst.mode == Reg_None)
	{
		opOne = 0x00FF & thees->regFile.readReg(inst.operandOne);
	}
	else if(inst.mode == MemReg16_None)
	{
		opOne = 0x00FF & thees->mem->read(thees->regFile.readRegPair(inst.operandTwo));
	}
	else if(inst.mode == Reg16_None)
	{
		//Signed immeadiate must be sign extended to at least 16 bits.
		opOne = thees->regFile.readRegPair(inst.operandOne);
	}
	else
	{
		return false;
	}
	result = opOne + 0x01;
	//16-bit INC/DEC does not affect the flags.
	if(inst.mode != Reg16_None)
	{
		//Carry is unaffected
		thees->regFile.modifyFlag(GbFlag::GbFlag::Z, (result & 0x0FF) ==0);
		//Half Carry
		thees->regFile.modifyFlag(GbFlag::GbFlag::H, (((opOne & 0x0F) + 1) == 0x10));
		//Clear N
		thees->regFile.modifyFlag(GbFlag::GbFlag::N, false);
	}
	//Write Back
	if(inst.mode == Reg_None)
	{
		thees->regFile.writeReg(inst.operandOne, (uint8_t)(result & 0x0FF));
	}
	else if(inst.mode == Mem_None)
	{
		thees->mem->write(thees->regFile.readRegPair(inst.operandTwo), (uint8_t)(result & 0x0FF));
	}
	else if(inst.mode == Reg16_None)
	{
		//Signed immeadiate must be sign extended to at least 16 bits.
		thees->regFile.writeRegPair(inst.operandOne, result);
	}
	return true;
}
bool Execute::dec(void* instance, GbInstruction inst, uint8_t* instBytes)
{
	Execute* thees = (Execute*)instance;
	thees->emitCycles(inst.cycles);
	uint16_t opOne;
	uint16_t result;
	//Fetch Operands
	if(inst.mode == Reg_None)
	{
		opOne = 0x00FF & thees->regFile.readReg(inst.operandOne);
	}
	else if(inst.mode == Mem_None)
	{
		opOne = 0x00FF & thees->mem->read(thees->regFile.readRegPair(inst.operandTwo));
	}
	else if(inst.mode == Reg16_None)
	{
		//Signed immeadiate must be sign extended to at least 16 bits.
		opOne = thees->regFile.readRegPair(inst.operandOne);
	}
	else
	{
		return false;
	}
	result = opOne - 0x01;
	//16-bit INC/DEC does not affect the flags.
	if(inst.mode != Reg16_None)
	{
		//Carry is unaffected
		thees->regFile.modifyFlag(GbFlag::GbFlag::Z, (result & 0x0FF) ==0);
		//Half Carry
		thees->regFile.modifyFlag(GbFlag::GbFlag::H, (opOne & 0x0F) < 0x01 );
		//Clear N
		thees->regFile.modifyFlag(GbFlag::GbFlag::N, true);
	}
	//Write Back
	if(inst.mode == Reg_None)
	{
		thees->regFile.writeReg(inst.operandOne, (uint8_t)(result & 0x0FF));
	}
	else if(inst.mode == Mem_None)
	{
		thees->mem->write(thees->regFile.readRegPair(inst.operandTwo), (uint8_t)(result & 0x0FF));
	}
	else if(inst.mode == Reg16_None)
	{
		//Signed immeadiate must be sign extended to at least 16 bits.
		thees->regFile.writeRegPair(inst.operandOne, result);
	}
	return true;
}
bool Execute::add(void* instance, GbInstruction inst, uint8_t* instBytes)
{
	Execute* thees = (Execute*)instance;
	thees->emitCycles(inst.cycles);
	uint16_t opOne;
	uint16_t opTwo;
	uint32_t result;
	//Fetch Operands
	if(inst.mode == Reg_Reg)
	{
		opOne = 0x00FF & thees->regFile.readReg(inst.operandOne);
		opTwo = 0x00FF & thees->regFile.readReg(inst.operandTwo);
	}
	else if(inst.mode == Reg_Imm8)
	{
		opOne = 0x00FF & thees->regFile.readReg(inst.operandOne);
		opTwo = 0x00FF & instBytes[1];
	}
	else if(inst.mode == Reg_MemReg16)
	{
		opOne = 0x00FF & thees->regFile.readReg(inst.operandOne);
		opTwo = 0x00FF & thees->mem->read(thees->regFile.readRegPair(inst.operandTwo));
	}
	else if(inst.mode == Reg16_Simm8)
	{
		//Signed immeadiate must be sign extended to at least 16 bits.
		opOne = thees->regFile.readRegPair(inst.operandOne);
		opTwo = ((instBytes[1] & 0x80) == 0x80) ? (instBytes[1]) | 0xFF00 : instBytes[1] & 0x00FF;
		//Set Zero flag to zero. This is due to 16-bit arithmetic being double pumped, other than the INC/DEC which feature 16-bit units with proper flag checks for that width.
		thees->regFile.modifyFlag(GbFlag::GbFlag::Z, false);
	}
	else if(inst.mode == Reg16_Reg16)
	{
		opOne = 0xFFFF & thees->regFile.readRegPair(inst.operandOne);
		opTwo = 0xFFFF & thees->regFile.readRegPair(inst.operandTwo);
	}
	else
	{
		return false;
	}
	//Execute Instruction
	result = opOne + opTwo;
	//Set N flag to 0
	thees->regFile.modifyFlag(GbFlag::GbFlag::N, false);
	//Write Back Results.
	if(inst.mode == Reg_Reg || inst.mode == Reg_Imm8 || inst.mode == Reg_MemReg16)
	{
		//Check carry & Half Carry
		thees->regFile.modifyFlag(GbFlag::GbFlag::H,(((opOne & 0x0F) + (opTwo & 0x0F)) & 0x10) == 0x10);
		thees->regFile.modifyFlag(GbFlag::GbFlag::C,(result & 0x100) == 0x100);
		//set zero flag
		thees->regFile.modifyFlag(GbFlag::GbFlag::Z, (uint8_t)(result & 0x00FF) == 0x00);
		thees->regFile.writeReg(inst.operandOne, (uint8_t)(result & 0x00FF));
	}
	else if(inst.mode == Reg16_Simm8 || inst.mode == Reg16_Reg16)
	{
		//Check carry and half carry.
		thees->regFile.modifyFlag(GbFlag::GbFlag::H,(((opOne & 0x0FFF) + (opTwo & 0x0FFF)) & 0x1000) == 0x1000);
		thees->regFile.modifyFlag(GbFlag::GbFlag::C,(result & 0x10000) == 0x10000);
		thees->regFile.writeRegPair(inst.operandOne, (uint16_t)(result & 0x0FFFF));
	}
	else
	{
		return false;
	}
	return true;
}
bool Execute::adc(void* instance, GbInstruction inst, uint8_t* instBytes)
{
	Execute* thees = (Execute*)instance;
	thees->emitCycles(inst.cycles);
	uint8_t opOne;
	uint8_t opTwo;
	uint16_t result;
	opOne = 0x00FF & thees->regFile.readReg(inst.operandOne);
	//Fetch Operands
	if(inst.mode == Reg_Reg)
	{
		opTwo = 0x00FF & thees->regFile.readReg(inst.operandTwo);
	}
	else if(inst.mode == Reg_Imm8)
	{
		opTwo = 0x00FF & instBytes[1];
	}
	else if(inst.mode == Reg_MemReg16)
	{
		opTwo = 0x00FF & thees->mem->read(thees->regFile.readRegPair(inst.operandTwo));
	}
	else
	{
		return false;
	}
	//Get carry bit.
	uint8_t carry = thees->regFile.checkFlag(GbFlag::GbFlag::C) ? 0x01 : 0x00;
	//Execute Instruction
	result = opOne + opTwo + carry;
	//Check Zero
	thees->regFile.modifyFlag(GbFlag::GbFlag::Z, result == 0x0000);
	//Check Carry
	thees->regFile.modifyFlag(GbFlag::GbFlag::C, (result & 0x100) == 0x100);
	//Check Half Carry
	thees->regFile.modifyFlag(GbFlag::GbFlag::H,(((opOne & 0x0F) + (opTwo &0x0F) + carry) & 0x10) == 0x10);
	//Clear N flag
	thees->regFile.modifyFlag(GbFlag::GbFlag::N, false);
	//Write Result.
	thees->regFile.writeReg(inst.operandOne, (uint8_t)(result & 0xFF));
	return true;
}
bool Execute::sub(void* instance, GbInstruction inst, uint8_t* instBytes)
{
	Execute* thees = (Execute*)instance;
	thees->emitCycles(inst.cycles);
	uint8_t opOne;
	uint8_t opTwo;
	uint16_t result;

	opOne = thees->regFile.readReg(inst.operandOne);
	if(inst.mode == Reg_Reg)
	{
		opTwo = thees->regFile.readReg(inst.operandTwo);
	}
	else if(inst.mode == Reg_Imm8)
	{
		opTwo = instBytes[1];
	}
	else if(inst.mode == Reg_MemReg16)
	{
		opTwo = thees->mem->read(thees->regFile.readRegPair(inst.operandTwo));
	}
	else
	{
		return false;
	}
	//Execute Instruction
	result = opOne - opTwo;
	//Check Half/Full Carry & Zero
	thees->regFile.modifyFlag(GbFlag::GbFlag::C, (opOne & 0x0FF) < (opTwo & 0x0FF));
	thees->regFile.modifyFlag(GbFlag::GbFlag::H, (opOne & 0x00F) < (opTwo & 0x00F));
	thees->regFile.modifyFlag(GbFlag::GbFlag::Z, (result & 0x0FF) == 0x00);
	//Set Negative
	thees->regFile.modifyFlag(GbFlag::GbFlag::N, true);
	//Write Result if not the compare instruction.
	if(inst.op != CP)
	{
		thees->regFile.writeReg(inst.operandOne, (uint8_t)(result & 0x0FF));
	}
	return true;
}
bool Execute::sbc(void* instance, GbInstruction inst, uint8_t* instBytes)
{
	Execute* thees = (Execute*)instance;
	thees->emitCycles(inst.cycles);
	uint8_t opOne;
	uint8_t opTwo;
	uint16_t result;

	opOne = thees->regFile.readReg(inst.operandOne);
	if(inst.mode == Reg_Reg)
	{
		opTwo = thees->regFile.readReg(inst.operandTwo);
	}
	else if(inst.mode == Reg_Imm8)
	{
		opTwo = instBytes[1];
	}
	else if(inst.mode == Reg_MemReg16)
	{
		opTwo = thees->mem->read(thees->regFile.readRegPair(inst.operandTwo));
	}
	else
	{
		return false;
	}
	//Get carry bit.
	uint8_t carry = thees->regFile.checkFlag(GbFlag::GbFlag::C) ? 0x01 : 0x00;
	//Execute Instruction
	result = opOne - (opTwo + carry);
	//Check Half/Full Carry & Zero
	thees->regFile.modifyFlag(GbFlag::GbFlag::C, (opOne & 0x0FF) < ((opTwo + carry) & 0x0FF));
	thees->regFile.modifyFlag(GbFlag::GbFlag::H, (opOne & 0x00F) < ((opTwo + carry) & 0x00F));
	thees->regFile.modifyFlag(GbFlag::GbFlag::Z, (result & 0x0FF) == 0x00);
	//Set Negative
	thees->regFile.modifyFlag(GbFlag::GbFlag::N, true);
	//Write Result
	thees->regFile.writeReg(inst.operandOne, (uint8_t)(result & 0x0FF));
	return true;
}
bool Execute::bwAnd(void* instance, GbInstruction inst, uint8_t* instBytes)
{
	Execute* thees = (Execute*)instance;
	thees->emitCycles(inst.cycles);
	uint8_t opOne;
	uint8_t opTwo;
	uint8_t result;

	opOne = thees->regFile.readReg(inst.operandOne);
	if(inst.mode == Reg_Reg)
	{
		opTwo = thees->regFile.readReg(inst.operandTwo);
	}
	else if(inst.mode == Reg_Imm8)
	{
		opTwo = instBytes[1];
	}
	else if(inst.mode == Reg_MemReg16)
	{
		opTwo = thees->mem->read(thees->regFile.readRegPair(inst.operandTwo));
	}
	else
	{
		return false;
	}
	//Execute Instruction
	result = opOne & opTwo;
	//Check Zero Flag
	thees->regFile.modifyFlag(GbFlag::GbFlag::Z, result == 0x00);
	//Set flags
	thees->regFile.modifyFlag(GbFlag::GbFlag::N, false);
	thees->regFile.modifyFlag(GbFlag::GbFlag::H, true);
	thees->regFile.modifyFlag(GbFlag::GbFlag::C, false);
	//Write Result
	thees->regFile.writeReg(inst.operandOne, result);
	return true;
}
bool Execute::bwXor(void* instance, GbInstruction inst, uint8_t* instBytes)
{
	Execute* thees = (Execute*)instance;
	thees->emitCycles(inst.cycles);
	uint8_t opOne;
	uint8_t opTwo;
	uint8_t result;

	opOne = thees->regFile.readReg(inst.operandOne);
	if(inst.mode == Reg_Reg)
	{
		opTwo = thees->regFile.readReg(inst.operandTwo);
	}
	else if(inst.mode == Reg_Imm8)
	{
		opTwo = instBytes[1];
	}
	else if(inst.mode == Reg_MemReg16)
	{
		opTwo = thees->mem->read(thees->regFile.readRegPair(inst.operandTwo));
	}
	else
	{
		return false;
	}
	//Execute Instruction
	result = opOne ^ opTwo;
	//Check Zero Flag
	thees->regFile.modifyFlag(GbFlag::GbFlag::Z, result == 0x00);
	//Set flags
	thees->regFile.modifyFlag(GbFlag::GbFlag::N, false);
	thees->regFile.modifyFlag(GbFlag::GbFlag::H, false);
	thees->regFile.modifyFlag(GbFlag::GbFlag::C, false);
	//Write Result
	thees->regFile.writeReg(inst.operandOne, result);
	return true;
}
bool Execute::bwOr(void* instance, GbInstruction inst, uint8_t* instBytes)
{
	Execute* thees = (Execute*)instance;
	thees->emitCycles(inst.cycles);
	uint8_t opOne;
	uint8_t opTwo;
	uint8_t result;

	opOne = thees->regFile.readReg(inst.operandOne);
	if(inst.mode == Reg_Reg)
	{
		opTwo = thees->regFile.readReg(inst.operandTwo);
	}
	else if(inst.mode == Reg_Imm8)
	{
		opTwo = instBytes[1];
	}
	else if(inst.mode == Reg_MemReg16)
	{
		opTwo = thees->mem->read(thees->regFile.readRegPair(inst.operandTwo));
	}
	else
	{
		return false;
	}
	//Execute Instruction
	result = opOne | opTwo;
	//Check Zero Flag
	thees->regFile.modifyFlag(GbFlag::GbFlag::Z, result == 0x00);
	//Set flags
	thees->regFile.modifyFlag(GbFlag::GbFlag::N, false);
	thees->regFile.modifyFlag(GbFlag::GbFlag::H, false);
	thees->regFile.modifyFlag(GbFlag::GbFlag::C, false);
	//Write Result
	thees->regFile.writeReg(inst.operandOne, result);
	return true;
}
bool Execute::bit(void* instance, GbInstruction inst, uint8_t* instBytes)
{
	Execute* thees = (Execute*)instance;
	thees->emitCycles(inst.cycles);
	//Operand One is the Bit operand. operand two is the register operand.
	uint8_t operand;
	//There are two addressing mode for thees Instruction. RegNone, and MemNone.
	if(inst.mode == Reg_None)
	{
		//Fetch Register.
		operand = thees->regFile.readReg(inst.operandTwo);
	}
	else if(inst.mode == MemReg16_None)
	{
		//Fetch Operand from memory.
		//Read Reg Pair
		uint16_t address = thees->regFile.readRegPair(inst.operandTwo);
		operand = thees->mem->read(address);
	}
	else
	{
		return false;
	}
	//Execute Instruction
	//create operand mask value. shift 1 left by bit nneded to test.
	uint8_t mask = 0x00 | 0x01 << GbFlag::getShiftValue(inst.condition);
	//Operand, is now wearing a chin diaper.
	operand = operand & mask;
	//I like thees line 9 line iof/else into a 1 liner....
	thees->regFile.modifyFlag(GbFlag::GbFlag::Z, operand >> GbFlag::getShiftValue(inst.condition) == 0x01);
	//Clear N, Set H.
	thees->regFile.modifyFlag(GbFlag::GbFlag::N, false);
	thees->regFile.modifyFlag(GbFlag::GbFlag::H, true);
	//Increment The Program Counter
	return true;
}
bool Execute::res(void* instance, GbInstruction inst, uint8_t* instBytes)
{
	Execute* thees = (Execute*)instance;
	thees->emitCycles(inst.cycles);
	//Operand One is the Bit operand. operand two is the register operand.
	uint8_t operand;
	uint16_t address;
	//There are two addressing mode for thees Instruction. RegNone, and MemNone.
	if(inst.mode == Reg_None)
	{
		//Fetch Register.
		operand = thees->regFile.readReg(inst.operandTwo);
	}
	else if(inst.mode == MemReg16_None)
	{
		//Fetch Operand from memory.
		//Read Reg Pair
		address = thees->regFile.readRegPair(inst.operandTwo);
		operand = thees->mem->read(address);
	}
	else
	{
		return false;
	}
	//clear bit inst.operandOne in the register or memory location pointed to by operand b.
	uint8_t mask = ~(0x00 | 0x01 << GbFlag::getShiftValue(inst.condition));
	//WEAR YOU"RE MASK COORECTlY;
	operand = operand & mask;
	//write back operand one.
	if(inst.mode == Reg_None)
	{
		//Write Register.
		thees->regFile.writeReg(inst.operandTwo, operand);
	}
	else
	{
		//Write Operand to memory.
		//Read Reg Pair
		thees->mem->write(address, operand);
	}
	//Flags RES does not affect any flags.
	//Increment The Program Counter
	return true;
}
bool Execute::set(void* instance, GbInstruction inst, uint8_t* instBytes)
{
	Execute* thees = (Execute*)instance;
	thees->emitCycles(inst.cycles);
	uint8_t operand;
	uint16_t address;
	//There are two addressing mode for thees Instruction. RegNone, and MemNone.
	if(inst.mode == Reg_None)
	{
		//Fetch Register.
		operand = thees->regFile.readReg(inst.operandTwo);
	}
	else if(inst.mode == MemReg16_None)
	{
		//Fetch Operand from memory.
		//Read Reg Pair
		address = thees->regFile.readRegPair(inst.operandTwo);
		operand = thees->mem->read(address);
	}
	else
	{
		return false;
	}
	//clear bit inst.operandOne in the register or memory location pointed to by operand b.
	uint8_t mask = 0x01 << GbFlag::getShiftValue(inst.condition);
	//WEAR YOU"RE MASK COORECTlY;
	operand = operand | mask;
	//write back operand one.
	if(inst.mode == Reg_None)
	{
		//Write Register.
		thees->regFile.writeReg(inst.operandTwo, operand);
	}
	else
	{
		//Write Operand to memory.
		//Read Reg Pair
		thees->mem->write(address, operand);
	}
	//Flags SET does not affect any flags.
	//Increment The Program Counter
	return true;
}
bool Execute::nop(void* instance, GbInstruction inst, uint8_t* instBytes)
{
	Execute* thees = (Execute*)instance;
	thees->emitCycles(inst.cycles);
	return true; //lol
}
bool Execute::swap(void* instance, GbInstruction inst, uint8_t* instBytes)
{
	Execute* thees = (Execute*)instance;
	thees->emitCycles(inst.cycles);
	uint8_t operand;
	uint16_t address;
	//There are two addressing mode for thees Instruction. RegNone, and MemNone.
	if(inst.mode == Reg_None)
	{
		//Fetch Register.
		operand = thees->regFile.readReg(inst.operandTwo);
	}
	else if(inst.mode == MemReg16_None)
	{
		//Fetch Operand from memory.
		//Read Reg Pair
		address = thees->regFile.readRegPair(inst.operandTwo);
		operand = thees->mem->read(address);
	}
	else
	{
		return false;
	}
	operand = ((operand & 0x0F) << 4) | ((operand & 0xF0) >> 4);
	//Check Zero Flag
	thees->regFile.modifyFlag(GbFlag::GbFlag::Z, operand == 0x00);
	//Set flags
	thees->regFile.modifyFlag(GbFlag::GbFlag::N, false);
	thees->regFile.modifyFlag(GbFlag::GbFlag::H, false);
	thees->regFile.modifyFlag(GbFlag::GbFlag::C, false);
	//write back operand one.
	if(inst.mode == Reg_None)
	{
		//Write Register.
		thees->regFile.writeReg(inst.operandTwo, operand);
	}
	else
	{
		//Write Operand to memory.
		//Read Reg Pair
		thees->mem->write(address, operand);
	}
	//Flags SET does not affect any flags.
	//Increment The Program Counter
	return true;
}
bool Execute::jp(void* instance, GbInstruction inst, uint8_t* instBytes)
{
	Execute* thees = (Execute*)instance;
	uint16_t address = ((instBytes[2] << 8) & 0xFF00) | (instBytes[1] & 0x00FF);
	bool jump = false;
	if(inst.condition == GbFlag::GbFlag::C)
	{
		jump = thees->regFile.checkFlag(GbFlag::GbFlag::C);
	}
	else if (inst.condition == GbFlag::GbFlag::Z)
	{
		jump = thees->regFile.checkFlag(GbFlag::GbFlag::Z);
	}
	else if (inst.condition == GbFlag::GbFlag::NC)
	{
		jump = !(thees->regFile.checkFlag(GbFlag::GbFlag::C));
	}
	else if (inst.condition == GbFlag::GbFlag::NZ)
	{
		jump = !(thees->regFile.checkFlag(GbFlag::GbFlag::Z));
	}
	else if (inst.condition == GbFlag::GbFlag::T)
	{
		jump = true;
	}
	else
	{
		return false;
	}
	if(jump)
	{
		thees->regFile.writeRegPair(GbRegister::GbRegister::PC, address);
		// emit cycles
		thees->emitCycles(inst.cyclesTaken);
	}
	else {
		// emit not taken cycles.
		thees->emitCycles(inst.cycles);
	}
	return true;
}
bool Execute::jr(void* instance, GbInstruction inst, uint8_t* instBytes)
{
	Execute* thees = (Execute*)instance;
	uint16_t pcOffset = ((instBytes[1] & 0x80) == 0x80) ? (instBytes[1]) | 0xFF00 : instBytes[1] & 0x00FF;
	bool jump = false;
	if(inst.condition == GbFlag::GbFlag::C)
	{
		jump = thees->regFile.checkFlag(GbFlag::GbFlag::C);
	}
	else if (inst.condition == GbFlag::GbFlag::Z)
	{
		jump = thees->regFile.checkFlag(GbFlag::GbFlag::Z);
	}
	else if (inst.condition == GbFlag::GbFlag::NC)
	{
		jump = !(thees->regFile.checkFlag(GbFlag::GbFlag::C));
	}
	else if (inst.condition == GbFlag::GbFlag::NZ)
	{
		jump = !(thees->regFile.checkFlag(GbFlag::GbFlag::Z));
	}
	else if (inst.condition == GbFlag::GbFlag::T)
	{
		jump = true;
	}
	else
	{
		return false;
	}
	if(jump)
	{
		thees->regFile.incPc(pcOffset);
		//emit cycles
		thees->emitCycles(inst.cyclesTaken);
	}
	else {
		//emit not taken cycles.
		thees->emitCycles(inst.cycles);
	}
	return true;
}
bool Execute::call(void* instance, GbInstruction inst, uint8_t* instBytes)
{
	Execute* thees = (Execute*)instance;
	uint16_t address = ((instBytes[2] << 8) & 0xFF00) | (instBytes[1] & 0x00FF);
	bool jump = false;
	if(inst.condition == GbFlag::GbFlag::C)
	{
		jump = thees->regFile.checkFlag(GbFlag::GbFlag::C);
	}
	else if (inst.condition == GbFlag::GbFlag::Z)
	{
		jump = thees->regFile.checkFlag(GbFlag::GbFlag::Z);
	}
	else if (inst.condition == GbFlag::GbFlag::NC)
	{
		jump = !(thees->regFile.checkFlag(GbFlag::GbFlag::C));
	}
	else if (inst.condition == GbFlag::GbFlag::NZ)
	{
		jump = !(thees->regFile.checkFlag(GbFlag::GbFlag::Z));
	}
	else if (inst.condition == GbFlag::GbFlag::T)
	{
		jump = true;
	}
	else
	{
		return false;
	}
	if(jump)
	{
		//get current sp
		uint16_t sp = thees->regFile.readRegPair(inst.operandOne) - 0x0001, pc = thees->regFile.readRegPair(inst.operandTwo) + inst.length;
		thees->mem->write(sp, (pc >> 8) & 0x00FF);
		thees->mem->write(sp - 1, pc & 0x00FF);
		thees->regFile.decSp();
		thees->regFile.writeRegPair(inst.operandTwo, address);
		// emit cycles
		thees->emitCycles(inst.cyclesTaken);
	}
	else {
		//TODO emit not taken cycles.
		thees->emitCycles(inst.cycles);
	}
	return true;
}
bool Execute::ret(void* instance, GbInstruction inst, uint8_t* instBytes)
{
	Execute* thees = (Execute*)instance;
	bool jump = false;
	if(inst.condition == GbFlag::GbFlag::C)
	{
		jump = thees->regFile.checkFlag(GbFlag::GbFlag::C);
	}
	else if (inst.condition == GbFlag::GbFlag::Z)
	{
		jump = thees->regFile.checkFlag(GbFlag::GbFlag::Z);
	}
	else if (inst.condition == GbFlag::GbFlag::NC)
	{
		jump = !(thees->regFile.checkFlag(GbFlag::GbFlag::C));
	}
	else if (inst.condition == GbFlag::GbFlag::NZ)
	{
		jump = !(thees->regFile.checkFlag(GbFlag::GbFlag::Z));
	}
	else if (inst.condition == GbFlag::GbFlag::T)
	{
		jump = true;
	}
	else
	{
		return false;
	}
	if(jump)
	{
		//get current sp
		uint16_t sp = thees->regFile.readRegPair(inst.operandOne);
		uint16_t pc = ((thees->mem->read(sp + 1) << 8) & 0xFF00) | thees->mem->read(sp);
		thees->regFile.incSp();
		thees->regFile.writeRegPair(inst.operandTwo, pc);
		//emit cycles
		thees->emitCycles(inst.cyclesTaken);
	}
	else {
		//emit not taken cycles.
		thees->emitCycles(inst.cycles);
	}
	if (inst.op == RETI)
	{
		//TODO emit interrupt return.
	}
	return true;
}
bool Execute::rst(void* instance, GbInstruction inst, uint8_t* instBytes)
{
	Execute* thees = (Execute*)instance;
	thees->emitCycles(inst.cycles);
	uint16_t address = GbFlag::getRstAddress(inst.condition);
	uint16_t pc = thees->regFile.readRegPair(inst.operandOne) + 1, sp = thees->regFile.readRegPair(inst.operandTwo) - 1;
	//push address of next instruciton
	thees->mem->write(sp, (pc >> 8) & 0x00FF);
	thees->mem->write(sp - 1, pc & 0x00FF);
	thees->regFile.decSp();
	//write prog cntr to reset address
	thees->regFile.writeRegPair(inst.operandOne, address);
	return true;
}


bool Execute::rotate(void* instance, GbInstruction inst, uint8_t* instBytes)
{
	Execute* thees = (Execute*)instance;
	thees->emitCycles(inst.cycles);
	uint8_t opOne;
	uint16_t address;
	if(inst.mode == MemReg16_None)
	{
		address = thees->regFile.readRegPair(inst.operandOne);
		opOne = thees->mem->read(address);
	}
	else if(inst.mode == Reg_None)
	{
		opOne = thees->regFile.readReg(inst.operandOne);
	}
	else
	{
		return false;
	}

	//Each rotate is unique
	switch(inst.op)
	{
		case RRCA :{
			//Rotate A right. Old bit 0 to Carry flag.
			thees->regFile.modifyFlag(GbFlag::GbFlag::C, (opOne & 0x01) == 0x01);
			opOne = (opOne >> 1) & 0x7F;
			opOne = opOne | (thees->regFile.checkFlag(GbFlag::GbFlag::C) ? 0x10 : 0x00);
			thees->regFile.modifyFlag(GbFlag::GbFlag::Z, false);
			break;
		}
		case RLCA :{
			//Rotate A left. Old bit 7 to Carry flag.
			thees->regFile.modifyFlag(GbFlag::GbFlag::C, (opOne & 0x10) == 0x10);
			opOne = (opOne << 1) & 0xFE;
			opOne = opOne | (thees->regFile.checkFlag(GbFlag::GbFlag::C) ? 0x01 : 0x00);
			thees->regFile.modifyFlag(GbFlag::GbFlag::Z, false);
			break;
		}
		case RRA :{
			//Rotate right: carry to bit 7, bit 0 to carry
			bool temp = (opOne & 0x01) == 0x01;
			opOne = ((opOne >> 1) & 0x7F) | (thees->regFile.checkFlag(GbFlag::GbFlag::C) ? 0x10 : 0x00);
			thees->regFile.modifyFlag(GbFlag::GbFlag::C, temp);
			thees->regFile.modifyFlag(GbFlag::GbFlag::Z, false);
			break;
		}
		case RLA :{
			//Rotate left: carry to bit 0, bit 7 to carry
			bool temp = (opOne & 0x10) == 0x10;
			opOne = ((opOne << 1) & 0xFE) | (thees->regFile.checkFlag(GbFlag::GbFlag::C) ? 0x01 : 0x00);
			thees->regFile.modifyFlag(GbFlag::GbFlag::C, temp);
			thees->regFile.modifyFlag(GbFlag::GbFlag::Z, false);
			break;
		}
		case RRC :{
			//Rotate A right. Old bit 0 to Carry flag.
			thees->regFile.modifyFlag(GbFlag::GbFlag::C, (opOne & 0x01) == 0x01);
			opOne = (opOne >> 1) & 0x7F;
			opOne = opOne | (thees->regFile.checkFlag(GbFlag::GbFlag::C) ? 0x10 : 0x00);
			thees->regFile.modifyFlag(GbFlag::GbFlag::Z, opOne == 0x00);
			break;
		}
		case RLC :{
			//Rotate A left. Old bit 7 to Carry flag.
			thees->regFile.modifyFlag(GbFlag::GbFlag::C, (opOne & 0x10) == 0x10);
			opOne = (opOne << 1) & 0xFE;
			opOne = opOne | (thees->regFile.checkFlag(GbFlag::GbFlag::C) ? 0x01 : 0x00);
			thees->regFile.modifyFlag(GbFlag::GbFlag::Z,opOne == 0x00);
			break;
		}
		case RR :{
			//Rotate right: carry to bit 7, bit 0 to carry
			bool temp = (opOne & 0x01) == 0x01;
			opOne = ((opOne >> 1) & 0x7F) | (thees->regFile.checkFlag(GbFlag::GbFlag::C) ? 0x10 : 0x00);
			thees->regFile.modifyFlag(GbFlag::GbFlag::C, temp);
			thees->regFile.modifyFlag(GbFlag::GbFlag::Z, opOne == 0x00);
			break;
		}
		case RL :{
			//Rotate left: carry to bit 0, bit 7 to carry
			bool temp = (opOne & 0x10) == 0x10;
			opOne = ((opOne << 1) & 0xFE) | (thees->regFile.checkFlag(GbFlag::GbFlag::C) ? 0x01 : 0x00);
			thees->regFile.modifyFlag(GbFlag::GbFlag::C, temp);
			thees->regFile.modifyFlag(GbFlag::GbFlag::Z, opOne == 0x00);
			break;
		}
		default :{
			return false;
		}
	}
	//reset H and N flags
	thees->regFile.modifyFlag(GbFlag::GbFlag::H, false);
	thees->regFile.modifyFlag(GbFlag::GbFlag::N, false);

	if(inst.mode == MemReg16_None)
	{
		thees->mem->write(address, opOne);
	}
	else
	{
		thees->regFile.writeReg(inst.operandOne, opOne);
	}
}

bool Execute::shift(void* instance, GbInstruction inst, uint8_t* instBytes)
{
	Execute* thees = (Execute*)instance;
	thees->emitCycles(inst.cycles);
	uint8_t opOne;
	uint16_t address;
	if(inst.mode == MemReg16_None)
	{
		address = thees->regFile.readRegPair(inst.operandOne);
		opOne = thees->mem->read(address);
	}
	else if(inst.mode == Reg_None)
	{
		opOne = thees->regFile.readReg(inst.operandOne);
	}
	else
	{
		return false;
	}

	//Each rotate is unique
	switch(inst.op)
	{
		case SLA :{
			//Shift left, bit 7 to C, 0 to bit 0
			thees->regFile.modifyFlag(GbFlag::GbFlag::C, (opOne & 0x10) == 0x10);
			opOne = (opOne << 1) & 0xFE;
			break;
		}
		case SRA :{
			//Shift right, bit 0 to C, bit 7 remains unchanged
			thees->regFile.modifyFlag(GbFlag::GbFlag::C, (opOne & 0x01) == 0x01);
			opOne = ((opOne >> 1) & 0x7F) | (opOne & 0x10);
			break;
		}
		case SRL :{
			//Shift right, 0 to bit 7, bit 0 to C
			thees->regFile.modifyFlag(GbFlag::GbFlag::C, (opOne & 0x01) == 0x01);
			opOne = (opOne >> 1) & 0x7F;
			break;
		}
		default :{
			return false;
		}
	}
	//reset H and N flags
	thees->regFile.modifyFlag(GbFlag::GbFlag::H, false);
	thees->regFile.modifyFlag(GbFlag::GbFlag::N, false);
	//Set Z if result is zero.
	thees->regFile.modifyFlag(GbFlag::GbFlag::Z, opOne = 0x00);

	if(inst.mode == MemReg16_None)
	{
		thees->mem->write(address, opOne);
	}
	else
	{
		thees->regFile.writeReg(inst.operandOne, opOne);
	}

}

bool illegal(void* instance, GbInstruction inst, uint8_t* instBytes)
{
	LOG("Illegal Opcode");
	return false;
}

/*
<++> Execute::<++>()
{

}
*/
