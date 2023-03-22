#include <stdio.h>
#include <stdlib.h>
#include "raylib.h"

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

#define NUM_HEADER_OPTIONS 3

static const int screenWidth = 800;
static const int screenHeight = 600;
static const Vector2 center = {screenWidth / 2, screenHeight / 2};

// Wheel variables
static const int wheelRadius = 225;
static const Vector2 ringCenter = {screenWidth / 2, screenHeight / 2 + 50};
// Header
static const Rectangle wheelHeader = {screenWidth / 2 - 150, 25, 300, 75};
static const char *headerOptions[NUM_HEADER_OPTIONS] = {"Camera", "Processing", "Tools"};
static int headerSelection = 1;

static int framesCounter = 0;
static Texture2D TestTex;
static Texture2D GrayscaleTestTex;

static void InitGame(void);        // Initialize game
static void UpdateGame(void);      // Update game (one frame)
static void DrawGame(void);        // Draw game (one frame)
static void UnloadGame(void);      // Unload game
static void UpdateDrawFrame(void); // Update and Draw (one frame)
static void DrawHeader(void);
static void DrawButton(const char *text, int posX, int posY, int button);
static int ApplyButton(int button);
static void DrawRingBackground(void);

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
            DrawHeader();
            DrawRingBackground();

            // Testing
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

void DrawHeader(void)
{
    DrawRectangleRec(wheelHeader, Fade(BLACK, 0.8f));
    DrawButton("RB", (int)center.x, 38, GAMEPAD_BUTTON_RIGHT_TRIGGER_1);

    // Determine the width of all the header options together
    int fontSize = 20;
    int totalWidth = 0;
    for (int i = 0; i < NUM_HEADER_OPTIONS; i++)
    {
        totalWidth += MeasureText(headerOptions[i], fontSize) + 10;
    }

    // Draw the header options
    int txtPos = (int)((int)center.x - totalWidth / 2.0);
    for (int i = 0; i < 3; i++)
    {
        Color c = headerSelection == i ? WHITE : Fade(WHITE, 0.3f);
        DrawText(headerOptions[i], txtPos, 67, fontSize, c);
        txtPos += (int)MeasureText(headerOptions[i], fontSize) + 10;
    }
}

void DrawButton(const char *text, int posX, int posY, int button)
{
    bool buttonPressed = ApplyButton(button) == 1;
    int fontSize = buttonPressed ? 20 : 16;
    int txtWid = MeasureText(text, fontSize);
    int txtPos = posX - txtWid / 2;
    DrawRectangleRounded((Rectangle){txtPos - 5, posY - 5, txtWid + 10, fontSize + 10}, 0.2, 10, WHITE);
    DrawText(text, txtPos, posY, fontSize, BLACK);
}

int ApplyButton(int button)
{
    bool buttonPressed = IsGamepadButtonPressed(0, button);
    if (buttonPressed)
    {
        switch (button)
        {
        case GAMEPAD_BUTTON_RIGHT_TRIGGER_1:
            headerSelection = (headerSelection + 1) % NUM_HEADER_OPTIONS;
            break;
        default:
            break;
        }
    }
    return buttonPressed ? 1 : 0;
}

void DrawRingBackground(void)
{
    for (int i = 0; i < 8; i++)
    {
        DrawRing(ringCenter, wheelRadius * 0.6, wheelRadius, i * 45 - 21.5, i * 45 + 21.5, 100, Fade(BLACK, 0.8f));
    }
    DrawCircleV(ringCenter, wheelRadius * 0.57, Fade(BLACK, 0.5f));
}