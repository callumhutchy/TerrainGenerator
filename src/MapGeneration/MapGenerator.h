
#include "raylib.h"

#include "Noise.h"
#include <string>
#include <vector>
#include <exception>

struct TerrainType;

struct TerrainType
{
public:
    std::string name;
    float height;
    Color color;

    TerrainType() {}

    TerrainType(std::string _name, float _height, unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255)
    {
        name = _name;
        height = _height;
        color = {r, g, b, a};
    }

    TerrainType(std::string _name, float _height, Color _color)
    {
        name = _name;
        height = _height;
        color = _color;
    }

    std::string ToString()
    {
        std::string val = name + ",";
        val += std::to_string(height);
        val += ",";
        val += std::to_string(color.r) + "," + std::to_string(color.g) + "," + std::to_string(color.b) + "," + std::to_string(color.a);
        return val;
    }

    static TerrainType DeserialiseFromString(std::string serialised)
    {
        std::vector<std::string> splits;
        std::string temp = "";
        for (int i = 0; i < serialised.length(); i++)
        {
            if (serialised[i] == ',')
            {
                splits.push_back(temp);
                temp = "";
            }
            else
            {
                temp += serialised[i];
            }
        }
        splits.push_back(temp);

        if (splits.size() != 6)
        {
            throw std::runtime_error("Invalid number of elements in serialised Terrain Type");
        }

        TerrainType newTT;
        newTT.name = splits[0];
        newTT.height = atof(splits[1].c_str());
        newTT.color = {(unsigned char)atoi(splits[2].c_str()), (unsigned char)atoi(splits[3].c_str()), (unsigned char)atoi(splits[4].c_str()), (unsigned char)atoi(splits[5].c_str())};
        return newTT;
    }
};

enum DrawMode
{
    NoiseMap = 0,
    ColourMap = 1
};

int seed = 12345;
int mapSize = 1024;
float scale = 25.0f;
int octaves = 8;
float persistence = 0.5f;
float lacunarity = 2.0f;
bool normalise = true;

std::vector<TerrainType> regions{};

Texture2D DrawNoiseMap(float *noiseMap, unsigned int size)
{
    Image image;

    image.width = size;
    image.height = size;
    image.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
    image.mipmaps = 1;

    Color *colourMap = new Color[size * size];
    for (int y = 0; y < size; y++)
    {
        for (int x = 0; x < size; x++)
        {
            colourMap[y * size + x] = ColorLerp(BLACK, WHITE, noiseMap[y * size + x]);
        }
    }

    image.data = colourMap;

    return LoadTextureFromImage(image);
}

Texture2D DrawColourMap(float *noiseMap, unsigned int size)
{
    Image image;

    image.width = size;
    image.height = size;
    image.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
    image.mipmaps = 1;

    Color *colourMap = new Color[mapSize * mapSize];

    for (int y = 0; y < mapSize; y++)
    {
        for (int x = 0; x < mapSize; x++)
        {
            float currentHeight = noiseMap[y * mapSize + x];
            for (int i = 0; i < regions.size(); i++)
            {
                if (currentHeight <= regions[i].height)
                {
                    colourMap[y * mapSize + x] = regions[i].color;
                    break;
                }
            }
        }
    }

    image.data = colourMap;

    return LoadTextureFromImage(image);
}

Texture GenerateMap(DrawMode drawMode)
{
    Noise n{seed};
    float *noiseMap = n.GenerateNoiseMap(mapSize, scale, octaves, persistence, lacunarity, normalise);

    Texture heightMap;
    if (drawMode == DrawMode::NoiseMap)
    {
        heightMap = DrawNoiseMap(noiseMap, mapSize);
    }
    else if (drawMode == DrawMode::ColourMap)
    {
        heightMap = DrawColourMap(noiseMap, mapSize);
    }
    delete noiseMap;
    return heightMap;
}