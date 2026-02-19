/*********************************************************************
 * \file	Vec3.cs
 * \brief	Defines a Vec3 class in C#.
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
    public struct Vec3
    {
        public float x;
        public float y;
        public float z;

        public Vec3(float x, float y, float z)
        {
            this.x = x;
            this.y = y;
            this.z = z;
        }

        public static float Distance(Vec3 lhs, Vec3 rhs)
        {
            float dx = rhs.x - lhs.x;
            float dy = rhs.y - lhs.y;
            float dz = rhs.z - lhs.z;

            return (float)Math.Sqrt(dx * dx + dy * dy + dz * dz);
        }
        #region Added Functions
        public static Vec3 AddToX(Vec3 vector, float amount)
        {
            return new Vec3(vector.x + amount, vector.y, vector.z);
        }
        public static Vec3 AddToY(Vec3 vector, float amount)
        {
            return new Vec3(vector.x, vector.y + amount, vector.z);
        }
        public static Vec3 AddToZ(Vec3 vector, float amount)
        {
            return new Vec3(vector.x, vector.y, vector.z + amount);
        }
        #endregion
    }
}
