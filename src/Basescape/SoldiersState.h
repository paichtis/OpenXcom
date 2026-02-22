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
#include <vector>
#include "SoldierSortUtil.h"
#include "BaseSwitcher.h"

namespace OpenXcom
{

class TextButton;
class Window;
class Text;
class TextList;
class ComboBox;
class CategoryComboBox;
class Base;
class Soldier;
struct SortFunctor;

/**
 * Soldiers screen that lets the player
 * manage all the soldiers in a base.
 */
class SoldiersState : public State, public BaseSwitcher
{
private:
	TextButton *_btnOk, *_btnPsiTraining, *_btnTraining, *_btnMemorial;
	Window *_window;
	Text *_txtTitle, *_txtName, *_txtRank, *_txtCraft;
	ComboBox* _cbxSortBy;
	CategoryComboBox	*_cbxScreenActions;
	TextList *_lstSoldiers;
	Base *_base;
	std::vector<Soldier *> _origSoldierOrder, _filteredListOfSoldiers;
	std::vector<int> _filteredIndicesOfSoldiers;
	std::vector<SortFunctor *> _sortFunctors;
	getStatFn_t _dynGetter;
//	std::vector<std::string> _availableOptions;
	size_t _mainOffset;
	///initializes the display list based on the craft soldier's list and the position to display
	void initList(size_t scrl);

	/// -- start of base switching related members and methods --

	inline static size_t _selectedSort = -1; // set before moving to another base, used to restore selection after move
	inline static std::string _selectedAction = "";
	bool _inited = false;
	inline static bool _lastSortShiftPressed = false;
	bool BaseSwitcherReverse() const;

	void doBeforeBaseChange() override; 
	void doAfterBaseChange() override;
	void doPush(Base* base) override { _game->pushState(new SoldiersState(base));  } // nullptr because this combo has to be recalculated for each base

    bool ignoreBase(Base* base) const override;


	BASE_SWITCHER_HANDLERS(SoldiersState);

	/// -- end of base switching related members and methods --

	int getSelectedSoldierId() const;

public:
	/// Creates the Soldiers state.
	SoldiersState(Base *base);
	/// Cleans up the Soldiers state.
	~SoldiersState();
	/// Handler for changing the sort by combobox.
	void cbxSortByChange(Action *action);
	/// Updates the soldier names.
	void init() override;
	/// Handler for clicking the Soldiers reordering button.
	void lstItemsLeftArrowClick(Action *action);
	/// Moves a soldier up.
	void moveSoldierUp(Action *action, unsigned int row, bool max = false);
	/// Handler for clicking the Soldiers reordering button.
	void lstItemsRightArrowClick(Action *action);
	/// Moves a soldier down.
	void moveSoldierDown(Action *action, unsigned int row, bool max = false);
	/// Handler for clicking the OK button.
	void btnOkClick(Action *action);
	/// Handler for clicking the Psi Training button.
	void btnPsiTrainingClick(Action *action);
	void btnTrainingClick(Action *action);
	/// Handler for clicking the Memorial button.
	void btnMemorialClick(Action *action);
	/// Handler for clicking the Transformations Overview button/hotkey.
	void btnTransformationsOverviewClick(Action *action);
	/// Handler for changing the screen actions combo box.
	void cbxScreenActionsChange(Action *action);
	/// Handler for clicking the Inventory button.
	void btnInventoryClick(Action *action);
	/// Handler for clicking the Soldiers list.
	void lstSoldiersClick(Action *action);
	/// Handler for pressing-down a mouse-button in the list.
	void lstSoldiersMousePress(Action *action);
};

}
