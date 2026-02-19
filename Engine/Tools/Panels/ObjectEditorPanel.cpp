/*********************************************************************
 * \file		ObjectEditorPanel.cpp
 * \brief		Defines a ScenePanel class that manages everything 
 *				related to the object editor panel in the workspace.
 *
 * \author		y.ziyangirwen, 2301345
 * \author		wengkhong.thang, 2301372
 * \author		irwinjun.l, 2301305
 * \author		Wilmerjunrong.lee , 2200691
 * \author		woonli.wong, 2301308
 * \email		wengkhong.thang@digipen.edu, irwinjun.l@digipen.edu,
 *				woonli.wong@digipen.edu
 * \date		29 September 2024

 * \copyright	Copyright(C) 2024 DigiPen Institute of Technology.
 *				Reproduction or disclosure of this file or its
 *              contents without the prior written consent of DigiPen
 *              Institute of Technology is prohibited.
 *********************************************************************/
#include "ObjectEditorPanel.hpp"

// IMGUI
#include "../ImGui/imgui.h"
#include "../ImGui/imgui_impl_glfw.h"
#include "../ImGui/imgui_impl_opengl3.h"
#include "../ImGui/imgui_internal.h"
#include "../ImGui/cpp/imgui_stdlib.h"


#include "../Engine/ECS/ECSManager.hpp"
#include "../Engine/Graphics/GraphicsManager.hpp"
#include "../Engine/Components/Name.hpp"
#include "../Engine/Components/Transform.hpp"
#include "../Engine/Components/Renderer.hpp"
#include "../Engine/Components/Rigidbody2D.hpp"
#include "../Engine/Components/ScriptComponent.hpp"
#include "../Engine/Components/Animation.hpp"
#include "../Engine/Components/AudioSource.hpp"
#include "../Engine/Components/UI.hpp"
#include "../Engine/Components/StateMachineComponent.hpp"
#include "../Engine/Components/Textbox.hpp"
#include "../Scripting/ScriptEngine.hpp"
#include "../PrefabManager.hpp"
#include "../../Utility/Serializer.hpp"
#include "../../Layers/LayerManager.hpp"
#include "../../Audio/AudioManager.hpp"
#include "../../StateMachine/CustomState.hpp"

#include "../../AssetManager.hpp"
#include "../../Layers/SortingLayerManager.hpp"
#include "../../Tools/Scripting/ScriptGlue.hpp"
#include "../../Components/VideoPlayer.hpp"
#include "../../Components/Camera.hpp"

extern EngineState engineState;

ObjectEditorPanel::ObjectEditorPanel(float* color, float* size)
    : m_Color(color), m_Size(size) {
    name = "Object Editor";  // Set the panel name
    show = true;             // Default visibility to true
}

void ObjectEditorPanel::DrawAddComponentDropdown() {
	if (ImGui::Button("Add Component")) {
		ImGui::OpenPopup("ComponentList");
	}

	if (ImGui::BeginPopup("ComponentList")) {
		if (ImGui::MenuItem("Renderer")) {
			std::pair<size_t, size_t> mesh = ECSManager::GetInstance().renderSystem->AddMesh(3);
			ECSManager::GetInstance().AddComponent(selectedEntity->id, Renderer(mesh.first, mesh.second, ""));
			ECSManager::GetInstance().renderSystem->SetColorToEntity(selectedEntity->id, ECSManager::GetInstance().renderSystem->EncodeColor(selectedEntity->id));

			for (auto& batch : GraphicsManager::GetInstance().batches) {
				batch.UpdateBuffers();
			}
		}
		else if (ImGui::MenuItem("Rigidbody2D")) {
			const auto& transform = ECSManager::GetInstance().GetComponent<Transform>(selectedEntity->id);
			ECSManager::GetInstance().physicsSystem->AddRigidbodyComponent(selectedEntity->id, Vec2(transform.position), Vec2{}, 1.f, 0.f, 1.f);
		}
		else if (ImGui::MenuItem("AABBCollider2D")) {
			ECSManager::GetInstance().physicsSystem->AddAABBColliderComponent(
				selectedEntity->id
				);
		}
		else if (ImGui::MenuItem("Animation")) {
			ECSManager::GetInstance().AddComponent(selectedEntity->id, Animation{});
		}
		else if (ImGui::MenuItem("Scripts")) {
			ECSManager::GetInstance().AddComponent(selectedEntity->id, ScriptComponent());
		}
		else if (ImGui::MenuItem("AudioSource")) {
			ECSManager::GetInstance().AddComponent(selectedEntity->id, AudioSource{});
		}
		else if (ImGui::MenuItem("StateMachine")) {
			ECSManager::GetInstance().AddComponent(selectedEntity->id, StateMachineComponent(selectedEntity->id));
		}
		ImGui::EndPopup();
	}
}

void ObjectEditorPanel::Update() {
	RegisterAllStates();
    //ImGui::SetNextWindowPos(ImVec2(1286, 435), ImGuiCond_Once);
    //ImGui::SetNextWindowSize(ImVec2(1308, 581), ImGuiCond_Once);
    //ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoMove;
    ImGui::Begin(name.c_str(), nullptr);  // Begin ImGui window with panel's name

	if (selectedEntity) {
		ImGui::Text("Edit Entity Properties");
		ImGui::Separator();
		DrawComponents();
		DrawAddComponentDropdown();
	}

    ImGui::End();
}

static bool DrawBoolControl(const char* label, bool& value) {
	if (ImGui::Checkbox(label, &value)) {
		return true;
	}
	return false;
}

static bool DrawFloatControl(const char* label, float& value) {
	if (ImGui::InputFloat(label, &value)) {
		return true;
	}
	return false;
}

static bool DrawUIntControl(const char* label, unsigned int& value) {
	if (ImGui::DragScalar(label, ImGuiDataType_U32, &value, 1.0f, nullptr, nullptr)) {
		return true;
	}
	return false;
}

