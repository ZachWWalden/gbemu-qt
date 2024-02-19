/*==================================================================================
 *Class - IoRam
 *Author - Zach Walden
 *Created - 2/19/24
 *Last Changed - 2/19/24
 *Description - I/O Registers.
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

//$FF00 -> FF7F
#include <cstdint>
namespace io_reg
{
	enum IoReg
	{
		//Joypad input
		P1JOYP = 0x00,
		//Serial Transfer
		SB = 0x01,
		SC = 0x02,
		//Timer & Divider
		DIV = 0x04,
		TMIA = 0x05,
		TMA = 0x06,
		TAC = 0x07,
		//Interrupt Flags
		IF = 0x0F,
		//Audio
		NR10 = 0x10,
		NR11 = 0x11,
		NR12 = 0x12,
		NR13 = 0x13,
		NR14 = 0x14,
		NR21 = 0x16,
		NR22 = 0x17,
		NR23 = 0x18,
		NR24 = 0x19,
		NR30 = 0x1A,
		NR31 = 0x1B,
		NR32 = 0x1C,
		NR33 = 0x1D,
		NR34 = 0x1E,
		NR41 = 0x20,
		NR42 = 0x21,
		NR43 = 0x22,
		NR44 = 0x23,
		NR50 = 0x24,
		NR51 = 0x25,
		NR52 = 0x26,
		//Wave Ram
		WRSTRT = 0x30,
		WREND = 0x3F,
		//LCD Control, Status, Position, Scrolling, and Palettes
		LCDC = 0x40,
		STAT = 0x41,
		SCY = 0x42,
		SCX = 0x43,
		LY = 0x44,
		LYC = 0x45,
		DMA = 0x46,
		BGP = 0x47,
		OBP0 = 0x48,
		OBP1 = 0x49,
		WY = 0x4A,
		WX = 0x4B,
		//VRAM
		KEY1 = 0x4D,
		VBK = 0x4F,
		DBOOTR = 0x50, // set to non zero to disable boot rom
		HDMA1 = 0x51,
		HDMA2 = 0x52,
		HDMA3 = 0x53,
		HDMA4 = 0x54,
		HDMA5 = 0x55,
		RP = 0x56,
		BCPS_BGPI = 0x68,
		BCPD_BGPD = 0x69,
		OCPS_OBPI = 0x6A,
		OCPD_OBPD = 0x6B,
		OPRI = 0x6C,
		SVBK = 0x70,
		PCM12 = 0x76,
		PCM34 = 0x77,
		IE = 0xFF
	};
};

class IoRam
{
	//Attributes
public:

private:
	//Methods
public:
	IoRam();
	~IoRam();

	uint8_t read(uint16_t addr);
	void write(uint16_t addr, uint8_t value);

private:
	uint8_t readReg(uint8_t reg);
};
