#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <raylib.h>

#include "./gamevars.h"
#include "./functions.h"

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

int main(void)
{
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
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
        float midAngle = (1 / 360.0) * PI * (startAngles[i] + endAngles[i] - 180.0);
        segmentCenters[i] = (Vector2){wheelCenter.x + cos(midAngle) * wheelRadius * 0.8,
                                      wheelCenter.y - sin(midAngle) * wheelRadius * 0.8};
    }

    // Generate the test textures
    Image testImage = LoadImage("resources/images/fire.png");
    Image testGrayscaleImage = ImageCopy(testImage);
    ImageColorGrayscale(&testGrayscaleImage);
    ImageResize(&testImage, screenWidth, screenHeight);
    testTex = LoadTextureFromImage(testImage);
    grayscaleTestTex = LoadTextureFromImage(testGrayscaleImage);
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
        SetMusicVolume(music, 1.0 - selectedWheelOptions[2][0] / 2.0);
        if (!IsMusicStreamPlaying(music))
            PlayMusicStream(music);
        else
            UpdateMusicStream(music);

        if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_TRIGGER_1))
            wheelSelection = NULL_VAL;

        if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_TRIGGER_1))
        {
            DrawTexture(grayscaleTestTex, 0, 0, WHITE);
            DrawHeader();
            ApplyRightStick();
            DrawWheel();
            DrawWheelSelection();
        }
        else
        {
            DrawTexture(testTex, 0, 0, WHITE);
            DrawButton("LB", 50, screenHeight - 50, GAMEPAD_BUTTON_LEFT_TRIGGER_1, 20);
            DrawText("To open wheel", 75, screenHeight - 50, 20, WHITE);
            if (headerSelection == 2 && wheelSelection == 1) // TESTING
                DrawText("You can do it!", center.x - MeasureText("You can do it!", 70) / 2, center.y - 35, 70, GREEN);
        }
    }
    else
    {
        DrawText(startText, center.x - MeasureText(startText, 20) / 2, center.y - 20, 20, GRAY);
    }

    EndDrawing();
}

void UnloadGame(void)
{
    UnloadTexture(testTex);
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
        case GAMEPAD_BUTTON_RIGHT_TRIGGER_2:
            IncrementWheelSelection();
            framesCounter = 0;
            break;
        case GAMEPAD_BUTTON_LEFT_TRIGGER_2:
            DecrementWheelSelection();
            framesCounter = 0;
            break;
        default:
            break;
        }
    }
    return buttonPressed ? 1 : 0;
}

void ApplyRightStick(void)
{
    // Determine if the rightStickWheel vector is greater than the ring radius * 0.99
    Vector2 rightStick = (Vector2){GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_X), GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_Y)};
    Vector2 rightStickWheel = (Vector2){wheelCenter.x + rightStick.x * wheelRadius, wheelCenter.y + rightStick.y * wheelRadius};
    float rightStickWheelMag = sqrt(pow(rightStickWheel.x - wheelCenter.x, 2) + pow(rightStickWheel.y - wheelCenter.y, 2));

    if (rightStickWheelMag > wheelRadius * 0.99) // User is selecting a segment
    {
        float angle = atan2(rightStick.y, rightStick.x) * 180 / PI - 90;
        if (angle < 0)
            angle += 360;
        angle = abs((int)(360 - angle));

        for (int i = 0; i < NUM_WHEEL_OPTIONS; i++)
        {
            // Determine if the angle points towards the ring segement being drawn and
            // solve edge case crossing quadrant 4 -> quadrant 3
            if ((angle >= startAngles[i] && angle <= endAngles[i]) ||
                (i == 0 && angle > 360.0 - halfUsedAngleSpan && angle <= 360.0))
            {
                wheelSelection = i;
                framesCounter = 0;
            }
        }
    }
}

