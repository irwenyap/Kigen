/*********************************************************************
 * \file	    VideoClip.cpp
 * \brief       Base VideoClip asset declaration
 *
 * \author		y.ziyangirwen, 2301345 (y.ziyangirwen@digipen.edu)
 * \date		30 January 2025
 *
 * \copyright	Copyright(C) 2024 DigiPen Institute of Technology.
 *				Reproduction or disclosure of this file or its
 *              contents without the prior written consent of DigiPen
 *              Institute of Technology is prohibited.
 *********************************************************************/

#include "VideoClip.hpp"

#include <glad/glad.h>

#include "../Graphics/Texture.hpp"
#include "Logger.hpp"
#include "../pl_mpeg.h"
#include "../Graphics/GraphicsManager.hpp"

bool VideoClip::LoadFromFile(const std::string& filePath)
{
    plm_t* video = plm_create_with_filename(filePath.c_str());
    if (!video) {
        Logger::Instance().Log(Logger::Level::ERR, "Failed to load video: " + filePath);
        return false;
    }

    plm_set_video_enabled(video, true);

    int width = plm_get_width(video);
    int height = plm_get_height(video);
    int numFrames = static_cast<int>(plm_get_framerate(video) * plm_get_duration(video));

    SetTextureArrayToUse(width, height);
    if (!(texArrayIndex >= 0 && texArrayIndex < Texture::textureArrays.size())) return false;

    texLayerStartIndex = Texture::textureArrays[texArrayIndex].currentLayers;
    texLayerEndIndex = texLayerStartIndex + numFrames - 1;

	GLenum internalFormat = GraphicsManager::GetInstance().GetInternalFormat();

    if (Texture::textureArrays[texArrayIndex].id_gl == 0) {
        Texture::textureArrays[texArrayIndex].allocatedLayers = numFrames + 10;

        glGenTextures(1, &Texture::textureArrays[texArrayIndex].id_gl);
        glBindTexture(GL_TEXTURE_2D_ARRAY, Texture::textureArrays[texArrayIndex].id_gl);

        // Basic parameters
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexStorage3D(
            GL_TEXTURE_2D_ARRAY,
            1,
            internalFormat,
            width,
            height,
            Texture::textureArrays[texArrayIndex].allocatedLayers
        );
    } 
    else if ((Texture::textureArrays[texArrayIndex].currentLayers + numFrames) > Texture::textureArrays[texArrayIndex].allocatedLayers) {
        GLuint newID;
        glGenTextures(1, &newID);
        glBindTexture(GL_TEXTURE_2D_ARRAY, newID);

        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        Texture::textureArrays[texArrayIndex].allocatedLayers += numFrames + 10;
        glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, internalFormat, width, height, Texture::textureArrays[texArrayIndex].allocatedLayers);

        CopyAllTextureLayers(Texture::textureArrays[texArrayIndex].id_gl, newID, width, height, Texture::textureArrays[texArrayIndex].currentLayers - 1);

        // Delete the old texture
        glDeleteTextures(1, &Texture::textureArrays[texArrayIndex].id_gl);

        // Reassign the new texture ID
        Texture::textureArrays[texArrayIndex].id_gl = newID;
    }
    else {
        glBindTexture(GL_TEXTURE_2D_ARRAY, Texture::textureArrays[texArrayIndex].id_gl);
    }

    for (int i = 0; i < numFrames; i++) {
        // Decode one frame
        plm_frame_t* frame = plm_decode_video(video);
        if (!frame) break; // just in case the file ends early

        // Convert YCbCr->RGB using plm_frame_to_rgb
        // 'stride' = width * 3 for an 8-bit RGB image
        std::vector<uint8_t> rgbData(width * height * 3);
        plm_frame_to_rgb(frame, rgbData.data(), width * 3);

        // Upload to the texture array layer
        // For "frame i," the layer is (texLayerStartIndex + i)
        glTexSubImage3D(
            GL_TEXTURE_2D_ARRAY,
            0,                                   // mip level
            0, 0,                                // x, y offsets
            texLayerStartIndex + i,             // z offset = which layer
            width,
            height,
            1,                                   // depth = 1 layer
            GL_RGB,                              // data format
            GL_UNSIGNED_BYTE,                    // data type
            rgbData.data()
        );
    }

    Texture::textureArrays[texArrayIndex].currentLayers += numFrames;

    plm_destroy(video);
    return true;
}

size_t VideoClip::SetTextureArrayToUse(int widthImage, int heightImage)
{
    bool newArrayFlag{ false };
    texArrayIndex = static_cast<size_t>(-1);

    for (size_t i = 0; i < Texture::textureArrays.size(); ++i) {
        if (!newArrayFlag && Texture::textureArrays[i].width == 0 && Texture::textureArrays[i].height == 0 && Texture::textureArrays[i].id_gl == 0) {
            texArrayIndex = i;
            newArrayFlag = true;
        }

        if (Texture::textureArrays[i].width == widthImage && Texture::textureArrays[i].height == heightImage && Texture::textureArrays[i].id_gl != 0) {
            texArrayIndex = i;
            newArrayFlag = false;
            break;
        }
    }

    if (texArrayIndex < Texture::textureArrays.size()) {
        // Reserve layers for this video
        texLayerStartIndex = Texture::textureArrays[texArrayIndex].currentLayers;
        texLayerEndIndex = texLayerStartIndex;  // This will be adjusted in `LoadFromFile`
    } else {
        Logger::Instance().Log(Logger::Level::ERR, "[VideoClip] SetTextureArrayToUse: No available texture array slots");
        return static_cast<size_t>(-1);
    }

    if (newArrayFlag) {
        Texture::textureArrays[texArrayIndex].width = widthImage;
        Texture::textureArrays[texArrayIndex].height = heightImage;
    }

    return texArrayIndex;
}

void VideoClip::CopyAllTextureLayers(GLuint srcTex, GLuint destTex, int width, int height, int numLayers)
{
    GLuint readFramebuffer, drawFramebuffer;
    glGenFramebuffers(1, &readFramebuffer);
    glGenFramebuffers(1, &drawFramebuffer);

    for (GLuint layer = 0; layer < static_cast<GLuint>(numLayers); ++layer) {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, readFramebuffer);
        glFramebufferTextureLayer(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, srcTex, 0, layer);

        if (glCheckFramebufferStatus(GL_READ_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            Logger::Instance().Log(Logger::Level::ERR, "[VideoClip] CopyAllTextureLayers(): Read framebuffer is not complete");
            continue; // Skip to the next layer
        }

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, drawFramebuffer);
        glFramebufferTextureLayer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, destTex, 0, layer);

        if (glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            Logger::Instance().Log(Logger::Level::ERR, "[VideoClip] CopyAllTextureLayers(): Draw framebuffer is not complete");
            continue; // Skip to the next layer
        }

        glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    }

    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &readFramebuffer);
    glDeleteFramebuffers(1, &drawFramebuffer);
}