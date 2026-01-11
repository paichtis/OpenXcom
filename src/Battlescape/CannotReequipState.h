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
#include "DebriefingState.h"
#include <vector>

namespace OpenXcom
{

class TextButton;
class Window;
class Text;
class TextList;
class Base;
class RuleItem;

/**
 * Screen shown when there's not enough equipment
 * to re-equip a craft after a mission, when loading a saved equipement loadout or when it lacks ammunition.
 *
 * This class is created in two steps
 *  - first via create() and is used to keep track of missing items. It deletes itself if there are no missing items
 *  - after that and only if items are missing delayedInit() is called by the state manager to actually create the interface elements
 */
class CannotReequipState : public State
{
  public:
	using RulePtr = const RuleItem*;
	/// Comparator for RuleItem pointers based on their list order.
	struct RuleComparator
	{
		bool operator()(const RulePtr& a, const RulePtr& b) const
		{
			if (a->getListOrder() == b->getListOrder())
			{ // paichits : AFAIK schould not happen but ...
				return a < b; // default pointer comparison 
			}
			return a->getListOrder() < b->getListOrder();
		}
	};
  private:
	//std::vector<ReequipStat> _missingItems;
	using MissingItemsMap = std::map<const RuleItem*, int, CannotReequipState::RuleComparator>;
	MissingItemsMap _missingItemsMap;
	Base *_base;
	bool _bigTitle;
	std::string _craftName;
	std::string _message; 

	TextButton *_btnOk, *_btnManufacture, *_btnPurchase, *_btnTransfert;
	Window *_window;
	Text *_txtTitle, *_txtItem, *_txtQuantity, *_txtCraft;
	TextList *_lstItems;
	bool _delayedInitDone = false;


	/// Creates the Cannot Reequip state. is private to force use of create() and therefore new/delete.
	CannotReequipState(Base* base, std::string craftName, std::string message, bool bigTitle = true);

	public:
	/// Creates the Cannot Reequip state.
	  static CannotReequipState* create(Base* base, std::string craftName, std::string message, bool bigTitle = true)
	{
		  return new CannotReequipState(base, craftName, message, bigTitle);
	}
	/// Checks if there are no missing items, deletes the state if so.
	bool deleteIfEmpty();

	/// Pushes the state to the state manager if there are missing items, deletes it otherwise.
	bool pushOrDeleteIfEmpty(bool popCurrentState = false);

	/// Cleans up the Cannot Reequip state.
	virtual ~CannotReequipState();
	/// Resets state.
	void init() override;

	/// Delayed initialization.
	void delayedInit();
	/// Handler for clicking the OK button.
	void btnOkClick(Action *action);
	/// Handler for clicking the Manufacture button.
	void btnManufactureClick(Action *action);
	/// Handler for clicking the Purchase/Hire button.
	void btnPurchaseClick(Action *action);

	/// Handler for clicking the Transfert button.
	void btnTransfertClick(Action* action);

	/// Handler for clicking the items list.
	void lstClick(Action* action);
	/// Gets the list of missing items.
	MissingItemsMap* getMissingItems();
	/// Adds or increases a missing item
	bool addMissingItem(const RuleItem* rule, int amount);
	// Decreases the number of missing items by the bought amount.
	bool decreaseMissingItem(const RuleItem* rule, int amount);
	// Checks if an item is missing.
	bool isMissing(const RuleItem* item) const { return _missingItemsMap.find(item) != _missingItemsMap.end(); }
	std::pair<const RuleItem*, int> getMissingItemByIndex(size_t index) const;
	auto find(const RuleItem* rule) const { return _missingItemsMap.find(rule); }
	auto end() const { return _missingItemsMap.end(); }
	bool checkAvailability(Base* base) const;
	bool checkAvailability() const;
};

}
