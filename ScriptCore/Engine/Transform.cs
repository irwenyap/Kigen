using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ScriptCore.Engine
{
    internal class Transform
    {
        // TransformSystem_SetPosition
        public static void SetPosition(UInt32 entityID, Vec3 position)
        {
            InternalCalls.TransformSystem_SetPosition(entityID, position);
        }

        // TransformSystem_SetRotation
        public static void SetRotation(UInt32 entityID, Vec3 rotation)
        {
            InternalCalls.TransformSystem_SetRotation(entityID, rotation);
        }

        // TransformSystem_SetScale
        public static void SetScale(UInt32 entityID, Vec3 scale)
        {
            InternalCalls.TransformSystem_SetScale(entityID, scale);
        }

        // TransformSystem_GetPosition
        public static void Translate(UInt32 entityID, Vec3 translation)
        {
            InternalCalls.TransformSystem_Translate(entityID, translation);
        }

        // TransformSystem_Rotate
        public static void Rotate(UInt32 entityID, Vec3 rotation)
        {
            InternalCalls.TransformSystem_Rotate(entityID, rotation);
        }

        // TransformSystem_Scale
        public static void Scale(UInt32 entityID, Vec3 scale)
        {
            InternalCalls.TransformSystem_Scale(entityID, scale);
        }

    }
}
