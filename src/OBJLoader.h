#pragma once

#include "Model.h"
#include <string>
#include <functional>

// Loads OBJ files and creates Model objects
class OBJLoader
{
public:
    // Progress callback: (current_lines, total_lines, status_message)
    using ProgressCallback = std::function<void(size_t, size_t, const std::string&)>;

    OBJLoader();
    ~OBJLoader();

    // Load OBJ file and return a Model
    // Returns nullptr on failure
    // progressCallback is optional - called periodically during loading
    static Model* LoadFromFile(const std::string& filepath, ProgressCallback progressCallback = nullptr);

private:
    // Parse OBJ format
    static bool ParseOBJ(const std::string& filepath,
                        std::vector<glm::vec3>& positions,
                        std::vector<glm::vec3>& normals,
                        std::vector<glm::vec2>& texCoords,
                        std::vector<Model::Vertex>& outVertices,
                        std::vector<unsigned int>& outIndices,
                        ProgressCallback progressCallback);

    // Get line count in file for progress tracking
    static size_t CountLines(const std::string& filepath);
};
