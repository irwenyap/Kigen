/*********************************************************************
 * \file	BitsetGrid.cpp
 * \brief	Defines a struct that represents a dynamic array of bitsets
			that is used to optimize broad-phase collision detection.
 *
 * \author	Wong Woon Li, woonli.wong, 2301308
 * \email	woonli.wong@digipen.edu
 * \date	20 October 2024

Copyright(C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 *********************************************************************/

#include <algorithm>
#include "BitsetGrid.hpp"
//#include "../Components/Renderer.hpp"
#include "../ECS/ECSManager.hpp"
#include "../Components/Renderer.hpp"
#include "../Components/Transform.hpp"

//extern ECSManager g_ecsManager;

// Default ctor: create a 10 x 10 grid with cells of size 200 and position at center of screen.
BitsetGrid::BitsetGrid() {
	// By default, create a 10 x 10 grid.
	numRows = 10, numCols = 10;
	numCells = numRows * numRows;
	rowsBitArray = std::make_unique< std::bitset<BITSET_SIZE>[]>(numRows);
	colsBitArray = std::make_unique< std::bitset<BITSET_SIZE>[]>(numCols);
	cellWidth = 250.f, cellHeight = 250.f;
	gridPos = Vec2{ 960, 540 };
	gridSizeX = cellWidth * static_cast<float>(numCols);
	gridSizeY = cellHeight * static_cast<float>(numRows);
}

// Ctor: Create a grid of specified rows and columns, cell size, and position.
BitsetGrid::BitsetGrid(int rows, int cols, float _cellWidth, float _cellHeight, const Vec2& _gridPos) {
	numRows = rows, numCols = cols;
	rowsBitArray = std::make_unique< std::bitset<BITSET_SIZE>[]>(numRows);
	colsBitArray = std::make_unique< std::bitset<BITSET_SIZE>[]>(numCols);
	cellWidth = _cellWidth, cellHeight = _cellHeight;
	gridPos = _gridPos;
	gridSizeX = cellWidth * static_cast<float>(numCols);
	gridSizeY = cellHeight * static_cast<float>(numRows);
}

BitsetGrid& BitsetGrid::operator=(const BitsetGrid& other) {
	numRows = other.numRows, numCols = other.numCols;
	rowsBitArray = std::make_unique< std::bitset<BITSET_SIZE>[]>(numRows);
	colsBitArray = std::make_unique< std::bitset<BITSET_SIZE>[]>(numCols);
	cellWidth = other.cellWidth, cellHeight = other.cellHeight;
	gridPos = other.gridPos;
	gridSizeX = cellWidth * static_cast<float>(numCols);
	gridSizeY = cellHeight * static_cast<float>(numRows);

	for (size_t i = 0; i < numRows; ++i) {
		rowsBitArray[i] = rowsBitArray[i] | other.rowsBitArray[i];
	}

	for (size_t i = 0; i < numCols; ++i) {
		colsBitArray[i] = colsBitArray[i] | other.colsBitArray[i];
	}

	return *this;
}

// TEMP: Draw a static debug grid.
void BitsetGrid::Init(std::shared_ptr<RenderSystem> renderSystem) {
	renderSystem;
	//float currentY = gridPos.y - (gridSizeY / 2.f);
	//float currentX = gridPos.x - (gridSizeX / 2.f);
	//
	//for (int row = 0; row <= numRows; ++row) {
	//	for (int col = 0; col <= numCols; ++col) {
	//		std::pair<size_t, size_t> id = renderSystem->AddMesh(RenderSystem::MeshType::QUAD);
	//		renderSystem->SetTextureToMesh(id.first, 1);
	//		renderSystem->SetVisibility(id.first, false);
	//		renderSystem->SetVisibility(id.second, true);

	//		// Create the entity and assign the mesh to it
	//		Entity line = ECSManager::GetInstance().CreateEntity();
	//		// Add Renderer component.
	//		ECSManager::GetInstance().AddComponent(line, Renderer(id.first, id.second));

	//		renderSystem->SetPosition(id.first, id.second, Vec2{ currentX, currentY });
	//		renderSystem->SetScale(id.first, id.second, Vec2{ 0.5f, 0.05f });

	//		id = renderSystem->AddMesh(RenderSystem::MeshType::QUAD);
	//		renderSystem->SetTextureToMesh(id.first, 1);
	//		renderSystem->SetVisibility(id.first, false);
	//		renderSystem->SetVisibility(id.second, true);

	//		// Create the entity and assign the mesh to it
	//		line = ECSManager::GetInstance().CreateEntity();
	//		// Add Renderer component.
	//		ECSManager::GetInstance().AddComponent(line, Renderer(id.first, id.second));
	//		renderSystem->SetPosition(id.first, id.second, Vec2{ currentX, currentY });
	//		renderSystem->SetScale(id.first, id.second, Vec2{ 0.5f, 0.05f });
	//		renderSystem->SetRotation(id.first, id.second, 90.f);

	//		currentX += cellWidth;
	//	}

	//	currentX = gridPos.x - (gridSizeX / 2.f);
	//	currentY += cellHeight;
	//}

	//for (auto& batch : GraphicsManager::GetInstance().batches) {
	//	batch.UpdateBuffers();
	//}
}

