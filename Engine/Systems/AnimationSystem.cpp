/*********************************************************************
 * \file		AnimationSystem.cpp
 * \brief		System to handle all the animation components
 *
 * \author		y.ziyangirwen, 2301345 (y.ziyangirwen@digipen.edu)
 * \author		t.yongchin, 2301359 (t.yongchin@digipen.edu)
 * \date		17 November 2024
 *
 * \copyright	Copyright(C) 2024 DigiPen Institute of Technology.
 *				Reproduction or disclosure of this file or its
 *              contents without the prior written consent of DigiPen
 *              Institute of Technology is prohibited.
 *********************************************************************/

#include "AnimationSystem.hpp"

#include <string>

#include "Math.hpp"
#include "../Graphics/GraphicsManager.hpp"
#include "../Application.hpp"
#include "../ECS/ECSManager.hpp"

#include "Logger.hpp"
// Components
#include "../Components/Renderer.hpp"
#include "../Components/Animation.hpp"

AnimationSystem::AnimationSystem() { }

AnimationSystem::~AnimationSystem() { }

void AnimationSystem::Init() {
    for (auto const& entity : m_entities) {
        auto& animation = ECSManager::GetInstance().GetComponent<Animation>(entity);
        auto& renderer = ECSManager::GetInstance().GetComponent<Renderer>(entity);


        AssignTexCoordsToMesh(renderer, animation);
        //animation.spriteWidth = 1.0f / animation.spritesPerRow;
        //animation.spriteHeight = 1.0f / animation.spritesPerCol;
    }
}

void AnimationSystem::Update(double dt) {
    //std::cout << dt << std::endl;
    for (auto const& entity : m_entities) {
        if (!ECSManager::GetInstance().GetEntityManager().GetActive(entity)) continue;

        auto& renderer = ECSManager::GetInstance().GetComponent<Renderer>(entity);
        auto& animation = ECSManager::GetInstance().GetComponent<Animation>(entity);
        if (renderer.isAnimated) {
            UpdateAnimation(renderer, animation, dt);
        }
    }
}

void AnimationSystem::Exit() {

}

void AnimationSystem::SetAnimation(Entity entity, Animation const& animation) {
	auto anim = ECSManager::GetInstance().TryGetComponent<Animation>(entity);
    if (anim.has_value()) {
		anim->get() = animation;
	}
    else {
		ECSManager::GetInstance().AddComponent(entity, animation);
	}
}

void AnimationSystem::SetAnimation(Entity entity,
    unsigned sprPerRow, unsigned sprPerCol,
    unsigned numFrames, unsigned startFrame, unsigned endFrame,
    double timePerFrame, bool isLooping) 
{
    Animation animation(sprPerRow, sprPerCol, numFrames, startFrame, endFrame, timePerFrame, isLooping);

    auto anim = ECSManager::GetInstance().TryGetComponent<Animation>(entity);
    if (anim.has_value()) {
		anim->get() = animation;
	}
    else {
		ECSManager::GetInstance().AddComponent(entity, animation);
	}
}

void AnimationSystem::PlayAnimation(Renderer& r, Animation& a, bool reset, bool refresh, bool playOnce, bool isLooping)
{
	r.isAnimated = true;
	if (refresh) {
        a.playOnce = playOnce;
        a.isLooping = isLooping;
	}
    
	if (reset) ResetAnimation(r, a);
}

void AnimationSystem::PlayAnimation(Entity entity, bool reset, bool refresh, bool playOnce, bool isLooping)
{
	auto r = ECSManager::GetInstance().TryGetComponent<Renderer>(entity);
	auto a = ECSManager::GetInstance().TryGetComponent<Animation>(entity);

    if (!r.has_value() || !a.has_value()) {
		Logger::Instance().Log(Logger::Level::ERR, "[AnimationSystem] PlayAnimation: Entity does not have a Renderer or Animation component.");
		return;
	}

	r->get().isAnimated = true;

	if (refresh) {
        a->get().playOnce = playOnce;
        a->get().isLooping = isLooping;
	}
	
    if (reset) ResetAnimation(r->get(), a->get());
}

