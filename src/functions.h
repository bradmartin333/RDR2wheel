void InitGame(void);        // Initialize game
void UpdateGame(void);      // Update game (one frame)
void DrawGame(void);        // Draw game (one frame)
void UnloadGame(void);      // Unload game
void UpdateDrawFrame(void); // Update and Draw (one frame)
void DrawHeader(void);
void DrawButton(const char *text, int posX, int posY, int button, int fontSize);
int ApplyButton(int button);
void ApplyRightStick(void);
void DrawWheel(void);
void DrawWheelSelection(void);
void IncrementWheelSelection(void);
void DecrementWheelSelection(void);