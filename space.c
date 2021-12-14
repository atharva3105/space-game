
#include <math.h>
#include "include/raylib.h"

//Struct Definition

typedef struct Player
{
    Rectangle rect;
    Vector2 speed;
    Color color;
} Player;

typedef struct Enemy
{
    Rectangle rect;
    Vector2 speed;
    bool active;
    Color color;
} Enemy;

typedef struct Shoot
{
    Rectangle rect;
    Vector2 speed;
    bool active;
    Color color;
} Shoot;

typedef enum
{
    STORAGE_POSITION_SCORE = 0,
    STORAGE_POSITION_HISCORE = 1
} StorageData;

#define NUM_SHOOTS 50
#define NUM_MAX_ENEMIES 50

static int screenWidth = 800;
static int screenHeight = 450;

static Player player;
static Enemy enemy[NUM_MAX_ENEMIES];
static Shoot shoot[NUM_SHOOTS];

static int shootRate;

static int activeEnemies;

// Module Functions Declaration (local)

static void InitGame(void);   // Initialize game
static void UpdateGame(void); // Update game (one frame)

int score;
int hiscore;
bool game_over = true;

int main()
{

    InitWindow(screenWidth, screenHeight, "Space Shooter");
    //loading image for background
    Image image = LoadImage("resource/background.png"); // Loaded in CPU memory (RAM)
    Texture2D texture = LoadTextureFromImage(image);
    UnloadImage(image);

    hiscore = 0;
    SaveStorageValue(STORAGE_POSITION_HISCORE, hiscore);

    InitGame();

    SetTargetFPS(60);

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        if (game_over)
        {
            score = LoadStorageValue(STORAGE_POSITION_SCORE);
            hiscore = LoadStorageValue(STORAGE_POSITION_HISCORE);

            BeginDrawing();

            ClearBackground(BLACK);
            DrawTexture(texture, screenWidth / 2 - texture.width / 2, screenHeight / 2 - texture.height / 2, WHITE);
            DrawText("SPACE SHOOTER", 210, 80, 40, RED);
            DrawText(TextFormat("SCORE: %i", score), 280, 130, 40, GREEN);
            DrawText(TextFormat("HI-SCORE: %i", hiscore), 210, 200, 50, GREEN);
            DrawText("Press ENTER to START", 250, 310, 20, RAYWHITE);

            EndDrawing();
            if (IsKeyPressed(KEY_ENTER))
            {
                InitGame();
                game_over = false;
            }
        }
        else
        {
            UpdateGame();

            // drawing game
            BeginDrawing();

            ClearBackground(BLACK);
            DrawTexture(texture, screenWidth / 2 - texture.width / 2, screenHeight / 2 - texture.height / 2, WHITE);

            DrawText(TextFormat("SCORE: %i", score), 10, 10, 20, LIME);

            DrawRectangleRec(player.rect, player.color);

            for (int i = 0; i < activeEnemies; i++)
            {
                if (enemy[i].active)
                    DrawRectangleRec(enemy[i].rect, enemy[i].color);
            }

            for (int i = 0; i < NUM_SHOOTS; i++)
            {
                if (shoot[i].active)
                    DrawRectangleRec(shoot[i].rect, shoot[i].color);
            }

            EndDrawing();
        }
    }
    UnloadTexture(texture);

    CloseWindow();
    return 0;
}

// Initialize game variables
void InitGame(void)
{
    activeEnemies = 10;
    shootRate = 0;
    score = 0;
    SaveStorageValue(STORAGE_POSITION_SCORE, score);

    // Initialize player
    player.rect.x = screenWidth / 2.0f;
    player.rect.y = screenHeight - 20;
    player.rect.width = 20;
    player.rect.height = 20;
    player.speed.x = 5;
    player.speed.y = 5;
    player.color = YELLOW;

    // Initialize enemies
    for (int i = 0; i < NUM_MAX_ENEMIES; i++)
    {
        enemy[i].rect.width = 20;
        enemy[i].rect.height = 20;
        enemy[i].rect.x = GetRandomValue(0, screenWidth);
        enemy[i].rect.y = GetRandomValue(-screenHeight, -20);
        enemy[i].speed.x = 5;
        enemy[i].speed.y = 5;
        enemy[i].active = true;
        enemy[i].color = BLUE;
    }

    // Initialize shoots
    for (int i = 0; i < NUM_SHOOTS; i++)
    {
        shoot[i].rect.x = player.rect.x;
        shoot[i].rect.y = player.rect.y + player.rect.height / 4;
        shoot[i].rect.width = 5;
        shoot[i].rect.height = 10;
        shoot[i].speed.x = 0;
        shoot[i].speed.y = -10;
        shoot[i].active = false;
        shoot[i].color = RED;
    }
}

