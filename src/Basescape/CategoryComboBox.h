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

#include "../Interface/ComboBox.h"
#include <vector>

namespace OpenXcom
{

/**
 * @brief A specialization of ComboBox that encapsulates the logic for category filters (used in sell or purchase screen for instance)	
 */

class CategoryComboBox : public ComboBox
{
	std::vector<std::string> _cats;
	size_t _vanillaSize;
	bool _initialized = false;

  public:
	CategoryComboBox(State* state, int width, int height, int x = 0, int y = 0, bool popupAboveButton = false)
		: ComboBox(state, width, height, x, y, popupAboveButton), _vanillaSize(0) {}

	inline void push_back(const std::string& cat) { _cats.push_back(cat); }
	inline bool pushIfUnique(const std::string& cat)
	{
		for (const auto& c : _cats)
		{
			if (c == cat)
				return false;
		}
		_cats.push_back(cat);
		return true;
	}
	inline void clear() noexcept
	{
		_cats.clear();
		_vanillaSize = 0;
		_initialized = false;
	}
	inline void setVanillaCategories(size_t num = -1) { _vanillaSize = (num >= 0) ? num : _cats.size(); }
	inline bool isVanillaCategory(size_t index) const { return index < _vanillaSize; }

	inline size_t getSize() const { return _cats.size(); }
	inline void setOptions()
	{
		ComboBox::setOptions(_cats, true);
		_initialized = true;
	}

	inline std::string getSelectedCategory() const { return _cats[ComboBox::getSelected()]; }
	inline bool isSelected(std::string cat) const { return getSelectedCategory() == cat; }

  private: // masked methods for encapsulation
	void setOptions(const std::vector<std::string>& options, bool translate) { ComboBox::setOptions(options, translate); }
};



} //namespace OpenXcom