void DrawWheel(void)
{
    DrawCircleV(wheelCenter, wheelRadius * 0.57, Fade(BLACK, 0.5f)); // Draw the background
    DrawLineV((Vector2){wheelCenter.x - 90, wheelCenter.y - 50}, (Vector2){wheelCenter.x + 90, wheelCenter.y - 50}, WHITE);
    DrawLineV((Vector2){wheelCenter.x - 90, wheelCenter.y + 50}, (Vector2){wheelCenter.x + 90, wheelCenter.y + 50}, WHITE);
    for (int i = 0; i < NUM_WHEEL_OPTIONS; i++) // Draw the segments
    {
        DrawRing(wheelCenter, wheelRadius * 0.6, wheelRadius, startAngles[i], endAngles[i], 100, Fade(BLACK, 0.8f));
        GuiDrawIcon(wheelOptions[headerSelection][i][selectedWheelOptions[headerSelection][i]],
                    segmentCenters[i].x - 24, segmentCenters[i].y - 24, 3,
                    wheelOptionColors[headerSelection][i][selectedWheelOptions[headerSelection][i]]);
    }
    if (wheelSelection != NULL_VAL) // Determine if the user is not doing anything
    {
        framesCounter++;
        if (framesCounter > GetFPS() * 1.5)
            wheelSelection = NULL_VAL;
    }
}

void DrawWheelSelection(void)
{
    if (wheelSelection == NULL_VAL || wheelOptions[headerSelection][wheelSelection][0] == ICON_NONE)
    {
        DrawText(" Move RS to\nselect a tool", wheelCenter.x - MeasureText(" Move RS to\nselect a tool", 20) / 2, wheelCenter.y + 60, 20, WHITE);
        return;
    }
    // Write the name and description of the selected tool
    char *thisToolString = toolStrings[wheelOptions[headerSelection][wheelSelection][selectedWheelOptions[headerSelection][wheelSelection]]];
    DrawText(thisToolString, wheelCenter.x - MeasureText(thisToolString, 24) / 2, wheelCenter.y - 88, 24, WHITE);
    DrawText("Release LB to\n  apply tool", wheelCenter.x - MeasureText("Release LB to\n  apply tool", 20) / 2, wheelCenter.y + 60, 20, WHITE);

    float startAngle = startAngles[wheelSelection];
    float endAngle = endAngles[wheelSelection];
    Vector2 center = segmentCenters[wheelSelection];
    DrawRing(wheelCenter, wheelRadius * 0.95, wheelRadius, startAngle, endAngle, 100, Fade(MAROON, 0.8f));
    // Not a scrollable option, so don't activate trigger buttons
    if (wheelOptions[headerSelection][wheelSelection][1] == ICON_NONE)
        return;
    // Find the intersection of the line in the logical place for the LT/RT buttons
    float buttonProjection = -pow(center.y, 2) + 2 * center.y * wheelCenter.y - pow(wheelCenter.y, 2);
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
    else if (center.x > wheelCenter.x)
    {
        DrawButton("LT", (int)(wheelCenter.x + buttonInner), (int)center.y, GAMEPAD_BUTTON_LEFT_TRIGGER_2, 10);
        DrawButton("RT", (int)(wheelCenter.x + buttonOuter), (int)center.y, GAMEPAD_BUTTON_RIGHT_TRIGGER_2, 10);
    }
    else
    {
        DrawButton("LT", (int)(wheelCenter.x - buttonOuter), (int)center.y, GAMEPAD_BUTTON_LEFT_TRIGGER_2, 10);
        DrawButton("RT", (int)(wheelCenter.x - buttonInner), (int)center.y, GAMEPAD_BUTTON_RIGHT_TRIGGER_2, 10);
    }
}

void IncrementWheelSelection(void)
{
    int s = selectedWheelOptions[headerSelection][wheelSelection] + 1;
    if (s >= NUM_WHEEL_OPTIONS)
        s = 0;
    while (wheelOptions[headerSelection][wheelSelection][s] == ICON_NONE)
    {
        s++;
        if (s == NUM_WHEEL_OPTIONS)
        {
            s = 0;
            break;
        }
    }
    selectedWheelOptions[headerSelection][wheelSelection] = s;
}

void DecrementWheelSelection(void)
{
    int s = selectedWheelOptions[headerSelection][wheelSelection] - 1;
    if (s < 0)
        s = NUM_WHEEL_OPTIONS - 1;
    while (wheelOptions[headerSelection][wheelSelection][s] == ICON_NONE)
    {
        s--;
        if (s == 0)
            break;
    }
    selectedWheelOptions[headerSelection][wheelSelection] = s;
}
