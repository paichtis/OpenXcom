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

#include "BaseSwitcher.h"
#include "../Engine/Action.h"
#include "../Engine/Game.h"
#include "../Savegame/Base.h"
#include "../Savegame/SavedGame.h"
#include "../Interface/TextButton.h"
#include "../Interface/Text.h"
#include "../Mod/RuleInterface.h"
#include "../Engine/Logger.h"
//#include "../Menu/ErrorMessageState.h"

#include <vector>

namespace OpenXcom {

size_t BaseSwitcher::getBaseIndex(Base* base) const
{
	const std::vector<Base*>* bases = State::_game->getSavedGame()->getBases();
	for (size_t i = 0; i < bases->size(); ++i)
	{
		if ((*bases)[i] == base)
			return i;
	}
	return 0;
}

/// Gets the (current/next/previous) base index depending on 'direction'.
size_t BaseSwitcher::nextBaseIndex(int direction) const
{
	if (_validBasesCount <= 1)
	{
		return 0;
	}

	const std::vector<Base*>* bases = State::_game->getSavedGame()->getBases();

	size_t currentIndex = _baseIndex;
	do
	{
		currentIndex = (currentIndex + bases->size() + direction) % bases->size();
	} while (ignoreBase((*bases)[currentIndex]));

	return currentIndex;
}

/// Gets the (current/next/previous) base name depending on 'direction'.
std::string BaseSwitcher::getBaseName(int direction) const
{
	if (!direction)
	{
		return _bsbase->getName(); // current base, no need to search
	}
	else if (direction == 1 || direction == -1)
	{
		return (*State::_game->getSavedGame()->getBases())[nextBaseIndex(direction)]->getName();
	}
	else
	{
		return "DEBUG : INVALID direction (schould not happen)";
	}
}

size_t BaseSwitcher::calculateValidBaseCount() const
{
	size_t count = 0;
	const std::vector<Base*>* bases = State::_game->getSavedGame()->getBases();
	for (auto base : *bases)
	{
		if (!ignoreBase(base))
		{
			++count;
		}
	}
	return count;
}

size_t BaseSwitcher::getValidBasesCount() const
{
	return State::_game->getSavedGame()->getBases()->size();
}

void BaseSwitcher::toggleNavigationButtons(bool show)
{
	if (_nextButton)
		_nextButton->setVisible(show);
	if (_nextText)
		_nextText->setVisible(show);
	if (_prevButton)
		_prevButton->setVisible(show && getValidBasesCount() > 1);
	if (_prevText)
		_prevText->setVisible(show && getValidBasesCount() > 1);
}


/// Adds navigation buttons if there are multiple bases.
void BaseSwitcher::addNavigationButtons(State* parent, InteractiveSurface *surface)
{
	_moved = false;
	_validBasesCount = calculateValidBaseCount(); 
	if (_validBasesCount <= 1)
		return;
	
	_nextButton = new TextButton(10, 8, 310, 0);
	_nextText = new Text(100, 8, 210, 0);
	parent->add(_nextButton);
	parent->add(_nextText);
	 
	_nextButton->setText("->");
	_nextButton->setAlign(ALIGN_RIGHT);
	_nextText->setSmall();
	_nextText->setAlign(ALIGN_RIGHT);
	_nextText->setText(getBaseName(1));
	Uint8 ammoColor = State::_game->getMod()->getInterface("sellMenu")->getElement("ammoColor")->color;
	_nextText->setColor(ammoColor); // using ammo color for lack of better
	_nextButton->onMouseClick(getNextButtonHandler());
	surface->onKeyboardPress(getNextButtonHandler(), SDLK_PAGEDOWN);
	
	if (_validBasesCount <= 2) // 2 bases only --> no need for previous button
		return;

	_prevText = new Text(100, 8, 10, 0);
	_prevButton = new TextButton(10, 8, 0, 0);
	parent->add(_prevButton);
	parent->add(_prevText);

	_prevButton->setText("<-");
	_prevButton->setAlign(ALIGN_LEFT);
	_prevText->setSmall();
	_prevText->setAlign(ALIGN_LEFT);
	_prevText->setText(getBaseName(-1));
	_prevText->setColor(ammoColor); // using ammo color for lack of better

	_prevButton->onMouseClick(getPrevButtonHandler());
	surface->onKeyboardPress(getPrevButtonHandler(), SDLK_PAGEUP);
	doAfterBaseChange();
}

/// Creates a new BaseSwitcher linked to a parent state and a base.
BaseSwitcher::BaseSwitcher(Base* base)
//	: InteractiveSurface(320, 8), _base(base), _parent(parent)
{
	_bsbase = base;
	_baseIndex = getBaseIndex(base);
}


/*
 * Cleans up the base switcher.
 */
BaseSwitcher::~BaseSwitcher()
{
	#if 0
	if (_nextButton)
		delete _nextButton;
	if (_nextText)
		delete _nextText;
	if (_prevButton)
		delete _prevButton;
	if (_prevText)
		delete _prevText;
	#endif
}

void BaseSwitcher::doBeforeBaseChange()
{
	State::_game->popState(); // close current state before switching : minimal action needed when overloaded
	return; 
}

void BaseSwitcher::doAfterBaseChange()
{
	return; // nothing but herited classes override as needed
}

/// Moves to the next/previous base.
void BaseSwitcher::nextBase(int direction)
{
	if ( _validBasesCount <= 1 || _moved)
		return;

	// first let's prevent multiple clicks
	//hideNavigationButtons();
	_moved = true;

	doBeforeBaseChange();
	doPush((*State::_game->getSavedGame()->getBases())[nextBaseIndex(direction)]);
}

} // namespace OpenXcom
