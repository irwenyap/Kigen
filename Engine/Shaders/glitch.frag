/*********************************************************************
 * \file	vignette.frag
 * \brief	
 *      This fragment shader processes fragment data to produce the final
 *      pixel color on the screen. It uses texture mapping and handles 
 *      transparency based on texture coordinates and texture IDs.
 *
 * \author	y.ziyangirwen, 2301345
 * \email	y.ziyangirwen@digipen.edu
 * \date	3 October 2024
 * 
 * Copyright(C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 *********************************************************************/
#version 460 core

in vec2 vTexCoords;
out vec4 FragColor;

uniform sampler2DArray screenTexture;   // The rendered framebuffer
uniform int layerIndex;

uniform float time;                // For animating glitch effects
uniform float glitchIntensity;     // Controls glitch strength (0.0 - 1.0)

float rand(vec2 co) {
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {
    vec2 uv = vTexCoords;

    // Horizontal displacement
    float glitch = rand(vec2(time * 0.5, uv.y)) * glitchIntensity;
    uv.x += glitch * 0.05;  // Shift pixels horizontally

    // chromatic aberration
    float r = texture(screenTexture, vec3(uv + vec2(0.002 * glitch, 0.0), layerIndex)).r;
    float g = texture(screenTexture, vec3(uv, layerIndex)).g;
    float b = texture(screenTexture, vec3(uv - vec2(0.002 * glitch, 0.0), layerIndex)).b;

    // scanline effect
    float scanline = sin(uv.y * 800.0 + time * 10.0) * 0.04 * glitchIntensity;

    // noise overlay
    float noise = (rand(uv * time) - 0.5) * 0.1 * glitchIntensity;

    FragColor = vec4(r, g, b, 1.0) + vec4(scanline + noise, scanline + noise, scanline + noise, 0.0);
}