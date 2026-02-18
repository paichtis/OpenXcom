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

#include "../Engine/State.h"
#include <functional>

namespace OpenXcom
{

class TextButton;
class Text;

/**
 * @brief a class displaying a confirmation dialog. Can be derived for more specialized behavior
 */

class ConfirmationDialogState : public State
{
  public:
	using doAction = std::function<bool(void*)>;
 protected:
	Window* _window;
	Text* _txtMessage;
	TextButton *_btnNo, *_btnYes;

	doAction _onYesAction, _onNoAction;
	bool _actionsSet = false;

	static bool defaultNo(void*)	{ return true;  }
	static bool defaultYes(void*)	{ return true; }

 public:
	/// Creates the Select Destination state.
   ConfirmationDialogState(const std::string& message, const std::string& interface, doAction yesAction = defaultYes, doAction noAction = defaultNo);
	/// Cleans up the Select Destination state.
   ~ConfirmationDialogState() = default;
	/// Handler for clicking the Cancel button.
	void btnNoClick(Action* action);
	/// Handler for clicking the Cydonia mission button.
	void btnYesClick(Action* action);
};

} // namespace OpenXcom

