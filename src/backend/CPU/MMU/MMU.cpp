/*==================================================================================
 *Class - MMU
 *Author - Zach Walden
 *Created - 7/22/22
 *Last Changed - 7/22/22
 *Description - Memory Management Unit. This unit hanldes all mapping of memory.
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

#include "MMU.hpp"


MMMU::MU(Cartridge* cart, VRAM* vram, IoRam* ioRam, InterruptController* intController, OamRam* oamRam)
{
	this->cart = cart;
	this->vram = vram;
	this->ioRam = ioRam;
	this->intController = intController;
}
~MMU::MMU()
{

}
uint8_t MMU::read(uint16_t address)
{
	uint8_t retVal = 0;
	//Decode what location in memory the address points to.
	MemUnit memUnit = this->decodeAddress(address);
	//Choose which read function to call
	switch(memUnit)
	{
		case VRAM :
		{
			retVal = this->readVram(address);
			break;
		}
		case CART :
		{
			retVal = this->readCartridge(address);
			break;
		}
		case OAM :
		{
			retVal = this->readOamRam(address);
			break;
		}
		case IO_RAM :
		{
			retVal = this->readIoRam(address);
			break;
		}
		case INT_RAM :
		{
			retVal = this->internalRam.read(address);
			break;
		}
		case INT_RAM_ECHO :
		{
			address = address - (uint16_t)0x2000;
			retVal = this->internalRam.read(address);
			break;
		}
		case BOOT_ROM :
		{
			retVal = this->bootRom.read(address);
			break;
		}
		case HRAM :
		{
			retVal = this->hRam.read(address);
			break;
		}
		case INT_REG :
		{
			retVal = this.readIntReg();
			break;
		}
		case NONE :
		{
			break;
		}
		case default :
		{
			break;
		}
	}
}
void MMU::write(uint16_t address, uint8_t newValue)
{
	//Decode what location in memory the address points to.
	MemUnit memUnit = this->decodeAddress(address);
	//Choose which read function to call
	switch(memUnit)
	{
		case VRAM :
		{
			this->writeVram(address, newValue);
			break;
		}
		case CART :
		{
			this->writeCartridge(address, newValue);
			break;
		}
		case OAM :
		{
			this->writeOamRam(address, newValue);
			break;
		}
		case IO_RAM :
		{
			this->writeIoRam(address, newValue);
			break;
		}
		case INT_RAM :
		{
			this->internalRam.write(address, newValue);
			break;
		}
		case INT_RAM_ECHO :
		{
			address = address - (uint16_t)0x2000;
			this->internalRam.write(address, newValue);
			break;
		}
		case BOOT_ROM :
		{
			break;
		}
		case HRAM :
		{
			this->hRam.write(address, newValue);
			break;
		}
		case INT_REG :
		{
			this->writeIntReg(newValue);
			break;
		}
		case NONE :
		{
			break;
		}
		case default :
		{
			break;
		}
	}
}

MemUnit MMU::decodeAddress(uint16_t address)
{
	MemUnit retVal = NONE;
	if(address >= 0x0000 && address < 0x0100)
	{
		//Check if bootRom is enabled
		if()
		{
			retVal = BOOT_ROM;
		}
		else
		{
			retVal = CART;
		}
	}
	else if(address <= this->cartBank1End)
	{
		retVal = CART;
	}
	else if(address <= this->vRamEnd)
	{
		retVal = VRAM;
	}
	else if(address <= this->ramEnd)
	{
		retVal = INT_RAM;
	}
	else if(address < this->oamRamStart)
	{
		retVal = INT_RAM_ECHO;
	}
	else if(address <= this->oamRamEnd)
	{
		retVal = OAM;
	}
	else if(address < this->ioRamStart)
	{
		retVal = NONE;
	}
	else if(address <= this->ioRamEnd)
	{
		retVal = IO_RAM;
	}
	else if(address <= this->hRamEnd)
	{
		retVal = HRAM;
	}
	else
	{
		retVal = INT_REG;
	}

	return retVal;
}
uint8_t MMU::readOamRam(uint16_t address)
{
	return this->oamRam->read(address);
}
void MMU::writeOamRam(uint16_t address, uint8_t newValue)
{
	this->oamRam->write(address, newValue);
}
uint8_t MMU::readVram(uint16_t address)
{
	return this->vram->read(address);
}
void MMU::writeVram(uint16_t address, uint8_t newValue)
{
	this->vram->write(address, newValue);
}
uint8_t MMU::readIoRam(uint16_t address)
{
	return this->ioRam(address);
}
void MMU::writeIoRam(uint16_t address, uint8_t newValue)
{
	this->ioRam->write(address, newValue);
}
uint8_t MMU::readCartridge(uint16_t address)
{
	return this->cart->read(address);
}
void MMU::writeCartridge(uint16_t address, uint8_t newValue)
{
	this->cart->write(address, newValue);
}
uint8_t MMU::readIntReg()
{
	return this->intController.readIntReg();
}
void MMU::writeIntReg(uint8_t newValue)
{
	this->intController->write(newValue);
}

/*
<++> MMU::<++>()
{

}
*/
