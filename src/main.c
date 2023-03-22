#include <stdio.h>
#include <stdlib.h>
#include "raylib.h"

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

static const int screenWidth = 800;
static const int screenHeight = 600;
static const Vector2 center = {screenWidth / 2, screenHeight / 2};

// Wheel variables
static const int wheelRadius = 225;
static const Vector2 ringCenter = {screenWidth / 2, screenHeight / 2 + 50};
static const Rectangle wheelHeader = {screenWidth / 2 - 150, 25, 300, 75};

static int framesCounter = 0;
static Texture2D TestTex;
static Texture2D GrayscaleTestTex;

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
    Image testGrayscaleImage = ImageCopy(testImage);
    ImageColorGrayscale(&testGrayscaleImage);
    ImageResize(&testImage, screenWidth, screenHeight);
    TestTex = LoadTextureFromImage(testImage);
    GrayscaleTestTex = LoadTextureFromImage(testGrayscaleImage);
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
            DrawTexture(GrayscaleTestTex, 0, 0, WHITE);
            DrawRectangleRec(wheelHeader, Fade(BLACK, 0.8f));
            DrawButtonInfo("RB", (int)center.x, 38, GAMEPAD_BUTTON_RIGHT_TRIGGER_1);
            for (int i = 0; i < 8; i++)
            {
                DrawRing(ringCenter, wheelRadius * 0.6, wheelRadius, i * 45 - 21.5, i * 45 + 21.5, 100, Fade(BLACK, 0.8f));
            }
            DrawCircleV(ringCenter, wheelRadius * 0.57, Fade(BLACK, 0.5f));

            // for (int i = 0; i <= 17; i++)
            // {
            //     DrawText(BUTTON_STRING[i], 10, i * 20, 20, IsGamepadButtonDown(0, i) ? RED : BLACK);
            // }

            DrawCircle(50 + (int)(GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X) * 20),
                       center.y + (int)(GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y) * 20), 25, BLACK);
            DrawCircle(screenWidth - 50 + (int)(GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_X) * 20),
                       center.y + (int)(GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_Y) * 20), 25, BLACK);
        }
        else
        {
            DrawTexture(TestTex, 0, 0, WHITE);
        }
    }
    else
    {
        DrawText("HELLO WASM", center.x - MeasureText("HELLO WASM", 20) / 2, center.y - 10, 20, GRAY);
    }
    //DrawTestGrid();
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

void DrawButtonInfo(const char *text, int posX, int posY, int button)
{
    int fontSize = IsGamepadButtonPressed(0, button) ? 20 : 16;
    int txtWid = MeasureText(text, fontSize);
    int txtPos = posX - txtWid / 2;
    DrawRectangleRounded((Rectangle){txtPos - 5, posY - 5, txtWid + 10, fontSize + 10}, 0.2, 10, WHITE);
    DrawText(text, txtPos, posY, fontSize, BLACK);
}

void DrawTestGrid(void)
{
    DrawLineEx((Vector2){center.x, 0}, (Vector2){center.x, screenHeight}, 3, MAGENTA);
    DrawLineEx((Vector2){0, center.y}, (Vector2){screenWidth, center.y}, 3, MAGENTA);
}