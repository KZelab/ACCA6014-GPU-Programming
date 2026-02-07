/**
 * GeometryFactory.cpp
 *
 * This file implements procedural geometry generation for common 3D primitives.
 *
 * KEY CONCEPTS COVERED:
 * ---------------------
 * 1. Vertex attributes: Position, Normal, Color, UV coordinates
 * 2. Index buffers and triangle winding order (counter-clockwise = front-facing)
 * 3. Spherical coordinate system (UV sphere generation)
 * 4. Cross product for normal calculation
 * 5. Normalized Device Coordinates (NDC) for fullscreen rendering
 * 6. HSV to RGB color space conversion
 * 7. Smooth vertex normals via accumulation and averaging
 */

#include "GeometryFactory.h"
#include <cmath>

// =============================================================================
// MESH FACTORY METHODS
// =============================================================================
// These are convenience wrappers that combine vertex and index generation
// into complete Mesh objects ready for GPU upload.

std::unique_ptr<Mesh> GeometryFactory::CreateTriangle() {
    std::vector<Vertex> vertices = GenerateTriangleVertices();
    std::vector<unsigned int> indices = GenerateTriangleIndices();
    return std::make_unique<Mesh>(vertices, indices);
}

std::unique_ptr<Mesh> GeometryFactory::CreateQuad() {
    std::vector<Vertex> vertices = GenerateQuadVertices();
    std::vector<unsigned int> indices = GenerateQuadIndices();
    return std::make_unique<Mesh>(vertices, indices);
}

std::unique_ptr<Mesh> GeometryFactory::CreateCube() {
    std::vector<Vertex> vertices = GenerateCubeVertices();
    std::vector<unsigned int> indices = GenerateCubeIndices();
    return std::make_unique<Mesh>(vertices, indices);
}

std::unique_ptr<Mesh> GeometryFactory::CreateSphere(int sectors, int stacks) {
    std::vector<Vertex> vertices = GenerateSphereVertices(sectors, stacks);
    std::vector<unsigned int> indices = GenerateSphereIndices(sectors, stacks);
    return std::make_unique<Mesh>(vertices, indices);
}

std::unique_ptr<Mesh> GeometryFactory::CreateFullscreenQuad() {
    std::vector<Vertex> vertices = GenerateFullscreenQuadVertices();
    std::vector<unsigned int> indices = GenerateFullscreenQuadIndices();
    return std::make_unique<Mesh>(vertices, indices);
}

// =============================================================================
// TRIANGLE GENERATION
// =============================================================================
/**
 * Generates a simple equilateral-ish triangle in the XY plane.
 *
 * VERTEX LAYOUT:
 *          (0, 0.5)
 *             /\
 *            /  \
 *           /    \
 *          /______\
 *   (-0.5,-0.5)  (0.5,-0.5)
 *
 * WINDING ORDER: Counter-clockwise (CCW) when viewed from +Z
 * This is the OpenGL default for front-facing triangles.
 *
 * NORMAL: All vertices share (0,0,1) - pointing toward the viewer
 * This is a "flat" normal appropriate for a single planar triangle.
 */
std::vector<Vertex> GeometryFactory::GenerateTriangleVertices() {
    std::vector<Vertex> vertices = {
        // Vertex constructor: (posX, posY, posZ, normX, normY, normZ, R, G, B, U, V)
        Vertex(0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f,  0.5f, 1.0f),  // Top vertex - Red
        Vertex(-0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f),  // Bottom-left - Green
        Vertex(0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f)   // Bottom-right - Blue
    };
    return vertices;
}

// =============================================================================
// QUAD GENERATION
// =============================================================================
/**
 * Generates a unit quad (1x1) centered at origin in the XY plane.
 *
 * VERTEX LAYOUT:
 *   3 -------- 2
 *   |          |
 *   |  (0,0)   |      UV coordinates map texture space:
 *   |          |      (0,0) = bottom-left, (1,1) = top-right
 *   0 -------- 1
 *
 * INDEXED RENDERING:
 * Instead of 6 vertices (2 triangles), we use 4 vertices + indices.
 * This saves memory and allows vertex data to be shared/cached by GPU.
 *
 * The quad is split into 2 triangles: (0,1,2) and (2,3,0)
 * Both maintain CCW winding order when viewed from +Z.
 */
