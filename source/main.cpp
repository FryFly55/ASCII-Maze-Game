//
// Created by FryFly55 on 10.02.2023.
//

#include <iostream>
#include <Windows.h>
#include <cmath>
#include <chrono>

int consoleWidth = 120;
int consoleHeight = 40;

struct Map {
    const char* map;
    float playerX;
    float playerY;
    float goalX;
    float goalY;
    float playerLookAngle;
    int mapWidth;
    int mapHeight;
} map1, map2;

const double PI = 3.141593;
float FOV = (float)PI / 4.0f;

float renderDistance = 16.0f;
float incrementStep = 0.1f;
float perspective = 1.5f; // for smaller rooms, increase this value and change the wall shading to have better
// differentiation for closer walls (increase the numbers and put them closer together)
// small rooms: 1.5f for perspective, and 8, 6, 4, 2 for the shading
//big rooms: 1.2-1.3f for perspective, and 4, 3, 2, 1 for the shading

float moveSpeed = 5.0f;
float turnSpeed = 1.2f;

float hitboxSize = 0.2f;

float titleScreenDelay = 3.0f;
float gameEndScreenDelay = 3.0f;

char compass = 'S';



void executeGame(const char* map, float playerX, float playerY, float goalX, float goalY, float playerAngle, int mapWidth, int mapHeight, char* screen, HANDLE hConsole);
void displayText(const char* text, int row, int index, float durationInSeconds, int screenWidth, int screenHeight, char* screen, HANDLE hConsole);

int main()
{
    // define the first map.
    map1.map = "################"
               "#....#.........#"
               "#.#..#....######"
               "..#..####..#...#"
               "#.#........#.#.#"
               "#x##..######.#.#"
               "#........#...###"
               "#x######.###...#"
               "#....#.....###.#"
               "#..#.#.#####...#"
               "#..#.#.#.....###"
               "#..###.#..#..#.#"
               "#......#..#..#.#"
               "#..########.##.#"
               "#..............#"
               "################";
    map1.playerX = 2.0f;
    map1.playerY = 15.0f;
    map1.goalX = 1.0f;
    map1.goalY = 4.0f;
    map1.playerLookAngle = PI;
    map1.mapWidth = 16.0f;
    map1.mapHeight = 16.0f;

    // define the second map
    map2.map = "################"
               "#.....#.....#..#"
               "......#...###..#"
               "#...###.....#..#"
               "#.....####..#..#"
               "#.....#........#"
               "###........#####"
               "#.......####...#"
               "#..........#...#"
               "#######........#"
               "##.#...........#"
               "#........###...#"
               "#....#####.....#"
               "###......#...#.#"
               "#........#...#.#"
               "################";
    map2.playerX = 2.0f;
    map2.playerY = 15.0f;
    map2.goalX = 1.0f;
    map2.goalY = 4.0f;
    map2.playerLookAngle = PI / 2;
    map2.mapWidth = 16.0f;
    map2.mapHeight = 16.0f;

    // Create Screen Buffer
    char * screen = new char (consoleWidth * consoleHeight + 1);
    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, nullptr, CONSOLE_TEXTMODE_BUFFER, nullptr);
    SetConsoleActiveScreenBuffer(hConsole);
    DWORD byteWriteCount = 0;

    // setting the buffer to be empty
    for (int i = 0; i < consoleWidth * consoleHeight; i++) {
        screen[i] = ' ';
    }

    int screenLen = consoleWidth * consoleHeight;

    displayText("MAZE GAME", 15, (consoleWidth - strlen("MAZE GAME")) / 2, 3, 120, 40, screen, hConsole);

    //_________________________________________________________________________________________________________________
    // start first level and print finish screen after.
    executeGame(map1.map, map1.playerX, map1.playerY, map1.goalX, map1.goalY, map1.playerLookAngle, map1.mapWidth, map1.mapHeight, screen, hConsole);
    float seconds = 0.0f;
    auto last = std::chrono::system_clock::now();
    while(seconds <= gameEndScreenDelay) {
        const char* text = "FINISHED LEVEL I";
        int row = 15;
        int index = (consoleWidth - strlen(text)) / 2;
        for (int i = 0; i < strlen(text); i++) {
            screen[(row * consoleWidth) + index + i] = text[i];
            screen[((row * consoleWidth) + index + i) + 120] = ' ';
            screen[((row * consoleWidth) + index + i) - 120] = ' ';
        }

        auto current = std::chrono::system_clock::now();
        std::chrono::duration<float> addedTime = current - last;
        last = current;
        float addTime = addedTime.count();
        seconds += addTime;

        screen[screenLen] = '\0';

        SetConsoleCP(CP_WINUNICODE);
        SetConsoleOutputCP(CP_WINUNICODE);
        WriteConsoleOutputCharacter(hConsole, screen, consoleWidth * consoleHeight, {0, 0},
                                    &byteWriteCount);
    }

    //_______________________________________________________________________________________________________________
    // start second level and print finish after.
    executeGame(map2.map, map2.playerX, map2.playerY, map2.goalX, map2.goalY, map2.playerLookAngle, map2.mapWidth, map2.mapHeight, screen, hConsole);
    seconds = 0.0f;
    last = std::chrono::system_clock::now();
    while(seconds <= gameEndScreenDelay) {
        const char* text = "YOU FINISHED LEVEL II, CONGRATS!";
        int row = 15;
        int index = (consoleWidth - strlen(text)) / 2;
        for (int i = 0; i < strlen(text); i++) {
            screen[(row * consoleWidth) + index + i] = text[i];
            screen[((row * consoleWidth) + index + i) + 120] = ' ';
            screen[((row * consoleWidth) + index + i) - 120] = ' ';
        }

        auto current = std::chrono::system_clock::now();
        std::chrono::duration<float> addedTime = current - last;
        last = current;
        float addTime = addedTime.count();
        seconds += addTime;

        screen[screenLen] = '\0';

        SetConsoleCP(CP_WINUNICODE);
        SetConsoleOutputCP(CP_WINUNICODE);
        WriteConsoleOutputCharacter(hConsole, screen, consoleWidth * consoleHeight, {0, 0},
                                    &byteWriteCount);
    }

    return 0;
}

