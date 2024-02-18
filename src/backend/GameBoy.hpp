/*==================================================================================
 *Class - GameBoy
 *Author - Zach Walden
 *Created - 7/22/22
 *Last Changed - 10/11/23
 *Description - Top Level GameBoy Module/Class
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

#include <string>


//TODO use a callback based system for dumping frames out of the emulator. this will allow full decoupling between backend and frontend allowing for a desktop interface or whatever really.
class GameBoy
{
	//Attributes
public:

private:
	//callback pointer
	void* frameDumpCallback;
	//Methods
public:
	GameBoy();
	~GameBoy();

	void setCallback(void* frameDumpCallback);

	//emulator goodies. will be useful for debugging as well.
	void saveState(std::string saveStateNamePath);
	void loadState(std::string saveStateNamePath);

	//resets the core and restarts with boot process.
	void loadRom(std::string romNamePath);

	void run();

private:
	void reboot();
};
