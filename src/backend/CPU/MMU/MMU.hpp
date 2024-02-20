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

#include "stdint.h"

#include "../../PPU/VRAM/VRAM.hpp"
#include "../../Cartridge/Cartridge.hpp"
#include "../../IoRam/IoRam.hpp"
#include "../../PPU/OamRam/OamRam.hpp"
#include "BootRom/BootRom.hpp"
#include "InternalRam/InternalRam.hpp"
#include "HRam/HRam.hpp"

enum MemUnit
{
	VRAM, CART, OAM, IO_RAM, INT_RAM, INT_RAM_ECHO, INT_REG, BOOT_ROM, HRAM, NONE
};

class MMU
{
	//Attributes
public:

private:
	//Pointers to different memories.
	Cartridge* cart;
	VRAM* vram;
	IoRam* ioRam;
	OamRam* oamRam;
	//Memories internal to the CPU
	BootRom bootRom;
	InternalRam internalRam;
	HRam hRam;
	//Address decoder values.
	int cartBank0Start = 0x0000, cartBank0End = 0x3FFF;
	int cartBank1Start = 0x4000, cartBank1End = 0x7FFF;
	//bankStartAddress = bankNumber * 16,384
	int vRamStart = 0x8000, vRamEnd = 0x9FFF;
	int exRamStart = 0xA000, exRamEnd = 0xBFFF;
	//Echoed from 0xE000 to 0xFDFF
	int ramStart = 0xC000, ramEnd = 0xDFFF;
	int oamRamStart = 0xFE00, oamRamEnd = 0xFE9F;
	int ioRamStart = 0xFF00, ioRamEnd = 0xFF4B;
	int hRamStart = 0xFF80, hRamEnd = 0xFFFF; //Technically ends at 0xFFFE, but I'm hacking the IE register in. since it is nice and clean.
	//Methods
public:
	MMU(Cartridge* cart, VRAM* vram, IoRam* ioRam, OamRam* oamRam);
	~MMU();

	uint8_t read(uint16_t address);
	void write(uint16_t address, uint8_t newValue);

private:
	MemUnit decodeAddress(uint16_t address);
	uint8_t readVram(uint16_t address);
	void writeVram(uint16_t address, uint8_t newValue);
	uint8_t readOamRam(uint16_t address);
	void writeOamRam(uint16_t address, uint8_t newValue);
	uint8_t readIoRam(uint16_t address);
	void writeIoRam(uint16_t address, uint8_t newValue);
	uint8_t readCartridge(uint16_t address);
	void writeCartridge(uint16_t address, uint8_t newValue);
};
