/*==================================================================================
 *Class - ProgramCounter
 *Author - Zach Walden
 *Created - 7/22/22
 *Last Changed - 7/25/22
 *Description - ProgramCounters instructions from the system memory.
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
#include <stdint.h>


class ProgramCounter
{
	//Attributes
public:

private:
	uint16_t value;
	//Methods
public:
	ProgramCounter();
	~ProgramCounter();

	uint16_t read();
	void write(uint16_t newValue);
	void increment(uint8_t incValue);

private:
};
