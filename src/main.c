#include <stdio.h>
#include <raylib.h>
#include <raymath.h>

#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"

// Model
Camera3D camera = {0};
Light light = {0};
float lightPhase = 0;
Vector3 planePosition = {0, 0, 0};
Vector3 columnPosition = {0, 0, 0};
Vector3 cubePosition = {0, 0.50f + 4, 0};

RenderTexture2D renderTexture = {0};
Shader shader = {0};
Shader shaderDefault = {0};
Model plane = {0};
Model column = {0};
Camera3D lightCamera = {0};

void Init()
{
    int glslVersion = 330;

    shader = LoadShader(TextFormat("../assets/shaders/glsl%i/base_lighting.vs", glslVersion),
                        TextFormat("../assets/shaders/glsl%i/lighting.fs", glslVersion));

    shader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocation(shader, "matModel");
    shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");

    int ambientLoc = GetShaderLocation(shader, "ambient");
    SetShaderValue(shader, ambientLoc, (float[4]){0.2f, 0.2f, 0.2f, 1.0f}, SHADER_UNIFORM_VEC4);

    light.position = (Vector3){0, 10, 0};
    light.target = Vector3Zero();

    light = CreateLight(LIGHT_POINT, light.position, light.target, WHITE, shader);

    camera.fovy = 45;
    camera.target = (Vector3){0, 0, 0};
    camera.position = (Vector3){0, 10, 10};
    camera.up = (Vector3){0, 1, 0};

    lightCamera.fovy = 45;
    lightCamera.position = light.position;
    lightCamera.up = (Vector3){0, 0, -1};

    lightCamera.target = (Vector3){0, 0, 0};

    Mesh meshPlane = GenMeshCube(10, 0.1f, 10);
    Mesh meshCylinder = GenMeshCylinder(0.3f, 7, 10);

    plane = LoadModelFromMesh(meshPlane);
    column = LoadModelFromMesh(meshCylinder);

    shaderDefault = column.materials[0].shader;

    int textureSize = 100;
    renderTexture = LoadRenderTexture(textureSize, textureSize);
    plane.materials[0].maps[MATERIAL_MAP_ALBEDO].texture = renderTexture.texture;
}

void Render()
{
    lightCamera.position = light.position;
    column.materials[0].shader = shaderDefault;

    UpdateShaderLightValues(shader, light);
    UpdateCamera(&camera, CAMERA_PERSPECTIVE);

    BeginTextureMode(renderTexture);
    {
        ClearBackground(GRAY);

        BeginMode3D(lightCamera);
        {
            DrawModel(column, columnPosition, 1.0f, BLACK);
            DrawCubeWires(cubePosition, 1, 1, 1, BLACK);
        }
        EndMode3D();
    }
    EndTextureMode();

    column.materials[0].shader = shader;

    BeginDrawing();
    {
        ClearBackground(BLACK);

        BeginMode3D(camera);
        {
            DrawModel(plane, planePosition, 1, GREEN);
            DrawSphereEx(light.position, 0.2f, 8, 8, light.color);

            DrawModel(column, columnPosition, 1, RED);
            DrawCubeWires(cubePosition, 1, 1, 1, BLUE);
        }
        EndMode3D();

        DrawFPS(10, 10);
    }
    EndDrawing();
}

int main()
{
    int windowSize = 600;

    InitWindow(windowSize, windowSize, "Dynamic shadow test");
    SetTargetFPS(60);
    DisableCursor();

    Init();

    while (!WindowShouldClose())
    {
        lightPhase += 0.01f;
        light.position.x = sinf(lightPhase) * 2;
        light.position.z = cosf(lightPhase) * 2;

        Render();
    }

    UnloadModel(plane);
    CloseWindow();

    return 0;
}
