
#include "raylib.h"
#include <cstring>

/* Not Used, currently I'm using a modified raylib function to turn a map into a mesh
class MeshData
{
private:
    int triangleIndex = 0;

public:
    Vector3 *vertices;
    int vertexCount;
    unsigned short *triangles;
    int triCount;
    Vector2 *uvs;
    int uvsCount;

    MeshData(int meshSize)
    {
        vertices = new Vector3[meshSize * meshSize];
        uvs = new Vector2[meshSize * meshSize];
        triangles = new unsigned short[(meshSize - 1) * (meshSize - 1) * 6];
    }

    void AddTriangles(int a, int b, int c)
    {
        triangles[triangleIndex] = a;
        triangles[triangleIndex + 1] = b;
        triangles[triangleIndex + 2] = c;
        triangleIndex += 3;
    }

    Mesh CreateMesh()
    {
        Mesh mesh;
        mesh.vertices = new float[vertexCount * 3];
        int vertexIndex = 0;
        for (int i = 0; i < vertexCount; i++)
        {
            mesh.vertices[vertexIndex] = vertices[i].x;
            mesh.vertices[vertexIndex + 1] = vertices[i].y;
            mesh.vertices[vertexIndex + 2] = vertices[i].z;
            vertexIndex += 3;
        }
        mesh.vertexCount = triangleIndex * 3;
        mesh.indices = triangles;
        return mesh;
    }
};
*/

