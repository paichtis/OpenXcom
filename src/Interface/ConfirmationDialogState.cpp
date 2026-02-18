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

#include "ConfirmationDialogState.h"
#include "../Engine/Game.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextButton.h"
#include "../Engine/Options.h"

namespace OpenXcom
{
/// Creates the Select Destination state.
ConfirmationDialogState::ConfirmationDialogState(const std::string& message, const std::string& interface, doAction yesAction, doAction noAction)
{
	_window = new Window(this, 256, 160, 32, 20);
	_btnYes = new TextButton(80, 20, 70, 142);
	_btnNo = new TextButton(80, 20, 170, 142);
	_txtMessage = new Text(224, 48, 48, 76);

	setInterface(interface);
	add(_window, "window", interface);
	add(_btnYes, "button", interface);
	add(_btnNo, "button", interface);
	add(_txtMessage, "text", interface);
	centerAllSurfaces();
	setWindowBackground(_window, interface);

	_txtMessage->setText(message);
	_txtMessage->setAlign(ALIGN_CENTER);
	_txtMessage->setBig();
	_txtMessage->setWordWrap(true);

	_btnNo->setText(tr("STR_NO"));
	_btnNo->onMouseClick((ActionHandler)&ConfirmationDialogState::btnNoClick);
	_btnNo->onKeyboardPress((ActionHandler)&ConfirmationDialogState::btnNoClick, Options::keyCancel);

	_btnYes->setText(tr("STR_YES"));
	_btnYes->onMouseClick((ActionHandler)&ConfirmationDialogState::btnYesClick);
	_btnYes->onKeyboardPress((ActionHandler)&ConfirmationDialogState::btnYesClick, Options::keyOk);
	
	_onYesAction = yesAction;
	_onNoAction = noAction;
	_actionsSet = true;
}

/// Handler for clicking the Cancel button.
void ConfirmationDialogState::btnNoClick(Action* action)
{
	_game->popState(); // pop this state
	if (_actionsSet)
	{
		if (_onNoAction(nullptr))
		{
			_game->popState(); // pop parent state
		}
	}
}
/// Handler for clicking the Cydonia mission button.
void ConfirmationDialogState::btnYesClick(Action* action)
{
	_game->popState();
	if (_actionsSet)
	{
		if (_onYesAction(nullptr))
		{
			_game->popState();
		}
	}
}

} // namespace OpenXcom
