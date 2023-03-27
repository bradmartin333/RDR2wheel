#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <raylib.h>

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

#define NULL_VAL 999
#define NUM_HEADER_OPTIONS 3
#define NUM_WHEEL_OPTIONS 8

// Game environment
static const int screenWidth = 800;
static const int screenHeight = 600;
static const Vector2 center = {screenWidth / 2, screenHeight / 2};
static int framesCounter = 0;
static Texture2D TestTex;
static Texture2D GrayscaleTestTex;
static Music music;

// Wheel header
static const Rectangle wheelHeader = {screenWidth / 2 - 150, 25, 300, 75};
static const char *headerOptions[NUM_HEADER_OPTIONS] = {"Camera", "Processing", "Tools"};
static int headerSelection = 1;

// Wheel
static const int wheelRadius = 225;
static const Vector2 wheelCenter = {screenWidth / 2, screenHeight / 2 + 50};
static int wheelSelection = NULL_VAL;
static float segmentAngleSpan;
static float halfUsedAngleSpan;
static float startAngles[NUM_WHEEL_OPTIONS];
static float endAngles[NUM_WHEEL_OPTIONS];

static void InitGame(void);        // Initialize game
static void UpdateGame(void);      // Update game (one frame)
static void DrawGame(void);        // Draw game (one frame)
static void UnloadGame(void);      // Unload game
static void UpdateDrawFrame(void); // Update and Draw (one frame)
static void DrawHeader(void);
static void DrawButton(const char *text, int posX, int posY, int button, int fontSize);
static int ApplyButton(int button);
static void DrawWheel(void);
static void DrawWheelSelection(void);

char *IntToString(int num)
{
    char *str = (char *)malloc(10 * sizeof(char));
    sprintf(str, "%d", num);
    return str;
}

char *FloatToString(float num)
{
    char *str = (char *)malloc(10 * sizeof(char));
    sprintf(str, "%f", num);
    return str;
}

int main(void)
{
    InitWindow(screenWidth, screenHeight, "RDR2 Wheel");
    InitGame();
    InitAudioDevice();
    music = LoadMusicStream("resources/sounds/western.mp3");

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

    // Init some wheel parameters
    segmentAngleSpan = 360.0 / NUM_WHEEL_OPTIONS;
    halfUsedAngleSpan = (segmentAngleSpan - 2.0) / 2.0;
    for (int i = 0; i < NUM_WHEEL_OPTIONS; i++)
    {
        startAngles[i] = i * segmentAngleSpan - halfUsedAngleSpan;
        endAngles[i] = i * segmentAngleSpan + halfUsedAngleSpan;
    }

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
        if (!IsMusicStreamPlaying(music))
            PlayMusicStream(music);
        else
            UpdateMusicStream(music);

        if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_TRIGGER_1))
        {
            DrawTexture(GrayscaleTestTex, 0, 0, WHITE);
            DrawHeader();
            DrawWheel();
            if (wheelSelection != NULL_VAL)
                DrawWheelSelection();
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
    UnloadMusicStream(music);
    CloseAudioDevice();
}

void UpdateDrawFrame(void)
{
    UpdateGame();
    DrawGame();
}

void DrawHeader(void)
{
    DrawRectangleRounded(wheelHeader, 0.2, 10, Fade(BLACK, 0.8f));
    DrawButton("RB", (int)center.x, 38, GAMEPAD_BUTTON_RIGHT_TRIGGER_1, 16);

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

void DrawButton(const char *text, int posX, int posY, int button, int fontSize)
{
    if (ApplyButton(button) == 1)
        fontSize += 4;
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
            wheelSelection = NULL_VAL;
            break;
        default:
            break;
        }
    }
    return buttonPressed ? 1 : 0;
}