#pragma region Vec3Control
struct Vec3ControlStyle {
	// Modern color scheme
	ImVec4 xButtonColor = { 0.91f, 0.3f, 0.24f, 0.9f };     // Soft red
	ImVec4 xButtonHovered = { 0.91f, 0.3f, 0.24f, 1.0f };
	ImVec4 xButtonActive = { 0.83f, 0.24f, 0.18f, 1.0f };

	ImVec4 yButtonColor = { 0.22f, 0.67f, 0.33f, 0.9f };    // Soft green
	ImVec4 yButtonHovered = { 0.22f, 0.67f, 0.33f, 1.0f };
	ImVec4 yButtonActive = { 0.17f, 0.61f, 0.27f, 1.0f };

	ImVec4 zButtonColor = { 0.25f, 0.54f, 0.89f, 0.9f };    // Soft blue
	ImVec4 zButtonHovered = { 0.25f, 0.54f, 0.89f, 1.0f };
	ImVec4 zButtonActive = { 0.20f, 0.48f, 0.83f, 1.0f };

	// Text colors
	ImVec4 labelColor = { 0.85f, 0.85f, 0.85f, 1.0f };      // Bright text
	ImVec4 valueColor = { 0.7f, 0.7f, 0.7f, 1.0f };         // Slightly dimmed value text

	// Styling
	float cornerRadius = 3.0f;                             // Rounded corners
	float spacing = 4.0f;                                  // Space between elements
	float buttonPadding = 6.0f;                           // Padding inside buttons
	ImVec2 framePadding = { 6.0f, 4.0f };                   // Padding for input fields
};

static bool DrawAxisControl(const char* label, float& value, float resetValue,
	const ImVec2& buttonSize, ImFont* boldFont,
	const ImVec4& buttonColor, const ImVec4& buttonHovered,
	const ImVec4& buttonActive, const Vec3ControlStyle& style)
{
	bool retVal = false;
	// Push rounded corners for the button
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, style.cornerRadius);
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, style.framePadding);

	// Button colors
	ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, buttonHovered);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, buttonActive);

	// Text color for the axis label
	ImGui::PushStyleColor(ImGuiCol_Text, style.labelColor);

	ImGui::PushFont(boldFont);
	if (ImGui::Button(label, buttonSize))
		value = resetValue;
	ImGui::PopFont();

	ImGui::PopStyleColor(4);

	ImGui::SameLine(0.0f, style.spacing);

	// Style for the input field
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.16f, 0.16f, 0.16f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.19f, 0.19f, 0.19f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.22f, 0.22f, 0.22f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_Text, style.valueColor);

	// Draw the value input
	if (ImGui::DragFloat(("##" + std::string(label)).c_str(), &value, 0.1f, 0.0f, 0.0f, "%.2f")) retVal = true;

	ImGui::PopStyleColor(4);
	ImGui::PopStyleVar(2);
	ImGui::PopItemWidth();
	ImGui::SameLine(0.0f, style.spacing);
	return retVal;
}

static bool DrawVec3Control(const std::string& label, Vec3& values,
	float resetValue = 0.0f, float columnWidth = 80.0f,
	const Vec3ControlStyle& style = Vec3ControlStyle())
{
	ImGuiIO& io = ImGui::GetIO();
	auto boldFont = io.Fonts->Fonts[0];

	ImGui::PushID(label.c_str());

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ style.spacing * 2, style.spacing });
	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, columnWidth);

	ImGui::PushStyleColor(ImGuiCol_Text, style.labelColor);
	ImGui::Dummy(ImVec2(0.f, 2.f));
	ImGui::Text(label.c_str());
	ImGui::PopStyleColor();

	ImGui::NextColumn();

	// Calculate sizes with padding
	float lineHeight = GImGui->Font->FontSize + style.buttonPadding * 2;
	ImVec2 buttonSize = { lineHeight, lineHeight };

	// Container frame styling
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ style.spacing, style.spacing });
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.13f, 0.13f, 0.13f, 0.8f));
	ImGui::BeginGroup();

	// Push width for the controls
	ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());

	// Track if any value is modified
	bool isModified = false;

	isModified |= DrawAxisControl("X", values.x, resetValue, buttonSize, boldFont,
		style.xButtonColor, style.xButtonHovered, style.xButtonActive, style);

	isModified |= DrawAxisControl("Y", values.y, resetValue, buttonSize, boldFont,
		style.yButtonColor, style.yButtonHovered, style.yButtonActive, style);

	isModified |= DrawAxisControl("Z", values.z, resetValue, buttonSize, boldFont,
		style.zButtonColor, style.zButtonHovered, style.zButtonActive, style);

	ImGui::EndGroup();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar(2);

	ImGui::Columns(1);
	ImGui::PopID();

	return isModified;
}
#pragma endregion

static bool DrawVec2Control(const std::string& label, Vec2& values,
	float resetValue = 0.0f, float columnWidth = 80.0f,
	const Vec3ControlStyle& style = Vec3ControlStyle())
{
	bool retVal = false;
	ImGuiIO& io = ImGui::GetIO();
	auto boldFont = io.Fonts->Fonts[0];

	ImGui::PushID(label.c_str());

	// Setup layout with some padding
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ style.spacing * 2, style.spacing });
	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, columnWidth);

	// Label styling
	ImGui::PushStyleColor(ImGuiCol_Text, style.labelColor);
	ImGui::Dummy(ImVec2(0.f, 2.f));
	ImGui::Text(label.c_str());
	ImGui::PopStyleColor();

	ImGui::NextColumn();

	// Calculate sizes with padding
	float lineHeight = GImGui->Font->FontSize + style.buttonPadding * 2;
	ImVec2 buttonSize = { lineHeight, lineHeight };

	// Container frame styling
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ style.spacing, style.spacing });
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.13f, 0.13f, 0.13f, 0.8f));
	ImGui::BeginGroup();

	// Push width for the controls
	ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());

	// Draw X, Y, Z controls
	if (DrawAxisControl("X", values.x, resetValue, buttonSize, boldFont,
		style.xButtonColor, style.xButtonHovered, style.xButtonActive, style)) {
		retVal = true;
	}

	if (DrawAxisControl("Y", values.y, resetValue, buttonSize, boldFont,
		style.yButtonColor, style.yButtonHovered, style.yButtonActive, style)) {
		retVal = true;
	}

	ImGui::EndGroup();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar(2);

	ImGui::Columns(1);
	ImGui::PopID();

	return retVal;
}

