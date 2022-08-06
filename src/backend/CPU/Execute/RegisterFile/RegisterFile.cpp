/*==================================================================================
 *Class - RegisterFile
 *Author - Zach Walden
 *Created - 7/22/22
 *Last Changed - 7/25/22
 *Description - GameBoy Register File Implementation.
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

#include "RegisterFile.hpp"

using namespace std;

 RegisterFile::RegisterFile()
{

}

 RegisterFile::~RegisterFile()
{

}

uint8_t RegisterFile::readReg(GbRegister reg)
{
	return this->regFile[reg].readReg()
}

void RegisterFile::writeReg(GbRegister reg, uint8_t newValue)
{
	this->regFile[reg].writeReg(newValue);
}

uint16_t RegisterFile::readRegPair(GbRegister regPair)
{
	GbRegister readVals[2];
	uint16_t retVal;
	switch(regPair
	{
		case AF :
		{
			readVals = {A, F};
			break;
		}
		case BC :
		{
			readVals = {B, C};
			break;
		}
		case DE :
		{
			readVals = {D, E};
			break;
		}
		case HL :
		{
			readVals = {H, L};
			break;
		}
		case SP :
		{
			retVal = this->sp;
		}
		case default : break;
	}

	retVal = ((readVals[0] << 8) & 0x0FF00) | (readVals[1] & 0x0FF);
	return retVal;
}

void RegisterFile::modifyFlag(GbFlag flag, bool newVal)
{
	//read flags
	uint8_t flags = this->readReg(F);
	//modify flags
	uint8_t modVal;
	if(newVal)
	{
		modVal = 0x00 | 1 << flag;
		flags = flags | modVal
	}
	else
	{
		modVal = ~(0x00 | 1 << flag);
		flags = flags & modVal;
	}
	//write flags
	this->writeReg(F, flags);
}

bool RegisterFile::checkFlag(GbFlag flag)
{
	uint8_t flags = this->readReg(F);
	uint8_t val = (flags >> flag) & 0x01;
	bool retVal = False;
	if(val == 1)
	{
		retVal = true;
	}
	return retVal;
}

/*
<++> RegisterFile::<++>()
{

}
*/
