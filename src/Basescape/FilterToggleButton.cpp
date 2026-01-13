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
void FilterToggleButton::setListAndBase(TextList* list, Base* base)
{
	_list = list;
	_base = base;
	_lastOffset = noOffset;
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

} //namespace OpenXcom
