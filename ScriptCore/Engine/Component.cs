/*********************************************************************
 * \file	Component.cs
 * \brief	Defines a Component class that setup the components for entity in C#.
 *
 * \author	wengkhong.thang, 2301372
 * \email	wengkhong.thang@digipen.edu
 * \date	08 November 2024

Copyright(C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 *********************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ScriptCore
{
    public abstract class Component
    {
        public Entity Entity { get; internal set; }
    }

    public class Transform: Component
    {
        public Vec3 Translation
        {
            get
            {
                InternalCalls.TransformComponent_GetTranslation(Entity.ID, out Vec3 translation);
                return translation;
            }
            set
            {
                InternalCalls.TransformComponent_SetTranslation(Entity.ID, ref value);
            }
        }

        public Vec3 Rotation
        {
            get
            {
                InternalCalls.TransformComponent_GetRotation(Entity.ID, out Vec3 rotation);
                return rotation;
            }
            set
            {
                InternalCalls.TransformComponent_SetRotation(Entity.ID, ref value);
            }
        }

        public Vec3 Scale
        {
            get
            {
                InternalCalls.TransformComponent_GetScale(Entity.ID, out Vec3 scale);
                return scale;
            }
            set
            {
                InternalCalls.TransformComponent_SetScale(Entity.ID, ref value);
            }
        }
    }

    public class VideoPlayer : Component
    {

        public bool IsPlaying
        {
            get
            {
                InternalCalls.VideoPlayerComponent_GetIsPlaying(Entity.ID, out bool IsPlaying);
                return IsPlaying;
            }
            set
            {
                InternalCalls.VideoPlayerComponent_SetIsPlaying(Entity.ID, ref value);
            }
        }

        public void PlayVideo(string videoPath)
        {
            InternalCalls.VideoPlayerComponent_PlayVideo(Entity.ID);
        }

        public void StopVideo()
        {
            InternalCalls.VideoPlayerComponent_StopVideo(Entity.ID);
        }
    }


    //public class Renderer: Component
    //{

    //    public Renderer()
    //    {
    //    }

    //    public Renderer(ulong meshID, ulong meshDebugID, ulong currentTextureID = ulong.MaxValue, ulong animationID = ulong.MaxValue, bool animated = false)
    //    {
    //        this.CurrentMeshID = meshID;
    //        this.CurrentMeshDebugID = meshDebugID;
    //        this.CurrentTextureID = currentTextureID;
    //        this.CurrentAnimationID = animationID;
    //        this.Animated = animated;
    //    }

    //    public ulong CurrentMeshID { get; set; }
    //    public ulong CurrentMeshDebugID { get; set; }
    //    public ulong CurrentTextureID { get; set; }
    //    public ulong CurrentAnimationID { get; set; }
    //    public bool Animated { get; set; }

    //    public int Mesh { get; set; }
    //    public int Texture { get; set; }

    //    public void SetMeshID(ulong meshID)
    //    {
    //        CurrentMeshID = meshID;
    //        InternalCalls.RendererComponent_SetMeshID(meshID);
    //    }

    //     public void SetMeshDebugID(ulong meshID)
    //     {
    //         CurrentMeshDebugID = meshID;
    //         InternalCalls.RendererComponent_SetMeshDebugID(meshID);
    //     }
    // }
}