// how to execute each map
void executeGame(const char* map, float playerX, float playerY, float goalX, float goalY, float playerAngle, int mapWidth, int mapHeight, char* screen, HANDLE hConsole) {
    int screenLen = consoleWidth * consoleHeight;

    // setting the time points for dt
    auto tp1 = std::chrono::system_clock::now();
    auto tp2 = std::chrono::system_clock::now();

    // Game loop
    while(1) {
        tp2 = std::chrono::system_clock::now();
        std::chrono::duration<float> elapsedTime = tp2 - tp1;
        tp1 = tp2;
        float dt = elapsedTime.count();

        // Controls
        float movX = sinf(playerAngle) * moveSpeed * dt;
        float movY = cosf(playerAngle) * moveSpeed * dt;
        float strafeMovX = movY;
        float strafeMovY = movX;

        // Rotation
        if (GetAsyncKeyState((unsigned short)'J') & 0x8000)
            playerAngle -= turnSpeed * dt;

        if (GetAsyncKeyState((unsigned short) 'L') & 0x8000)
            playerAngle += turnSpeed * dt;

        // walking forward
        if (GetAsyncKeyState((unsigned short)'W') & 0x8000) {
            int testX = (int)(playerX + sinf(playerAngle) * hitboxSize);
            int testY = (int)(playerY + cosf(playerAngle) * hitboxSize);
            if (map[testY * mapWidth + testX] != '#') {
                playerX += movX;
                playerY += movY;
            }
        }

        // walking backwards
        if (GetAsyncKeyState((unsigned short)'S') & 0x8000) {
            int testX = (int)(playerX - sinf(playerAngle) * hitboxSize);
            int testY = (int)(playerY - cosf(playerAngle) * hitboxSize);
            if (map[testY * mapWidth + testX] != '#') {
                playerX -= movX;
                playerY -= movY;
            }
        }

        // strafing left
        if (GetAsyncKeyState((unsigned short)'A') & 0x8000) {
            int testX = (int)(playerX - sinf(playerAngle) * hitboxSize);
            int testY = (int)(playerY + cosf(playerAngle) * hitboxSize);
            if (map[testY * mapWidth + testX] != '#') {
                playerX -= strafeMovX;
                playerY += strafeMovY;
            }
        }

        // strafing right
        if (GetAsyncKeyState((unsigned short)'D') & 0x8000) {
            int testX = (int)(playerX + sinf(playerAngle) * hitboxSize);
            int testY = (int)(playerY - cosf(playerAngle) * hitboxSize);
            if (map[testY * mapWidth + testX] != '#') {
                playerX += strafeMovX;
                playerY -= strafeMovY;
            }
        }

        // graphic calculations and shading
        for (int x = 0; x < consoleWidth; x++) {
            // cast 120 rays, each representing a column in the console
            float rayAngle = (playerAngle - FOV / 2.0f) + ((float)x / (float)consoleWidth) * FOV;
            float distanceToWall = 0;

            float rayAngleX = sinf(rayAngle);
            float rayAngleY = cosf(rayAngle);

            bool hitWall = false;
            while(!hitWall && distanceToWall < renderDistance) {
                distanceToWall += incrementStep;

                int testX = (int)(playerX + rayAngleX * distanceToWall);
                int testY = (int)(playerY + rayAngleY * distanceToWall);

                // Test if ray is out of bounds
                if (testX < 0 || testX >= mapWidth || testY < 0 || testY >= mapHeight) {
                    hitWall = true;
                    distanceToWall = renderDistance;
                }
                    // ray is inbounds to test to see if the ray is hitting a wall
                else if (map[testY * mapWidth + testX] == '#') {
                    hitWall = true;
                }
            }

            // calculate distance to ceiling and floor
            int ceiling = (float)(consoleHeight / 2.0) - consoleHeight / ((float)distanceToWall * perspective);
            int floor = consoleHeight - ceiling;

            // calculate shade
            short shade = ' ';
            if (distanceToWall <= renderDistance / 8) // very close
                shade = '#';
            else if (distanceToWall < renderDistance / 6)
                shade = 'I';
            else if (distanceToWall < renderDistance / 4)
                shade = '=';
            else if (distanceToWall < renderDistance / 2)
                shade = '\'';
            else if (distanceToWall < renderDistance)
                shade = '.';
            else
                shade = ' '; // too far away

            // shade everything in, the walls based on distance, and the floor based on how high it is,
            // relative to the height of the console (std. 40 rows)
            for (int y = 0; y < consoleHeight; y++) {
                if (y < ceiling) {
                    screen[y * consoleWidth + x] = ' ';
                }
                else if (y > ceiling && y < floor) {
                    screen[y * consoleWidth + x] = shade;
                }
                else if (x % 2 == 0){
                    screen[y * consoleWidth + x] = ' ';
                }
                else if (y > (consoleHeight * 0.4f) && y <= (consoleHeight * 0.6f)){
                    screen[y * consoleWidth + x] = ' ';
                }
                else if (y > (consoleHeight * 0.6f) && y <= (consoleHeight * 0.8f)){
                    screen[y * consoleWidth + x] = '.';
                }
                else if (y > (consoleHeight * 0.8f)) {
                    screen[y * consoleWidth + x] = '_';
                }
                else {
                    screen[y * consoleWidth + x] = ' ';
                }
            }
        }

        // Calculate the UI a little later, so it is on top
        float cleanPlayerAngle = playerAngle - (((int)(playerAngle / (2 * PI))) * 2 * PI);
        if (cleanPlayerAngle < 0.25 * PI && cleanPlayerAngle > 1.75 * PI) {
            compass = 'S';
        }
        else if (cleanPlayerAngle < 0.75 * PI && cleanPlayerAngle > 0.25 * PI) {
            compass = 'W';
        }
        else if (cleanPlayerAngle < 1.25 * PI && cleanPlayerAngle > 0.75 * PI) {
            compass = 'N';
        }
        else if (cleanPlayerAngle < 1.75 * PI && cleanPlayerAngle > 1.25 * PI) {
            compass = 'E';
        }
        else {
            compass = 'S';
        }
        screen[consoleWidth + (consoleWidth / 2)] = compass;
        screen[consoleWidth + (consoleWidth / 2) + 1] = '-';
        screen[consoleWidth + (consoleWidth / 2) - 1] = '-';
        screen[consoleWidth + (consoleWidth / 2) + 120] = '|';
        screen[consoleWidth + (consoleWidth / 2) - 120] = '|';

        // Game logic, checking if player reached the goal.
        bool reachedGoal = false;
        if ((playerX < goalX + 0.5f && playerX > goalX - 0.5f) && (playerY < goalY + 0.5f && playerY > goalY - 0.5f)) {
            break;
        }

        screen[screenLen] = '\0';

        DWORD byteWriteCount = 0;
        SetConsoleCP(CP_WINUNICODE);
        SetConsoleOutputCP(CP_WINUNICODE);
        WriteConsoleOutputCharacter(hConsole, screen, consoleWidth * consoleHeight, {0, 0},
                                    &byteWriteCount);
    }

}

void displayText(const char* text, int row, int index, float durationInSeconds, int screenWidth, int screenHeight, char* screen, HANDLE hConsole) {
    int screenLen = screenWidth * screenHeight;
    float seconds = 0.0f;
    auto last = std::chrono::system_clock::now();
    while(seconds <= durationInSeconds) {
        for (int i = 0; i < strlen(text); i++) {
            screen[(row * consoleWidth) + index + i] = text[i];
            screen[((row * consoleWidth) + index + i) + 120] = ' ';
            screen[((row * consoleWidth) + index + i) - 120] = ' ';
        }

        auto current = std::chrono::system_clock::now();
        std::chrono::duration<float> addedTime = current - last;
        last = current;
        float addTime = addedTime.count();
        seconds += addTime;

        screen[screenLen] = '\0';

        DWORD byteWriteCount;
        SetConsoleCP(CP_WINUNICODE);
        SetConsoleOutputCP(CP_WINUNICODE);
        WriteConsoleOutputCharacter(hConsole, screen, consoleWidth * consoleHeight, {0, 0},
                                    &byteWriteCount);

    }
}