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

// Wheel
static int numWheelOptions = 8;
static const int wheelRadius = 225;
static const Vector2 ringCenter = {screenWidth / 2, screenHeight / 2 + 50};

// Wheel header
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
static void DrawCompleteRing(void);
static void DrawRingBackground(float angle);

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

    // Generate the test textures
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
            DrawCompleteRing();
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

void DrawCompleteRing(void)
{
    // Determine if the rightStickWheel vector is greater than the ring radius * 0.9
    Vector2 rightStick = (Vector2){GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_X), GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_Y)};
    Vector2 rightStickWheel = (Vector2){ringCenter.x + rightStick.x * wheelRadius, ringCenter.y + rightStick.y * wheelRadius};
    float rightStickWheelMag = sqrt(pow(rightStickWheel.x - ringCenter.x, 2) + pow(rightStickWheel.y - ringCenter.y, 2));
    float angle = 999.0;
    if (rightStickWheelMag > wheelRadius * 0.9)
    {
        // Get the angle of the right stick vector with straight down as 0 degrees
        // and stright right as 90 degrees
        angle = atan2(rightStick.y, rightStick.x) * 180 / PI - 90;
        if (angle < 0)
            angle += 360;
        angle = abs(360 - angle);
    }
    DrawRingBackground(angle);
}

void DrawRingBackground(float angle)
{
    float segmentAngleSpan = 360.0 / numWheelOptions;
    float halfUsedAngleSpan = (segmentAngleSpan - 2.0) / 2.0;
    for (int i = 0; i < numWheelOptions; i++)
    {
        float startAngle = i * segmentAngleSpan - halfUsedAngleSpan;
        float endAngle = i * segmentAngleSpan + halfUsedAngleSpan;
        // Determine if the angle points towards the ring segement being drawn
        bool selected = angle >= startAngle && angle <= endAngle;
        // Solve edge case crossing into quadrant 3 from quadrant 4
        if (i == 0 && angle > 360.0 - halfUsedAngleSpan && angle <= 360.0)
            selected = true;
        Color c = selected ? Fade(MAROON, 0.8f) : Fade(BLACK, 0.8f);
        DrawRing(ringCenter, wheelRadius * 0.6, wheelRadius, startAngle, endAngle, 100, c);
    }
    DrawCircleV(ringCenter, wheelRadius * 0.57, Fade(BLACK, 0.5f));
}