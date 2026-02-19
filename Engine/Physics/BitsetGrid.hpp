/*********************************************************************
 * \file	BitsetGrid.hpp
 * \brief	Declares a struct that represents a dynamic array of bitsets
			that is used to optimize broad-phase collision detection.
 *
 * \author	Wong Woon Li, woonli.wong, 2301308
 * \email	woonli.wong@digipen.edu
 * \date	20 October 2024

Copyright(C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 *********************************************************************/

#ifndef BITSET_GRID_HPP
#define BITSET_GRID_HPP

#include <set>
#include <bitset>
#include <optional>
#include "Vec2.hpp"
#include "../ECS/Entity.hpp"
#include "../Components/Collider2D.hpp"
#include "../Graphics/RenderSystem.hpp"

/**
 * \struct BitsetGrid
 * \brief A dynamically-sized grid with each cell containing 500 bits.
 *
 * Each bit can potentially represent 1 entity that is in the cell.
 */
struct BitsetGrid {
	static const size_t BITSET_SIZE = 500;	// Compile-time constant. Represents the maximum number of physics objects that can potentially be in a cell.

	std::unique_ptr<std::bitset<BITSET_SIZE>[]> rowsBitArray;	// Dynamic array for the number of rows of the grid.
	std::unique_ptr<std::bitset<BITSET_SIZE>[]> colsBitArray;	// Dynamic array for the number of columns of the grid.

	int numCells;	// Total number of cells in the grid.
	int numRows, numCols;	// Number of rows and columns the grid has.
	float cellWidth, cellHeight;	// Width and height of each cell of the grid.
	Vec2 gridPos;	// Center position of the grid in the world.
	float gridSizeX, gridSizeY;	// X and Y size of the grid.

	BitsetGrid();

	/**
	 * \brief Constructs a bitset grid with properties based on the parameters.
	 *
	 * \param rows Number of rows the grid has.
	 * \param cols Number of columns the grid has.
	 * \param _cellWidth Width of each cell of the grid.
	 * \param _cellHeight Height of each cell in the grid.
	 * \param _gridPos Center world position of the grid.
	 */
	BitsetGrid(int rows, int cols, float _cellWidth, float _cellHeight, const Vec2& _gridPos);
	~BitsetGrid() = default;
	BitsetGrid& operator=(const BitsetGrid& other);

	/**
	 * \brief Initializes the debug lines to draw the grid. Used for debugging purposes.
	 *
	 * \param renderSystem Pointer to the Render System to draw the grid.
	 */
	void Init(std::shared_ptr<RenderSystem> renderSystem);

	/**
	 * \brief Gets the range of rows that the bounding box specified by min and max occupies.
	 *
	 * \param min Minimum world position of the bounding box.
	 * \param max Maximum world position of the bounding box.
	 * \return A pair of ints representing <start row range, end row range>.
	 */
	std::pair<int, int> GetRowsFromYExtents(float min, float max);

	/**
	 * \brief Gets the range of columns that the bounding box specified by min and max occupies.
	 *
	 * \param min Minimum world position of the bounding box.
	 * \param max Maximum world position of the bounding box.
	 * \return A pair of ints representing <start column range, end column range>.
	 */
	std::pair<int, int> GetColsFromXExtents(float min, float max);

	/**
	 * \brief Returns the position of the most significant bit that is set to one in the bitset.
	 *
	 * \param bitset Const reference to the bitset to check.
	 * \return Position of the bit that is set to one. If all bits are set to 1, return std::nullopt.
	 */
	std::optional<size_t> GetLastBitSetToOne(const std::bitset<BITSET_SIZE>& bitset);

	/**
	 * \brief Returns a unique ID for the entity in the grid based on the rows and columns it occupies.
	 *
	 * \param rowRange Range of rows that the entity occupies in the grid.
	 * \param colRange Range of columns that the entity occupies in the grid.
	 * \return Unique ID for the entity in the grid.
	 */
	size_t GetUniqueIDInGrid(const std::pair<int, int>& rowRange, const std::pair<int, int>& colRange);

	/**
	 * \brief Handles the adding of an entity to the bitset grid based on its collider.
	 *
	 * \param collider The collider of the entity to be added to the grid.
	 */
	bool InsertToGrid(AABBCollider2D& collider);

	/**
	 * \brief Resets all bits in the bitsets to 0.
	 */
	void ClearGrid();
	//void DrawGrid();
};

#endif