std::vector<Vertex> GeometryFactory::GenerateQuadVertices() {
    std::vector<Vertex> vertices = {
        Vertex(-0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f),  // Bottom-left - Red
        Vertex(0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f),  // Bottom-right - Green
        Vertex(0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f),  // Top-right - Blue
        Vertex(-0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f, 0.0f,  0.0f, 1.0f)   // Top-left - Yellow
    };
    return vertices;
}

// =============================================================================
// CUBE GENERATION
// =============================================================================
/**
 * Generates a unit cube (1x1x1) centered at origin.
 *
 * VERTEX LAYOUT (8 vertices, shared between faces):
 *
 *        7 -------- 6
 *       /|         /|
 *      / |        / |
 *     3 -------- 2  |
 *     |  |       |  |
 *     |  4 ------|- 5
 *     | /        | /
 *     |/         |/
 *     0 -------- 1
 *
 *   Front face: Z = +0.5 (vertices 0,1,2,3)
 *   Back face:  Z = -0.5 (vertices 4,5,6,7)
 *
 * NORMALS:
 * This implementation uses per-vertex normals set to face normals.
 * For proper lighting on a cube, you'd typically want 24 vertices
 * (4 per face) so each face can have its own distinct normal.
 * This simplified version shares vertices between faces.
 *
 * The normals here point outward from the face they're assigned to:
 * - Front/Back: ±Z
 * - Left/Right: ±X
 * - Top/Bottom: ±Y
 */
std::vector<Vertex> GeometryFactory::GenerateCubeVertices() {
    std::vector<Vertex> vertices = {
        // Front face vertices (Z = +0.5) - Normal pointing towards +Z
        Vertex(-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f),  // 0: Front-bottom-left - Red
        Vertex(0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f),  // 1: Front-bottom-right - Green
        Vertex(0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f),  // 2: Front-top-right - Blue
        Vertex(-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f, 0.0f,  0.0f, 1.0f),  // 3: Front-top-left - Yellow

        // Back face vertices (Z = -0.5) - Normal pointing towards -Z
        Vertex(-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f, 1.0f,  1.0f, 0.0f),  // 4: Back-bottom-left - Magenta
        Vertex(0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f,  0.0f, 0.0f),  // 5: Back-bottom-right - Cyan
        Vertex(0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.5f, 0.0f,  0.0f, 1.0f),  // 6: Back-top-right - Orange
        Vertex(-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.5f, 0.5f, 0.5f,  1.0f, 1.0f)   // 7: Back-top-left - Grey
    };
    return vertices;
}

// =============================================================================
// INDEX BUFFER GENERATION
// =============================================================================
/**
 * INDEX BUFFERS:
 * Instead of duplicating vertex data for each triangle, we reference
 * vertices by their index in the vertex array. This:
 * - Reduces memory usage
 * - Improves cache efficiency (GPU vertex cache can reuse transformed vertices)
 * - Allows vertex sharing between triangles
 *
 * Each group of 3 indices defines one triangle.
 * Winding order (CCW) determines front-facing direction.
 */

std::vector<unsigned int> GeometryFactory::GenerateTriangleIndices() {
    // Single triangle: vertices 0 -> 1 -> 2 (CCW)
    return { 0, 1, 2 };
}

std::vector<unsigned int> GeometryFactory::GenerateQuadIndices() {
    // Two triangles forming a quad:
    // Triangle 1: 0 -> 1 -> 2 (bottom-left, bottom-right, top-right)
    // Triangle 2: 2 -> 3 -> 0 (top-right, top-left, bottom-left)
    return { 0, 1, 2, 2, 3, 0 };
}

/**
 * Cube index generation - 6 faces × 2 triangles × 3 indices = 36 indices
 *
 * FACE WINDING:
 * Each face is wound CCW when viewed from OUTSIDE the cube.
 * This ensures correct face culling (back faces hidden, front faces visible).
 */
