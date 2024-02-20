/*==================================================================================
 *Class - InterruptController
 *Author - Zach Walden
 *Created -
 *Last Changed -
 *Description -
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

#include "InterruptController.hpp"
#include <cstdint>

InterruptController::InterruptController(MMU* newMem)
{
	this->mem = newMem;
}
InterruptController::~InterruptController()
{

}

void InterruptController::getNextPC()
{

}

void InterruptController::setIME(bool nextIme)
{
	this->ime = nextIme;
}

	//Execute Class will call this when it encounters a halt/stop instruciton.
void InterruptController::processControlEvent()
{

}
	//this will be called each instruction cycle to check for interrupts.
void InterruptController::handleInterrupts()
{
	uint8_t ifr = this->mem->read(0xFF0F); //Read IF register
	uint8_t ier = this->mem->read(0xFFFF); //Read IE register
	uint8_t validInt = ifr & ier;
	//Check Each bit
	if(this->ime)
	{
		if((validInt & 0x01) == 0x01)
		{
			//vblank requested
			this->isrAddr = 0x0040;
		}
		else if((validInt & 0x02) == 0x02)
		{
			//lcd requested
			this->isrAddr = 0x0048;
		}
		else if((validInt & 0x04) == 0x04)
		{
			//timer requested
			this->isrAddr = 0x0050;
		}
		else if ((validInt & 0x08) == 0x08)
		{
			//serial requested
			this->isrAddr = 0x0058;
		}
		else if((validInt & 0x10) == 0x10)
		{
			//joypad requested
			this->isrAddr = 0x0060;
		}
		this->processControlEvent(GbInt::GbEvent::INTERRUPT);
	}
}


/*
<++> InterruptController::<++>()
{

}
*/
