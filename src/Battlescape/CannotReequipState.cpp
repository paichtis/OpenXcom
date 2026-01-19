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
#include "CannotReequipState.h"
#include <sstream>
#include "../Engine/Game.h"
#include "../Mod/Mod.h"
#include "../Engine/LocalizedText.h"
#include "../Interface/TextButton.h"
#include "../Interface/ComboBox.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Engine/Options.h"
#include "../Savegame/Base.h"
#include "../Savegame/ItemContainer.h"
#include "../Basescape/ManufactureState.h"
#include "../Basescape/PurchaseState.h"
#include "../Basescape/ItemLocationsState.h"
#include "../Basescape/TransferBaseState.h"

namespace OpenXcom
{

/**
 * Checks if any missing items are available in the specified base.
 * @param base Relevant xcom base.
 * @return True if at least one missing item is available in the base.
 */

bool CannotReequipState::checkAvailability(Base* base) const
{
	if (_missingItemsMap.empty())
	{
		return false;
	}
	for (const auto& pair : _missingItemsMap)
	{
		const OpenXcom::ItemContainer* items = base->getStorageItems();
		int available = items ? items->getItem(pair.first) : 0;
		if (available > 0)
		{
			return true;
		}
	}
	return false;
}

/**
 * Checks if any missing items are available in other bases.
 * @return True if at least one missing item is available in another base.
 */
bool CannotReequipState::checkAvailability() const
{
	if (_missingItemsMap.empty())
	{
		return false;
	}
	for (auto *xbase : *_game->getSavedGame()->getBases())
	{
		if (xbase == _base)
		{
			continue;
		}
		if (checkAvailability(xbase))
		{
			return true;
		}
	}
	return false;
}

/**
 * Initializes all the elements in the Cannot Reequip screen.
 * @param missingItems List of items still needed for reequip.
 * @param base Relevant xcom base.
 */

CannotReequipState::CannotReequipState(Base* base, std::string craftName, std::string message, bool bigTitle) : _base(base), _craftName(craftName),
	 _delayedInitDone(false), _btnOk(nullptr), _btnManufacture(nullptr), _btnPurchase(nullptr), _btnTransfert(nullptr),
	_window(nullptr), _txtTitle(nullptr), _txtItem(nullptr), _txtQuantity(nullptr), _txtCraft(nullptr), _lstItems(nullptr),
	_missingItemsMap(), _message(message), _bigTitle(bigTitle)
{
	_missingItemsMap.clear(); // extra safety...
}

void CannotReequipState::delayedInit()
{
	if (_delayedInitDone)
		return;
	_delayedInitDone = true;

		// Create objects
	_window = new Window(this, 320, 200, 0, 0);
	_btnManufacture = new TextButton(128, 14, 10, 178);
	_btnPurchase = new TextButton(128, 14, 144, 178);
	_btnTransfert = new TextButton(128, 14, 10, 162);
	_btnOk = new TextButton(34, 14, 278, 178);
	_txtTitle = new Text(220, 32, 50, 8);
	_txtItem = new Text(142, 9, 10, 50);
	_txtQuantity = new Text(88, 9, 152, 50);
	_txtCraft = new Text(74, 9, 218, 50);
	_lstItems = new TextList(288, 96, 8, 58);

	// Set palette
	const std::string& category = "cannotReequip";
	setInterface(category);

	add(_window, "window", category);
	add(_btnManufacture, "button", category);
	add(_btnPurchase, "button", category);
	add(_btnTransfert, "button", category);
	add(_btnOk, "button", category);
	add(_txtTitle, "heading", category);
	add(_txtItem, "text", category);
	add(_txtQuantity, "text", category);
	add(_txtCraft, "text", category);
	add(_lstItems, "list", category);

	centerAllSurfaces();

	// Set up objects
	setWindowBackground(_window, category);

	_btnManufacture->setText(tr("STR_MANUFACTURE"));
	_btnManufacture->onMouseClick((ActionHandler)&CannotReequipState::btnManufactureClick);

	_btnPurchase->setText(tr("STR_PURCHASE_RECRUIT"));
	_btnPurchase->onMouseClick((ActionHandler)&CannotReequipState::btnPurchaseClick);

	_btnTransfert->setText(tr("STR_TRANSFERT_IN"));
	_btnTransfert->onMouseClick((ActionHandler)&CannotReequipState::btnTransfertClick);

	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&CannotReequipState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&CannotReequipState::btnOkClick, Options::keyOk);
	_btnOk->onKeyboardPress((ActionHandler)&CannotReequipState::btnOkClick, Options::keyCancel);

	_txtTitle->setText(_message);
	if (_bigTitle)
		_txtTitle->setBig();

	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setWordWrap(true);

	_txtItem->setText(tr("STR_ITEM"));

	_txtQuantity->setText(tr("STR_QUANTITY_UC"));