std::vector<unsigned int> GeometryFactory::GenerateCubeIndices() {
    return {
        // Front face (Z = +0.5) - viewed from +Z, CCW is 0->1->2, 2->3->0
        0, 1, 2,  2, 3, 0,
        // Back face (Z = -0.5) - viewed from -Z, vertices appear mirrored
        4, 5, 6,  6, 7, 4,
        // Left face (X = -0.5)
        7, 3, 0,  0, 4, 7,
        // Right face (X = +0.5)
        1, 5, 6,  6, 2, 1,
        // Bottom face (Y = -0.5)
        4, 0, 1,  1, 5, 4,
        // Top face (Y = +0.5)
        3, 7, 6,  6, 2, 3
    };
}

// =============================================================================
// UV COORDINATE ASSIGNMENT
// =============================================================================
/**
 * UV MAPPING (Texture Coordinates):
 * UV coordinates map 2D texture space onto 3D geometry.
 * - U: horizontal axis (0 = left, 1 = right)
 * - V: vertical axis (0 = bottom, 1 = top)
 *
 * The GPU interpolates UVs across the triangle surface,
 * then uses them to sample the texture at each pixel.
 */
void GeometryFactory::AssignUVCoordinates(std::vector<Vertex>& vertices, int faceVertexCount) {
    for (size_t i = 0; i < vertices.size(); ++i) {
        if (faceVertexCount == 3) {
            // Triangle UV mapping - centered at top
            //     (0.5, 1.0)
            //        /\
            //       /  \
            //      /____\
            // (0,0)      (1,0)
            switch (i % 3) {
            case 0: vertices[i].texCoords[0] = 0.5f; vertices[i].texCoords[1] = 1.0f; break;
            case 1: vertices[i].texCoords[0] = 0.0f; vertices[i].texCoords[1] = 0.0f; break;
            case 2: vertices[i].texCoords[0] = 1.0f; vertices[i].texCoords[1] = 0.0f; break;
            }
        }
        else if (faceVertexCount == 4) {
            // Quad UV mapping - standard corners
            // (0,1) ---- (1,1)
            //   |          |
            //   |          |
            // (0,0) ---- (1,0)
            switch (i % 4) {
            case 0: vertices[i].texCoords[0] = 0.0f; vertices[i].texCoords[1] = 0.0f; break;
            case 1: vertices[i].texCoords[0] = 1.0f; vertices[i].texCoords[1] = 0.0f; break;
            case 2: vertices[i].texCoords[0] = 1.0f; vertices[i].texCoords[1] = 1.0f; break;
            case 3: vertices[i].texCoords[0] = 0.0f; vertices[i].texCoords[1] = 1.0f; break;
            }
        }
    }
}

// =============================================================================
// UV SPHERE GENERATION
// =============================================================================
/**
 * SPHERICAL COORDINATE SYSTEM:
 *
 * A UV sphere is generated by converting spherical coordinates to Cartesian.
 *
 * Spherical coordinates (r, φ, θ):
 * - r (radius): distance from origin
 * - φ (phi): polar angle from +Y axis (0 at north pole, π at south pole)
 * - θ (theta): azimuthal angle around Y axis (0 to 2π)
 *
 * CONVERSION TO CARTESIAN:
 *   x = r × sin(φ) × cos(θ)
 *   y = r × cos(φ)
 *   z = r × sin(φ) × sin(θ)
 *
 * VISUALIZATION:
 *           +Y (φ=0, north pole)
 *            |
 *            |_____ +X (θ=0)
 *           /
 *          +Z (θ=π/2)
 *
 * TOPOLOGY:
 * - "Stacks" = horizontal slices (latitude lines)
 * - "Sectors" = vertical slices (longitude lines)
 * - Creates a grid of quads, each split into 2 triangles
 * - Poles are singular points where sectors converge
 *
 * UV MAPPING:
 * The sphere gets "unwrapped" like a world map projection:
 * - U = θ / 2π (wraps horizontally)
 * - V = φ / π (0 at top, 1 at bottom)
 *
 * NORMALS:
 * For a unit sphere centered at origin, the normal at any point
 * equals the normalized position vector (points radially outward).
 */
