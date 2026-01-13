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

#include "../Interface/ToggleTextButton.h"
#include <functional>

namespace OpenXcom
{

class Soldier;
class TextList;
class Base;

/**
 * @brief a specialiszed version of ToggleTextButton to filter soldiers
 */

class FilterToggleButton : public ToggleTextButton
{
	static const size_t noOffset = 0;
 public:
	using FilterFunction = std::function<bool(const Soldier*)>;
 private:
	mutable size_t _lastOffset = noOffset;
	bool _hasFilter = false;
	FilterFunction _filter;

	TextList* _list;
	Base* _base;


	// internal getter to shorten the code
	inline Soldier* _getSld(size_t index) const;

 public:
	FilterToggleButton(int width, int height, int x, int y) : ToggleTextButton(width, height, x, y)
	{
		setText("-");
	}
	void setFilter(FilterFunction filter)
	{
		_filter = filter;
		_hasFilter = true;
	}
	void setListAndBase(TextList* list, Base* base);

	size_t getlastOffset() const { return _lastOffset; }

	bool ignore(const Soldier* sol) const {	return getPressed() && _hasFilter && _filter(sol); }
	Soldier* getSoldierAt(size_t index) const;
	Soldier* getSelectedSoldier() const;
};

} // OpenXcom
