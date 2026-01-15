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

    float *GenerateNoiseMap(int mapChunkSize, float scale, float octaves, float persistence, float lacunarity, bool normalise)
    {
        // 2D array block, access index with [y * mapWidth + x]
        float *noiseMap = new float[mapChunkSize * mapChunkSize];

        float maxNoiseHeight = std::numeric_limits<float>::min();
        float minNoiseHeight = std::numeric_limits<float>::max();

        float halfSize = mapChunkSize / 2;

        for (int y = 0; y < mapChunkSize; y++)
        {
            for (int x = 0; x < mapChunkSize; x++)
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
                noiseMap[y * mapChunkSize + x] = noiseHeight;
            }
        }
        if (normalise)
        {
            for (int y = 0; y < mapChunkSize; y++)
            {
                for (int x = 0; x < mapChunkSize; x++)
                {
                    noiseMap[y * mapChunkSize + x] = (noiseMap[y * mapChunkSize + x] - minNoiseHeight) / (maxNoiseHeight - minNoiseHeight);
                }
            }
        }
        return noiseMap;
    }
};