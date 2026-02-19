/*********************************************************************
 * \file	Audio.cs
 * \brief	Setups Audio.
 *
 * \author	wengkhong.thang, 2301372
 * \co-author wilmer.leejunrong, 2200691
 * \email	wengkhong.thang@digipen.edu
 * \date	27 November 2024

Copyright(C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 *********************************************************************/
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.Remoting.Channels;
using System.Text;
using System.Threading.Tasks;

namespace ScriptCore
{
    public class Audio
    {
        public static bool DialogueIsPlaying = false;
        public static string CurrentDialogueClip = "";
        public static float MasterVol = 1.0f;

        /// <summary>
        /// Plays a sound from the given file path.
        /// </summary>
        /// <param name="path">The path to the audio file.</param>
        /// <param name="vol">The playback volume (0.0 - 1.0).</param>
        public static void PlaySound(UInt32 entityID, string path, float vol, bool loop = false)
        {
            float finalVolume = (DialogueIsPlaying && path != CurrentDialogueClip) ? vol * 0.6f : vol;
            finalVolume *= MasterVol;

            InternalCalls.AudioSystem_PlaySound(entityID, path, finalVolume, loop);
        }

        /// <summary>
        /// Stops a currently playing sound.
        /// </summary>
        /// <param name="path">The path to the audio file.</param>
        public static void StopSound(string path)
        {
            InternalCalls.AudioSystem_StopSound(path);
        }

        /// <summary>
        /// Plays a spatial sound at a specific 3D position.
        /// </summary>
        /// <param name="path">The path to the audio file.</param>
        /// <param name="pos">The 3D position of the sound source.</param>
        /// <param name="vol">The playback volume (0.0 - 1.0).</param>
        public static void PlaySpatial(UInt32 entityId, string path, Vec3 pos, float vol)
        {
            float finalVolume = (DialogueIsPlaying && path != CurrentDialogueClip) ? vol * 0.1f : vol;
            finalVolume *= MasterVol;
            InternalCalls.AudioSystem_SpatialSound(entityId, path, pos, finalVolume);
        }

        /// <summary>
        /// Sets the audio listener's position and orientation for spatial audio.
        /// </summary>
        /// <param name="pos">The listener's position in 3D space.</param>
        /// <param name="Vel">The listener's velocity for Doppler effects.</param>
        /// <param name="forward">The listener's forward direction.</param>
        /// <param name="up">The listener's up direction.</param>
        public static void SetListener(Vec3 pos, Vec3 Vel, Vec3 forward, Vec3 up)
        {
            InternalCalls.AudioSystem_SetListener(pos, Vel, forward, up);
        }

        /// <summary>
        /// Updates the volume of a playing sound.
        /// </summary>
        /// <param name="path">The path to the audio file.</param>
        /// <param name="vol">The new volume level (0.0 - 1.0).</param>
        public static void UpdateSound(UInt32 entityID, string path, float vol)
        {
            float finalVol = (DialogueIsPlaying && path != CurrentDialogueClip) ? vol * 0.1f : vol;
            finalVol *= MasterVol;

            InternalCalls.AudioSystem_UpdateSound(entityID, path, finalVol);
        }

        /// <summary>
        /// Sets the stereo panning for a specific audio clip played by an entity.
        /// </summary>
        /// <param name="entityID">The unique ID of the entity playing the sound.</param>
        /// <param name="filePath">The file path of the audio clip.</param>
        /// <param name="volume">The volume level of the sound (range: 0.0 - 1.0).</param>
        public static void SetPanning(uint entityID, string filePath, float volume)
        {
            float finalVolume = (DialogueIsPlaying && filePath != CurrentDialogueClip) ? volume * 0.1f : volume;
            finalVolume *= MasterVol;
            InternalCalls.AudioSystem_SetPanning(entityID, filePath, finalVolume);
        }
        /// <summary>
        /// Plays an audio clip for a specific entity with optional stereo panning.
        /// </summary>
        /// <param name="entityID">The unique ID of the entity playing the sound.</param>
        /// <param name="filePath">The file path of the audio clip.</param>
        /// <param name="volume">The volume level of the sound (range: 0.0 - 1.0).</param>
        /// <param name="isPanning">If true, enables panning based on the entity's position.</param>
        public static void PlaySoundWithPanning(uint entityID, string filePath, float volume, bool loop, bool isPanning)
        {
            float finalVolume = (DialogueIsPlaying && filePath != CurrentDialogueClip) ? volume * 0.1f : volume;
            finalVolume *= MasterVol;

            InternalCalls.AudioSystem_PlaySoundWithPanning(entityID, filePath, finalVolume, loop, isPanning);
        }
        /// <summary>
        /// Checks if an audio clip is currently playing.
        /// </summary>
        /// <param name="path">The file path of the audio clip.</param>
        /// <returns>True if the audio clip is playing, otherwise false.</returns>
        public static bool ClipIsPlaying(string path)
        {
            return InternalCalls.AudioSystem_ClipIsPlaying(path);
        }

        public static void DuckAllExcept(UInt32 entityID, string path, float vol)
        {
            InternalCalls.AudioSystem_DuckAllExcept(entityID, path, vol);
        }

        public static void RestoreAllVolumes()
        {
            InternalCalls.AudioSystem_RestoreAllVolumes();
        }

        public static void SetVolume(float vol)
        {
            InternalCalls.AudioSystem_SetGlobalVolume(vol);
        }
        
        public static void StopClip(uint entityID, string path)
        {
            InternalCalls.AudioSystem_StopClip(entityID, path);
        }

        public static void SetCurrentDialogueClip(string clip)
        {
            CurrentDialogueClip = clip;
        }

        public static bool IsDialogueClip(string clip)
        {
            return DialogueIsPlaying && clip == CurrentDialogueClip;
        }

        public static void ResumeAll()
        {
            InternalCalls.AudioSystem_ResumeAll();
        }
        public static void PauseAll()
        {
            InternalCalls.AudioSystem_PauseAll();
        }
        public static void StopAll()
        {
            InternalCalls.AudioSystem_StopAll();
        }

    }
}
