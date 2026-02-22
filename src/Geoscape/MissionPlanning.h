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
#include <array>

namespace OpenXcom
{
/**
 * Handles the steps involved in preparing and launching a mission.
 */

class Craft;
class AlienDeployment;
class RuleStartingCondition;
class Target;
class Game;
class Soldier;
class Text;
class TextButton;
class Window;


/*enum Steps
{
	eAllowedCraft = 0,
	eHasFuel,
	eRequiredItems,
	eCommanderAssigned,
	eAllowedSoldiers,
	eAllowedArmors,
	eAllowedEquipment
};
*/

/**
*  A class handling the various steps involved in preparing and launching a mission with specific rules.
*  This is a no interface state
*/


class MissionPlanning: public State
{
	static const int _stepsCount = 7;

	//Text* _titleTxt;
	//Window *_window;
	//
	//TextButton* _BtnSelectCraft;
	//TextButton* _BtnSaveLoadOut; 
	//TextButton* _BtnRestoreLoadOut;
	//TextButton* _BtnSelectSoldiers;
	//TextButton* _BtnSaveSoldiers;
	//TextButton* _BtnSelectSoldierArmors;
	//TextButton* _BtnSaveSoldierArmors;
	//TextButton* _BtnSaveSoldierEquipement;
	//TextButton* _BtnSelectEquipment;

	//TextButton* _BtnLaunch;
	//TextButton* _BtnAbort;


	AlienDeployment* _deploymentRule;
	RuleStartingCondition* _startingCondition;
	Target* _target;
	Craft* _craft;
	bool crewSelected = false;
	bool _launched = false;
	bool _finished = false;

	bool _savedLoadout = false;
	int _id = -1;
	int _loadOutId = -1; // no loadOut currently



	bool checkFuel(Craft* craft, bool strict);
	MissionPlanning(AlienDeployment* deploymentRule, Target* target);


	bool tryEquipRequiredItems(bool rClic);

	public:
	static MissionPlanning* create(AlienDeployment* deploymentRule, Target* target);

	void confirmAbort(std::string interface);
	bool isCraftEligible(Craft* craft, bool strict = false);

	int saveEquipementLoadout();
	bool askRestorePreMissionState(const std::string& interface, bool popme = false);

	bool isCraftSelected() const { return _craft != nullptr; }
	void selectCraft(Craft* craft, bool rClic);
	void goToCraft(bool rClic);
	void goNextState();

	bool requiresCommander() const;
	bool isSoldierPermitted(const Soldier* sol) const;
	bool isSoldierInCraft(const Soldier* sol) const;
	void launch();

	int getId() const { return _id; }
	void setId(int id) { _id = id; }
	int getLoadOutId() const { return _loadOutId; }
};

} // OpenXcom
