#pragma once
/*
 * Copyright 2010-2016 OpenXcom Developers.
 *
 * This file is part of OpenXcom.
 *
 * OpenXcom is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenXcom is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenXcom.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "../Interface/TextButton.h"

namespace OpenXcom
{

/// Notification text displayed in the Geoscape.
/// Later version could be more dynammic (like the minimised dogfights).  For now we only have two possible notifications so we keep it simple

class NotificationText : public TextButton
{
  private:
	inline static bool _initialized = false;
	inline static int _width, _height, _x0, _y0, _deltaX, _deltaY;
  
	size_t _index;
	bool _suppressed = false; // this is not saved in the game save, only for the current session and while trigger is true 

  public:

	static void setup(int width, int height, int x0, int y0, int deltaX = 0, int deltaY = -1);
	/// Creates a new notification text with the specified size and position.
	NotificationText(size_t index);
	void display(const std::string& text);
	void hide();
	void mouseClick(Action* action, State* state) override;
	void suppress(bool yesno = true);

	/// future extensions
  private:                            // hide unImplemented methods
	bool trigger() { return false; }  // condition that will trigger the notification
	void action() {}				  // action performed when clicked
	void setTrigger() {}              // set the trigger condition
	void setAction() {}               // set the action performed when clicked
	void track() {}                // register the notification in the geoscape notification system
};

}
