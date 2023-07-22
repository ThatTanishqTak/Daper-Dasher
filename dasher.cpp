#include "raylib.h"
#include <iostream>

struct animData
{
    Rectangle rect;
    Vector2 pos;
    int frame;
    float updateTime, runningTime;
};

bool isonGround(animData data, int windowHeight)
{
    return data.pos.y > windowHeight - data.rect.height;
}

animData update(animData data, float deltaTime, int maxFrame)
{
    data.runningTime += deltaTime;
    if(data.runningTime >= data.updateTime)
    {
        data.runningTime = 0.0f;
        data.rect.x = data.frame * data.rect.width;
        data.frame++;
        if(data.frame > maxFrame)
            data.frame = 0;
    }

    return data;
}

int main()
{
    const int windowDimensions[2]{1080, 720};
    InitWindow(windowDimensions[0], windowDimensions[1], "Daper Dasher");

    const int gravity{1000}, jumpVelocity{-500};
    int velocity{0};
    bool inAir{false}, collision{false};

    int nebVelocity{-250};
    
    const int size{6};
    Texture2D nebula = LoadTexture("textures/12_nebula_spritesheet.png");
    animData nebulae[size]{};
    float finishLine{nebulae[size - 1].pos.x};

    for(int i = 0; i < size; i++)
    {
        nebulae[i].rect.x = 0.0f;
        nebulae[i].rect.y = 0.0f;
        nebulae[i].rect.width = nebula.width/8.0f;
        nebulae[i].rect.height = nebula.height/8.0f;

        nebulae[i].pos.y = windowDimensions[1] - nebula.height/8.0f;
        nebulae[i].frame = 0;
        nebulae[i].runningTime = 0.0f;
        nebulae[i].updateTime = 1.0f/16.0f;     
    }

    for(int i = 0; i < size; i++)
        nebulae[i].pos.x = windowDimensions[0] + 300 * i;

    Texture2D scarfy = LoadTexture("textures/scarfy.png");
    animData scarfyData{{0.0f, 0.0f, scarfy.width/6.0f, (float)scarfy.height},
                        {windowDimensions[0]/2.0f - scarfy.width/2.0f,
                        (float)windowDimensions[1] - scarfy.height * 2},
                        0, 1.0f/12.0f, 0};

    Texture2D background = LoadTexture("textures/far-buildings.png");
    Texture2D midground = LoadTexture("textures/back-buildings.png");
    Texture2D foreground = LoadTexture("textures/foreground.png");
    float bgX{}, mgX{}, fgX{};

    SetTargetFPS(60);
    while(!WindowShouldClose())
    {
        const float deltaTime{GetFrameTime()};

        BeginDrawing();
        ClearBackground(BLACK);

        bgX -= 20 * deltaTime;
        mgX -= 40 * deltaTime;
        fgX -= 80 * deltaTime;
        if(bgX <= -background.width * 5)
            bgX = 0.0f;
        if(mgX <= -midground.width * 5)
            mgX = 0.0f;
        if(fgX <= -foreground.width * 5)
            fgX = 0.0f;

        Vector2 bg1Pos{bgX, 0.0f};
        Vector2 bg2Pos{bgX + background.width * 5, 0.0f};
        Vector2 mg1Pos{mgX, 0.0f};
        Vector2 mg2Pos{mgX + midground.width * 5, 0.0f};
        Vector2 fg1Pos{fgX, 0.0f};
        Vector2 fg2Pos{fgX + foreground.width * 5, 0.0f};
        DrawTextureEx(background,bg1Pos, 0.0f, 5.0f, WHITE);
        DrawTextureEx(background,bg2Pos, 0.0f, 5.0f, WHITE);
        DrawTextureEx(midground,mg1Pos, 0.0f, 5.0f, WHITE);
        DrawTextureEx(midground,mg2Pos, 0.0f, 5.0f, WHITE);
        DrawTextureEx(foreground,fg1Pos, 0.0f, 5.0f, WHITE);
        DrawTextureEx(foreground,fg2Pos, 0.0f, 5.0f, WHITE);

        if(isonGround(scarfyData, windowDimensions[1]))
        {
            velocity = 0;
            inAir = false;
        }
        else
        {
            velocity += gravity * deltaTime;
            inAir = true;
        }

        if(IsKeyPressed(KEY_SPACE) && !inAir)
            velocity += jumpVelocity;

        for(int i = 0; i < size; i++)
            nebulae[i].pos.x += nebVelocity * deltaTime;

        finishLine += nebVelocity * deltaTime;
        scarfyData.pos.y += velocity * deltaTime;

        if(!inAir)
            scarfyData = update(scarfyData, deltaTime, 5);

        for(animData nebula : nebulae)
        {
            float pad{50};
            Rectangle nebRect{nebula.pos.x + pad, nebula.pos.y + pad, 
                              nebula.rect.width - 2 * pad, nebula.rect.height - 2 * pad};
            Rectangle scarfyRect{scarfyData.pos.x, scarfyData.pos.y,
                                 scarfyData.rect.width, scarfyData.rect.height};
            
            if(CheckCollisionRecs(nebRect, scarfyRect))
                collision = true;
        }

        if(collision)
            DrawText("GAME OVER!", windowDimensions[0] / 4, windowDimensions[1] / 2, 45, WHITE);
        else if(scarfyData.pos.x >= finishLine)
            DrawText("YOU WIN!", windowDimensions[0] / 4, windowDimensions[1] / 2, 45, GOLD);
        else
        {
            for(int i = 0; i < size; i++)
                nebulae[i] = update(nebulae[i], deltaTime, 7);
            DrawTextureRec(scarfy, scarfyData.rect, scarfyData.pos, WHITE);
            
            for(int i = 0; i < size; i++)
                DrawTextureRec(nebula, nebulae[i].rect, nebulae[i].pos, WHITE);
        }

        EndDrawing();
    }

    UnloadTexture(scarfy);
    UnloadTexture(nebula);
    UnloadTexture(background);
    UnloadTexture(midground);
    UnloadTexture(foreground);
    CloseWindow();

    return 0;
}