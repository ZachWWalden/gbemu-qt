/*==================================================================================
 *Class - BootRom
 *Author - Zach Walden
 *Created - 7/25/22
 *Last Changed - 7/25/22
 *Description - Selects and Presents Correct Boot Rom based off the Presented Rom
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

#include "BootRom.hpp"

 BootRom::BootRom()
{

}
 BootRom::~BootRom()
{

}
uint8_t BootRom::read(uint16_t address)
{
	uint8_t localAddr = address & (uint8_t) 0x0FF;
	return this->dmgBootRom[localAddr];
}
void BootRom::write(uint16_t address, uint8_t newValue)
{
	uint8_t localAddr = address & (uint8_t) 0x0FF;
	this->dmgBootRom[localAddr] = newValue;
}

void BootRom::disableBootRom()
{
	this->enabled = false;
}

/*
<++> BootRom::<++>()
{

}
*/
