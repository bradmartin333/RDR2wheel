#include "raylib.h"

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

static const char *BUTTON_STRING[] = {
    "unknown (unused)",
    "left face up",
    "left face right",
    "left face down",
    "left face left",
    "right face up",
    "right face right",
    "right face down",
    "right face left",
    "left trig 1",
    "left trig 2",
    "right trig 1",
    "right trig 2",
    "mid left",
    "mid (unused)",
    "mid right",
    "left thumb",
    "right thumb",
};

static const int screenWidth = 800;
static const int screenHeight = 600;

static int framesCounter = 0;
static Texture2D TestTex;

static void InitGame(void);        // Initialize game
static void UpdateGame(void);      // Update game (one frame)
static void DrawGame(void);        // Draw game (one frame)
static void UnloadGame(void);      // Unload game
static void UpdateDrawFrame(void); // Update and Draw (one frame)

int main(void)
{
    InitWindow(screenWidth, screenHeight, "RDR2 Wheel");
    InitGame();

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
    SetTargetFPS(60);
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        UpdateDrawFrame();
    }
#endif
    UnloadGame();
    CloseWindow();
    return 0;
}

void InitGame(void)
{
    framesCounter = 0;

    Image testImage = LoadImage("resources/images/fire.png");
    ImageResize(&testImage, screenWidth, screenHeight);
    TestTex = LoadTextureFromImage(testImage);
    UnloadImage(testImage);
}

void UpdateGame(void)
{
    framesCounter++;
}

void DrawGame(void)
{
    BeginDrawing();
    ClearBackground(RAYWHITE);
    if (IsGamepadAvailable(0))
    {
        if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_TRIGGER_1))
        {
            DrawCircle(screenWidth / 2, screenHeight / 2, 100, RED);

            // for (int i = 0; i <= 17; i++)
            // {
            //     DrawText(BUTTON_STRING[i], 10, i * 20, 20, IsGamepadButtonDown(0, i) ? RED : BLACK);
            // }

            DrawCircle(50 + (int)(GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X) * 20),
                       screenHeight / 2 + (int)(GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y) * 20), 25, BLACK);
            DrawCircle(screenWidth - 50 + (int)(GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_X) * 20),
                       screenHeight / 2 + (int)(GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_Y) * 20), 25, BLACK);
        }
        else
        {
            DrawTexture(TestTex, 0, 0, WHITE);
        }
    }
    else
    {
        DrawText("HELLO WASM", GetScreenWidth() / 2 - MeasureText("HELLO WASM", 20) / 2, GetScreenHeight() / 2 - 10, 20, GRAY);
    }
    EndDrawing();
}

void UnloadGame(void)
{
    UnloadTexture(TestTex);
}

void UpdateDrawFrame(void)
{
    UpdateGame();
    DrawGame();
}
