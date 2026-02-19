/*********************************************************************
 * \file	Vertex.cs
 * \brief	Defines a Vertex class in C#.
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
   
    

    public struct Vertex
    {
        Vec3 position;  // Position in 2D space
        Vec4 color;     // Color in RGBA format
        Vec3 normal;    // Normal vector
        Vec2 texCoord;  // Texture coordinates
        int texID;      // Texture ID
        int visible;    // Visibility state


        public Vertex(Vec3 pos, Vec4 clr, Vec3 norm, Vec2 texCoord, int texID, int visible)
        {
            this.position = pos;
            this.color = clr;
            this.normal = norm;
            this.texCoord = texCoord;
            this.texID = texID;
            this.visible = visible;
        }
    }
}
