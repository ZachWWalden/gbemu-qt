/*==================================================================================
 *Class - InterruptController
 *Author - Zach Walden
 *Created - 2/19/24
 *Last Changed - 2/19/24
 *Description - Interrupt Controller, and State Controller
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

#include "../../CycleListener/CycleListener.hpp"
#include "../Execute/RegisterFile/RegisterFile.hpp"
#include "../MMU/MMU.hpp"

namespace GbInt
{
	enum GbState
	{
		INTERRUPTED, STOPPED, HALTED, NORMAL
	};
	enum GbEvent
	{
		STOP, HALT, EI, DI, INTERRUPT, INSTCYCLE
	};
};

class InterruptController
{
	//Attributes
public:

private:
	RegisterFile* regFile;
	CycleListener* listener;

	bool ime = false;
	bool nextIme = false;
	bool imeChangePending = false;
	uint8_t imeCycleCount = 0;

	uint16_t isrAddr = 0x0000;
	bool interruptPending = false;
	uint8_t intIdent = 0x00;

	MMU* mem;
	GbInt::GbState state = GbInt::GbState::NORMAL;
	//Methods
public:
	InterruptController(MMU* newMem, RegisterFile* regs, CycleListener* nListener);
	~InterruptController();

	void getNextPC();
	//Execute Class will call this when it encounters a halt/stop instruciton.
	void processControlEvent(GbInt::GbEvent event);
	void setIME(bool nextIME);
private:
	//this will be called each instruction cycle to check for interrupts.
	void handleInterrupts();
};
