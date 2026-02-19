/*********************************************************************
 * \file	UISystem.hpp
 * \brief
 *      This file declares the UISystem class. The UISystem class is
 *		responsible for handling the UI & Textbox components of the
 *		entities.
 *
 * \author	t.yongchin, 2301359
 * \email	t.yongchin@digipen.edu
 * \date	26 November 2024
 *
 * Copyright(C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 *********************************************************************/
#pragma once

#include "../ECS/System.hpp"
#include <string>
#include <vector>

struct Vec2;
struct Vec3;

/*!*****************************************************************************
\class UISystem
\brief
	A class for managing UI and Textbox components of entities. 
	
	The UISystem is responsible for setting the text, font, color, position, 
	scale, size, and visibility of the entities.
*******************************************************************************/
class UISystem : public System {
public:

	/*
	* \brief Constructor for the UISystem
	*/
	UISystem();

	/*
	* \brief Destructor for the UISystem
	*/
	~UISystem();

	/*
	* \brief Initialises the UISystem
	*/
	void Init();

	/*
	* \brief Updates the UISystem
	*/
	//void Update();

	/*
	* \brief Exits the UISystem
	*/
	void Update(double dt);
	void Exit();

	/*
	* \brief Sets the textbox based on the entity's textbox component
	* \param entity The entity to set the textbox for
	*/
	void SetTextbox(Entity entity);

	/*
	* \brief Sets the character to the mesh
	*	Only used within the UISystem
	* 
	* \param meshID The mesh to set the character to
	* \param fontUUID The font to use
	* \param character The character to set
	*/
	void SetCharacterToMesh(size_t meshID, std::string fontUUID, char character);

	/*
	* \brief Sets the position of the entity
	* Used for the UI component
	* 
	* \param entity The entity to set the position for
	* \param position The position to set
	*/
	void SetPosition(Entity entity, Vec3 position);
	/*
	* \brief Sets the scale of the entity
	* Used for the UI component
	* 
	* \param entity The entity to set the scale for
	* \param scale The scale to set
	*/
	void SetScale(Entity entity, Vec2 scale);
	/*
	* \brief Sets the size of the entity
	* Used for the UI component
	*
	* \param entity The entity to set the size for
	* \param size The size to set
	*/
	void SetSize(Entity entity, Vec2 size);

	/*
	* \brief Sets the text of the entity
	* Used for the Textbox component
	* 
	* \param entity The entity to set the text for
	* \param text The text to set
	*/
	void SetText(Entity entity, std::string text);
	/*
	* \brief Sets the font of the entity
	* Used for the Textbox component
	* 
	* \param entity The entity to set the font for
	* \param fontUUID The font to set
	*/
	void SetFont(Entity entity, std::string fontUUID);
	/*
	* \brief Sets the color of the entity
	* Used for the Textbox component
	* 
	* \param entity The entity to set the color for
	* \param color The color to set
	*/
	void SetColor(Entity entity, Vec3 color);

	/*
	* \brief Sets the visibility of the entity
	* 
	* \param entity The entity to set the visibility for
	* \param visible The visibility to set
	*/
	void SetVisibility(Entity entity, bool visible);

	/*
	* \brief Sets whether the text is center aligned
	* 
	* \param entity The entity to set the center alignment for
	* \param centerAlign The alignment to set, true for center aligned, false for left aligned
	*/
	void SetCenterAlignment(Entity entity, bool centerAligned);

	/*
	* \brief Converts normalised screen coordinates to NDC
	*	Only used within the UISystem
	* 
	* \param screenPos The screen position to convert
	* \return The NDC position
	*/
	Vec3 NormalisedScreenToNDC(Vec3 screenPos);
};