// Update game (one frame)
void UpdateGame(void)
{
    // Player movement
    if (IsKeyDown(KEY_RIGHT))
        player.rect.x += player.speed.x;
    if (IsKeyDown(KEY_LEFT))
        player.rect.x -= player.speed.x;

    // Enemy behaviour
    for (int i = 0; i < activeEnemies; i++)
    {
        if (enemy[i].active)
        {
            enemy[i].rect.y += enemy[i].speed.y;

            if (enemy[i].rect.y > screenHeight)
            {
                enemy[i].rect.x = GetRandomValue(0, screenWidth);
                enemy[i].rect.y = GetRandomValue(-screenHeight, -20);
            }
        }
    }

    // Wall behaviour
    if (player.rect.x <= 0)
        player.rect.x = 0;
    if (player.rect.x + player.rect.width >= screenWidth)
        player.rect.x = screenWidth - player.rect.width;

    //Shoot initialization
    if (IsKeyDown(KEY_SPACE))
    {

        shootRate += 5;

        for (int i = 0; i < NUM_SHOOTS; i++)
        {
            if (!shoot[i].active && shootRate % 40 == 0)
            {

                shoot[i].rect.x = player.rect.x;
                shoot[i].rect.y = player.rect.y + player.rect.height / 4;
                shoot[i].active = true;
                break;
            }
        }
    }

    // Shoot logic
    for (int i = 0; i < NUM_SHOOTS; i++)
    {
        if (shoot[i].active)
        {
            // Movement
            shoot[i].rect.y += shoot[i].speed.y;

            // Collision with enemy
            for (int j = 0; j < activeEnemies; j++)
            {
                if (enemy[j].active)
                {
                    if (CheckCollisionRecs(player.rect, enemy[j].rect))
                    {
                        while (!IsKeyPressed(KEY_ENTER))
                        {
                            BeginDrawing();

                            ClearBackground(BLACK);

                            DrawText(TextFormat("GAME OVER"), 280, 130, 40, RED);
                            DrawText("Press ENTER ", 310, 200, 20, RAYWHITE);

                            EndDrawing();
                        }

                        game_over = true;
                    }
                    if (CheckCollisionRecs(shoot[i].rect, enemy[j].rect))
                    {
                        shoot[i].active = false;
                        enemy[j].rect.x = GetRandomValue(screenWidth, screenWidth + 1000);
                        enemy[j].rect.y = GetRandomValue(0, screenHeight - enemy[j].rect.height);
                        shootRate = 0;
                        score += 100;
                        if (hiscore < score)
                        {
                            hiscore = score;
                        }
                        if (score == 1000)
                        {
                            activeEnemies += 5;
                        }
                        if (score == 5000)
                        {
                            activeEnemies += 5;
                        }
                        if (score == 10000)
                        {
                            activeEnemies += 5;
                        }

                        SaveStorageValue(STORAGE_POSITION_SCORE, score);
                        SaveStorageValue(STORAGE_POSITION_HISCORE, hiscore);
                    }

                    if (shoot[i].rect.y <= 0) //goes above the screen
                    {
                        shoot[i].active = false;
                        shootRate = 0;
                    }
                }
            }
        }
    }
    //enemy and player collision
    for (int j = 0; j < activeEnemies; j++)
    {
        if (enemy[j].active)
        {
            if (CheckCollisionRecs(player.rect, enemy[j].rect))
            {
                while (!IsKeyPressed(KEY_ENTER))
                {
                    BeginDrawing();

                    ClearBackground(BLACK);

                    DrawText(TextFormat("GAME OVER"), 280, 130, 40, RED);
                    DrawText("Press ENTER ", 310, 200, 20, RAYWHITE);

                    EndDrawing();
                }

                game_over = true;
            }
        }
    }
}
