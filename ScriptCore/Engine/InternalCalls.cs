/*********************************************************************
 * \file	InternalCalls.cs
 * \brief	Setups the internal calls into the C++ engine functions.
 *
 * \author	wengkhong.thang, 2301372
 * \co-author Wilmerjunrong.lee 2200691
 * \email	wengkhong.thang@digipen.edu
 * \date	08 November 2024

Copyright(C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 *********************************************************************/

using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace ScriptCore
{
    
    public static class InternalCalls
    {
        
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void ChangeScene(string path);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void QuitGame();
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void HideCursor();
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void UnhideCursor();
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static int GetWindowWidth();
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static int GetWindowHeight();
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static double GetFPS();

        #region Entity
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static UInt32 Entity_Create();
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Entity_GetActive(UInt32 entityid);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Entity_SetActive(UInt32 entityid, bool b);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Entity_HasComponent(UInt32 entityID, Type componentType);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Entity_AddComponent(UInt32 entityID, Type componentType);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static UInt32 Entity_FindEntityByName(string name);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static UInt32 Entity_FindEntityByID(UInt32 uuid);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static object GetScriptInstance(UInt32 entityID);
        #endregion

        #region TransformComponent
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_GetTranslation(UInt32 entityID, out Vec3 translation);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_SetTranslation(UInt32 entityID, ref Vec3 translation);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_GetRotation(UInt32 entityID, out Vec3 rotation);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_SetRotation(UInt32 entityID, ref Vec3 rotation);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_GetScale(UInt32 entityID, out Vec3 rotation);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_SetScale(UInt32 entityID, ref Vec3 rotation);
        #endregion

        #region PhysicsSystem
        //[MethodImplAttribute(MethodImplOptions.InternalCall)]
        //internal extern static void PhysicsSystem_ActivateForce(UInt32 entityID, Physics.LinearForces LinearForces);
        //[MethodImplAttribute(MethodImplOptions.InternalCall)]
        //internal extern static void PhysicsSystem_SetRBGrounded(UInt32 entityID, bool b);
        //[MethodImplAttribute(MethodImplOptions.InternalCall)]
        //internal extern static void PhysicsSystem_AddRigidbodyComponent(UInt32 id, Vec2 pos, Vec2 vel, float mass, float drag, float gravityScale, bool isStatic = false, bool isKinematic = false);
        //[MethodImplAttribute(MethodImplOptions.InternalCall)]
        //internal extern static void PhysicsSystem_AddImpulseForce(UInt32 id, Vec2 dir, float mag);
        //[MethodImplAttribute(MethodImplOptions.InternalCall)]
        //internal extern static void PhysicsSystem_AddAABBColliderComponent(UInt32 id, float bounciness, Vec2 min, Vec2 max);
        //[MethodImplAttribute(MethodImplOptions.InternalCall)]
        //internal extern static void PhysicsSystem_AddDragAndGravity(UInt32 entityID);
        #endregion

        #region Rigidbody2DComponent
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Rigidbody2DComponent_GetRBPosition(UInt32 entityID, out Vec2 position);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Rigidbody2DComponent_SetRBPosition(UInt32 entityID, Vec2 pos);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Rigidbody2DComponent_GetRBVelocity(UInt32 entityID, out Vec2 velocity);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Rigidbody2DComponent_SetRBVelocity(UInt32 entityID, Vec2 vel);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float Rigidbody2DComponent_GetRBMass(UInt32 entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float Rigidbody2DComponent_SetRBMass(UInt32 entityID, float newMass);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float Rigidbody2DComponent_GetRBDrag(UInt32 entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Rigidbody2DComponent_SetRBDrag(UInt32 entityID, float drag);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float Rigidbody2DComponent_GetRBGravityScale(UInt32 entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Rigidbody2DComponent_SetGravityScale(UInt32 entityID, float newScale);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Rigidbody2DComponent_IsRBStatic(UInt32 entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Rigidbody2DComponent_SetRBStatic(UInt32 entityID, bool value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Rigidbody2DComponent_IsRBKinematic(UInt32 entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Rigidbody2DComponent_SetRBKinematic(UInt32 entityID, bool value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static ulong Rigidbody2DComponent_AddImpulseForce(UInt32 entityID, Vec2 direction, float magnitude);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static ulong Rigidbody2DComponent_AddForceOverTime(UInt32 entityID, Vec2 direction, float magnitude, float duration);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static ulong Rigidbody2DComponent_AddAlwaysActiveForce(UInt32 entityID, ulong forceID, Vec2 direction, float magnitude);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static ulong Rigidbody2DComponent_AddForeverImpulseForce(UInt32 entityID, Vec2 direction, float magnitude, bool startActive, ulong forceID = ulong.MaxValue);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Rigidbody2DComponent_ActivateForce(UInt32 entityID, ulong forceID, bool activate);

        #endregion

        #region AABBColliderComponent
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float AABBColliderComponent_GetBounciness(UInt32 entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void AABBColliderComponent_SetBounciness(UInt32 entityID, float value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void AABBColliderComponent_GetMin(UInt32 entityID, out Vec2 min);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void AABBColliderComponent_SetMin(UInt32 entityID, Vec2 newMin);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void AABBColliderComponent_GetMax(UInt32 entityID, out Vec2 max);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void AABBColliderComponent_SetMax(UInt32 entityID, Vec2 newMax);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float AABBColliderComponent_GetSizeX(UInt32 entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float AABBColliderComponent_GetSizeY(UInt32 entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool AABBColliderComponent_GetIsTrigger(UInt32 entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void AABBColliderComponent_SetIsTrigger(UInt32 entityID,bool t);

        #endregion

        #region RenderSystem
        //[MethodImplAttribute(MethodImplOptions.InternalCall)]
        //internal extern static (uint,uint) RenderSystem_AddMesh(Render.MeshType mtype, string path = "", IReadOnlyList<Vertex> vertices = null);

        // RenderSystem_SetVisibility
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void RenderSystem_SetVisibility(UInt32 entityID, bool b);

        // RenderSystem_SetDebugMode
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void RenderSystem_SetDebugMode(bool b);

        // RenderSystem_SetTextureToEntity
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void RenderSystem_SetTextureToEntity(UInt32 entityID, string texID);

        // RenderSystem_GetClickedEntity
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static UInt32 RenderSystem_GetClickedEntity(FrameBufferCode fbo);

        #endregion

        #region UISystem

        // UISystem_SetPosition
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void UISystem_SetPosition(UInt32 entityID, Vec3 pos);

        // UISystem_SetScale
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void UISystem_SetScale(UInt32 entityID, Vec2 scale);

        // UISystem_SetSize
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void UISystem_SetSize(UInt32 entityID, Vec2 size);

        // UISystem_SetText
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void UISystem_SetText(UInt32 entityID, string text);

        // UISystem_SetFont
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void UISystem_SetFont(UInt32 entityID, string font);

        // UISystem_SetColor
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void UISystem_SetColor(UInt32 entityID, Vec3 color);

        // UISystem_SetVisibility
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void UISystem_SetVisibility(UInt32 entityID, bool visible);

        // UISystem_SetCenterAlign
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void UISystem_SetCenterAlignment(UInt32 entityID, bool centerAlign);

        #endregion

        #region TransformSystem

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformSystem_SetPosition(UInt32 entityID, Vec3 pos);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformSystem_SetRotation(UInt32 entityID, Vec3 rot);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformSystem_SetScale(UInt32 entityID, Vec3 scale);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformSystem_Translate(UInt32 entityID, Vec3 translation);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformSystem_Rotate(UInt32 entityID, Vec3 rotation);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformSystem_Scale(UInt32 entityID, Vec3 scale);

        #endregion

        #region AnimationSystem

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void AnimationSystem_SetAnimation(UInt32 entityID, AniData anim);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void AnimationComponent_GetData(UInt32 entityID, out AniData anim);

        // AnimationSystem_PlayAnimation
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void AnimationSystem_PlayAnimation(UInt32 entityID, bool reset,bool refresh, bool playOnce, bool isLooping);

        // AnimationSystem_PauseAnimation
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void AnimationSystem_PauseAnimation(UInt32 entityID, bool reset);

        #endregion

        #region RendererComponent
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void RendererComponent_SetMeshID(ulong meshID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void RendererComponent_SetMeshDebugID(ulong meshID);
        #endregion

        #region Camera
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Camera_SetPosition(ref Vec3 inVec);
        #endregion

        #region Audio
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void AudioSystem_PlaySound(UInt32 entityID, string path, float vol, bool loop);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void AudioSystem_SpatialSound(UInt32 entityID, string path, Vec3 pos, float vol);
        
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void AudioSystem_StopSound(string path);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void AudioSystem_SetListener(Vec3 pos, Vec3 Vel, Vec3 forward, Vec3 up);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void AudioSystem_UpdateSound(UInt32 entityID, string path, float vol);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool AudioSystem_ClipIsPlaying(string path);
        
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool AudioSystem_SetPanning(UInt32 entityID, string filePath, float volume);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool AudioSystem_PlaySoundWithPanning(UInt32 entityID, string filePath, float volume,bool loop, bool isPanning);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool AudioSystem_DuckAllExcept(UInt32 entityID, string path, float volume);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool AudioSystem_RestoreAllVolumes();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool AudioSystem_SetGlobalVolume(float vol);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool AudioSystem_StopClip(UInt32 entityID, string path);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal static extern void AudioSystem_ResumeAll();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal static extern void AudioSystem_PauseAll();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal static extern void AudioSystem_StopAll();
        #endregion

        #region Input
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Input_IsKeyDown(KeyCode keycode);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Input_IsKeyPressed(KeyCode keycode);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Input_GetMouseX(out float x);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Input_GetMouseY(out float y);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Input_GetMouseDown(int button);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Input_GetMouseHold(int button);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Input_GetGamepadLeftStick(out Vec2 direction);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Input_IsGamepadButtonDown(ButtonCode button);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Input_IsGamepadButtonPressed(ButtonCode button);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Input_IsGamepadConnected();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Input_GetAnyKeyDown();

        #endregion

        #region Logger

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Logger_Log(string message, int level);

        #endregion

        #region StateMachine

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void StateMachineComponent_AddState(UInt32 entityID, string stateName);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void StateMachineComponent_AddTransition(UInt32 entityID, string from, string to);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void StateMachineComponent_TriggerTransition(UInt32 entityID, string targetState);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static string StateMachineComponent_GetCurrentState(UInt32 entityID);

        #endregion

        #region CameraSystem

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CameraSystem_SetZoom(UInt32 entityID, float zoom);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CameraSystem_SetBloom(float bloomIntensity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CameraSystem_SetActive(UInt32 entityID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CameraSystem_Disable(UInt32 entityID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CameraSystem_SetMain(UInt32 entityID);

        #endregion

        #region VideoPlayerComponent
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void VideoPlayerComponent_GetIsPlaying(UInt32 entityID,out bool b);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void VideoPlayerComponent_SetIsPlaying(UInt32 entityID, ref bool b);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void VideoPlayerComponent_PlayVideo(UInt32 entityID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void VideoPlayerComponent_StopVideo(UInt32 entityID);



        #endregion

    }
}
