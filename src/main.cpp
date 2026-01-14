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

void SaveTerrainTypesToFile()
{
    std::ofstream outFile(terrainTypeFile, std::ios::trunc);
    for (int i = 0; i < regions.size(); i++)
    {
        outFile << regions[i].ToString() << "\n";
    }
    outFile.close();
}

void ParseTerrainTypesFile()
{
    std::ifstream infile(terrainTypeFile);
    std::string line;
    while (std::getline(infile, line))
    {
        regions.push_back(TerrainType::DeserialiseFromString(line));
    }
    infile.close();
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
            if (ImGui::Button("Save Terrain Types"))
            {
                SaveTerrainTypesToFile();
            }
            
            /* TODO
            if(ImGui::Button("Add Region"))
            {

            }
            */

            ImGui::Separator();
            // If any delete buttons are pressed, record them here and then handle after
            std::vector<int> regionsToDelete;
            for (int i = 0; i < regions.size(); i++)
            {
                std::string nameStr = regions[i].name;
                char *name = (char *)nameStr.c_str();
                ImGui::InputText(("Name##" + nameStr).c_str(), name, 32);
                regions[i].name = name;

                float oldHeight = regions[i].height;
                ImGui::InputFloat(("Height##" + nameStr).c_str(), &oldHeight, 0.0f, 1.0f);
                regions[i].height = oldHeight;
                
                float *oldColour = new float[3]{regions[i].color.r * (1.0f / 255.0f), regions[i].color.g * (1.0f / 255.0f), regions[i].color.b * (1.0f / 255.0f)};
                if (ImGui::ColorPicker3(("Colour##" + nameStr).c_str(), oldColour), ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoAlpha)
                    regions[i].color = {(unsigned char)(round(oldColour[0] * 255)), (unsigned char)(round(oldColour[1] * 255)), (unsigned char)(round(oldColour[2] * 255)), 255};
                
                    /* TODO - Manage deleting and adding regions
                if (ImGui::Button(("Delete##" + nameStr).c_str()))
                {
                    regionsToDelete.push_back(i);
                */
                
                ImGui::Separator();
            }

            // Delete regions
            /* TODO - It was deleting from the back regardless of what value for i was stored
            for (int i = regions.size() - 1; i >= 0; i--)
            {
                for (int j = 0; j < regionsToDelete.size(); j++)
                {
                    if (regionsToDelete[j] == i)
                        regions.erase(std::next(regions.begin() + i));
                }
            }
            */
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