std::vector<Vertex> GeometryFactory::GenerateSphereVertices(int sectors, int stacks) {
    std::vector<Vertex> vertices;
    float radius = 0.5f;
    const float PI = 3.14159265359f;

    // Generate vertices stack by stack (top to bottom)
    for (int stack = 0; stack <= stacks; ++stack) {
        // φ (phi) goes from 0 (north pole) to π (south pole)
        float phi = PI * (float(stack) / stacks);

        // Pre-compute trigonometric values for this latitude
        float y = cos(phi);           // Y component (1 at top, -1 at bottom)
        float sinPhi = sin(phi);      // Radius of this latitude circle

        // Generate vertices around this latitude circle
        for (int sector = 0; sector <= sectors; ++sector) {
            // θ (theta) goes from 0 to 2π around the Y axis
            float theta = 2.0f * PI * (float(sector) / sectors);

            // Convert spherical to Cartesian coordinates
            float x = sinPhi * cos(theta);  // X = sin(φ) × cos(θ)
            float z = sinPhi * sin(theta);  // Z = sin(φ) × sin(θ)

            // UV coordinates for texture mapping
            float u = float(sector) / sectors;  // 0 to 1 horizontally
            float v = float(stack) / stacks;    // 0 to 1 vertically

            // Scale by radius for final position
            float posX = radius * x;
            float posY = radius * y;
            float posZ = radius * z;

            // Normal = normalized position (for unit sphere at origin)
            // This points radially outward from the sphere center
            float normX = x;
            float normY = y;
            float normZ = z;

            // Generate color based on position for visual variety
            // Maps position range [-1,1] to color range [0,1]
            float r = (x + 1.0f) * 0.5f;
            float g = (y + 1.0f) * 0.5f;
            float b = (z + 1.0f) * 0.5f;

            vertices.emplace_back(posX, posY, posZ, normX, normY, normZ, r, g, b, u, v);
        }
    }

    return vertices;
}

/**
 * SPHERE INDEX GENERATION:
 *
 * Creates triangle indices connecting the vertex grid.
 *
 * GRID TOPOLOGY:
 * Each row (stack) has (sectors+1) vertices to allow texture wrapping.
 *
 *   k2 ---- k2+1
 *    | \      |
 *    |   \    |      Each quad becomes 2 triangles:
 *    |     \  |      - Upper: k1, k2, k1+1 (skipped at north pole)
 *   k1 ---- k1+1     - Lower: k1+1, k2, k2+1 (skipped at south pole)
 *
 * POLE HANDLING:
 * - North pole (stack=0): Only create lower triangles (fans from pole)
 * - South pole (stack=stacks-1): Only create upper triangles
 * - This avoids degenerate zero-area triangles at poles
 */
