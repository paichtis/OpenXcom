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
class Action;

/**
 * @brief a specialiszed version of ToggleTextButton to filter soldiers
 */

class FilterToggleButton : public ToggleTextButton
{
	static const size_t noOffset = 0;
	static const Uint8 grey = 8;
 public:
	using FilterFunction = std::function<bool(const Soldier*)>;
 private:
	mutable size_t _lastOffset = noOffset;
	bool _hasFilter = false;
	FilterFunction _filter;

	TextList* _list = nullptr;
	Base* _base = nullptr;
	bool _pressedEnforced = false;


	int _arrowsPos = -1;
	bool _arrowsVisible = true;


	// internal getter to shorten the code
	inline Soldier* _getSld(size_t index) const;
	void handleArrows();

 public:
	FilterToggleButton(int width, int height, int x, int y);

	// set, and, or, xor and not filter functions
	void setFilter(FilterFunction filter);
	void andFilter(FilterFunction filter);
	void orFilter(FilterFunction filter);
	void xorFilter(FilterFunction filter);
	void notFilter(FilterFunction filter);

	// link to the list and base to filter
	void setListAndBase(TextList* list, Base* base);
	// override mouseClick to update the list
	void mouseClick(Action* action, State* state) override;

	// check if a soldier should be ignored
	bool ignore(const Soldier* sol) const {	return getPressed() && _hasFilter && _filter(sol); }

	// Soldier accessors
	Soldier* getSoldierAt(size_t index) const;
	Soldier* getSelectedSoldier() const;

	// offset calculation and caching
	size_t getlastOffset() const { return _lastOffset; }
	size_t calculateOffset(size_t index = -1) const;

	// press/unpress button making sure to update arrows
	void setPressed(bool pressed);
	void enforcePressed(bool force = true);
};

} // OpenXcom
