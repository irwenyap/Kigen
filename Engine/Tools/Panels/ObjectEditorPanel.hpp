/*********************************************************************
 * \file	ObjectEditorPanel.hpp
 * \brief	Defines a ScenePanel class that manages everything related to the object editor panel in the workspace.
 *
 * \author	wengkhong.thang, 2301372, irwinjun.l, 2301305
 * \email	wengkhong.thang@digipen.edu, irwinjun.l@digipen.edu
 * \date	29 September 2024

Copyright(C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 *********************************************************************/
#pragma once
#include "../EditorPanel.hpp"  


class ObjectEditorPanel : public EditorPanel {
public:
    // Constructor with color and size parameters
    ObjectEditorPanel(float* color, float* size);  

    /**
     * @brief Updates the contents of the object editor panel.
     *
     * This method creates an ImGui window to allow users to edit the
     * properties of the object. It includes a color picker for changing
     * the object's color and a slider for adjusting the object's scale.
     */
    void Update() override;   

    /*template<typename T>
    void DisplayAddComponentEntry(const std::string& entryName);*/
    //template <typename T>
    //void DrawComponentEditor(T)
private:
    void DrawAddComponentDropdown();
    void SetInactive(std::list<Gui::Entity*>& children, bool isActive);
    void DrawComponents();//need to parse in entity object eventually

    float* m_Color;
    float* m_Size;
};
