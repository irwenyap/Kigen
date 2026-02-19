/*********************************************************************
 * \file	ScriptGlue.hpp
 * \brief	Defines a ScriptGlue class that links the C# Engine interface with the C++ Engine functionality.
 *
 * \author	wengkhong.thang, 2301372
 * \email	wengkhong.thang@digipen.edu
 * \date	08 November 2024

Copyright(C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*********************************************************************/
#pragma once
#include <memory>
#include "../../Components/Collider2D.hpp"
#include "../../Components/Rigidbody2D.hpp"
#include "../../Components/Transform.hpp"
#include "../../Components/ScriptComponent.hpp"
#include "../../Components/Name.hpp"
#include "../../Components/Animation.hpp"
#include "../../Components/UI.hpp"
#include "../../Components/Renderer.hpp"
#include "../../Components/VideoPlayer.hpp"
#include "../../Physics/PhysicsSystem.hpp"
#include "../../Graphics/RenderSystem.hpp"
#include "../../Core/Logger.hpp"
#include "../../Components/AudioSource.hpp"
#include "../../Components/Camera.hpp"
#include "../../Components/StateMachineComponent.hpp"
#include "../../Systems/StateMachineSystem.hpp"


class ScriptGlue
{
public:

	/**
	 * @brief Clears the existing component registration and registers all components.
	 *
	 * This function is responsible for clearing the `entityComponentMap` and
	 * re-registering all components defined in the `AllComponents` group.
	 */
	static void RegisterComponents();

	/**
	 * @brief Registers internal C++ functions to be called from C# scripts.
	 *
	 * This function registers various C++ functions as internal calls, making them available
	 * to be invoked from C# scripts running within the Mono runtime. These functions are
	 * typically used to interact with game components, physics systems, input systems, etc.
	 * The registration is done using the `ADD_INTERNAL_CALL` macro which binds the C++ function
	 * to its corresponding C# function signature in the script context.
	 *
	 * These functions can then be called from C# code as if they were regular methods or properties
	 * of C# classes.
	 */
	static void RegisterFunctions();


	static uint32_t FindEntityID(uint32_t transformID);

public:
	static std::shared_ptr<PhysicsSystem> ps;
	static std::shared_ptr<RenderSystem> rs;
	static std::shared_ptr<UISystem> uis;
	static std::shared_ptr<TransformSystem> ts;
	static std::shared_ptr<AnimationSystem> as;
	static std::shared_ptr<AudioSystem> AudioSys;
	static std::shared_ptr<CameraSystem> cs;
	static std::shared_ptr<StateMachineSystem> sms;

	//static std::shared_ptr<GraphicsManager> gm;
};

template<typename... Component>
struct ComponentGroup
{
};

using AllComponents =
ComponentGroup<Transform, ScriptComponent, Rigidbody2D, AABBCollider2D, Animation, UI, Renderer, Camera, VideoPlayer>;
