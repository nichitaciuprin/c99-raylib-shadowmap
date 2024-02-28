#include <stdio.h>
#include <raylib.h>
#include <raymath.h>

#define WIDTH 600
#define HEIGHT 600

#define GLSL_VERSION 330

#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"

Camera3D camera = {0};
Camera3D camera_shadow_map = {0};
Vector3 torusPosition = {0, 1, 0};
Model quad = {0};
Model torus = {0};
Model column = {0};
RenderTexture2D render_texture = {0};
Vector3 phase = {0};
Shader shader = {0};
Shader shader_default = {0};
Light light = {0};
float ligthHeight = 7.5f;

float phaseLight = 0;

void InitShader()
{
    shader = LoadShader(TextFormat("../assets/shaders/glsl%i/base_lighting.vs", GLSL_VERSION),
                        TextFormat("../assets/shaders/glsl%i/lighting.fs", GLSL_VERSION));

    shader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocation(shader, "matModel");
    shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");

    int ambientLoc = GetShaderLocation(shader, "ambient");
    SetShaderValue(shader, ambientLoc, (float[4]){ 0.2f, 0.2f, 0.2f, 1.0f }, SHADER_UNIFORM_VEC4);

    light = CreateLight(LIGHT_POINT, (Vector3){ 0, ligthHeight, 0 }, Vector3Zero(), PURPLE, shader);
    UpdateLightValues(shader, light);
}

void DrawColoredColumns(Color color)
{
    DrawModel(column, (Vector3){ 0, 0, 0 }, 1.0f, color);
}

void Init()
{
    DisableCursor();

    camera.fovy = 45;
    camera.target = (Vector3){0, 0, 0};
    camera.position = (Vector3){0, 10, 10};
    camera.up = (Vector3){0, 1, 0};

    camera_shadow_map.fovy = 45.0f;
    camera_shadow_map.position = (Vector3){0, ligthHeight, 0};
    camera_shadow_map.up = (Vector3){0, 0, -1};

    camera_shadow_map.target = (Vector3){ 0, 0, 0 };

    torus = LoadModelFromMesh(GenMeshTorus(0.3f, 2, 20, 20));
    column = LoadModelFromMesh(GenMeshCylinder(0.3f, 7, 10));
    shader_default = torus.materials[0].shader;
    Mesh plane_mesh = GenMeshCube(10, 0.1f, 10);
    quad = LoadModelFromMesh(plane_mesh);
    render_texture = LoadRenderTexture(160, 100);

    quad.materials[0].maps[MATERIAL_MAP_ALBEDO].texture = render_texture.texture;
}

void Update()
{
    phaseLight += 0.01f;
    camera_shadow_map.position.x = light.position.x = sinf(phaseLight) * 2;
    camera_shadow_map.position.z = light.position.z = cosf(phaseLight) * 2;
    UpdateLightValues(shader, light);

    phase = Vector3Add(phase, (Vector3){0.01f, 0.02f, 0.03f});
    torus.transform = MatrixRotateXYZ(phase);
    UpdateCamera(&camera, CAMERA_PERSPECTIVE);

    torus.materials[0].shader = shader_default;
    column.materials[0].shader = shader_default;

    camera_shadow_map.fovy += IsKeyDown(KEY_KP_ADD) * 0.1f;
    camera_shadow_map.fovy -= IsKeyDown(KEY_KP_SUBTRACT) * 0.1f;

    BeginTextureMode(render_texture);
    {
        ClearBackground(GRAY);

        BeginMode3D(camera_shadow_map);
        {
            DrawColoredColumns(BLACK);
            // DrawModel(torus, torusPosition, 1, DARKGRAY);
            DrawCubeWires((Vector3){0, 0.5f, 0}, 1, 1, 1, BLACK);
        }
        EndMode3D();
    }
    EndTextureMode();

    torus.materials[0].shader = shader;
    column.materials[0].shader = shader;

    BeginDrawing();
    {
        ClearBackground(BLACK);

        DrawFPS(10, 10);

        BeginMode3D(camera);
        {
            DrawSphereEx(light.position, 0.2f, 8, 8, light.color);

            DrawColoredColumns(RED);
            // DrawModel(torus, torusPosition, 1, RED);
            DrawCubeWires((Vector3){0, 0.5f, 0}, 1, 1, 1, BLUE);

            DrawModel(quad, (Vector3){0, 0, 0}, 1, GREEN);
        }
        EndMode3D();

        // if (IsKeyPressed(KEY_LEFT))  torusPosition.x -= 1;
        // if (IsKeyPressed(KEY_RIGHT)) torusPosition.x += 1;
        // if (IsKeyPressed(KEY_UP))    torusPosition.z -= 1;
        // if (IsKeyPressed(KEY_DOWN))  torusPosition.z += 1;
    }
    EndDrawing();
}

int main(void)
{
    InitWindow(WIDTH, HEIGHT, "Dynamic shadow test");
    // SetWindowPosition(700, 100);
    SetTargetFPS(60);
    InitShader();
    Init();

    while (!WindowShouldClose())
    {
        Update();
    }

    UnloadModel(quad);
    UnloadModel(torus);
    CloseWindow();

    return 0;
}