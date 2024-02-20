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

InterruptController::InterruptController(MMU* newMem, RegisterFile* regs, CycleListener* nListener)
{
	this->mem = newMem;
	this->regFile = regs;
	this->listener = nListener;
}
InterruptController::~InterruptController()
{

}

void InterruptController::getNextPC()
{
	//Increment Program Counter
	this->regFile->incPc(this->listener->getNumBytes());
	//Handle delaying effect of EI/DI till after the next instruction.
	if(this->imeChangePending)
	{
		if(this->imeCycleCount == 1)
		{
			this->ime = this->nextIme;
			this->imeChangePending = false;
		}
		this->imeCycleCount++;
	}
	//Check for an intterupt
	this->handleInterrupts();
	if(this->interruptPending)
	{
		//handle interrupt
		//reset ime flag
		this->ime = false;
		//reset IF flag for requested interrupt.
		uint8_t ifr = this->mem->read(0xFF0F); //Read IF register
		ifr = ifr & (~(this->intIdent));
		this->mem->write(0xFF0F, ifr);
		//Push PC to the stack
		uint16_t curPc = this->regFile->readRegPair(GbRegister::GbRegister::PC);
		uint16_t address = this->regFile->readRegPair(GbRegister::GbRegister::SP);
		this->mem->write(address - 1, (curPc >> 8) & 0x00FF);
		this->mem->write(address - 2, curPc & 0x00FF);
		this->regFile->decSp();
		//change PC to ISR vector address;
		this->regFile->writeRegPair(GbRegister::GbRegister::PC, this->isrAddr);
		//TODO emit 5 M-Cycles
	}
}

void InterruptController::setIME(bool nextIme)
{
	this->ime = nextIme;
}

	//Execute Class will call this when it encounters a halt/stop instruciton.
void InterruptController::processControlEvent(GbInt::GbEvent event)
{
	GbInt::GbState nextState;
	switch(this->state)
	{
		case GbInt::NORMAL : {
			if(event == GbInt::EI)
			{
				this->imeChangePending = true;
				this->nextIme = true;
			}
			if(event == GbInt::DI)
			{
				this->imeChangePending = true;
				this->nextIme = false;
			}
			break;
		}
		case GbInt::STOPPED : {

			break;
		}
		case GbInt::HALTED : {

			break;
		}
		case GbInt::INTERRUPTED : {

			break;
		}
	}
}
	//this will be called each instruction cycle to check for interrupts.
void InterruptController::handleInterrupts()
{
	uint8_t ifr = this->mem->read(0xFF0F); //Read IF register
	uint8_t ier = this->mem->read(0xFFFF); //Read IE register
	uint8_t validInt = ifr & ier;
	bool intPending = false;
	//Check Each bit
	if((validInt & 0x01) == 0x01)
	{
		//vblank requested
		this->isrAddr = 0x0040;
		intPending = true;
		this->intIdent = 0x01;
	}
	else if((validInt & 0x02) == 0x02)
	{
		//lcd requested
		this->isrAddr = 0x0048;
		intPending = true;
		this->intIdent = 0x02;
	}
	else if((validInt & 0x04) == 0x04)
	{
		//timer requested
		this->isrAddr = 0x0050;
		intPending = true;
		this->intIdent = 0x04;
	}
	else if ((validInt & 0x08) == 0x08)
	{
		//serial requested
		this->isrAddr = 0x0058;
		intPending = true;
		this->intIdent = 0x08;
	}
	else if((validInt & 0x10) == 0x10)
	{
		//joypad requested
		this->isrAddr = 0x0060;
		intPending = true;
		this->intIdent = 0x10;
	}
	if(intPending)
		this->interruptPending = true;
}


/*
<++> InterruptController::<++>()
{

}
*/
