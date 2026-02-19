/*********************************************************************
 * \file	Animation.cs
 * \brief	Setups Animation.
 *
 * \author	wengkhong.thang, 2301372
 * \email	wengkhong.thang@digipen.edu
 * \date	27 November 2024

Copyright(C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 *********************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;


namespace ScriptCore
{

    public struct AniData
    {
        public uint sprPerRow;
        public uint sprPerCol;
        public uint numFrames;
        public uint startFrame;
        public uint endFrame;

        public uint currentFrame;
        public double timePerFrame;
        public double elapsedTime;

        public float spriteWidth;
        public float spriteHeight;

        public bool playOnce;
        public bool isLooping;

        public AniData(uint sprPerRow, uint sprPerCol,
        uint numFrames, uint startFrame, uint endFrame,
        double timePerFrame, bool isLooping = false)
        {
            this.sprPerRow = sprPerRow;
            this.sprPerCol = sprPerCol;
            this.numFrames = numFrames;
            this.startFrame = startFrame;
            this.endFrame = endFrame;

            this.currentFrame = 0;
            this.timePerFrame = timePerFrame;
            this.elapsedTime = 0;

            this.spriteWidth = 1.0f / sprPerRow;
            this.spriteHeight = 1.0f / sprPerCol;

            this.playOnce = false;
            this.isLooping = isLooping;
        }
    }
    public class Animation : Component
    {

        public AniData data
        {
            get
            {
                InternalCalls.AnimationComponent_GetData(Entity.ID, out AniData tmp);
                return tmp;

            }
            set
            {
                InternalCalls.AnimationSystem_SetAnimation(Entity.ID, value);
            }

        }
        //public Animation()
        //{
        //    data.sprPerRow = 5;
        //    data.sprPerCol = 5; data.numFrames = 25;
        //    data.startFrame = 0; data.endFrame = 5;
        //    data.timePerFrame = 0.15;
        //    data.isLooping = false;
        //    data.currentFrame = 0;
        //    data.elapsedTime = 0.0;

        //    data.spriteWidth = 1.0f / data.sprPerRow;
        //    data.spriteHeight = 1.0f / data.sprPerCol;

        //}
        //public Animation(
        //    UInt32 sprPerRow, UInt32 sprPerCol,
        //    UInt32 numFrames, UInt32 startFrame, UInt32 endFrame,
        //    double timePerFrame, bool isLooping)
        //{
        //    this.data.sprPerRow = sprPerRow;
        //    this.data.sprPerCol = sprPerCol; this.data.numFrames = numFrames;
        //    this.data.startFrame = startFrame; this.data.endFrame = endFrame;
        //    this.data.timePerFrame = timePerFrame;
        //    this.data.isLooping = isLooping;
        //    data.currentFrame = startFrame;
        //    data.elapsedTime = 0.0;

        //    data.spriteWidth = 1.0f / sprPerRow;
        //    data.spriteHeight = 1.0f / sprPerCol;
        //}

        public void SetAnimation(Animation animation)
        {
            InternalCalls.AnimationSystem_SetAnimation(Entity.ID, animation.data);
        }

        public void PlayAnimation(bool reset = false, bool refresh = false, bool playOnce = true, bool isLooping = false)
        {
            InternalCalls.AnimationSystem_PlayAnimation(Entity.ID, reset, refresh, playOnce, isLooping);
        }

        public void PauseAnimation(bool reset = false)
        {
            InternalCalls.AnimationSystem_PauseAnimation(Entity.ID, reset);
        }

    }
}
