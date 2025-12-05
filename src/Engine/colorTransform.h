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

#include <unordered_map>
#include <limits>
#include <vector>
#include <SDL.h>
#include "Palette.h"

namespace OpenXcom
{

using Transform = SDL_Color (*)(const SDL_Color&);

/**
 * @brief Template helper that caches the result of transforming palette indices.
 *
 * Usage:
 *   // define a function matching Transform
 *   SDL_Color makeRedder(const SDL_Color &c) { ... }
 *   // then use ColorTransform<makeRedder>::get(idx, palette, paletteSize)
 *
 * NOTE: the cache is keyed only by palette index. If you call with different
 * palettes you must call clearCache() between them (typical case: project uses
 * a single global palette).
 */

template <Transform transform>
class ColorTransform
{
  private:
	inline static bool _initialized = false;
	inline static std::vector<SDL_Color> _palette; 
	inline static int _paletteSize = 0;

	static std::unordered_map<Uint8, Uint8> _colors;

	// squared Euclidean distance between two SDL_Color
	static inline int colorDistSq(const SDL_Color& a, const SDL_Color& b) noexcept
	{
		int dr = int(a.r) - int(b.r);
		int dg = int(a.g) - int(b.g);
		int db = int(a.b) - int(b.b);
		return dr * dr + dg * dg + db * db;
	}

	static Uint8 apllyChanges(Uint8 colorIndex)
	{
		if (!_initialized || _paletteSize <= 0 || colorIndex >= _paletteSize)
		{
			// TODO : log error
			return 0;
		}

		// no idea why :D
		if (colorIndex >= 240)
			colorIndex -= 3; 

		SDL_Color original = _palette[colorIndex];
		SDL_Color target = transform(original);

		// Find the closest matching color in the palette
		
		Uint8 bestIdx = 0;
		int bestDist = -1;
		for (int i = 0; i < _paletteSize; ++i)
		{
			int dist = colorDistSq(target, _palette[i]);
			if (bestDist < 0)
			{
				bestDist = dist;
				if (dist == 0)
					break;
			}
			else if (dist < bestDist)
			{
				bestDist = dist;
				bestIdx = i;
				if (dist == 0)
					break; // exact match found
			}
		}
		
		return bestIdx;	
	}

	public:

	/// Initialize the palette to use for transformations.
	/// If force is true, re-initialize even if already initialized.
	
	static void initPalette(SDL_Color* palette, int paletteSize, bool force = false)
	{
		if (!force && _initialized)
				return;
		if (!palette || paletteSize <= 0)
		{
			// TODO : log error
			_initialized = false;
			return;
		}
		_palette.assign(palette, palette + paletteSize);
		_paletteSize = paletteSize;
		_initialized = true;
		_colors.clear();
	}

	/// Get transformed color for inputColor, using internal cache.
	static Uint8 get(Uint8 inputColor)
	{
		auto it = _colors.find(inputColor);
		if (it != _colors.end())
		{
			return it->second; // Return the transformed color from the cache
		}
		else
		{
			// Otherwise, compute the transformation
			Uint8 transformed = apllyChanges(inputColor);
			// Store in the cache
			_colors[inputColor] = transformed;
			return transformed;
		}
	}

	/// Clear internal cache (call when palette changes).
	static void clearCache()
	{
		_colors.clear();
	}

};

// static member definition
template <Transform transform>
std::unordered_map<Uint8, Uint8> ColorTransform<transform>::_colors;

}
