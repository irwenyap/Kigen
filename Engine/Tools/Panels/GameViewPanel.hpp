/*********************************************************************
 * \file	GameViewPanel.hpp
 * \brief	Defines a GameViewPanel class that manages everything related to 
 *			the game view panel in the workspace.
 *
 * \author	y.ziyangirwen, 2301345
 * \date	29 September 2024

Copyright(C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 *********************************************************************/
#pragma once

#include "../EditorPanel.hpp"

class GameViewPanel : public EditorPanel {
public:
	GameViewPanel();

	void Init();
	void Update() override;
	void Exit();

private:
	std::string textureUUID;
	GLuint tempTexture2D;
};