void DrawWheel(void)
{
    DrawCircleV(wheelCenter, wheelRadius * 0.57, Fade(BLACK, 0.5f));

    // Determine if the rightStickWheel vector is greater than the ring radius * 0.99
    Vector2 rightStick = (Vector2){GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_X), GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_Y)};
    Vector2 rightStickWheel = (Vector2){wheelCenter.x + rightStick.x * wheelRadius, wheelCenter.y + rightStick.y * wheelRadius};
    float rightStickWheelMag = sqrt(pow(rightStickWheel.x - wheelCenter.x, 2) + pow(rightStickWheel.y - wheelCenter.y, 2));

    float angle = NULL_VAL;
    if (rightStickWheelMag > wheelRadius * 0.25) // User is trying to do something
    {
        // Get the angle of the right stick vector with straight down as 0 degrees
        // and straight right as 90 degrees
        angle = atan2(rightStick.y, rightStick.x) * 180 / PI - 90;
        if (angle < 0)
            angle += 360;
        angle = abs((int)(360 - angle));
    }

    if (rightStickWheelMag > wheelRadius * 0.99) // User is selecting a segment
    {
        for (int i = 0; i < NUM_WHEEL_OPTIONS; i++)
        {
            // Determine if the angle points towards the ring segement being drawn and
            // solve edge case crossing quadrant 4 -> quadrant 3
            if ((angle >= startAngles[i] && angle <= endAngles[i]) ||
                (i == 0 && angle > 360.0 - halfUsedAngleSpan && angle <= 360.0))
                wheelSelection = i;
        }
    }
    else if (rightStickWheelMag > wheelRadius * 0.35 && wheelSelection != NULL_VAL)
    {
        // User is trying to deselect, convert to radians for simplicity
        angle *= PI / 180.0;
        // Determine the opposite start and end angles of the currently selected ring segment in radians
        float startAngle = ((wheelSelection * segmentAngleSpan - halfUsedAngleSpan) - 180) * PI / 180.0;
        float endAngle = ((wheelSelection * segmentAngleSpan + halfUsedAngleSpan) - 180) * PI / 180.0;
        // Normalize the angles
        if (startAngle < 0)
            startAngle += 2 * PI;
        if (endAngle < 0)
            endAngle += 2 * PI;
        // Determine if the angle points towards the ring segement being drawn and
        // solve edge case of crossing quadrant 1,2 -> quadrant 3,4
        if ((endAngle > angle && angle > startAngle) ||
            (wheelSelection == NUM_WHEEL_OPTIONS / 2 && endAngle > angle && angle < startAngle && rightStick.y > 0))
            wheelSelection = NULL_VAL;
    }

    // Draw the segments
    for (int i = 0; i < NUM_WHEEL_OPTIONS; i++)
    {
        DrawRing(wheelCenter, wheelRadius * 0.6, wheelRadius, startAngles[i], endAngles[i], 100, Fade(BLACK, 0.8f));
    }
}

void DrawWheelSelection(void)
{
    // Determine the start and end angles of the ring segment and draw it
    float startAngle = startAngles[wheelSelection];
    float endAngle = endAngles[wheelSelection];
    float midAngle = (1 / 360.0) * PI * (startAngle + endAngle - 180.0);
    DrawRing(wheelCenter, wheelRadius * 0.95, wheelRadius, startAngle, endAngle, 100, Fade(MAROON, 0.8f));
    Vector2 segmentCenter = (Vector2){wheelCenter.x + cos(midAngle) * wheelRadius * 0.8,
                                      wheelCenter.y - sin(midAngle) * wheelRadius * 0.8};
    DrawCircleV(segmentCenter, 10, Fade(MAROON, 0.8f));
    // Find the intersection of the line in the logical place for the LT/RT buttons
    float buttonProjection = -pow(segmentCenter.y, 2) + 2 * segmentCenter.y * wheelCenter.y - pow(wheelCenter.y, 2);
    int buttonInner = sqrt(buttonProjection + pow(wheelRadius * 0.625, 2));
    int buttonOuter = sqrt(buttonProjection + pow(wheelRadius, 2));
    // Finally place the buttons depending on the angle of the segment
    if (isnan(buttonInner) || abs(buttonInner) > NULL_VAL)
    {
        double r = wheelRadius * 0.8;
        float RTangle = (startAngle >= 0 && startAngle <= 180) ? startAngle : endAngle;
        float LTangle = (startAngle >= 0 && startAngle <= 180) ? endAngle : startAngle;
        DrawButton("LT", (int)(r * cos((RTangle + 90.0) * PI / 180.0) + wheelCenter.x),
                   (int)(r * sin((RTangle + 90.0) * PI / 180.0) + wheelCenter.y), GAMEPAD_BUTTON_LEFT_TRIGGER_2, 10);
        DrawButton("RT", (int)(r * cos((LTangle + 90.0) * PI / 180.0) + wheelCenter.x),
                   (int)(r * sin((LTangle + 90.0) * PI / 180.0) + wheelCenter.y), GAMEPAD_BUTTON_RIGHT_TRIGGER_2, 10);
    }
    else if (midAngle >= -PI / 2.0 && midAngle <= PI / 2.0)
    {
        DrawButton("LT", (int)(wheelCenter.x + buttonInner), (int)segmentCenter.y, GAMEPAD_BUTTON_LEFT_TRIGGER_2, 10);
        DrawButton("RT", (int)(wheelCenter.x + buttonOuter), (int)segmentCenter.y, GAMEPAD_BUTTON_RIGHT_TRIGGER_2, 10);
    }
    else
    {
        DrawButton("LT", (int)(wheelCenter.x - buttonOuter), (int)segmentCenter.y, GAMEPAD_BUTTON_LEFT_TRIGGER_2, 10);
        DrawButton("RT", (int)(wheelCenter.x - buttonInner), (int)segmentCenter.y, GAMEPAD_BUTTON_RIGHT_TRIGGER_2, 10);
    }
}