std::vector<unsigned int> GeometryFactory::GenerateSphereIndices(int sectors, int stacks) {
    std::vector<unsigned int> indices;

    for (int stack = 0; stack < stacks; ++stack) {
        // k1 = first vertex index of current stack
        // k2 = first vertex index of next stack
        int k1 = stack * (sectors + 1);
        int k2 = k1 + sectors + 1;

        for (int sector = 0; sector < sectors; ++sector, ++k1, ++k2) {
            // Upper triangle (skip at north pole where it would be degenerate)
            if (stack != 0) {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }
            // Lower triangle (skip at south pole where it would be degenerate)
            if (stack != (stacks - 1)) {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }

    return indices;
}

// =============================================================================
// FULLSCREEN QUAD (Post-Processing)
// =============================================================================
/**
 * NORMALIZED DEVICE COORDINATES (NDC):
 *
 * After all transformations (model → view → projection → perspective divide),
 * vertices end up in NDC space:
 * - X: -1 (left) to +1 (right)
 * - Y: -1 (bottom) to +1 (top)
 * - Z: -1 (near) to +1 (far) in OpenGL, 0 to 1 in DirectX
 *
 * A fullscreen quad spans the entire NDC XY range, covering every pixel.
 * This is used for:
 * - Post-processing effects (blur, bloom, color grading)
 * - Deferred rendering (lighting pass)
 * - Screen-space effects (SSAO, SSR)
 * - Displaying render-to-texture results
 *
 * TYPICAL USAGE:
 * 1. Render scene to a framebuffer texture
 * 2. Bind that texture
 * 3. Draw fullscreen quad with a post-process shader
 * 4. Shader reads texture via UV coords to apply effects
 *
 * Note: The vertex shader can pass through positions directly
 * (no MVP matrix needed) since they're already in NDC.
 */
std::vector<Vertex> GeometryFactory::GenerateFullscreenQuadVertices() {
    std::vector<Vertex> vertices = {
        // NDC coordinates for fullscreen coverage
        // Positions are in clip space (-1 to +1)
        // UVs map texture space (0 to 1)
        Vertex(-1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f),  // Bottom-left
        Vertex(1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,  1.0f, 1.0f, 1.0f,  1.0f, 0.0f),  // Bottom-right
        Vertex(-1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f),  // Top-left
        Vertex(1.0f,  1.0f, 0.0f,   0.0f, 0.0f, 1.0f,  1.0f, 1.0f, 1.0f,  1.0f, 1.0f)   // Top-right
    };
    return vertices;
}

std::vector<unsigned int> GeometryFactory::GenerateFullscreenQuadIndices() {
    // Two triangles: (0,1,2) and (1,3,2)
    // Uses a different vertex order than regular quad for triangle strip compatibility
    return { 0, 1, 2, 1, 3, 2 };
}

// =============================================================================
// HSV TO RGB COLOR CONVERSION
// =============================================================================
/**
 * HSV (Hue-Saturation-Value) COLOR MODEL:
 *
 * HSV is often more intuitive for generating colors programmatically:
 * - Hue: Color wheel position (0-360°)
 *   - 0°/360° = Red
 *   - 60° = Yellow
 *   - 120° = Green
 *   - 180° = Cyan
 *   - 240° = Blue
 *   - 300° = Magenta
 * - Saturation: Color intensity (0 = grey, 1 = vivid)
 * - Value: Brightness (0 = black, 1 = full brightness)
 *
 * CONVERSION ALGORITHM:
 * The HSV hexagon is divided into 6 sectors (60° each).
 * For each sector, two of R,G,B are at their extremes,
 * and one transitions linearly.
 *
 * c = V × S (chroma - color intensity)
 * x = c × (1 - |H/60° mod 2 - 1|) (intermediate value)
 * m = V - c (minimum RGB component)
 *
 * Then based on hue sector, assign (c, x, 0) to (R,G,B) in different orders.
 */
void GeometryFactory::AssignColors(std::vector<Vertex>& vertices) {
    for (size_t i = 0; i < vertices.size(); ++i) {
        // Distribute hue evenly across all vertices (0° to 360°)
        float hue = (float)i / vertices.size() * 360.0f;

        // HSV to RGB conversion with S=1, V=1 (fully saturated, full brightness)
        float c = 1.0f;  // Chroma (S × V)
        float x = c * (1.0f - std::abs(std::fmod(hue / 60.0f, 2.0f) - 1.0f));
        float m = 0.0f;  // Minimum (V - C = 0 when V=1, S=1)

        float r, g, b;
        // Select RGB based on which 60° sector of the color wheel
        if (hue < 60.0f) { r = c; g = x; b = 0; }       // Red -> Yellow
        else if (hue < 120.0f) { r = x; g = c; b = 0; } // Yellow -> Green
        else if (hue < 180.0f) { r = 0; g = c; b = x; } // Green -> Cyan
        else if (hue < 240.0f) { r = 0; g = x; b = c; } // Cyan -> Blue
        else if (hue < 300.0f) { r = x; g = 0; b = c; } // Blue -> Magenta
        else { r = c; g = 0; b = x; }                   // Magenta -> Red

        vertices[i].colour[0] = r + m;
        vertices[i].colour[1] = g + m;
        vertices[i].colour[2] = b + m;
    }
}

// =============================================================================
// NORMAL CALCULATION (Smooth Shading)
// =============================================================================
/**
 * VERTEX NORMAL CALCULATION:
 *
 * Normals are essential for lighting calculations. There are two approaches:
 *
 * 1. FLAT SHADING: Each face has one normal (faceted look)
 * 2. SMOOTH SHADING: Normals are averaged at shared vertices (smooth curves)
 *
 * This function implements smooth shading via normal accumulation:
 *
 * ALGORITHM:
 * 1. Reset all vertex normals to zero
 * 2. For each triangle, compute face normal using cross product
 * 3. Add face normal to all three vertices of that triangle
 * 4. Normalize all vertex normals to unit length
 *
 * CROSS PRODUCT FOR FACE NORMAL:
 * Given triangle vertices A, B, C:
 * - Edge1 = B - A
 * - Edge2 = C - A
 * - Normal = Edge1 × Edge2 (cross product)
 *
 * Cross product formula:
 *   N.x = E1.y × E2.z - E1.z × E2.y
 *   N.y = E1.z × E2.x - E1.x × E2.z
 *   N.z = E1.x × E2.y - E1.y × E2.x
 *
 * The resulting normal is perpendicular to the triangle face.
 * Direction follows right-hand rule (CCW winding = outward normal).
 *
 * WHY ACCUMULATE?
 * When multiple triangles share a vertex, their face normals are averaged.
 * This creates smooth interpolation across the surface in the fragment shader.
 *
 * Note: This simple averaging treats all faces equally. More sophisticated
 * methods weight by face area or angle at the vertex.
 */
void GeometryFactory::CalculateNormals(std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
    // Step 1: Reset all normals to zero
    for (auto& vertex : vertices) {
        vertex.normal[0] = vertex.normal[1] = vertex.normal[2] = 0.0f;
    }

    // Step 2: Calculate and accumulate face normals
    for (size_t i = 0; i < indices.size(); i += 3) {
        unsigned int i0 = indices[i];
        unsigned int i1 = indices[i + 1];
        unsigned int i2 = indices[i + 2];

        // Calculate edge vectors from first vertex
        // v1 = vertex[i1] - vertex[i0] (edge A→B)
        float v1[3] = { vertices[i1].position[0] - vertices[i0].position[0],
                       vertices[i1].position[1] - vertices[i0].position[1],
                       vertices[i1].position[2] - vertices[i0].position[2] };

        // v2 = vertex[i2] - vertex[i0] (edge A→C)
        float v2[3] = { vertices[i2].position[0] - vertices[i0].position[0],
                       vertices[i2].position[1] - vertices[i0].position[1],
                       vertices[i2].position[2] - vertices[i0].position[2] };

        // Cross product: normal = v1 × v2
        // This gives a vector perpendicular to the triangle face
        // Magnitude is proportional to triangle area (larger triangles contribute more)
        float normal[3] = { v1[1] * v2[2] - v1[2] * v2[1],   // Nx = v1.y*v2.z - v1.z*v2.y
                           v1[2] * v2[0] - v1[0] * v2[2],    // Ny = v1.z*v2.x - v1.x*v2.z
                           v1[0] * v2[1] - v1[1] * v2[0] };  // Nz = v1.x*v2.y - v1.y*v2.x

        // Accumulate this face normal to all three vertices
        for (int j = 0; j < 3; j++) {
            vertices[i0].normal[j] += normal[j];
            vertices[i1].normal[j] += normal[j];
            vertices[i2].normal[j] += normal[j];
        }
    }

    // Step 3: Normalize all vertex normals to unit length
    // |N| = sqrt(Nx² + Ny² + Nz²)
    // N_normalized = N / |N|
    for (auto& vertex : vertices) {
        float length = sqrt(vertex.normal[0] * vertex.normal[0] +
            vertex.normal[1] * vertex.normal[1] +
            vertex.normal[2] * vertex.normal[2]);
        if (length > 0.0f) {
            vertex.normal[0] /= length;
            vertex.normal[1] /= length;
            vertex.normal[2] /= length;
        }
    }
}
