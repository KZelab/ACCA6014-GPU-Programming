#include <GL/glew.h>  // MUST be included before any OpenGL headers

#include "OBJLoader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>

OBJLoader::OBJLoader()
{
}

OBJLoader::~OBJLoader()
{
}

Model* OBJLoader::LoadFromFile(const std::string& filepath, ProgressCallback progressCallback)
{
    std::cout << "Loading OBJ file: " << filepath << std::endl;

    // Temporary storage for OBJ data
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texCoords;

    // Final vertex and index data
    std::vector<Model::Vertex> vertices;
    std::vector<unsigned int> indices;

    // Parse the OBJ file
    if (!ParseOBJ(filepath, positions, normals, texCoords, vertices, indices, progressCallback))
    {
        std::cerr << "Failed to parse OBJ file: " << filepath << std::endl;
        return nullptr;
    }

    // Create model and set mesh data
    Model* model = new Model();
    model->SetMeshData(vertices, indices);

    std::cout << "Model loaded successfully!" << std::endl;
    std::cout << "  Vertices: " << model->GetVertexCount() << std::endl;
    std::cout << "  Triangles: " << model->GetTriangleCount() << std::endl;
    std::cout << "  Bounds: (" << model->GetBoundsMin().x << ", " << model->GetBoundsMin().y << ", " << model->GetBoundsMin().z << ") to ("
              << model->GetBoundsMax().x << ", " << model->GetBoundsMax().y << ", " << model->GetBoundsMax().z << ")" << std::endl;
    std::cout << "  Size: " << model->GetBoundsSize().x << " x " << model->GetBoundsSize().y << " x " << model->GetBoundsSize().z << std::endl;

    return model;
}

size_t OBJLoader::CountLines(const std::string& filepath)
{
    std::ifstream file(filepath);
    if (!file.is_open())
        return 0;

    size_t count = 0;
    std::string line;
    while (std::getline(file, line))
        ++count;

    return count;
}

bool OBJLoader::ParseOBJ(const std::string& filepath,
                        std::vector<glm::vec3>& positions,
                        std::vector<glm::vec3>& normals,
                        std::vector<glm::vec2>& texCoords,
                        std::vector<Model::Vertex>& outVertices,
                        std::vector<unsigned int>& outIndices,
                        ProgressCallback progressCallback)
{
    std::ifstream file(filepath);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file: " << filepath << std::endl;
        return false;
    }

    // Pre-allocate approximate sizes (helps with performance)
    positions.reserve(10000000);  // Reserve 10M positions
    normals.reserve(10000000);
    texCoords.reserve(5000000);
    outVertices.reserve(10000000);
    outIndices.reserve(60000000);  // ~20M triangles * 3

    // For progress tracking
    size_t lineCount = 0;
    size_t progressUpdateInterval = 100000;  // Update every 100k lines

    // Hash map to avoid duplicate vertices (vertex deduplication)
    // Key: "posIdx/texIdx/normIdx" -> Value: vertex index in outVertices
    std::unordered_map<std::string, unsigned int> vertexCache;

    std::string line;
    while (std::getline(file, line))
    {
        ++lineCount;

        // Show progress periodically
        if (progressCallback && lineCount % progressUpdateInterval == 0)
        {
            progressCallback(lineCount, 0, "Parsing OBJ file...");
        }

        // Skip empty lines and comments
        if (line.empty() || line[0] == '#')
            continue;

        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "v")
        {
            // Vertex position
            glm::vec3 pos;
            iss >> pos.x >> pos.y >> pos.z;
            positions.push_back(pos);
        }
        else if (prefix == "vn")
        {
            // Vertex normal
            glm::vec3 norm;
            iss >> norm.x >> norm.y >> norm.z;
            normals.push_back(norm);
        }
        else if (prefix == "vt")
        {
            // Texture coordinate
            glm::vec2 tex;
            iss >> tex.x >> tex.y;
            texCoords.push_back(tex);
        }
        else if (prefix == "f")
        {
            // Face (triangle)
            // Format: f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3
            std::string vertexStr;
            std::vector<unsigned int> faceIndices;

            while (iss >> vertexStr)
            {
                // Check if we've already created this exact vertex
                auto it = vertexCache.find(vertexStr);
                if (it != vertexCache.end())
                {
                    // Reuse existing vertex
                    faceIndices.push_back(it->second);
                }
                else
                {
                    // Parse vertex string: "v/vt/vn"
                    int posIdx = 0, texIdx = 0, normIdx = 0;
                    char slash;

                    std::istringstream viss(vertexStr);
                    viss >> posIdx >> slash >> texIdx >> slash >> normIdx;

                    // Convert from 1-based to 0-based indexing
                    posIdx--;
                    texIdx--;
                    normIdx--;

                    // Create new vertex
                    Model::Vertex vertex;

                    if (posIdx >= 0 && posIdx < (int)positions.size())
                        vertex.position = positions[posIdx];

                    if (normIdx >= 0 && normIdx < (int)normals.size())
                        vertex.normal = normals[normIdx];

                    if (texIdx >= 0 && texIdx < (int)texCoords.size())
                        vertex.texCoord = texCoords[texIdx];

                    // Add to output vertices
                    unsigned int vertexIndex = (unsigned int)outVertices.size();
                    outVertices.push_back(vertex);
                    faceIndices.push_back(vertexIndex);

                    // Cache this vertex
                    vertexCache[vertexStr] = vertexIndex;
                }
            }

            // Add face indices (must be a triangle)
            if (faceIndices.size() == 3)
            {
                outIndices.push_back(faceIndices[0]);
                outIndices.push_back(faceIndices[1]);
                outIndices.push_back(faceIndices[2]);
            }
            else if (faceIndices.size() > 3)
            {
                // Triangulate polygon (simple fan triangulation)
                for (size_t i = 1; i < faceIndices.size() - 1; ++i)
                {
                    outIndices.push_back(faceIndices[0]);
                    outIndices.push_back(faceIndices[i]);
                    outIndices.push_back(faceIndices[i + 1]);
                }
            }
        }
    }

    if (progressCallback)
    {
        progressCallback(lineCount, lineCount, "Parsing complete!");
    }

    std::cout << "Parsed " << lineCount << " lines" << std::endl;
    std::cout << "Raw data: " << positions.size() << " positions, "
              << normals.size() << " normals, "
              << texCoords.size() << " texCoords" << std::endl;
    std::cout << "Final mesh: " << outVertices.size() << " unique vertices, "
              << outIndices.size() << " indices (" << outIndices.size() / 3 << " triangles)" << std::endl;

    return !outVertices.empty() && !outIndices.empty();
}
