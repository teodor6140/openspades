/*
 Copyright (c) 2013 yvt

 This file is part of OpenSpades.

 OpenSpades is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 OpenSpades is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with OpenSpades.  If not, see <http://www.gnu.org/licenses/>.

 */

#pragma once

#include <cstdint>

#include "IStream.h"
#include "Math.h"
#include <Core/Debug.h>
#include <Core/RefCountedObject.h>

namespace spades {
	/**
	 * Material IDs used by `VoxelModel`.
	 */
	enum class MaterialType : uint8_t {
		Default = 0,
		Emissive = 1,
	};

	class VoxelModel : public RefCountedObject {
		Vector3 origin;
		int width, height, depth;
		uint64_t *solidBits;
		uint32_t *colors;

	protected:
		~VoxelModel();

	public:
		VoxelModel(int width, int height, int depth);

		void HollowFill();

		/**
		 * Load a `VoxelModel` from a stream in the KV6 format.
		 *
		 * The KV6 format does not include material information, so the material
		 * IDs of the loaded voxels are set to `0`.
		 */
		static VoxelModel *LoadKV6(IStream *);

		/**
		 * Replace the material ID with the specified value.
		 */
		void ForceMaterial(MaterialType newMaterialId);

		const uint64_t &GetSolidBitsAt(int x, int y) const {
			SPAssert(x >= 0);
			SPAssert(x < width);
			SPAssert(y >= 0);
			SPAssert(y < height);
			return solidBits[x + y * width];
		}
		uint64_t &GetSolidBitsAt(int x, int y) {
			SPAssert(x >= 0);
			SPAssert(x < width);
			SPAssert(y >= 0);
			SPAssert(y < height);
			return solidBits[x + y * width];
		}

		/**
		 * Get the color value of a voxel.
		 *
		 * The color value is a 32-bit value where the lower 24 bits represent
		 * a color. The remaining 8 bits represent a material ID. See
		 * `MaterialType` for the predefined material IDs.
		 */
		const uint32_t &GetColor(int x, int y, int z) const {
			SPAssert(x >= 0);
			SPAssert(x < width);
			SPAssert(y >= 0);
			SPAssert(y < height);
			SPAssert(z >= 0);
			SPAssert(z < depth);
			return colors[(x + y * width) * depth + z];
		}

		uint32_t &GetColor(int x, int y, int z) {
			SPAssert(x >= 0);
			SPAssert(x < width);
			SPAssert(y >= 0);
			SPAssert(y < height);
			SPAssert(z >= 0);
			SPAssert(z < depth);
			return colors[(x + y * width) * depth + z];
		}

		bool IsSolid(int x, int y, int z) const {
			if (z < 0 || z >= depth)
				return false;
			if (x < 0 || y < 0 || x >= width || y >= height)
				return false;
			uint64_t bits = GetSolidBitsAt(x, y);
			return (bits >> z) & 1;
		}

		void SetAir(int x, int y, int z) {
			SPAssert(z >= 0);
			SPAssert(z < depth);
			uint64_t mask = 1ULL << z;
			GetSolidBitsAt(x, y) &= ~mask;
		}

		void SetSolid(int x, int y, int z, uint32_t color) {
			uint64_t mask = 1ULL << z;
			GetSolidBitsAt(x, y) |= mask;
			GetColor(x, y, z) = color;
		}

		Vector3 GetOrigin() { return origin; }

		void SetOrigin(Vector3 v) { origin = v; }

		int GetWidth() const { return width; }
		int GetHeight() const { return height; }
		int GetDepth() const { return depth; }
	};
}
