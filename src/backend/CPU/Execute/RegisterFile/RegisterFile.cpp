/*==================================================================================
 *Class - RegisterFile
 *Author - Zach Walden
 *Created - 7/22/22
 *Last Changed - 10/13/23
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
#include <cstdint>

using namespace std;

 RegisterFile::RegisterFile()
{

}

 RegisterFile::~RegisterFile()
{

}

uint8_t RegisterFile::readReg(GbRegister::GbRegister reg)
{
	return this->regFile[reg].readReg();
}

void RegisterFile::writeReg(GbRegister::GbRegister reg, uint8_t newValue)
{
	this->regFile[reg].writeReg(newValue);
}

uint16_t RegisterFile::readRegPair(GbRegister::GbRegister regPair)
{
	GbRegister::GbRegister reg1, reg2;
	bool sp = false;
	uint16_t retVal;
	switch(regPair)
	{
		case GbRegister::AF :
		{
			reg1 = GbRegister::A;
			reg2 = GbRegister::F;
			break;
		}
		case GbRegister::BC :
		{
			reg1 = GbRegister::B;
			reg2 = GbRegister::C;
			break;
		}
		case GbRegister::DE :
		{
			reg1 = GbRegister::D;
			reg2 = GbRegister::E;
			break;
		}
		case GbRegister::HL :
		{
			reg1 = GbRegister::H;
			reg2 = GbRegister::L;
			break;
		}
		case GbRegister::SP :
		{
			retVal = this->sp.read();
			sp = true;
			break;
		}
		case GbRegister::PC :
		{
			retVal = this->pc.read();
			sp = true;
			break;
		}
		default :
		{
			break;
		}
	}
	if(!sp)
	{
		retVal = ((this->readReg(reg1) << 8) & 0x0FF00) | (this->readReg(reg2) & 0x0FF);
	}
	return retVal;
}

void RegisterFile::writeRegPair(GbRegister::GbRegister regPair, uint16_t newValue)
{
	GbRegister::GbRegister reg1, reg2;
	bool sp = false;
	switch(regPair)
	{
		case GbRegister::AF :
		{
			reg1 = GbRegister::A;
			reg2 = GbRegister::F;
			break;
		}
		case GbRegister::BC :
		{
			reg1 = GbRegister::B;
			reg2 = GbRegister::C;
			break;
		}
		case GbRegister::DE :
		{
			reg1 = GbRegister::D;
			reg2 = GbRegister::E;
			break;
		}
		case GbRegister::HL :
		{
			reg1 = GbRegister::H;
			reg2 = GbRegister::L;
			break;
		}
		case GbRegister::SP :
		{
			this->sp.write(newValue);
			sp = true;
			break;
		}
		case GbRegister::PC :
		{
			this->pc.write(newValue);
			sp = true;
			break;
		}
		default : break;
	}
	if(!sp)
	{
		this->writeReg(reg1, (newValue >> 8) & 0x00FF);
		this->writeReg(reg2, newValue & 0x00FF);
	}
}

void RegisterFile::incRegPair(GbRegister::GbRegister regPair)
{
	uint16_t reg = this->readRegPair(regPair);
	reg++;
	this->writeRegPair(regPair, reg);
}

void RegisterFile::decRegPair(GbRegister::GbRegister regPair)
{
	uint16_t reg = this->readRegPair(regPair);
	reg--;
	this->writeRegPair(regPair, reg);
}

void RegisterFile::incPc(uint8_t incVal)
{
	this->pc.increment(incVal);
}
void RegisterFile::incSp()
{
	this->sp.increment();
}
void RegisterFile::decSp()
{
	this->sp.decrement();
}

void RegisterFile::modifyFlag(GbFlag::GbFlag flag, bool newVal)
{
	//read flags
	uint8_t flags = this->readReg(GbRegister::F);
	//modify flags
	uint8_t modVal;
	if(newVal)
	{
		modVal = 0x00 | 1 << flag;
		flags = flags | modVal;
	}
	else
	{
		modVal = ~(0x00 | 1 << flag);
		flags = flags & modVal;
	}
	//write flags
	this->writeReg(GbRegister::F, flags);
}

bool RegisterFile::checkFlag(GbFlag::GbFlag flag)
{
	uint8_t flags = this->readReg(GbRegister::F);
	uint8_t val = (flags >> flag) & 0x01;
	bool retVal = false;
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
