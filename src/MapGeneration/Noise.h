#include "PerlinNoise.hpp"
#include <chrono>
#include <limits>

class Noise
{
private:
    // Define and init seed and perlin, we will take arguments in the constuctor if they should be seeded externally
    siv::PerlinNoise::seed_type seed = std::chrono::seconds(std::time(NULL)).count();
    siv::PerlinNoise perlin{seed};

public:
    Noise() {}

    Noise(unsigned int _seed)
    {
        seed = _seed;
        perlin = siv::PerlinNoise{seed};
    }

    float *GenerateNoiseMap(int mapSize, float scale, float octaves, float persistence, float lacunarity, bool normalise)
    {
        // 2D array block, access index with [y * mapWidth + x]
        float *noiseMap = new float[mapSize * mapSize];

        float maxNoiseHeight = std::numeric_limits<float>::min();
        float minNoiseHeight = std::numeric_limits<float>::max();

        float halfSize = mapSize / 2;

        for (int y = 0; y < mapSize; y++)
        {
            for (int x = 0; x < mapSize; x++)
            {
                float amplitude = 1.0f;
                float frequency = 1.0f;
                float noiseHeight = 0.0f;

                for (int i = 0; i < octaves; i++)
                {
                    double fx = (x - halfSize) / scale * frequency;
                    double fy = (y - halfSize) / scale * frequency;

                    float perlinValue = perlin.noise2D(fx, fy);

                    noiseHeight += perlinValue * amplitude;

                    amplitude *= persistence;
                    frequency *= lacunarity;
                }
                if (normalise)
                {
                    if (noiseHeight > maxNoiseHeight)
                    {
                        maxNoiseHeight = noiseHeight;
                    }
                    else if (noiseHeight < minNoiseHeight)
                    {
                        minNoiseHeight = noiseHeight;
                    }
                }
                noiseMap[y * mapSize + x] = noiseHeight;
            }
        }
        if (normalise)
        {
            for (int y = 0; y < mapSize; y++)
            {
                for (int x = 0; x < mapSize; x++)
                {
                    noiseMap[y * mapSize + x] = (noiseMap[y * mapSize + x] - minNoiseHeight) / (maxNoiseHeight - minNoiseHeight);
                }
            }
        }
        return noiseMap;
    }
};