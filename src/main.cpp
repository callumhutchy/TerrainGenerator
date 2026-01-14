#include "raylib.h"
#include <math.h>
#include <iostream>
#include <stdlib.h>
#include <algorithm>
#include <cstring>
#include <stdio.h>
#include <string>
#include <filesystem>
#include "Profiler.h"
#include <iostream>
#include <fstream>

#include "RaylibHelpers.h"

#include "raylibExtras/rlImGui.h"
#include "imgui.h"
#include "raymath.h"
#include "MapGenerator.h"

std::filesystem::path currentPath;
std::string resourcePath;
std::string terrainTypeFile;

Texture heightMap;

DrawMode drawMode = DrawMode::NoiseMap;

bool regenerateHeightMap = false;

void ParseTerrainTypesFile()
{
    std::ifstream infile(terrainTypeFile);
    std::string line;
    while (std::getline(infile, line))
    {
        regions.push_back(TerrainType::DeserialiseFromString(line));
    }
}

int main()
{
    PROFILE_ME;
    currentPath = std::filesystem::current_path();
    resourcePath = currentPath.string() + "/resources/";
    terrainTypeFile = resourcePath + "TerrainTypes.csv";
    printf(terrainTypeFile.c_str());
    ParseTerrainTypesFile();

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Terrain Tester");

    Camera3D camera = Create3DPerspectiveCamera();

    heightMap = GenerateMap(drawMode);

    rlImGuiSetup(true);

    while (!WindowShouldClose())
    {
        {
            // Manage Controls
            ManageCameraControls(camera);

            // Regenerate map
            if (IsKeyPressed(KEY_R) || IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) || regenerateHeightMap)
            {
                heightMap = GenerateMap(drawMode);
                regenerateHeightMap = false;
            }
        }
        BeginDrawing();

        ClearBackground(RAYWHITE);
        BeginMode3D(camera);

        // Draw stuff

        EndMode3D();

        // Draw UI here

        // DrawTexture(heightMap, (GetScreenWidth() / 2) - (mapHeight / 2), GetScreenHeight() / 2 - (mapWidth / 2), WHITE);

        rlImGuiBegin();
        bool open = true;
        if (ImGui::Begin("Height Map Preview", &open))
        {
            // This makes the map no matter what the dimensions scale with the window
            rlImGuiImageSize(&heightMap, ImGui::GetWindowWidth() - 5, ImGui::GetWindowHeight() - 5);
        }
        ImGui::End();

        // Height Map Properties

        if (ImGui::Begin("Height Map Regenerate", &open))
        {
            ImGui::InputInt("Seed", &seed);
            ImGui::SliderInt("Map Size", &mapSize, 10, 1024);
            ImGui::SliderFloat("Scale", &scale, 0.0f, 200.0f);
            ImGui::SliderInt("Octaves", &octaves, 1, 20);
            ImGui::SliderFloat("Persistence", &persistence, 0.0f, 1.0f);
            ImGui::SliderFloat("Lacunarity", &lacunarity, 1.0f, 5.0f);
            ImGui::Checkbox("Normalise", &normalise);
            int drawModeNum = (int)drawMode;
            ImGui::Combo("Draw Mode", &drawModeNum, "Noise Map\0Colour Map\0\0");
            drawMode = (DrawMode)drawModeNum;
            regenerateHeightMap = ImGui::Button("Regenerate");
        }
        ImGui::End();

        // A Windows to add and remove regions
        if (ImGui::Begin("Region Management", &open))
        {
        }
        ImGui::End();

        // end ImGui Content
        rlImGuiEnd();

        EndDrawing();
    }

    rlImGuiShutdown();

    PROFILE_END;
    profiler::getInstance().print(std::cout, 60);
    CloseWindow();
    return 0;
}