	_txtCraft->setText(tr("STR_CRAFT"));

	_lstItems->setColumns(3, 162, 46, 80);
	_lstItems->setSelectable(true);
	_lstItems->setBackground(_window);
	_lstItems->setMargin(2);
	_lstItems->onMouseClick((ActionHandler)&CannotReequipState::lstClick);
}


/**
 *
 */
CannotReequipState::~CannotReequipState()
{
}

/**
 * Resets stuff when coming back from other screens.
 */
void CannotReequipState::init()
{
	delayedInit();
	State::init();

	_lstItems->clearList();

	for (const auto& pair : _missingItemsMap)
	{
		if (pair.second > 0)
		{
			std::ostringstream ss;
			ss << pair.second;
			_lstItems->addRow(3, tr(pair.first->getType()).c_str(), ss.str().c_str(), _craftName);
		}
		else
			_missingItemsMap.erase(pair.first); // remove items with no missing qty
	}

	_btnTransfert->setVisible(checkAvailability());
	_btnManufacture->setVisible(!_missingItemsMap.empty());
	_btnPurchase->setVisible(!_missingItemsMap.empty());
}

/**
 * Checks if there are no missing items, deletes the state if so.
 * @return true if the state was empty thus deleted.
 */

bool CannotReequipState::deleteIfEmpty()
{
	if (_missingItemsMap.empty())
	{
		delete this;
		return true;
	}
	return false;
}

/**
 * @brief Pushes the state to the state manager if there are missing items, deletes it otherwise.
 * @param popCurrentState shall we pop the current state before pushing the new one ?
 * @return true if the state was pushed, false if deleted. 
 */

bool CannotReequipState::pushOrDeleteIfEmpty(bool popCurrentState)
{
	if (deleteIfEmpty())
	{
		return false;
	}
	if (popCurrentState)
	{
		_game->popState();
	}
	_game->pushState(this);
	return true;
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void CannotReequipState::btnOkClick(Action *)
{
	_game->popState();
}

/**
 * Goes to the Manufacture screen.
 * @param action Pointer to an action.
 */
void CannotReequipState::btnManufactureClick(Action *)
{
	_game->pushState(new ManufactureState(_base, this));
}

/**
 * Goes to the Purchase screen.
 * @param action Pointer to an action.
 */
void CannotReequipState::btnPurchaseClick(Action *)
{
	_game->pushState(new PurchaseState(_base, this));
}

void CannotReequipState::lstClick(Action* action)
{
	int row = _lstItems->getSelectedRow();
	auto it = _missingItemsMap.begin();
	std::advance(it, row);
	_game->pushState(new ItemLocationsState(it->first));
}

void CannotReequipState::btnTransfertClick(Action* action)
{
	_game->pushState(new TransferBaseState(_base, nullptr, this));
}

/**
 * Gets the list of missing items.
 */
CannotReequipState::MissingItemsMap* CannotReequipState::getMissingItems()
{
	if (_missingItemsMap.empty())
		return nullptr;
	return &_missingItemsMap;
}

/**
* Adds or increases a missing item
 * @param rule Type of item.
 * @param amount Number of items missing.
 * @return true if the item was added/increased.
 */

bool CannotReequipState::addMissingItem(const RuleItem* rule, int amount)
{
	if (!rule || (amount <= 0)) // do not allow null rules
		return false;

	auto it = _missingItemsMap.find(rule);
	if (it == _missingItemsMap.end())
	{
		_missingItemsMap[rule] = amount;
	}
	else
		it->second += amount;
	return true;
}
/**
 *  calculates how many items (if any) are missing and adds them to the map if needed
 * @param rule Type of item.
 * @param amount Number of items needed.
 * @return true if the item was added to the map
*/

bool CannotReequipState::calculateMissingItem(const RuleItem* rule, int needed)
{
	return addMissingItem(rule, needed - _base->getStorageItems()->getItem(rule));
}

/**
 * Decreases the number of missing items by the bought amount.
 * @param rule Type of item.
 * @param amount Number of items bought.
 * @return false if there are *now* no more missing items of this type. (false means a change of state)
 */
bool CannotReequipState::decreaseMissingItem(const RuleItem* rule, int amount)
{
	auto it = _missingItemsMap.find(rule);
	if (it != _missingItemsMap.end())
	{
		it->second = std::max(0, it->second - amount);
		if (it->second <= 0)
		{
			_missingItemsMap.erase(it);
			return false;
		}
	}
	return true;
}

std::pair<const RuleItem*, int> CannotReequipState::getMissingItemByIndex(size_t index) const 
{
	if (index >= _missingItemsMap.size())
	{
		return std::pair<RuleItem*, int>(nullptr, 0);
	}
	auto it = _missingItemsMap.begin();
	std::advance(it, index);
	return std::pair<const RuleItem*, int>(it->first, it->second);
}

} // namespace OpenXcom
