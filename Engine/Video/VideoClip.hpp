/*********************************************************************
 * \file	    VideoClip.hpp
 * \brief       Base VideoClip asset declaration
 *
 * \author		y.ziyangirwen, 2301345 (y.ziyangirwen@digipen.edu)
 * \date		30 January 2025
 *
 * \copyright	Copyright(C) 2024 DigiPen Institute of Technology.
 *				Reproduction or disclosure of this file or its
 *              contents without the prior written consent of DigiPen
 *              Institute of Technology is prohibited.
 *********************************************************************/

#ifndef VIDEO_CLIP_HPP
#define VIDEO_CLIP_HPP

#include "../Asset.hpp"
typedef unsigned int GLuint;

class VideoClip : public IAsset {
public:
	VideoClip() = default;
	~VideoClip() = default;

	bool LoadFromFile(const std::string& filePath) override;
	size_t SetTextureArrayToUse(int widthImage, int heightImage);

	void CopyAllTextureLayers(GLuint srcTex, GLuint destTex, int width, int height, int numLayers);

	size_t texArrayIndex;
	size_t texLayerStartIndex;
	size_t texLayerEndIndex;
};

#endif