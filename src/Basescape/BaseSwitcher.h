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

#include <string>
#include "../Engine/State.h"
#include "../Engine/InteractiveSurface.h"

namespace OpenXcom
{
class TextButton;
class Text;
class Base;
class Action;



/*
class BaseSwitcherTraits
{
public:
	static constexpr int BUTTON_WIDTH = 10;
	static constexpr int BUTTON_HEIGHT = 8;
	static constexpr int TEXT_WIDTH = 100;
	static constexpr int TEXT_HEIGHT = 8;
	static constexpr int SURFACE_WIDTH = 320;
	static constexpr int SURFACE_HEIGHT = 8;
	static constexpr int NEXT_BUTTON_X = 310;
	static constexpr int NEXT_TEXT_X = 210;
	static constexpr int PREV_BUTTON_X = 0;
	static constexpr int PREV_TEXT_X = 10;

	virtual bool ignoreBase(Base* base) const { return false; }
	virtual size_t getValidBasesCount() const;
	virtual void doBeforeBaseChange();
	virtual void doAfterBaseChange();

};
*/

//class Game;
/**
 *	A virtual class ineherited by states that allow switching between bases
 */

class BaseSwitcher
{
  protected:
	size_t _baseIndex;
	size_t _validBasesCount;
	Base* _bsbase = nullptr;

  private:
//	State* _parent = nullptr;
	TextButton *_prevButton = nullptr, *_nextButton = nullptr; // Navigation buttons
	Text *_nextText= nullptr, *_prevText = nullptr;

	bool _moved = false;

	size_t getBaseIndex(Base* base) const;
	/// Gets the (current/next/previous) base index depending on 'direction'.
	size_t nextBaseIndex(int direction = 0) const;
	/// Gets the (current/next/previous) base name depending on 'direction'.
	std::string getBaseName(int direction = 0) const;
	/// Shows or hides the navigation buttons.
	void toggleNavigationButtons(bool show);
	/// Calculates the number of valid bases, provided to be used by getValidBasesCount() when ignoreBase() is overridden.
	size_t calculateValidBaseCount() const;

  protected:	
	virtual bool ignoreBase(Base* base) const { return false; }
	virtual size_t getValidBasesCount() const;
	virtual void doBeforeBaseChange();
	virtual void doAfterBaseChange();
	/// pushes the new state for the selected base
	virtual void doPush(Base* base) = 0;
	virtual ActionHandler getPrevButtonHandler() = 0;
	virtual ActionHandler getNextButtonHandler() = 0;

public:
	/// Creates a new BaseSwitcher linked to a parent state and a base.
	BaseSwitcher(Base* base);	
	virtual ~BaseSwitcher();
	/// Moves to the next/previous base.
	void nextBase(int direction);

	/// Adds navigation buttons if there are multiple bases.
	void addNavigationButtons(State* parent, InteractiveSurface* surface);
	void hideNavigationButtons() { toggleNavigationButtons(false); }
	void showNavigationButtons() { toggleNavigationButtons(true); }	
};

#define BASE_SWITCHER_HANDLERS(ImplementationClass) \
private: \
	ActionHandler getPrevButtonHandler() { return (ActionHandler)(&ImplementationClass::btnPrevBaseClick); } \
	ActionHandler getNextButtonHandler() { return (ActionHandler)(&ImplementationClass::btnNextBaseClick); } \
public: \
	void btnPrevBaseClick(Action* action) { nextBase(-1); } \
	void btnNextBaseClick(Action* action) { nextBase(1); } \
	bool _dummyToForceSemicolonAfterMacro = true



} // namespace OpenXcom