std::pair<int, int> BitsetGrid::GetRowsFromYExtents(float min, float max) {
	std::pair<int, int> rowRange{};
	float gridMinY = gridPos.y - (gridSizeY / 2.f);
	float relativeYMin = min - gridMinY;
	rowRange.first = static_cast<int>(relativeYMin / cellHeight);
	float relativeYMax = max - gridMinY;
	rowRange.second = static_cast<int>(relativeYMax / cellHeight);
	return rowRange;
}

std::pair<int, int> BitsetGrid::GetColsFromXExtents(float min, float max) {
	std::pair<int, int> colRange{};
	float gridMinX = gridPos.x - (gridSizeX / 2.f);
	float relativeXMin = min - gridMinX;
	colRange.first = static_cast<int>(relativeXMin / cellWidth);
	float relativeXMax = max - gridMinX;
	colRange.second = static_cast<int>(relativeXMax / cellWidth);
	return colRange;
}

bool BitsetGrid::InsertToGrid(AABBCollider2D& collider) {
	// Get the range of rows of the grid this collider occupies.
	std::pair<int, int> rowRange = GetRowsFromYExtents(collider.min.y, collider.max.y);
	
	// If the collider partially lies in the grid, update the range of rows it occupies accordingly.
	if (rowRange.first < 0) 
		rowRange.first = 0;
	if (rowRange.second >= numRows) 
		rowRange.second = numRows - 1;

	// Get the range of columns of the grid this collider occupies.
	std::pair<int, int> colRange = GetColsFromXExtents(collider.min.x, collider.max.x);

	// If the collider lies outside the grid, just return.
	if ((rowRange.first >= numRows || rowRange.second < 0) || (colRange.first >= numCols || colRange.second < 0))
		return false;
	// Else if the collider partially lies in the grid, update the range of columns it occupies accordingly.
	if (colRange.first < 0)
		colRange.first = 0;
	if (colRange.second >= numCols)
		colRange.second = numCols - 1;

	// Get a unique ID for the collider in the grid.
	size_t id = GetUniqueIDInGrid(rowRange, colRange);
	collider.idInGrid = id;
	collider.inGrid = true;
	collider.rowsOccupied = rowRange;
	collider.colsOccupied = colRange;
	
	// Set the bitset of the rows the collider occupies.
	// The bit of each row bitset at position id will be set to 1.
	for (int row = rowRange.first; row <= rowRange.second; ++row) {
		rowsBitArray[row].set(id);
		//std::cout << "Row " << row << " bitset set to: " << rowsBitArray[row] << std::endl;
	}

	// Set the bitset of the columns the collider occupies.
	// The bit of each column bitset at position id will be set to 1.
	for (int col = colRange.first; col <= colRange.second; ++col) {
		colsBitArray[col].set(id);
		//std::cout << "Col " << col << " bitset set to: " << colsBitArray[col] << std::endl;

		//size_t cellIndex = (row * numRows) + col;
		//size_t id = grid[cellIndex].count();

		//grid[cellIndex].set(id);
		//collider.gridCellIndexToIDMap[cellIndex] = id;
	}

	return true;
}

size_t BitsetGrid::GetUniqueIDInGrid(const std::pair<int, int>& rowRange, const std::pair<int, int>& colRange) {
	size_t id{};
	for (int row = rowRange.first; row <= rowRange.second; ++row) {
		//std::cout << "Row " << row << " bitset: " << rowsBitArray[row] << std::endl;
		auto rowPos = GetLastBitSetToOne(rowsBitArray[row]);
		if (rowPos.has_value()) {
			//std::cout << "Row " << row << " bitset available pos : " << rowPos.value() << std::endl;
			if (id < rowPos.value()) id = rowPos.value();
		}
	}

	for (int col = colRange.first; col <= colRange.second; ++col) {
		//std::cout << "Col " << col << " bitset: " << colsBitArray[col] << std::endl;
		auto colPos = GetLastBitSetToOne(colsBitArray[col]);
		if (colPos.has_value()) {
			//std::cout << "Col " << col << " bitset available pos : " << colPos.value() << std::endl;
			if (id < colPos.value()) id = colPos.value();
		}
	}
	return id;
}

std::optional<size_t> BitsetGrid::GetLastBitSetToOne(const std::bitset<BITSET_SIZE>& bitset) {
	if (bitset.none()) return std::optional(0);
	else if (bitset.all()) return std::nullopt;
	else {
		for (size_t pos = bitset.size() - 1; pos >= 0; --pos) {
			if (bitset.test(pos)) {
				return std::optional(pos + 1);
			}
		}

		return std::nullopt;
	}
}

void BitsetGrid::ClearGrid() {
	for (size_t i = 0; i < numRows; ++i) {
		rowsBitArray[i].reset();
	}

	for (size_t i = 0; i < numCols; ++i) {
		colsBitArray[i].reset();
	}
}
