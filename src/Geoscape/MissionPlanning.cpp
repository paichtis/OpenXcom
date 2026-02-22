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

#include "MissionPlanning.h"
#include "../Engine/Game.h"
#include "../Mod/Mod.h"
#include "../Mod/AlienDeployment.h"
#include "../Mod/RuleStartingCondition.h"
#include "../Mod/RuleSoldier.h"
#include "../Savegame/Craft.h"
#include "../Engine/Logger.h"
#include "../Interface/ConfirmationDialogState.h"
#include "../Basescape/CraftEquipmentState.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/ItemContainer.h"
#include "../Savegame/Base.h"
#include "../Battlescape/CannotReequipState.h"
#include "../Basescape/CraftInfoState.h"
#include "../Basescape/CraftSoldiersState.h"
#include "../Savegame/Soldier.h"
#include "ConfirmDestinationState.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextButton.h"

namespace OpenXcom
{

MissionPlanning::MissionPlanning(AlienDeployment* deploymentRule, Target* target)
	: _deploymentRule(deploymentRule), _target(target), _craft(nullptr)
{
	_startingCondition = _game->getMod()->getStartingCondition(_deploymentRule->getStartingCondition());

	//_window = new Window(this, 220, 160, 50, 20, POPUP_BOTH);
	//_titleTxt = new Text(0, 0);
	//
	//_BtnSelectEquipment = new TextButton( 10, 0, 200, 30);
	//_BtnSelectCraft = new TextButton(10, 20, 200, 30);
	//_BtnSaveLoadOut = new TextButton(10, 50, 200, 30);
	//_BtnRestoreLoadOut = new TextButton(10, 80, 200, 30);
	//_BtnSelectSoldiers = new TextButton(10, 110, 200, 30);
	//_BtnSaveSoldiers = new TextButton(10, 140, 200, 30);
	//_BtnSelectSoldierArmors = new TextButton(10, 170, 200, 30);
	//_BtnSaveSoldierArmors = new TextButton(10, 200, 200, 30);
	//_BtnSaveSoldierEquipement = new TextButton(10, 230, 200, 30);
	//_BtnSelectEquipment = new TextButton(10, 260, 200, 30);

	//_BtnLaunch = new TextButton(10, 290, 200, 30);
	//_BtnAbort = new TextButton(10, 320, 200, 30);

	//setInterface("targetInfo");

	//add(_window, "window", "targetInfo");
	//add(_titleTxt);
	//add(_BtnSelectEquipment);
	//add(_BtnSelectCraft);
	//add(_BtnSaveLoadOut);
	//add(_BtnRestoreLoadOut);
	//add(_BtnSelectSoldiers);
	//add(_BtnSaveSoldiers);
	//add(_BtnSelectSoldierArmors);
	//add(_BtnSaveSoldierArmors);
	//add(_BtnSaveSoldierEquipement);
	//add(_BtnSelectEquipment);
	//add(_BtnLaunch);
	//add(_BtnAbort);

}


bool MissionPlanning::checkFuel(Craft* craft, bool strict)
{
	bool enoughFuel = (2 * craft->getFuelLimit(_target)) <= (strict ? craft->getFuel() : craft->getFuelMax());
	return enoughFuel;
}


MissionPlanning* MissionPlanning::create(AlienDeployment* deploymentRule, Target* target)
{
	return _game->getSavedGame()->planMission(new MissionPlanning(deploymentRule, target));
}

bool MissionPlanning::isCraftEligible(Craft* craft, bool strict)
{
	if (_target == nullptr)
		return true;
	bool enoughFuel = checkFuel(craft, strict);

	if (_deploymentRule == nullptr || _startingCondition == nullptr)
		return enoughFuel;

	bool available = craft->getStatus() == "STR_READY" || ((Options::craftLaunchAlways) && craft->getStatus() != "STR_OUT" &&
														   !craft->getLowFuel() &&
														   !craft->getMissionComplete()); // we exclude STR_OUT crafts on purpose from this 'prepare mission' flow

	// now verify craft has enough room to hold required items
	int requiredItemsCount = 0;
	for (const auto& itemReq : _startingCondition->getRequiredItems())
	{
		requiredItemsCount += itemReq.second;
	}

	if ((craft->getMaxItemsClamped() < requiredItemsCount) && _startingCondition->isCraftPermitted(craft->getType()))
	{
		Log(LOG_DEBUG) << "Craft " << craft->getDefaultName(_game->getLanguage()) << " cannot go on mission because it only has room for "
					   << craft->getMaxItemsClamped() << " items, but the mission requires " << requiredItemsCount << " items.";
	}

	available &= craft->getMaxItemsClamped() >= requiredItemsCount;

	return available && _startingCondition->isCraftPermitted(craft->getType()) && enoughFuel;
}

void MissionPlanning::confirmAbort(std::string interface)
{
	ConfirmationDialogState* confirm = new ConfirmationDialogState(State::tr("STR_CONFIRM_ABORT_MISSION_PLANNING"), interface,
		[this, interface](void*)
		{  // yes --> cancel the mission and close current state
			if (_savedLoadout || _craft->hasAssignedSoldiersBackup())
			{
				askRestorePreMissionState(interface);
				return false;
			}
			else
			{
				_game->getSavedGame()->releaseMissionPlanning();
				return true;
			}
		},
		[&](void*)
		{	// no --> don't cancel mission and don't close state
			return false;
		}
	);
	_game->pushState(confirm);
}

bool MissionPlanning::tryEquipRequiredItems(bool rClic)
{
	auto& requiredItems = _startingCondition->getRequiredItems();
	if (!_craft->areRequiredItemsOnboard(requiredItems))
	{
		Base* base = _craft->getBase();
		auto* crState = CannotReequipState::create(base, _craft->getName(_game->getLanguage()), tr("STR_STARTING_CONDITION_ITEM"), false);
		for (auto item : requiredItems)
		{
			crState->calculateMissingItem(item.first, item.second - _craft->getItems()->getItem(item.first));
		}
		if (crState->pushOrDeleteIfEmpty(false))
			return false; // some items are missing, cannot proceed

		saveEquipementLoadout();	// save before adding new items

		ConfirmationDialogState* confirmEquip =
			new ConfirmationDialogState(tr("STR_EQUIP_CRAFT_FOR_MISSION_CONFIRMATION").arg(_craft->getName(_game->getLanguage())), "confirmationdialog",
			[=](void*) // fill the craft if accepted by the user
			{
				for (auto item : requiredItems)
				{
					int toEquip = item.second - _craft->getItems()->getItem(item.first);
					if (toEquip > 0)
					{
						base->getStorageItems()->removeItem(item.first, toEquip);
						RuleItem* ruleItem = _game->getMod()->getItem(item.first);
						_craft->getItems()->addItem(ruleItem, toEquip);
					}
				}
				goToCraft(rClic);
				return false;
			},
			[=](void*)
			{
				goToCraft(rClic);
				return false;
			});

		_game->replaceState(confirmEquip);
		return false; // we will manage the transition here
	}
	goToCraft(rClic);
	return true;
}
/*
*	 Saves current craft's loadout
* @return the id of the save, -1 if no save performed
**/

int MissionPlanning::saveEquipementLoadout()
{
	if (_craft == nullptr)
		return false;

	_loadOutId  = SavedGame::MAX_CRAFT_LOADOUT_TEMPLATES + _id; // TODO get and register a sensible id

	// let's save the equipement at hidden slot with our id
	_game->getSavedGame()->getGlobalCraftLoadout(_loadOutId)->copy(_craft->getItems());
	_savedLoadout = true;
	// TODO : tell the user we saved the equipement : STR_SAVED_PREMISSION_LOADOUT
	return _loadOutId;
}

bool MissionPlanning::askRestorePreMissionState(const std::string& interface, bool popme)
{
	if (popme)
		_game->popState();
	ConfirmationDialogState* confirm = new ConfirmationDialogState(State::tr("STR_RESTORE_PREMISSION_STATE"), interface,
																   [this](void*) {
																		if (_craft->hasAssignedSoldiersBackup())
																		   _craft->restoreAssignedSoldiersFromBackup();
																		if (_savedLoadout)
																			State::getGame()->pushState(new CraftEquipmentState(_craft, true) );
																		return true; // TODO : check if correct here !
																   },
																   [=](void*) { // no --> just release the mission planner : we finished the flow !
																	   _game->getSavedGame()->releaseMissionPlanning();
																	   return true;
																   });
	_game->pushState(confirm);
	return false;
}


void MissionPlanning::selectCraft(Craft* craft, bool rClic)
{
	_craft = craft;
	if (!tryEquipRequiredItems(rClic))
	{
		_craft = nullptr;
	}
}

void MissionPlanning::goToCraft(bool rClic)
{
	for (auto* xbase : *_game->getSavedGame()->getBases())
	{
		for (size_t ci = 0; ci < xbase->getCrafts()->size(); ++ci)
		{
			if (_craft == xbase->getCrafts()->at(ci))
			{
				if (rClic)
					_game->replaceState(new CraftInfoState(xbase, ci, this));
				else
				{
					_craft->backupAssignedSoldiers();
					_game->replaceState(new CraftSoldiersState(xbase, ci, this));
				}
				return;
			}
		}
	}
}

void MissionPlanning::goNextState()
{
	crewSelected = true;
	goToCraft(true);
}

bool MissionPlanning::requiresCommander() const
{
	return _startingCondition->requiresCommanderOnboard();
}

bool MissionPlanning::isSoldierPermitted(const Soldier* sol) const
{
	return _startingCondition->isSoldierTypePermitted(sol->getRules()->getType());
}

bool MissionPlanning::isSoldierInCraft(const Soldier* sol) const
{
	return sol->getCraft() == _craft;
}

void MissionPlanning::launch()
{
	std::vector<Craft*> myCrafts;
	myCrafts.emplace_back(_craft);
	_game->pushState(new ConfirmDestinationState(myCrafts, _target, this));
}



} // OpenXcom
