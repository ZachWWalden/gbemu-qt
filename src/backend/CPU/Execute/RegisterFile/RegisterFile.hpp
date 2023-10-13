/*==================================================================================
 *Class - RegisterFile
 *Author - Zach Walden
 *Created - 7/22/22
 *Last Changed - 10/13/23
 *Description - Register File, models gameboy. 8 registers A F, B C, D E, H L
 * 		Each 8-bit register may be paired with its
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
#include "Register/Register.hpp"
#include "ProgramCounter/ProgramCounter.hpp"
#include "StackPointer/StackPointer.hpp"
#include <cstdint>

#define NUM_REG 8

namespace GbRegister
{
enum GbRegister
{
	A=0, F=1, B=2, C=3, D=4, E=5, H=6, L=7, AF, BC, DE, HL, SP, PC, NONE
};
}
namespace GbFlag
{
enum GbFlag
{
	NONE, Z = 7, N = 6, H = 5, C = 4, T, TI, NZ, NC, PoI, PoD, H00, H08, H10, H18, H20, H28, H30, H38, B0, B1, B2, B3, B4, B5, B6, B7
};
}

class RegisterFile
{
	//Attributes
public:

private:
	Register regFile[NUM_REG];
	ProgramCounter pc;
	StackPointer sp;
	//Methods
public:
	RegisterFile();
	~RegisterFile();

	uint8_t readReg(GbRegister::GbRegister reg);
	void writeReg(GbRegister::GbRegister reg, uint8_t newValue);

	uint16_t readRegPair(GbRegister::GbRegister regPair);
	void writeRegPair(GbRegister::GbRegister regPair, uint16_t newValue);

	void incPc(uint8_t incVal);
	void incSp();
	void decSp();

	void modifyFlag(GbFlag::GbFlag flag, bool newVal);
	bool checkFlag(GbFlag::GbFlag flag);
private:
};
