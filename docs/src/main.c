#include <stdio.h>
#include <raylib.h>
#include <raymath.h>

#ifdef OS_WEB
#include <emscripten/emscripten.h>
#endif

#define WIDTH 800
#define HEIGHT 600

#if defined(OS_WEB)
#define GLSL_VERSION            100
#else   // PLATFORM_WEB
#define GLSL_VERSION            330
#endif

#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"

Camera3D camera = {0};
Camera3D lightCamera = {0};
Vector3 cube_position = {0.0f, 1.0f, 0.0f};
Model quad = {0};
Model torus = {0};
Model column = {0};
RenderTexture2D renderTexture = {0};
Vector3 phase = {0};
Shader shader = {0};
Shader shaderDefault = {0};
Light light = {0};

void InitShader(){
    shader = LoadShader(FormatText("./assets/shaders/glsl%i/base_lighting.vs", GLSL_VERSION),
                            FormatText("./assets/shaders/glsl%i/lighting.fs", GLSL_VERSION));
    shader.locs[LOC_MATRIX_MODEL] = GetShaderLocation(shader, "matModel");
    shader.locs[LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");

    int ambientLoc = GetShaderLocation(shader, "ambient");
    SetShaderValue(shader, ambientLoc, (float[4]){ 0.2f, 0.2f, 0.2f, 1.0f }, UNIFORM_VEC4);

    light = CreateLight(LIGHT_POINT, (Vector3){ 0, 15.f, 0}, Vector3Zero(), PURPLE, shader);
    UpdateShaderLightValues(shader, light);

}

void DrawColoredColumns(Color color){
    DrawModel(column, (Vector3){1.5f, -1.f, -1.5f}, 1.f, color);
    DrawModel(column, (Vector3){-1.5f, -1.f, 1.5f}, 1.f, color);
    DrawModel(column, (Vector3){1.5f, -1.f, 1.5f}, 1.f, color);
    DrawModel(column, (Vector3){-1.5f, -1.f, -1.5f}, 1.f, color);
}

void DrawColumns(void)
{
    DrawColoredColumns(RED);
}

void DrawColumnsShadow(void)
{
    DrawColoredColumns(DARKGRAY);
}

float lightPhase = .0f;

void update_frame()
{
    lightPhase += 0.05f;
    lightCamera.position.x = light.position.x = sinf(lightPhase) * 4.0f;
    UpdateShaderLightValues(shader, light);

    phase = Vector3Add(phase, (Vector3){0.01f, 0.02f, 0.03f});
    torus.transform = MatrixRotateXYZ(phase);
    UpdateCamera(&camera);

    torus.materials[0].shader = shaderDefault;
    column.materials[0].shader = shaderDefault;

    lightCamera.fovy += IsKeyDown(KEY_KP_ADD) * 0.1f;
    lightCamera.fovy -= IsKeyDown(KEY_KP_SUBTRACT) * 0.1f;



    BeginTextureMode(renderTexture);{
        ClearBackground(GRAY);
        BeginMode3D(lightCamera);
        {
            DrawColumnsShadow();
            DrawModel(torus, cube_position, 1.f, DARKGRAY);
            DrawCubeWires((Vector3){2.f, 2.f, 1.f}, 1, 1, 1, DARKGRAY);
        }
        EndMode3D();
    }EndTextureMode();

    torus.materials[0].shader = shader;
    column.materials[0].shader = shader;

    BeginDrawing();
    {

        ClearBackground(BLACK);
        DrawFPS(10, 10);

        BeginMode3D(camera);
        {
            DrawColumns();
            DrawModel(torus, cube_position, 1.f, RED);
            DrawCubeWires((Vector3){2.f, 2.f, 1.f}, 1, 1, 1, BLUE);
            DrawModel(quad, (Vector3){0, -1.f, 0}, 1.f, GREEN);
        }
        EndMode3D();

        if (IsKeyPressed(KEY_LEFT))
            cube_position.x -= 1.0f;
        if (IsKeyPressed(KEY_RIGHT))
            cube_position.x += 1.0f;
        if (IsKeyPressed(KEY_UP))
            cube_position.z -= 1.0f;
        if (IsKeyPressed(KEY_DOWN))
            cube_position.z += 1.0f;
    }
    EndDrawing();
}

int main(void)
{
#ifdef OS_Windows_NT
    printf("Windows dettected\n");
#elif defined OS_Linux
    printf("LINUS dettected\n");
#elif defined OS_Darwin
    printf("MacOS dettected\n");
#endif

    InitWindow(WIDTH, HEIGHT, "This is a dynamic shadow test");
    SetTargetFPS(60);

    InitShader();

    camera.fovy = 45.0f;
    camera.target = (Vector3){.0f, .0f, .0f};
    camera.position = (Vector3){0.0f, 10.0f, 10.0f};
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    camera.type = CAMERA_PERSPECTIVE;
    SetCameraMode(camera, CAMERA_ORBITAL);

    lightCamera.fovy = 20.0f;
    lightCamera.target = (Vector3){.0f, .0f, .0f};
    lightCamera.position = (Vector3){0.0f, 10.0f, 0.0f};
    lightCamera.up = (Vector3){0.0f, 0.0f,-1.0f};
    lightCamera.type = CAMERA_PERSPECTIVE;


    torus = LoadModelFromMesh(GenMeshTorus(.3f, 2.f, 20, 20));
    column = LoadModelFromMesh(GenMeshCylinder(0.3f, 7.f, 10));
    shaderDefault = torus.materials[0].shader;
    Mesh plane_mesh = GenMeshCube(10.f, .1f, 10.f);
    quad = LoadModelFromMesh(plane_mesh);
    renderTexture = LoadRenderTexture(160, 100);

    quad.materials[0].maps[MAP_DIFFUSE].texture = renderTexture.texture;

#ifdef OS_WEB
    emscripten_set_main_loop(update_frame, 0, 1);
#else
    while (!WindowShouldClose())
    {
        update_frame();
    }
#endif
    UnloadModel(quad);
    UnloadModel(torus);
    CloseWindow();

    return 0;
}