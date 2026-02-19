/*********************************************************************
 * \file	Vec2.cs
 * \brief	Defines a Vec2 class in C#.
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
    public struct Vec2
    {

        public float x;
        public float y;


        public Vec2(float x)
        {
            this.x = x;
            this.y = x;
        }
        public Vec2(float x, float y)
        {
            this.x = x;
            this.y = y;

        }

        public static Vec2 operator +(Vec2 lhs, Vec2 rhs)
        {
            return new Vec2(lhs.x + rhs.x, lhs.y + rhs.y);
        }
        public static Vec2 operator* (Vec2 lhs, float rhs)
        {
            return new Vec2(lhs.x * rhs, lhs.y * rhs);
        }

        public static Vec2 operator/ (Vec2 lhs, float rhs)
        {
            return new Vec2(lhs.x / rhs, lhs.y / rhs);
        }

        public float LengthSquared()
        {
            return (x * x) + (y * y);
        }

        public float Length()
        {
            return (float)Math.Sqrt(LengthSquared());
        }
        
        public Vec2 Normalized()
        {
            float d = Length();
            if (d <= float.Epsilon && -d <= float.Epsilon)
            {
                return this;
            }
            return (this / d);
        }
        #region Added Functions
        public static Vec2 AddToX(Vec2 vector, float amount)
        {
            return new Vec2(vector.x + amount, vector.y);
        }
        public static Vec2 AddToY(Vec2 vector, float amount)
        {
            return new Vec2(vector.x, vector.y + amount);
        }
        public static Vec2 GetRandom(Vec2 min, Vec2 max)
        {
            Random random = new Random();

            return new Vec2((float)(random.NextDouble() * (max.x - min.x) + min.x),
                (float)(random.NextDouble() * (max.y - min.y) + min.y));
        }
        #endregion
    }
}