void AnimationSystem::UpdateAnimation(Renderer& r, Animation& a, double dt)
{
    a.elapsedTime += dt;
    int steps = static_cast<int>(a.elapsedTime / a.timePerFrame);

    if (steps > 0) {
        a.elapsedTime -= steps * a.timePerFrame;
        if (a.playOnce) {
			a.currentFrame += steps;
            if (a.currentFrame > a.endFrame) {
                a.currentFrame = a.endFrame;
                a.playOnce = false;
				//a.isLooping = false;
            }
        }
        else if (a.isLooping) {
			// Starting frame + (Total frames moved % Range of frames)
            a.currentFrame = a.startFrame + 
                ((a.currentFrame + steps - a.startFrame) % (a.endFrame - a.startFrame + 1));
        }
        AssignTexCoordsToMesh(r, a);
    }

    /*
    if (a.playOnce) {
        a.elapsedTime += dt;
        if (a.elapsedTime >= a.timePerFrame) {
            a.elapsedTime -= a.timePerFrame;
            ++a.currentFrame;
            if (a.currentFrame > a.endFrame) {
                a.currentFrame = a.startFrame;
            }
        }
        AssignTexCoordsToMesh(r, a);

        if (a.currentFrame == a.endFrame) {
            a.currentFrame = a.startFrame;
            a.playOnce = false;
        }
    }
    else if (a.isLooping) {
        a.elapsedTime += dt;
        if (a.elapsedTime >= a.timePerFrame) {
            a.elapsedTime -= a.timePerFrame;
            ++a.currentFrame;
            if (a.currentFrame > a.endFrame) {
                a.currentFrame = a.startFrame;
            }
        }
        AssignTexCoordsToMesh(r, a);
    }    
    */
}

void AnimationSystem::PauseAnimation(Renderer& r, Animation& a, bool reset)
{
    r.isAnimated = false;
    if (reset) ResetAnimation(r, a);
}

void AnimationSystem::PauseAnimation(Entity entity, bool reset)
{
	auto r = ECSManager::GetInstance().TryGetComponent<Renderer>(entity);
	auto a = ECSManager::GetInstance().TryGetComponent<Animation>(entity);

    if (!r.has_value() || !a.has_value()) {
		Logger::Instance().Log(Logger::Level::ERR, "[AnimationSystem] StopAnimation: Entity does not have a Renderer or Animation component.");
		return;
	}

    r->get().isAnimated = false;
    if (reset) ResetAnimation(r->get(), a->get());
}

void AnimationSystem::ResetAnimation(Renderer& r, Animation& a)
{
	a.currentFrame = a.startFrame;
	a.elapsedTime = 0.0f;

	AssignTexCoordsToMesh(r, a);
}

void AnimationSystem::AssignTexCoordsToMesh(Renderer& r, Animation& a)
{
    unsigned spriteRow = a.currentFrame / a.spritesPerRow;
    unsigned spriteColumn = a.currentFrame % a.spritesPerRow;

    float texLeft = spriteColumn * a.spriteWidth;
    float texRight = texLeft + a.spriteWidth;
    float texTop = 1.f - spriteRow * a.spriteHeight;
    float texBottom = texTop - a.spriteHeight;

    GraphicsManager::GetInstance().meshes[r.currentMeshID].vertices[3].texCoord = Vec2(texLeft, texBottom);
    GraphicsManager::GetInstance().meshes[r.currentMeshID].vertices[2].texCoord = Vec2(texRight, texBottom);
    GraphicsManager::GetInstance().meshes[r.currentMeshID].vertices[1].texCoord = Vec2(texRight, texTop);
    GraphicsManager::GetInstance().meshes[r.currentMeshID].vertices[0].texCoord = Vec2(texLeft, texTop);

    GraphicsManager::GetInstance().SetBatchUpdateFlag(r.currentMeshID);
}
