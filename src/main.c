#include <stdio.h>
#include <raylib.h>
#include <raymath.h>

#define WINDOW_SIZE 600

#define GLSL_VERSION 330

#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"

// Model
Camera3D camera = {0};
Light light = {0};
float lightPhase = 0;
Vector3 columnPosition = { 0, 0, 0 };
Vector3 cubePosition = { 0, 0.50f + 0.05f, 0 }; /* + 0.05f to see bottom part */

RenderTexture2D renderTexture = {0};
Shader shader = {0};
Shader shaderDefault = {0};
Model plane = {0};
Model column = {0};
Camera3D lightCamera = {0};

void Init()
{
    shader = LoadShader(TextFormat("../assets/shaders/glsl%i/base_lighting.vs", GLSL_VERSION),
                        TextFormat("../assets/shaders/glsl%i/lighting.fs", GLSL_VERSION));

    shader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocation(shader, "matModel");
    shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");

    int ambientLoc = GetShaderLocation(shader, "ambient");
    SetShaderValue(shader, ambientLoc, (float[4]){ 0.2f, 0.2f, 0.2f, 1.0f }, SHADER_UNIFORM_VEC4);

    light = CreateLight(LIGHT_POINT, (Vector3){ 0, 7.5f, 0 }, Vector3Zero(), WHITE, shader);
    UpdateShaderLightValues(shader, light);

    DisableCursor();

    camera.fovy = 45;
    camera.target = (Vector3){0, 0, 0};
    camera.position = (Vector3){0, 10, 10};
    camera.up = (Vector3){0, 1, 0};

    lightCamera.fovy = 45;
    lightCamera.position = light.position;
    lightCamera.up = (Vector3){0, 0, -1};

    lightCamera.target = (Vector3){ 0, 0, 0 };

    column = LoadModelFromMesh(GenMeshCylinder(0.3f, 7, 10));
    shaderDefault = column.materials[0].shader;
    Mesh plane_mesh = GenMeshCube(10, 0.1f, 10);
    plane = LoadModelFromMesh(plane_mesh);
    renderTexture = LoadRenderTexture(160, 100);

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
            DrawModel(plane, (Vector3){0, 0, 0}, 1, GREEN);

            DrawSphereEx(light.position, 0.2f, 8, 8, light.color);
            DrawModel(column, (Vector3){ 0, 0, 0 }, 1, RED);
            DrawCubeWires(cubePosition, 1, 1, 1, BLUE);
        }
        EndMode3D();

        DrawFPS(10, 10);
    }
    EndDrawing();
}

int main(void)
{
    InitWindow(WINDOW_SIZE, WINDOW_SIZE, "Dynamic shadow test");
    SetTargetFPS(60);

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