//char inputBuffer[256] = ""; //to be changed to entity name
std::string input;
static bool DrawTextName(std::string& enttName) {
	// Text box for input
	if (ImGui::InputText("##text", &input, ImGuiInputTextFlags_EnterReturnsTrue)) {
		enttName = input;
		return true;
		// This will execute when Enter is pressed while the text box is focused
		//input.clear();  // Clear the buffer after execution
	}
	return false;
	// Button to submit input
	//ImGui::SameLine();
	//if (ImGui::Button("Set")) {
	//	input.clear();  // Clear the buffer after execution
	//}

	//ImGui::Text(inputBuffer); //to check inputbuffer
}

void FieldInternalTypesBehavior(uint32_t* selectedEntity, bool scriptClassExists) {

	auto& sComponent = ECSManager::GetInstance().GetComponent<ScriptComponent>(*selectedEntity);

	float f{};
	double d{};
	bool b{};
	uint16_t ui16{};
	uint32_t ui32{};
	uint64_t ui64{};
	int16_t i16{};
	int32_t i32{};
	int64_t i64{};
	if (engineState == EngineState::PLAYING || engineState == EngineState::PAUSED) {
		std::shared_ptr<ScriptInstance> sInstance = ScriptEngine::GetEntityScriptInstance(*selectedEntity);
		if (sInstance) {
			const auto& fields = sInstance->GetScriptClass()->GetFields();
			for (const auto& [fieldname, field] : fields) {

					/*float data = sInstance->GetFieldValue<float>(fieldname);
					if (ImGui::DragFloat(fieldname.c_str(), &data)) {
						sInstance->SetFieldValue(fieldname, data);
					}*/
					
				switch (field.Type)
				{
				case ScriptFieldType::None:
					break;
				case ScriptFieldType::Float:
					f = sInstance->GetFieldValue<float>(fieldname);
					if (ImGui::DragFloat(fieldname.c_str(), &f)) {
						sInstance->SetFieldValue(fieldname, f);
					}
					break;
				case ScriptFieldType::Double:
					d = sInstance->GetFieldValue<double>(fieldname);
					if (ImGui::InputDouble(fieldname.c_str(), &d)) {
						sInstance->SetFieldValue(fieldname, d);
					}
					break;
				case ScriptFieldType::Bool:
					b = sInstance->GetFieldValue<bool>(fieldname);
					
					if (DrawBoolControl(fieldname.c_str(),b)) {
	
						sInstance->SetFieldValue(fieldname, b);
					}
					break;
				case ScriptFieldType::Short:
					i16 = sInstance->GetFieldValue<int16_t>(fieldname);
					if (ImGui::DragInt(fieldname.c_str(), (int*)&i16)) {
						sInstance->SetFieldValue(fieldname, i16);
					}
					break;
				case ScriptFieldType::Int:
					i32 = sInstance->GetFieldValue<int32_t>(fieldname);
					if (ImGui::DragInt(fieldname.c_str(), &i32)) {
						sInstance->SetFieldValue(fieldname, i32);
					}
					break;
				case ScriptFieldType::Long:
					i32 = static_cast<int32_t>(sInstance->GetFieldValue<int64_t>(fieldname));
					if (ImGui::DragInt(fieldname.c_str(), &i32)) {
						i64 = (int64_t)i32;
						sInstance->SetFieldValue(fieldname, i64);
					}
					break;
				case ScriptFieldType::UShort:
					ui16 = sInstance->GetFieldValue<uint16_t>(fieldname);
					if (DrawUIntControl(fieldname.c_str(), (unsigned int&)ui16)) {
						sInstance->SetFieldValue(fieldname, ui16);
					}
					break;
				case ScriptFieldType::UInt:
					ui32 = sInstance->GetFieldValue<uint32_t>(fieldname);
					if (DrawUIntControl(fieldname.c_str(), ui32)) {
						sInstance->SetFieldValue(fieldname, ui32);
					}
					break;
				case ScriptFieldType::ULong:
					ui32 = static_cast<uint32_t>(sInstance->GetFieldValue<uint64_t>(fieldname));
					if (DrawUIntControl(fieldname.c_str(), ui32)) {
						ui64 = (uint64_t)ui32;
						sInstance->SetFieldValue(fieldname, ui64);
					}
					break;
				/*case ScriptFieldType::Entity:
					ui32 = sInstance->GetFieldValue<uint32_t>(fieldname);
					if (DrawUIntControl(fieldname.c_str(), ui32)) {
						uint32_t eID = ScriptGlue::FindEntityID(ui32);
						if (eID != static_cast<uint32_t>(-1)) {
							sInstance->SetFieldValue(fieldname, eID, true);
						}
					}
					break;*/
				}
				
			}
		}
	}
	else {
		if (scriptClassExists) {
			std::shared_ptr<ScriptClass> entityClass = ScriptEngine::GetEntityClass(sComponent.className);
			const auto& fields = entityClass->GetFields();

			auto& entityFields = ScriptEngine::GetScriptFieldMap(*selectedEntity);
			for (const auto& [fieldname, field] : fields) {

				// Fields set in editor
				if (entityFields.find(fieldname) != entityFields.end()) {
					ScriptFieldInstance& scriptField = entityFields.at(fieldname);

					/*if (field.Type == ScriptFieldType::Float) {

						float data = scriptField.GetValue<float>();
						if (ImGui::DragFloat(fieldname.c_str(), &data)) {
							scriptField.SetValue(data);
						}
					}*/

					switch (field.Type)
					{
					case ScriptFieldType::None:
						break;
					case ScriptFieldType::Float:
						f = scriptField.GetValue<float>();
						if (ImGui::DragFloat(fieldname.c_str(), &f)) {
							scriptField.SetValue(f);
						}
						break;
					case ScriptFieldType::Double:
						d = scriptField.GetValue<double>();
						if (ImGui::InputDouble(fieldname.c_str(), &d)) {
							scriptField.SetValue(d);
						}
						break;
					case ScriptFieldType::Bool:
						b = scriptField.GetValue<bool>();

						if (DrawBoolControl(fieldname.c_str(), b)) {
							scriptField.SetValue(b);
						}
						break;
					case ScriptFieldType::Short:
						i16 = scriptField.GetValue<int16_t>();;
						if (ImGui::DragInt(fieldname.c_str(), (int*)&i16)) {
							scriptField.SetValue(i16);
						}
						break;
					case ScriptFieldType::Int:
						i32 = scriptField.GetValue<int32_t>();
						if (ImGui::DragInt(fieldname.c_str(), &i32)) {
							scriptField.SetValue(i32);
						}
						break;
					case ScriptFieldType::Long:
						i64 = scriptField.GetValue<int64_t>();
						i32 = static_cast<int32_t>(i64);
						if (ImGui::DragInt(fieldname.c_str(), &i32)) {
							i64 = static_cast<int64_t>(i32);
							scriptField.SetValue(i64);
						}
						break;
					case ScriptFieldType::UShort:
						ui16 = scriptField.GetValue<uint16_t>();
						if (DrawUIntControl(fieldname.c_str(), (unsigned int&)ui16)) {
							scriptField.SetValue(ui16);
						}
						break;
					case ScriptFieldType::UInt:
						ui32 = scriptField.GetValue<uint32_t>();
						if (DrawUIntControl(fieldname.c_str(), ui32)) {
							scriptField.SetValue(ui32);
						}
						break;
					case ScriptFieldType::ULong:
						ui64 = scriptField.GetValue<uint64_t>();
						ui32 = static_cast<uint32_t>(ui64);
						if (DrawUIntControl(fieldname.c_str(), ui32)) {
							ui64 = static_cast<uint64_t>(ui32);
							scriptField.SetValue(ui64);
						}
						break;
					/*case ScriptFieldType::Entity:
						ui32 = scriptField.GetValue<uint32_t>();
						if (DrawUIntControl(fieldname.c_str(), ui32)) {
							uint32_t eID = ScriptGlue::FindEntityID(ui32);
							if (eID != static_cast<uint32_t>(-1)) {
								scriptField.SetValue(eID);
							}
							
						}
						break;*/
					}
				}
				else {
					//if (field.Type == ScriptFieldType::Float) {

						/*float data = 0.f;
						if (ImGui::DragFloat(fieldname.c_str(), &data)) {
							ScriptFieldInstance& scriptField = entityFields[fieldname];
							scriptField.Field = field;
							scriptField.SetValue(data);
						}*/
					//}
					switch (field.Type)
					{
					case ScriptFieldType::None:
						break;
					case ScriptFieldType::Float:
						f = 0;
						if (ImGui::DragFloat(fieldname.c_str(), &f)) {
							ScriptFieldInstance& scriptField = entityFields[fieldname];
							scriptField.Field = field;
							scriptField.SetValue(f);
						}
						break;
					case ScriptFieldType::Double:
						d = 0;
						if (ImGui::InputDouble(fieldname.c_str(), &d)) {
							ScriptFieldInstance& scriptField = entityFields[fieldname];
							scriptField.Field = field;
							scriptField.SetValue(d);
						}
						break;
					case ScriptFieldType::Bool:
						b = false;
						if (DrawBoolControl(fieldname.c_str(), b)) {
							ScriptFieldInstance& scriptField = entityFields[fieldname];
							scriptField.Field = field;
							scriptField.SetValue(b);
						}
						break;
					case ScriptFieldType::Short:
						i16 = 0;
						if (ImGui::DragInt(fieldname.c_str(), (int*)&i16)) {
							ScriptFieldInstance& scriptField = entityFields[fieldname];
							scriptField.Field = field;
							scriptField.SetValue(i16);
						}
						break;
					case ScriptFieldType::Int:
						i32 = 0;
						if (ImGui::DragInt(fieldname.c_str(), &i32)) {
							ScriptFieldInstance& scriptField = entityFields[fieldname];
							scriptField.Field = field;
							scriptField.SetValue(i32);
						}
						break;
					case ScriptFieldType::Long:
						i64 = 0;
						i32 = static_cast<int32_t>(i64);
						if (ImGui::DragInt(fieldname.c_str(), &i32)) {
							i64 = static_cast<int64_t>(i32);
							ScriptFieldInstance& scriptField = entityFields[fieldname];
							scriptField.Field = field;
							scriptField.SetValue(i64);
						}
						break;
					case ScriptFieldType::UShort:
						ui16 = 0;
						if (DrawUIntControl(fieldname.c_str(), (unsigned int&)ui16)) {
							ScriptFieldInstance& scriptField = entityFields[fieldname];
							scriptField.Field = field;
							scriptField.SetValue(ui16);
						}
						break;
					case ScriptFieldType::UInt:
						ui32 = 0;
						if (DrawUIntControl(fieldname.c_str(), ui32)) {
							ScriptFieldInstance& scriptField = entityFields[fieldname];
							scriptField.Field = field;
							scriptField.SetValue(ui32);
						}
						break;
					case ScriptFieldType::ULong:
						ui64 = 0;
						ui32 = static_cast<uint32_t>(ui64);
						if (DrawUIntControl(fieldname.c_str(), ui32)) {
							ui64 = static_cast<uint64_t>(ui32);
							ScriptFieldInstance& scriptField = entityFields[fieldname];
							scriptField.Field = field;
							scriptField.SetValue(ui64);
						}
						break;
					/*case ScriptFieldType::Entity:
						ui32 = 0;						
						if (DrawUIntControl(fieldname.c_str(), ui32)) {
							ScriptFieldInstance& scriptField = entityFields[fieldname];
							scriptField.Field = field;
							uint32_t eID = ScriptGlue::FindEntityID(ui32);
							if (eID != static_cast<uint32_t>(-1)) {
								scriptField.SetValue(eID);
							}
						}
						break;		*/			
					}
				}

			}
		}
	}

	

	
	/*if (field.Type == ScriptFieldType::Float) {

		float data = scriptField.GetValue<float>();
		if (ImGui::DragFloat(fieldname.c_str(), &data)) {
			scriptField.SetValue(data);
		}
	}*/
}

