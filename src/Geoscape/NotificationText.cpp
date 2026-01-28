
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
#include "NotificationText.h"
//#include "../Interface/ConfirmationDialogState.h"
#include "../Engine/State.h"
#include "../Engine/Game.h"	
#include "../Engine/Action.h"

namespace OpenXcom
{
	void NotificationText::setup(int width, int height, int x0, int y0, int deltaX, int deltaY)
	{
		if (_initialized)
			return;
		_width = width;
		_height = height;
		_x0 = x0;
		_y0 = y0;
		_deltaX = deltaX;
		_deltaY = deltaY > 0 ? deltaY : (height + 1);
		_initialized = true;
	}

	NotificationText::NotificationText(size_t index)
		: TextButton(_width, _height, _x0 + _deltaX*index, _y0 + (_deltaY*index), false), _index(index)
	{
		_visible = false;
		track(); // does nothing for now will register the notification in the geoscape notification system
	}

	void NotificationText::display(const std::string& text)
	{
		if (_suppressed)
			return;
		setText(text);
		setVisible(true);
	}

	void NotificationText::hide()
	{
		setVisible(false);
		_suppressed = false;
	}

	void NotificationText::suppress(bool yesno)
	{
		_suppressed = yesno;
		_visible = false;
	}

	void NotificationText::mouseClick(Action* action, State* state)
	{
		//if (action->getDetails()->button.button == SDL_BUTTON_RIGHT)
		if (State::getGame()->isRightClick(action, true))
		{
			// TODO : show confirmation dialog before suppressing 
			suppress(true); // TODO : remove this once the confirmation dialog is implemented
			return;
		}
		TextButton::mouseClick(action, state);
	}


	} //OpenXcom