static Mesh GenerateMeshFromMap(float *map, int size, Color *colourMap, float heightMultiplier, int levelOfDetail)
{
    Mesh mesh = {0};

    int mapX = size;
    int mapZ = size;

    int meshSimplificationIncrement = levelOfDetail == 0 ? 1 : levelOfDetail * 2;
    int verticesPerLine = (size - 1) / meshSimplificationIncrement + 1;

    // NOTE: One vertex per pixel
    mesh.triangleCount = (mapX - 1) * (mapZ - 1) * 2; // One quad every four pixels

    mesh.vertexCount = mesh.triangleCount * 3;

    mesh.vertices = (float *)RL_MALLOC(mesh.vertexCount * 3 * sizeof(float));
    mesh.normals = (float *)RL_MALLOC(mesh.vertexCount * 3 * sizeof(float));
    mesh.texcoords = (float *)RL_MALLOC(mesh.vertexCount * 2 * sizeof(float));
    mesh.colors = (unsigned char *)RL_MALLOC(mesh.vertexCount * 4 * sizeof(unsigned char));

    int vCounter = 0;  // Used to count vertices float by float
    int tcCounter = 0; // Used to count texcoords float by float
    int nCounter = 0;  // Used to count normals float by float
    int cCounter = 0;

    Vector3 scaleFactor = {size / (mapX - 1), 255.0f / 255.0f, size / (mapZ - 1)};

    Vector3 vA = {0};
    Vector3 vB = {0};
    Vector3 vC = {0};
    Vector3 vN = {0};

    for (int z = 0; z < mapZ - 1; z+=meshSimplificationIncrement)
    {
        for (int x = 0; x < mapX - 1; x+=meshSimplificationIncrement)
        {
            // Fill vertices array with data
            //----------------------------------------------------------

            // one triangle - 3 vertex
            mesh.vertices[vCounter] = (float)x * scaleFactor.x;
            mesh.vertices[vCounter + 1] = map[x + z * mapX] * scaleFactor.y * heightMultiplier;
            mesh.vertices[vCounter + 2] = (float)z * scaleFactor.z;

            Color color = colourMap[x + z * mapX];

            mesh.colors[cCounter] = color.r;
            mesh.colors[cCounter + 1] = color.g;
            mesh.colors[cCounter + 2] = color.b;
            mesh.colors[cCounter + 3] = color.a;

            mesh.vertices[vCounter + 3] = (float)x * scaleFactor.x;
            mesh.vertices[vCounter + 4] = map[x + (z + 1) * mapX] * scaleFactor.y * heightMultiplier;
            mesh.vertices[vCounter + 5] = (float)(z + 1) * scaleFactor.z;

            color = colourMap[x + (z + 1) * mapX];

            mesh.colors[cCounter + 4] = color.r;
            mesh.colors[cCounter + 5] = color.g;
            mesh.colors[cCounter + 6] = color.b;
            mesh.colors[cCounter + 7] = color.a;

            mesh.vertices[vCounter + 6] = (float)(x + 1) * scaleFactor.x;
            mesh.vertices[vCounter + 7] = map[(x + 1) + z * mapX] * scaleFactor.y * heightMultiplier;
            mesh.vertices[vCounter + 8] = (float)z * scaleFactor.z;

            color = colourMap[(x + 1) + z * mapX];

            mesh.colors[cCounter + 8] = color.r;
            mesh.colors[cCounter + 9] = color.g;
            mesh.colors[cCounter + 10] = color.b;
            mesh.colors[cCounter + 11] = color.a;

            // Another triangle - 3 vertex
            mesh.vertices[vCounter + 9] = mesh.vertices[vCounter + 6];
            mesh.vertices[vCounter + 10] = mesh.vertices[vCounter + 7];
            mesh.vertices[vCounter + 11] = mesh.vertices[vCounter + 8];

            color = colourMap[(x + 1) + z * mapX];

            mesh.colors[cCounter + 12] = color.r;
            mesh.colors[cCounter + 13] = color.g;
            mesh.colors[cCounter + 14] = color.b;
            mesh.colors[cCounter + 15] = color.a;

            mesh.vertices[vCounter + 12] = mesh.vertices[vCounter + 3];
            mesh.vertices[vCounter + 13] = mesh.vertices[vCounter + 4];
            mesh.vertices[vCounter + 14] = mesh.vertices[vCounter + 5];

            color = colourMap[x + (z + 1) * mapX];

            mesh.colors[cCounter + 16] = color.r;
            mesh.colors[cCounter + 17] = color.g;
            mesh.colors[cCounter + 18] = color.b;
            mesh.colors[cCounter + 19] = color.a;

            mesh.vertices[vCounter + 15] = (float)(x + 1) * scaleFactor.x;
            mesh.vertices[vCounter + 16] = map[(x + 1) + (z + 1) * mapX] * scaleFactor.y * heightMultiplier;
            mesh.vertices[vCounter + 17] = (float)(z + 1) * scaleFactor.z;

            color = colourMap[(x + 1) + (z + 1) * mapX];

            mesh.colors[cCounter + 20] = color.r;
            mesh.colors[cCounter + 21] = color.g;
            mesh.colors[cCounter + 22] = color.b;
            mesh.colors[cCounter + 23] = color.a;

            vCounter += 18; // 6 vertex, 18 floats
            cCounter += 24;
            // Fill texcoords array with data
            //--------------------------------------------------------------
            mesh.texcoords[tcCounter] = (float)x / (mapX - 1);
            mesh.texcoords[tcCounter + 1] = (float)z / (mapZ - 1);

            mesh.texcoords[tcCounter + 2] = (float)x / (mapX - 1);
            mesh.texcoords[tcCounter + 3] = (float)(z + 1) / (mapZ - 1);

            mesh.texcoords[tcCounter + 4] = (float)(x + 1) / (mapX - 1);
            mesh.texcoords[tcCounter + 5] = (float)z / (mapZ - 1);

            mesh.texcoords[tcCounter + 6] = mesh.texcoords[tcCounter + 4];
            mesh.texcoords[tcCounter + 7] = mesh.texcoords[tcCounter + 5];

            mesh.texcoords[tcCounter + 8] = mesh.texcoords[tcCounter + 2];
            mesh.texcoords[tcCounter + 9] = mesh.texcoords[tcCounter + 3];

            mesh.texcoords[tcCounter + 10] = (float)(x + 1) / (mapX - 1);
            mesh.texcoords[tcCounter + 11] = (float)(z + 1) / (mapZ - 1);
            tcCounter += 12; // 6 texcoords, 12 floats

            // Fill normals array with data
            //--------------------------------------------------------------
            for (int i = 0; i < 18; i += 9)
            {
                vA.x = mesh.vertices[nCounter + i];
                vA.y = mesh.vertices[nCounter + i + 1];
                vA.z = mesh.vertices[nCounter + i + 2];

                vB.x = mesh.vertices[nCounter + i + 3];
                vB.y = mesh.vertices[nCounter + i + 4];
                vB.z = mesh.vertices[nCounter + i + 5];

                vC.x = mesh.vertices[nCounter + i + 6];
                vC.y = mesh.vertices[nCounter + i + 7];
                vC.z = mesh.vertices[nCounter + i + 8];

                vN = Vector3Normalize(Vector3CrossProduct(Vector3Subtract(vB, vA), Vector3Subtract(vC, vA)));

                mesh.normals[nCounter + i] = vN.x;
                mesh.normals[nCounter + i + 1] = vN.y;
                mesh.normals[nCounter + i + 2] = vN.z;

                mesh.normals[nCounter + i + 3] = vN.x;
                mesh.normals[nCounter + i + 4] = vN.y;
                mesh.normals[nCounter + i + 5] = vN.z;

                mesh.normals[nCounter + i + 6] = vN.x;
                mesh.normals[nCounter + i + 7] = vN.y;
                mesh.normals[nCounter + i + 8] = vN.z;
            }

            nCounter += 18; // 6 vertex, 18 floats
        }
    }

    // Upload vertex data to GPU (static mesh)
    UploadMesh(&mesh, false);

    return mesh;
}

/*
static MeshData GenerateTerrainMesh(float *heightMap, int mapChunkSize)
{

    float topLeftX = (mapChunkSize - 1) / -2.0f;
    float topLeftZ = (mapChunkSize - 1) / 2.0f;

    MeshData meshData(mapChunkSize);
    int vertexIndex = 0;

    for (int y = 0; y < mapChunkSize; y++)
    {
        for (int x = 0; x < mapChunkSize; x++)
        {
            meshData.vertices[vertexIndex] = {topLeftX + x, heightMap[y * mapChunkSize + x], topLeftZ - y};
            meshData.uvs[vertexIndex] = {x / (float)mapChunkSize, y / (float)mapChunkSize};

            if (x < mapChunkSize - 1 && y < mapChunkSize - 1)
            {
                meshData.AddTriangles(vertexIndex, vertexIndex + mapChunkSize + 1, vertexIndex + mapChunkSize);
                meshData.AddTriangles(vertexIndex + mapChunkSize + 1, vertexIndex, vertexIndex + 1);
            }

            vertexIndex++;
        }
    }

    meshData.vertexCount = vertexIndex;

    return meshData;
}
*/