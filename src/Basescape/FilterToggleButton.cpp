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

#include "FilterToggleButton.h"
#include "../Savegame/Base.h"
#include "../Interface/TextList.h"

namespace OpenXcom
{
inline Soldier* FilterToggleButton::_getSld(size_t index) const 
{
	return _base->getSoldiers()->at(index);
}


FilterToggleButton::FilterToggleButton(int width, int height, int x, int y) : ToggleTextButton(width, height, x, y)
{
	setText("-");
	_filter = [this](const Soldier* sol)	{ return true; }; // default filter
}

void FilterToggleButton::setFilter(FilterFunction filter)
{
	_filter = filter;
	_hasFilter = true;
}

void FilterToggleButton::andFilter(FilterFunction filter)
{
	FilterFunction previous = _filter;
	_filter = [previous, filter](const Soldier* sol)
	{
		return previous(sol) && filter(sol);
	};
	_hasFilter = true;
}

void FilterToggleButton::orFilter(FilterFunction filter)
{
	FilterFunction previous = _filter;
	_filter = [previous, filter](const Soldier* sol)
	{
		return previous(sol) || filter(sol);
	};
	_hasFilter = true;
}


void FilterToggleButton::xorFilter(FilterFunction filter)
{
	FilterFunction previous = _filter;
	_filter = [previous, filter](const Soldier* sol)
	{
		return previous(sol) != filter(sol);
	};
	_hasFilter = true;
}

void FilterToggleButton::notFilter(FilterFunction filter)
{
	_filter = [filter](const Soldier* sol)
	{
		return !filter(sol);
	};
	_hasFilter = true;
}


void FilterToggleButton::setListAndBase(TextList* list, Base* base)
{
	_list = list;
	_base = base;
	_lastOffset = noOffset;
	_arrowsPos = list->getArrowPos();
	_arrowsVisible = (_arrowsPos >= 0);
}

void FilterToggleButton::handleArrows()
{
	if (_arrowsPos < 0)
	{
		if (_list->getArrowPos() >= 0)
		{   // if arrows are visible but we don't know where they are, hide them anyway if we're filtering
			if (getPressed()) 
				_list->setArrowColumn(-1, ARROW_VERTICAL);
		}
		return; 
	}
		
	if (getPressed() && _arrowsVisible)
	{
		_list->setArrowColumn(-1, ARROW_VERTICAL);
		_arrowsVisible = false;
	}
	else if (!getPressed() && !_arrowsVisible)
	{
		_list->setArrowColumn(_arrowsPos, ARROW_VERTICAL);
		_arrowsVisible = true;
	}
}

void FilterToggleButton::mouseClick(Action* action, State* state)
{
	if (!_pressedEnforced)
	{
		ToggleTextButton::mouseClick(action, state);
		handleArrows();
	}
	else
		setPressed(true);
}

void FilterToggleButton::setPressed(bool pressed)
{
	ToggleTextButton::setPressed(pressed);
	handleArrows();
}

Soldier* FilterToggleButton::getSoldierAt(size_t index) const
{
	assert(_base && _filter);
	if (!getPressed()) // no filtering --> normal get
		return _getSld(index);
	size_t realIndex = index;
	_lastOffset = 0;
	for (size_t i = 0; i < _base->getSoldiers()->size(); ++i)
	{
		if (_filter(_getSld(i)))
			++realIndex, ++_lastOffset;
		else if (i == realIndex)
			return _getSld(i);
	}
	assert(false);
	return nullptr;
}

Soldier* FilterToggleButton::getSelectedSoldier() const
{
	return getSoldierAt(_list->getSelectedRow());
}

size_t FilterToggleButton::calculateOffset(size_t index) const
{
	if (index == -1)
		getSelectedSoldier();
	else
		getSoldierAt(index);
	return _lastOffset;
}


/// forces the button to stay pressed 
void FilterToggleButton::enforcePressed(bool force)
{
	_pressedEnforced = force;
	if (force && !getPressed())
	{
		setPressed(true);
		setColor(grey);
	}
}

} //namespace OpenXcom
