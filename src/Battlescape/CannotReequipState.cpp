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
CannotReequipState::CannotReequipState(std::vector<ReequipStat>& missingItems, Base* base, bool isRearm) : _base(base)
{
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
	_lstItems = new TextList(288, 112, 8, 58);

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

	if (isRearm)
	{
		std::string msg = tr("STR_NOT_ENOUGH_ITEM_TO_REARM_CRAFT_AT_BASE")
							  .arg(tr(missingItems[0].item))
							  .arg(missingItems[0].craft)
							  .arg(base->getName());
		_txtTitle->setText(msg);
	}
	else
	{
		_txtTitle->setText(tr("STR_NOT_ENOUGH_EQUIPMENT_TO_FULLY_RE_EQUIP_SQUAD"));
		_txtTitle->setBig();
	}
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

	for (auto& i : missingItems)
	{
		if (i.qty > 0)
		{
			auto* rule = _game->getMod()->getItem(i.item);
			if (rule)
			{
				_missingItemsMap[rule] = i.qty;
			}
		}
	}

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
	State::init();

	_lstItems->clearList();

	for (const auto& pair : _missingItemsMap)
	{
		if (pair.second > 0)
		{
			std::ostringstream ss;
			ss << pair.second;
			_lstItems->addRow(3, tr(pair.first->getType()).c_str(), ss.str().c_str(), "");
		}
	}

	_btnTransfert->setVisible(checkAvailability());
	_btnManufacture->setVisible(!_missingItemsMap.empty());
	_btnPurchase->setVisible(!_missingItemsMap.empty());
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
std::map<RuleItem*, int> *CannotReequipState::getMissingItems()
{
	if (_missingItemsMap.empty())
		return nullptr;
	return &_missingItemsMap;
}

/**
 * Decreases the number of missing items by the bought amount.
 * @param rule Type of item.
 * @param amount Number of items bought.
 * @return false if there are *now* no more missing items of this type. (false means a change of state)
 */
bool CannotReequipState::decreaseMissingItemCount(const RuleItem* rule, int amount)
{
	auto it = _missingItemsMap.find(const_cast<RuleItem*>(rule));
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

std::pair<RuleItem*, int> CannotReequipState::getMissingItemByIndex(size_t index) const 
{
	if (index >= _missingItemsMap.size())
	{
		return std::pair<RuleItem*, int>(nullptr, 0);
	}
	auto it = _missingItemsMap.begin();
	std::advance(it, index);
	return std::pair<RuleItem*, int>(it->first, it->second);
}

} // namespace OpenXcom
