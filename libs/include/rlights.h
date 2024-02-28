/**********************************************************************************************
*
*   raylib.lights - Some useful functions to deal with lights data
*
*   CONFIGURATION:
*
*   #define RLIGHTS_IMPLEMENTATION
*       Generates the implementation of the library into the included file.
*       If not defined, the library is in header only mode and can be included in other headers
*       or source files without problems. But only ONE file should hold the implementation.
*
*   LICENSE: zlib/libpng
*
*   Copyright (c) 2017-2019 Victor Fisac (@victorfisac) and Ramon Santamaria (@raysan5)
*
*   This software is provided "as-is", without any express or implied warranty. In no event
*   will the authors be held liable for any damages arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose, including commercial
*   applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*     1. The origin of this software must not be misrepresented; you must not claim that you
*     wrote the original software. If you use this software in a product, an acknowledgment
*     in the product documentation would be appreciated but is not required.
*
*     2. Altered source versions must be plainly marked as such, and must not be misrepresented
*     as being the original software.
*
*     3. This notice may not be removed or altered from any source distribution.
*
**********************************************************************************************/

#ifndef RLIGHTS_H
#define RLIGHTS_H

// Max dynamic lights supported by shader
#define MAX_LIGHTS 4

typedef struct
{
    int type;
    Vector3 position;
    Vector3 target;
    Color color;
    bool enabled;

    // Shader locations
    int enabledLoc;
    int typeLoc;
    int posLoc;
    int targetLoc;
    int colorLoc;
}
Light;

typedef enum
{
    LIGHT_DIRECTIONAL,
    LIGHT_POINT
}
LightType;

// Prevents name mangling of functions
#ifdef __cplusplus
extern "C" {
#endif

// Current amount of created lights
int lightsCount = 0;

Light CreateLight(int type, Vector3 position, Vector3 target, Color color, Shader shader);
void UpdateShaderLightValues(Shader shader, Light light);

#ifdef __cplusplus
}
#endif

#endif // RLIGHTS_H

#if defined(RLIGHTS_IMPLEMENTATION)

#include "raylib.h"

Light CreateLight(int type, Vector3 position, Vector3 target, Color color, Shader shader)
{
    Light light = { 0 };

    if (lightsCount == MAX_LIGHTS) return light;

    light.enabled = true;
    light.type = type;
    light.position = position;
    light.target = target;
    light.color = color;

    // TODO
    char enabledName[32] = "lights[x].enabled\0";
    char typeName[32]    = "lights[x].type\0";
    char posName[32]     = "lights[x].position\0";
    char targetName[32]  = "lights[x].target\0";
    char colorName[32]   = "lights[x].color\0";

    enabledName[7] = '0' + lightsCount;
    typeName[7]    = '0' + lightsCount;
    posName[7]     = '0' + lightsCount;
    targetName[7]  = '0' + lightsCount;
    colorName[7]   = '0' + lightsCount;

    light.enabledLoc = GetShaderLocation(shader, enabledName);
    light.typeLoc    = GetShaderLocation(shader, typeName);
    light.posLoc     = GetShaderLocation(shader, posName);
    light.targetLoc  = GetShaderLocation(shader, targetName);
    light.colorLoc   = GetShaderLocation(shader, colorName);

    UpdateShaderLightValues(shader, light);

    lightsCount++;

    return light;
}

// Light shader locations should be available!
void UpdateShaderLightValues(Shader shader, Light light)
{
    float position[3] = { light.position.x, light.position.y, light.position.z };
    float target[3] = { light.target.x, light.target.y, light.target.z };
    float color[4] =
    {
        (float)light.color.r / (float)255,
        (float)light.color.g / (float)255,
        (float)light.color.b / (float)255,
        (float)light.color.a / (float)255
    };

    SetShaderValue(shader, light.posLoc,     position,       SHADER_UNIFORM_VEC3);
    SetShaderValue(shader, light.targetLoc,  target,         SHADER_UNIFORM_VEC3);
    SetShaderValue(shader, light.colorLoc,   color,          SHADER_UNIFORM_VEC4);
    SetShaderValue(shader, light.enabledLoc, &light.enabled, SHADER_UNIFORM_INT);
    SetShaderValue(shader, light.typeLoc,    &light.type,    SHADER_UNIFORM_INT);
}

#endif // RLIGHTS_IMPLEMENTATION