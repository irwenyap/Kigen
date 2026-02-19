/*********************************************************************
 * \file		Animation.hpp
 * \brief		Defines the base Animation component
 *
 * \author		y.ziyangirwen, 2301345 (y.ziyangirwen@digipen.edu)
 * \date		01 November 2024
 *
 * \copyright	Copyright(C) 2024 DigiPen Institute of Technology.
 *				Reproduction or disclosure of this file or its
 *              contents without the prior written consent of DigiPen
 *              Institute of Technology is prohibited.
 *********************************************************************/
#pragma once

struct Animation {

	Animation();
	Animation(
		uint32_t sprPerRow, uint32_t sprPerCol,
		uint32_t numFrames, uint32_t startFrame, uint32_t endFrame,
		double timePerFrame, bool isLooping = false, bool playOnce = false
	);

	unsigned spritesPerRow;
	unsigned spritesPerCol;

	unsigned numFrames;
	unsigned startFrame;
	unsigned endFrame;

	unsigned currentFrame;
	double timePerFrame;
	double elapsedTime;

	float spriteWidth;
	float spriteHeight;
	
	bool playOnce;
	bool isLooping;
};

inline Animation::Animation()
	: spritesPerRow(5), spritesPerCol(5),
	numFrames(25), startFrame(0), endFrame(5),
	timePerFrame(0.15), isLooping(false), playOnce(false),
	currentFrame(0), elapsedTime(0.0)
{
	spriteWidth = 1.f / spritesPerRow;
	spriteHeight = 1.f / spritesPerCol;
}

inline Animation::Animation(
	uint32_t sprPerRow, uint32_t sprPerCol,
	uint32_t numFrames, uint32_t startFrame, uint32_t endFrame,
	double timePerFrame, bool isLooping, bool playOnce) 
	: spritesPerRow(sprPerRow), spritesPerCol(sprPerCol), 
	numFrames(numFrames), startFrame(startFrame), endFrame(endFrame), 
	timePerFrame(timePerFrame), isLooping(isLooping), playOnce(playOnce),
	currentFrame(startFrame), elapsedTime(0.0)
{
	spriteWidth = 1.f / spritesPerRow;
	spriteHeight = 1.f / spritesPerCol;
}
