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

/**
 *	A virtual class ineherited by states that allow switching between bases
 *  Manages the navigation buttons and logic 
 */

class BaseSwitcher
{
  protected:
	size_t _baseIndex;
	size_t _validBasesCount;
	Base* _bsbase = nullptr;

	State* _parent = nullptr;
	InteractiveSurface *_surface = nullptr;

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
	bool _ignoreBase(Base* base) const;

  protected:
	 /// virtual function fine tuning the class behavior
	virtual bool ignoreBase(Base* base) const { return false; }
	virtual size_t getValidBasesCount() const;
	virtual void doBeforeBaseChange();
	virtual void doAfterBaseChange();
	virtual bool allowSwitching() const { return true; }
	
	/// pushes the new state for the selected base, needs to be defined by derived classes
	virtual void doPush(Base* base) = 0;
	/// pure virtual functions managing the button handlers, the BASE_SWITCHER_HANDLERS macro below will take care of that 
	virtual ActionHandler getPrevButtonHandler() = 0;
	virtual ActionHandler getNextButtonHandler() = 0;

public:
	/// Creates a new BaseSwitcher linked to a parent state and a base.
	BaseSwitcher(Base* base);	
	virtual ~BaseSwitcher() = default;
	/// Moves to the next/previous base.
	void nextBase(int direction);
	/// moves to base
	void rebase(Base* base);

	/// Adds navigation buttons if there are multiple bases.
	void addNavigationButtons(State* parent, InteractiveSurface* surface, bool update = false);
	/// updates navigation buttons
	void updateNavigationButtons(State* parent, InteractiveSurface* surface);
	/// hide and show navigation buttons
	void hideNavigationButtons() { toggleNavigationButtons(false); }
	void showNavigationButtons() { toggleNavigationButtons(true); }
	/// pops the current state and tries to rebase the previous state if necessary
	void popStateAndRebase();

};


/// macro adding the navigation buttons and their handlers to the derived classes.
#define BASE_SWITCHER_HANDLERS(ImplementationClass) \
private: \
	ActionHandler getPrevButtonHandler() { return (ActionHandler)(&ImplementationClass::btnPrevBaseClick); } \
	ActionHandler getNextButtonHandler() { return (ActionHandler)(&ImplementationClass::btnNextBaseClick); } \
public: \
	void btnPrevBaseClick(Action* action) { nextBase(-1); } \
	void btnNextBaseClick(Action* action) { nextBase(1); } \
	bool _dummyToForceSemicolonAfterMacro = true



} // namespace OpenXcom