void ObjectEditorPanel::SetInactive(std::list<Gui::Entity*>& children, bool isActive) {
	for (auto& child : children) {
		ECSManager::GetInstance().GetEntityManager().SetActive(child->id, isActive);
		ECSManager::GetInstance().renderSystem->SetVisibility(child->id, isActive);
		if (child->children.size() > 0) {
			SetInactive(child->children, isActive);
		}
	}
}

void ObjectEditorPanel::DrawComponents() { //need to parse in entity object eventually
	auto& ecsManager = ECSManager::GetInstance();
	auto& enttName = ecsManager.GetComponent<Name>(selectedEntity->id);

	auto& layerManager = LayerManager::GetInstance();
	bool isActive = ecsManager.GetEntityManager().GetActive(selectedEntity->id);
	if (ImGui::Checkbox("##", &isActive)) {
		ecsManager.GetEntityManager().SetActive(selectedEntity->id, isActive);
		ecsManager.renderSystem->SetVisibility(selectedEntity->id, isActive);
		if (selectedEntity->children.size() > 0) {
			SetInactive(selectedEntity->children, isActive);
		}
	}
	ImGui::SameLine();
	input = enttName.name;
	if (DrawTextName(enttName.name)) {
		sceneEntityMap[selectedEntity->id]->name = enttName.name;
	}

	float availableWidth = (ImGui::GetContentRegionAvail().x - 80.f) / 2;
	ImGui::PushItemWidth(availableWidth);
	if (ImGui::BeginCombo("Tag", "None")) {
		for (auto i = 0; i < 1; ++i) {
			//bool isSelected = (selectedTag == static_cast<int>(i));
			//bool isSelected = false;
			if (ImGui::Selectable("Hello")) {
				//selectedTag = static_cast<int>(i); // Update selected tag
			}

			// Set the initial focus when opening the combo
			//if (isSelected)
			//	ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	ImGui::PopItemWidth();

	ImGui::PushItemWidth(availableWidth);
	ImGui::SameLine();

	Layer entityLayer = ecsManager.GetEntityManager().GetLayer(selectedEntity->id);
	std::string preview = entityLayer == MAX_LAYERS ? "None" : layerManager.layerNames[entityLayer];
	if (ImGui::BeginCombo("Layer", preview.c_str())) {
		for (Layer i = 0; i < MAX_LAYERS; ++i) {
			bool isSelected = ecsManager.GetEntityManager().GetLayer(selectedEntity->id) == i;
			if (ImGui::Selectable(layerManager.layerNames[i].c_str(), isSelected)) {
				ecsManager.GetEntityManager().SetLayer(selectedEntity->id, i);
			}

			// Set the initial focus when opening the combo
			//if (isSelected)
			//	ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	ImGui::PopItemWidth();

	ImGui::Separator();

	if (enttName.prefabID != "") {
		ImGui::Text("Prefab ID: ");
		ImGui::SameLine();
		ImGui::Text(enttName.prefabID.c_str());

		if (ImGui::Button("Update Prefab")) {
			PrefabManager::GetInstance().UpdatePrefab(enttName.prefabID, selectedEntity->id);
			Serializer::GetInstance().SerializePrefab(enttName.prefabPath, selectedEntity->id);
		}
		ImGui::SameLine();
		if (ImGui::Button("Unlink Prefab")) {
			PrefabManager::GetInstance().UnlinkPrefab(enttName.prefabID, selectedEntity->id);
			enttName.prefabID = "";
			enttName.prefabPath = "";
			//EditorPanel::sceneEntities[selectedEntity->id].isPrefab = false;
		}
		ImGui::Separator();
	}

	auto& transform = ecsManager.GetComponent<Transform>(selectedEntity->id);
	
	if (ImGui::TreeNodeEx("Transformation", ImGuiTreeNodeFlags_DefaultOpen)) {
		// Debug
		ImGui::Text("Transform UUID: ");
		ImGui::SameLine();
		ImGui::Text(std::to_string(transform.uuid).c_str());
		ImGui::Text("Parent UUID: ");
		ImGui::SameLine();
		ImGui::Text(std::to_string(transform.parentUUID).c_str());

		Vec3 position = transform.position;
		Vec3 scale = transform.scale;
		Vec3 rotation = transform.rotation;

		if (transform.parent == MAX_ENTITIES) {
			if (DrawVec3Control("Position", position)) 
				ECSManager::GetInstance().transformSystem->SetPosition(selectedEntity->id, position);
			if (DrawVec3Control("Scale", scale, 1.0f)) 
				ECSManager::GetInstance().transformSystem->SetScale(selectedEntity->id, scale);
			if (DrawVec3Control("Rotation", rotation))
				ECSManager::GetInstance().transformSystem->SetRotation(selectedEntity->id, rotation);
		} else {
			if (DrawVec3Control("Position", transform.localPosition)) transform.updated = true;
			if (DrawVec3Control("Scale", transform.localScale, 1.0f)) transform.updated = true;
			if (DrawVec3Control("Rotation", transform.localRotation)) transform.updated = true;
		}
		
		ImGui::TreePop();
	}

	ImGui::Separator();

	if (ecsManager.TryGetComponent<Renderer>(selectedEntity->id) != std::nullopt) {
		if (ImGui::TreeNodeEx("Renderer", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::SameLine(ImGui::GetContentRegionAvail().x - 20);
			if (ImGui::Button("...")) {
				ImGui::OpenPopup("Options");
			}

			auto& renderer = ecsManager.GetComponent<Renderer>(selectedEntity->id);
			ImGui::Text("Texture:");
			ImGui::SameLine();

			// Display the current texture or a placeholder
			//auto* texture = GraphicsManager::GetInstance().GetTextureByUUID(renderer.textureUUID);
			//if (texture) {
			//	//ImGui::Image((void*)(intptr_t)texture->id, ImVec2(64, 64));
			//	ImGui::Text(renderer.uuid);
			//} else {
			//	ImGui::Text("<None>");
			//}
			if (renderer.uuid != "")
				ImGui::Text(renderer.uuid.c_str());
			else
				ImGui::Text("<None>");



			// Make this a drop target
			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TEXTURE_UUID")) {
					// Payload contains the UUID of the dropped texture
					std::string droppedUUID(static_cast<const char*>(payload->Data), payload->DataSize);

					// Update the renderer component with the new texture UUID
					renderer.uuid = droppedUUID;
					GraphicsManager::GetInstance().SetTextureToMesh(
						renderer.currentMeshID,
						static_cast<int>(AssetManager::GetInstance().Get<Texture>(renderer.uuid)->texArrayIndex),
						static_cast<int>(AssetManager::GetInstance().Get<Texture>(renderer.uuid)->texLayerIndex)
					);

					//Logger::Instance().Log(Logger::Level::INFO, "Texture assigned: " + droppedUUID);
				}
				ImGui::EndDragDropTarget();
			}

			if (DrawBoolControl("Is Animated", renderer.isAnimated)) renderer.isDirty = true;

			// Dropdown menu for sorting layer.
			SortingLayer sLayer = renderer.sortingLayer;
			std::string sLayerStr = SortingLayerManager::GetInstance().srtgLayerNames[sLayer];
			if (ImGui::BeginCombo("Sorting Layer", sLayerStr.c_str())) {
				for (SortingLayer i = 0; i < MAX_SORTING_LAYERS; ++i) {
					bool isSelected = sLayer == i;
					if (ImGui::Selectable(SortingLayerManager::GetInstance().srtgLayerNames[i].c_str(), isSelected)) {
						// Change the renderer's sorting layer upon selection.
						renderer.sortingLayerChanged = true;
						renderer.prevSortingLayer = renderer.sortingLayer;
						renderer.sortingLayer = i;
					}

					// Set the initial focus when opening the combo
					//if (isSelected)
					//	ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			if (ImGui::BeginPopup("Options")) {
				if (ImGui::MenuItem("Delete Component")) {
					ecsManager.renderSystem->SetVisibility(renderer.currentMeshID, false);
					ecsManager.RemoveComponent<Renderer>(selectedEntity->id);

					for (auto& batch : GraphicsManager::GetInstance().batches) {
						batch.UpdateBuffers();
					}
				}
				ImGui::EndPopup();
			}
			ImGui::TreePop();
		}
		ImGui::Separator();
	}

	if (ecsManager.TryGetComponent<Rigidbody2D>(selectedEntity->id) != std::nullopt) {
		if (ImGui::TreeNodeEx("Rigidbody2D", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::SameLine(ImGui::GetContentRegionAvail().x - 20);
			if (ImGui::Button("...")) {
				ImGui::OpenPopup("Options");
			}

			auto& rb = ecsManager.GetComponent<Rigidbody2D>(selectedEntity->id);
			DrawFloatControl("Mass", rb.mass);
			//DrawFloatControl("Inverse Mass", rb.inverseMass);
			DrawFloatControl("Drag", rb.drag);
			DrawFloatControl("Gravity", rb.gravityScale);
			DrawBoolControl("Is Static", rb.isStatic);
			DrawBoolControl("Is Kinematic", rb.isKinematic);
			//DrawBoolControl("Is Grounded", rb.isGrounded);
			DrawVec2Control("RB Position", rb.position);
			DrawVec2Control("Velocity", rb.velocity);

			if (ImGui::BeginPopup("Options")) {
				if (ImGui::MenuItem("Delete Component")) {
					ecsManager.RemoveComponent<Rigidbody2D>(selectedEntity->id);
					//ecsManager.physicsSystem->RemoveRBFromMap(selectedEntity->id);
				}
				ImGui::EndPopup();
			}
			ImGui::TreePop();
		}
		ImGui::Separator();
	}

	if (ecsManager.TryGetComponent<AABBCollider2D>(selectedEntity->id) != std::nullopt) {
		if (ImGui::TreeNodeEx("AABBCollider2D", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::SameLine(ImGui::GetContentRegionAvail().x - 20);
			if (ImGui::Button("...")) {
				ImGui::OpenPopup("Options");
			}

			auto& col = ecsManager.GetComponent<AABBCollider2D>(selectedEntity->id);
			DrawFloatControl("Bounciness", col.bounciness);
			DrawVec2Control("Collider Min", col.min);
			DrawVec2Control("Collider Max", col.max);
			DrawBoolControl("Is Trigger", col.isTrigger);

			if (ImGui::BeginPopup("Options")) {
				if (ImGui::MenuItem("Delete Component")) {
					ecsManager.RemoveComponent<AABBCollider2D>(selectedEntity->id);
					//ecsManager.physicsSystem->RemoveAABBFromMap(selectedEntity->id);
				}
				ImGui::EndPopup();
			}
			ImGui::TreePop();
		}
		ImGui::Separator();
	}

	if (ecsManager.TryGetComponent<Animation>(selectedEntity->id) != std::nullopt) {
		if (ImGui::TreeNodeEx("Animation", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::SameLine(ImGui::GetContentRegionAvail().x - 20);
			if (ImGui::Button("...")) {
				ImGui::OpenPopup("Options");
			}

			auto& anim = ecsManager.GetComponent<Animation>(selectedEntity->id);

			if (DrawUIntControl("Sprites Per Row", anim.spritesPerRow)) {
				anim.spriteWidth = 1.0f / anim.spritesPerRow;
			}
			if (DrawUIntControl("Sprites Per Col", anim.spritesPerCol)) {
				anim.spriteHeight = 1.0f / anim.spritesPerCol;
			}
			DrawUIntControl("Number of Frames", anim.numFrames);

			DrawUIntControl("Start Frame", anim.startFrame);
			DrawUIntControl("End Frame", anim.endFrame);

			DrawUIntControl("Current Frame", anim.currentFrame);

			ImGui::InputDouble("Time Per Frame", &anim.timePerFrame);

			DrawBoolControl("Play Once", anim.playOnce);
			DrawBoolControl("Is Looping", anim.isLooping);

			if (ImGui::BeginPopup("Options")) {
				if (ImGui::MenuItem("Delete Component")) {
					ecsManager.RemoveComponent<Animation>(selectedEntity->id);
				}
				ImGui::EndPopup();
			}
			ImGui::TreePop();
		}
		ImGui::Separator();
	}

	if (ecsManager.TryGetComponent<AudioSource>(selectedEntity->id) != std::nullopt) {
		if (ImGui::TreeNodeEx("AudioSource", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::SameLine(ImGui::GetContentRegionAvail().x - 20);
			if (ImGui::Button("...")) {
				ImGui::OpenPopup("Options");
			}

			auto& audioSource = ecsManager.GetComponent<AudioSource>(selectedEntity->id);
			ImGui::Text("Audio Clip:");
			ImGui::SameLine();

			if (audioSource.audioClipUUID != "")
				ImGui::Text(audioSource.audioClipUUID.c_str());
			else
				ImGui::Text("<None>");

			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("AUDIO_UUID")) {
					std::string droppedUUID(static_cast<const char*>(payload->Data), payload->DataSize);

					audioSource.audioClipUUID = droppedUUID;
				}
				ImGui::EndDragDropTarget();
			}

			DrawBoolControl("Is Playing", audioSource.isPlaying);
			DrawBoolControl("Is Looping", audioSource.isLooping);

			bool stopAudio;
			if (ImGui::Checkbox("Stop Audio", &stopAudio)) {
				AudioManager::GetInstance().StopClip(audioSource.audioClipUUID);
			}

			if (ImGui::BeginPopup("Options")) {
				if (ImGui::MenuItem("Delete Component")) {
					ecsManager.RemoveComponent<AudioSource>(selectedEntity->id);
				}
				ImGui::EndPopup();
			}
			ImGui::TreePop();

		}
		ImGui::Separator();
	}


	if (ecsManager.TryGetComponent<UI>(selectedEntity->id) != std::nullopt) {
		if (ImGui::TreeNodeEx("UI", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::SameLine(ImGui::GetContentRegionAvail().x - 20);

			auto& ui = ecsManager.GetComponent<UI>(selectedEntity->id);

			if (DrawVec3Control("Position", ui.position))
				ui.isUpdated = false;
			if (DrawFloatControl("Rotation", ui.rotation))
				ui.isUpdated = false;
			if (DrawVec2Control("Size", ui.size))
				ui.isUpdated = false;

			Vec2 scale = ui.scale;
			if (DrawVec2Control("Scale", scale)) {
				ui.isUpdated = false;
				auto textbox = ecsManager.TryGetComponent<Textbox>(selectedEntity->id);
				if (textbox.has_value()) {
					if (scale.x != ui.scale.x) {
						ui.scale.x = scale.x;
						ui.scale.y = scale.x;
					}
					else if (scale.y != ui.scale.y) {
						ui.scale.x = scale.y;
						ui.scale.y = scale.y;
					}
				}
				else {
					ui.scale = scale;
				}
			}
				

			ImGui::TreePop();

		}
		ImGui::Separator();
	}

	if (ecsManager.TryGetComponent<Textbox>(selectedEntity->id) != std::nullopt) {
		if (ImGui::TreeNodeEx("Textbox", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::SameLine(ImGui::GetContentRegionAvail().x - 20);
			//if (ImGui::Button("...")) {
			//	ImGui::OpenPopup("Options");
			//}

			auto& tb = ecsManager.GetComponent<Textbox>(selectedEntity->id);
			auto& ui = ecsManager.GetComponent<UI>(selectedEntity->id);

			input = tb.text;
			if (DrawTextName(input)) {
				tb.text = input;
				ui.isUpdated = false;
			}

			if (DrawVec3Control("Text Colour", tb.color)) {
				tb.color.x = std::clamp(tb.color.x, 0.0f, 1.0f);
				tb.color.y = std::clamp(tb.color.y, 0.0f, 1.0f);
				tb.color.z = std::clamp(tb.color.z, 0.0f, 1.0f);
				ui.isUpdated = false;
			}

			if (DrawBoolControl("Center Aligned", tb.centerAligned)) {
				ui.isUpdated = false;
			}

			//if (ImGui::BeginPopup("Options")) {
			//	if (ImGui::MenuItem("Delete Component")) {
			//		ecsManager.RemoveComponent<UI>(selectedEntity->id);
			//	}
			//	ImGui::EndPopup();
			//}
			ImGui::TreePop();

		}
		ImGui::Separator();
	}

	if (ecsManager.TryGetComponent<VideoPlayer>(selectedEntity->id) != std::nullopt) {
		if (ImGui::TreeNodeEx("Video Player", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::SameLine(ImGui::GetContentRegionAvail().x - 20);
			if (ImGui::Button("...")) {
				ImGui::OpenPopup("Options");
			}

			auto& videoPlayer = ecsManager.GetComponent<VideoPlayer>(selectedEntity->id);

			if (videoPlayer.videoClipUUID != "")
				ImGui::Text(videoPlayer.videoClipUUID.c_str());
			else
				ImGui::Text("<None>");

			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("VIDEO_UUID")) {
					std::string droppedUUID(static_cast<const char*>(payload->Data), payload->DataSize);

					videoPlayer.videoClipUUID = droppedUUID;
					videoPlayer.videoClip = *AssetManager::GetInstance().Get<VideoClip>(videoPlayer.videoClipUUID);
				}
				ImGui::EndDragDropTarget();
			}

			if (ImGui::DragScalar("Current Frame", ImGuiDataType_U32, &videoPlayer.currentFrame, 1.0f, nullptr, nullptr)) {
				//return true;
			}
			DrawBoolControl("Is Playing", videoPlayer.isPlaying);
			DrawBoolControl("Play On Awake", videoPlayer.playOnAwake);
			DrawBoolControl("Is Looping", videoPlayer.isLooping);

			if (ImGui::BeginPopup("Options")) {
				if (ImGui::MenuItem("Delete Component")) {
					ecsManager.RemoveComponent<VideoPlayer>(selectedEntity->id);
				}
				ImGui::EndPopup();
			}
			ImGui::TreePop();

		}
		ImGui::Separator();
	}

	if (ecsManager.TryGetComponent<Camera>(selectedEntity->id)) {
		if (ImGui::TreeNodeEx("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::SameLine(ImGui::GetContentRegionAvail().x - 20);
			if (ImGui::Button("...")) {
				ImGui::OpenPopup("Options");
			}

			auto& camera = ecsManager.GetComponent<Camera>(selectedEntity->id);

			if (DrawFloatControl("Zoom", camera.zoom)) camera.hasUpdatedView = true;
			if (DrawFloatControl("Width", camera.width)) camera.hasUpdatedView = true;
			if (DrawFloatControl("Height", camera.height)) camera.hasUpdatedView = true;

			DrawBoolControl("Is Main Camera", camera.isMainCamera);
			DrawBoolControl("Play On Awake", camera.isActive);
			DrawBoolControl("Is Active", camera.isActive);

			DrawFloatControl("Bloom Intensity", camera.bloomIntensity);

			DrawFloatControl("Vignette Strength", camera.vignetteStrength);
			DrawFloatControl("Vignette Softness", camera.vignetteSoftness);
			DrawVec2Control("Vignette Center", camera.vignetteCenter);

			if (ImGui::BeginPopup("Options")) {
				if (ImGui::MenuItem("Delete Component")) {
					ecsManager.RemoveComponent<Camera>(selectedEntity->id);
				}
				ImGui::EndPopup();
			}
			ImGui::TreePop();

		}
		ImGui::Separator();
	}

	if (ecsManager.HasComponent<StateMachineComponent>(selectedEntity->id)) {
		if (ImGui::TreeNodeEx("State Machine", ImGuiTreeNodeFlags_DefaultOpen)) {
			auto& stateMachineComponent = ecsManager.GetComponent<StateMachineComponent>(selectedEntity->id);

			// Display the current state
			auto currentStateName = stateMachineComponent.stateMachine->GetCurrentStateName();
			ImGui::Text("Current State: %s", currentStateName.empty() ? "None" : currentStateName.c_str());


			// Fetch available states from the registry
			auto& stateRegistry = StateFactory::GetRegistry();
			static std::vector<std::string> stateNames;
			stateNames.clear();
			for (const auto& [stateName, _] : stateRegistry) {
				stateNames.push_back(stateName);
			}

			static int selectedStateIndex = 0;


			// Dropdown for available states
			ImGui::Text("Add State:");
			if (ImGui::Combo("Available States", &selectedStateIndex,
				[](void* data, int idx, const char** out_text) {
					auto& names = *static_cast<std::vector<std::string>*>(data);
					*out_text = names[idx].c_str();
					return true;
				},
				&stateNames, static_cast<int>(stateNames.size()))) {
				// Do nothing on selection
			}

			if (ImGui::Button("Add State")) {
				auto& selectedStateName = stateNames[selectedStateIndex];
				auto stateFactory = stateRegistry[selectedStateName];
				if (stateFactory) {
					auto newState = stateFactory();
					stateMachineComponent.stateMachine->AddState(newState);
				}
			}

			ImGui::Separator();

			// Display existing states
			auto& states = stateMachineComponent.stateMachine->GetStates();
			for (const auto& [stateName, state] : states) {
				if (ImGui::TreeNode(stateName.c_str())) {
					ImGui::Text("State Name: %s", stateName.c_str());
					ImGui::TreePop();
				}
			}

			ImGui::TreePop(); // Close the State Machine tree node
		}
		ImGui::Separator();
	}
	if (ecsManager.HasComponent<ScriptComponent>(selectedEntity->id)) {
		if (ImGui::TreeNodeEx("Script", ImGuiTreeNodeFlags_DefaultOpen)) {
			auto& sComponent = ecsManager.GetComponent<ScriptComponent>(selectedEntity->id);
			static int selectedScriptIndex = -1; // Index of the selected script
			static std::vector<std::string> availableScripts = ScriptEngine::GetAvailableScripts(); // Fetch available scripts once
			static std::vector<const char*> scriptOptions; // List of script options as const char* for ImGui

			//Check for available scripts that are not emptyString
			if (sComponent.className != "") {
				for (size_t i = 0; i < availableScripts.size(); ++i) {
					if (availableScripts[i] == sComponent.className) {
						selectedScriptIndex = static_cast<int>(i);
						break;
					}
				}
			}

			// Populate scriptOptions if it's empty
			if (scriptOptions.empty()) {
				for (const auto& script : availableScripts) {
					scriptOptions.push_back(script.c_str());
				}
			}

			// Dropdown box for script selection
			if (ImGui::Combo("Class", &selectedScriptIndex, scriptOptions.data(), static_cast<int>(scriptOptions.size()))) {
				if (selectedScriptIndex >= 0 && selectedScriptIndex < static_cast<int>(availableScripts.size())) {
					sComponent.className = availableScripts[selectedScriptIndex];
				}
			}

			bool scriptClassExists = ScriptEngine::EntityClassExists(sComponent.className);

			if (!scriptClassExists)
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.2f, 0.3f, 1.0f));

			ImGui::SameLine();
			if (ImGui::Button("Set")) {
				if (scriptClassExists) {
					ScriptEngine::OnCreateEntity(selectedEntity->id);
				}
				else {
					ImGui::TextColored(ImVec4(0.9f, 0.2f, 0.3f, 1.0f), "Cannot set an invalid class!");
				}
			}

			// Fields
			FieldInternalTypesBehavior(&selectedEntity->id, scriptClassExists);

			if (!scriptClassExists)
				ImGui::PopStyleColor();

			ImGui::TreePop();
		}
		ImGui::Separator();
	}
}