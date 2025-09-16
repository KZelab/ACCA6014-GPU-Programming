# Branch: 12-advanced-geometry

## Learning Objective
Master advanced geometry generation through parametric equations, tessellation, and subdivision techniques. This branch focuses on complex primitive construction, enhanced UV coordinate calculation, and mesh density control through systematic tessellation approaches, building upon the foundational geometry factory system.

## What You'll Build
An advanced geometry system extending the basic factory with parametric cylinder generation, tessellated plane construction, and enhanced UV mapping techniques. The system demonstrates mathematical surface generation, subdivision algorithms, and mesh complexity control through interactive visualisation with sophisticated lighting.

![Expected Result](docs/images/12-advanced-geometry.png)
*A 960x540 window with an ImGui interface demonstrating advanced primitive generation and tessellation*

## Key Concepts

### Core Concepts Learned:
- **Parametric Surface Generation**: Mathematical construction of complex surfaces using parametric equations
- **Tessellation and Subdivision**: Mesh density control through systematic subdivision algorithms
- **Advanced UV Mapping**: Cylindrical and planar coordinate projection techniques
- **Normal Vector Calculation**: Surface normal generation for complex parametric surfaces
- **Mesh Complexity Control**: Dynamic geometry resolution through subdivision parameters
- **Radial Coordinate Systems**: Cylindrical coordinate mathematics for surface generation
- **Grid-based Tessellation**: Systematic quad-based mesh subdivision techniques
- **Memory Optimisation**: Efficient vertex sharing in tessellated surfaces

### Test Demonstrations:
- **testClearColour**: Fundamental framebuffer clearing and colour management
- **testGeometryFactories**: Interactive advanced primitive generation with 7 geometry types
- **Previous Tests**: testTexture2D and testRayCasting from branch 10

### Technical Features:
- **Enhanced Triangle Factory**: Basic 3-vertex primitive with colour gradients
- **Optimised Quad Factory**: 4-vertex primitive using index buffers
- **Advanced Cube Factory**: 8-vertex primitive with proper face normals
- **Parametric Sphere Factory**: Spherical coordinate surface generation
- **Fullscreen Quad Factory**: NDC coordinates for post-processing
- **Parametric Cylinder Factory**: Radial surface generation with configurable parameters
- **Tessellated Plane Factory**: Grid-based subdivision with mesh density control
- **Advanced UV Mapping**: Cylindrical and planar coordinate projection
- **Interactive Tessellation**: Real-time mesh complexity adjustment
- **Enhanced Lighting**: Directional lighting with proper normal calculations

## Code Architecture

### File Structure
```
src/
├── CMakeHelloWorld.cpp       # Test harness with advanced geometry demos
├── Mesh/
│   ├── Mesh.h/.cpp          # Enhanced mesh class for complex geometry
│   ├── GeometryFactory.h/.cpp # Extended factory with advanced primitives
│   ├── Cube.h/.cpp          # Individual cube primitive class
│   ├── FullscreenQuad.h/.cpp # NDC quad primitive class
│   ├── Sphere.h/.cpp        # Parametric sphere primitive class
│   ├── Cylinder.h/.cpp      # Parametric cylinder primitive class (NEW)
│   └── Plane.h/.cpp         # Tessellated plane primitive class (NEW)
├── Shader.h/.cpp             # Shader compilation and uniform management
├── Renderer.h/.cpp           # High-level rendering commands
├── VertexArray.h/.cpp        # VAO wrapper
├── VertexBuffer.h/.cpp       # VBO wrapper
├── IndexBuffer.h/.cpp        # EBO wrapper
├── VertexBufferLayout.h      # Vertex attribute layout helper
├── tests/
│   ├── Tests.h/.cpp          # Base test framework classes
│   ├── testClearColour.h/.cpp # Basic framebuffer clearing
│   ├── testGeometryFactories.h/.cpp # Advanced primitive testing with 7 types
│   ├── testTexture2D.h/.cpp  # 2D texture mapping (from branch 10)
│   └── testRayCasting.h/.cpp # Ray casting algorithms (from branch 10)
└── vendor/
    └── imgui/                # Dear ImGui library for interactive GUI
```

### Enhanced Geometry Factory System

**Extended GeometryFactory Class:**
```cpp
class GeometryFactory {
public:
    // Basic primitive generators
    static Mesh* CreateTriangle();
    static Mesh* CreateQuad();
    static Mesh* CreateCube();
    static Mesh* CreateSphere(int sectors = 20, int stacks = 20);
    static Mesh* CreateFullscreenQuad();

    // Advanced primitive generators (NEW)
    static Mesh* CreateCylinder(int sectors = 20, int stacks = 1, float height = 1.0f, float radius = 0.5f);
    static Mesh* CreatePlane(int subdivisionsX = 1, int subdivisionsY = 1, float width = 1.0f, float height = 1.0f);

    // Enhanced vertex data generation
    static std::vector<Vertex> GenerateCylinderVertices(int sectors, int stacks, float height, float radius);
    static std::vector<Vertex> GeneratePlaneVertices(int subdivisionsX, int subdivisionsY, float width, float height);

    // Advanced index buffer generation
    static std::vector<unsigned int> GenerateCylinderIndices(int sectors, int stacks);
    static std::vector<unsigned int> GeneratePlaneIndices(int subdivisionsX, int subdivisionsY);

private:
    // Enhanced utility methods
    static void AssignUVCoordinates(std::vector<Vertex>& vertices, int faceVertexCount);
    static void AssignColours(std::vector<Vertex>& vertices);
    static void CalculateNormals(std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
};
```

### Educational Progression

**Advanced Learning Path:**
1. **Basic Primitives** → Triangle, Quad, Cube (foundational geometry)
2. **Parametric Surfaces** → Sphere (spherical coordinates)
3. **Screen-Space Geometry** → Fullscreen Quad (NDC coordinates)
4. **Cylindrical Surfaces** → Cylinder (radial parametric equations)
5. **Tessellation Control** → Plane (grid-based subdivision)

**Interactive Features:**
- Real-time geometry switching between 7 primitive types
- Wireframe/solid rendering toggle for mesh visualisation
- Rotation speed and scale adjustment with enhanced lighting
- Educational information display for each primitive type
- Index count and complexity statistics
- Basic directional lighting using calculated normals

## Understanding the Code

### Parametric Cylinder Implementation

**Cylindrical Coordinate Mathematics:**
```cpp
std::vector<Vertex> GeometryFactory::GenerateCylinderVertices(int sectors, int stacks, float height, float radius) {
    std::vector<Vertex> vertices;
    const float PI = 3.14159265359f;

    // Generate vertices for cylinder body using parametric equations
    for (int stack = 0; stack <= stacks; ++stack) {
        float y = height * (float(stack) / stacks) - height * 0.5f; // Vertical position

        for (int sector = 0; sector <= sectors; ++sector) {
            float theta = 2.0f * PI * (float(sector) / sectors);  // Angle parameter
            float x = radius * cos(theta);  // X position from polar coordinates
            float z = radius * sin(theta);  // Z position from polar coordinates

            // Normal vector pointing radially outward
            float normX = cos(theta);
            float normY = 0.0f;           // No vertical component for side normals
            float normZ = sin(theta);

            // Cylindrical UV mapping
            float u = float(sector) / sectors;  // Wraps around cylinder
            float v = float(stack) / stacks;    // Height-based coordinate

            vertices.emplace_back(x, y, z, normX, normY, normZ, r, g, b, u, v);
        }
    }
    return vertices;
}
```

**Key Learning Points:**
- **Parametric Equations**: Mathematical cylinder generation using polar coordinates
- **Radial Normals**: Each vertex normal points outward from the cylinder axis
- **Cylindrical UV Mapping**: U wraps around circumference, V maps to height
- **Configurable Parameters**: sectors/stacks control mesh resolution and quality
- **Perfect Topology**: Systematic vertex generation ensures proper connectivity

### Tessellated Plane Implementation

**Grid-Based Subdivision:**
```cpp
std::vector<Vertex> GeometryFactory::GeneratePlaneVertices(int subdivisionsX, int subdivisionsY, float width, float height) {
    std::vector<Vertex> vertices;
    int verticesX = subdivisionsX + 1;
    int verticesY = subdivisionsY + 1;

    // Generate grid of vertices with subdivision
    for (int y = 0; y < verticesY; ++y) {
        for (int x = 0; x < verticesX; ++x) {
            // Position calculation with subdivision
            float posX = (float(x) / subdivisionsX - 0.5f) * width;
            float posY = 0.0f;  // Plane lies in XZ plane
            float posZ = (float(y) / subdivisionsY - 0.5f) * height;

            // Consistent upward normal
            float normX = 0.0f, normY = 1.0f, normZ = 0.0f;

            // Planar UV mapping
            float u = float(x) / subdivisionsX;
            float v = float(y) / subdivisionsY;

            vertices.emplace_back(posX, posY, posZ, normX, normY, normZ, r, g, b, u, v);
        }
    }
    return vertices;
}
```

**Advanced Index Generation:**
```cpp
std::vector<unsigned int> GeometryFactory::GeneratePlaneIndices(int subdivisionsX, int subdivisionsY) {
    std::vector<unsigned int> indices;
    int verticesX = subdivisionsX + 1;

    // Generate quad-based tessellation
    for (int y = 0; y < subdivisionsY; ++y) {
        for (int x = 0; x < subdivisionsX; ++x) {
            // Calculate quad corners
            int topLeft = y * verticesX + x;
            int topRight = topLeft + 1;
            int bottomLeft = (y + 1) * verticesX + x;
            int bottomRight = bottomLeft + 1;

            // Two triangles per tessellation quad
            indices.push_back(topLeft);     indices.push_back(bottomLeft);  indices.push_back(topRight);
            indices.push_back(topRight);    indices.push_back(bottomLeft);  indices.push_back(bottomRight);
        }
    }
    return indices;
}
```

**Key Learning Points:**
- **Grid Tessellation**: Systematic subdivision creates uniform mesh density
- **Mesh Density Control**: subdivision parameters directly control vertex count
- **Quad Decomposition**: Each grid square becomes two triangles with proper winding
- **Memory Efficiency**: Index buffers enable vertex sharing between adjacent quads
- **Planar UV Mapping**: Direct coordinate mapping for texture projection

### Advanced UV Coordinate Techniques

**Cylindrical UV Projection:**
```cpp
// Cylinder UV mapping wraps around circumference
float u = float(sector) / sectors;    // 0.0 to 1.0 around circumference
float v = float(stack) / stacks;      // 0.0 to 1.0 from bottom to top

// Benefits:
// - Seamless texture wrapping around cylinder
// - Linear height mapping for consistent texture scaling
// - No UV seam distortion at connection point
```

**Planar UV Projection:**
```cpp
// Plane UV mapping uses normalised grid coordinates
float u = float(x) / subdivisionsX;   // 0.0 to 1.0 across width
float v = float(y) / subdivisionsY;   // 0.0 to 1.0 across height

// Benefits:
// - Direct 1:1 texture to surface mapping
// - No distortion or stretching
// - Ideal for terrain, floors, and architectural surfaces
```

**Spherical UV Projection (from previous branch):**
```cpp
// Sphere UV mapping uses spherical coordinates
float u = float(sector) / sectors;    // Longitude mapping (0 to 1)
float v = float(stack) / stacks;      // Latitude mapping (0 to 1)

// Benefits:
// - Mathematically correct spherical projection
// - Minimal distortion at equator
// - Standard mapping for planetary and spherical textures
```

### Interactive Test Enhancement

**Advanced Geometry Switching:**
```cpp
void testGeometryFactories::UpdateMesh() {
    switch (m_CurrentGeometry) {
        case CYLINDER:
            m_CurrentMesh = GeometryFactory::CreateCylinder(20, 1, 1.0f, 0.5f);
            break;
        case PLANE:
            m_CurrentMesh = GeometryFactory::CreatePlane(10, 10, 2.0f, 2.0f);
            break;
        // ... other cases
    }
}
```

**Enhanced Educational Interface:**
```cpp
void testGeometryFactories::RenderGUI() {
    const char* geometryTypes[] = {
        "Triangle", "Quad", "Cube", "Sphere", "Fullscreen Quad", "Cylinder", "Plane"
    };

    switch (m_CurrentGeometry) {
        case CYLINDER:
            ImGui::BulletText("Parametric cylinder generation");
            ImGui::BulletText("Demonstrates advanced UV mapping");
            ImGui::BulletText("Radial normal calculations");
            break;
        case PLANE:
            ImGui::BulletText("Tessellated plane with subdivisions");
            ImGui::BulletText("Shows tessellation concepts");
            ImGui::BulletText("Demonstrates mesh density control");
            break;
    }
}
```

## Mathematical Foundations

### Parametric Surface Mathematics

**Cylindrical Coordinates:**
```
x = r × cos(θ)     where θ = 2π × (sector/sectors)
z = r × sin(θ)     where r = radius
y = h × (stack/stacks) - h/2    where h = height

Normal Vector:
nx = cos(θ)        (radial outward direction)
ny = 0             (no vertical component for sides)
nz = sin(θ)        (radial outward direction)
```

**Grid Tessellation Mathematics:**
```
For plane subdivision:
vertices_total = (subdivisionsX + 1) × (subdivisionsY + 1)
triangles_total = subdivisionsX × subdivisionsY × 2
indices_total = triangles_total × 3

Memory efficiency:
vertex_reuse_ratio = indices_total / vertices_total
```

### Tessellation Theory

**Subdivision Algorithms:**
- **Uniform Subdivision**: Equal spacing creates consistent mesh density
- **Adaptive Subdivision**: Variable density based on surface curvature
- **Quad-based Tessellation**: Each grid cell becomes two triangles
- **Triangle Strip Optimisation**: Vertex cache efficiency through proper ordering

**Mesh Quality Metrics:**
- **Aspect Ratio**: Triangle shape quality (equilateral = optimal)
- **Vertex Valence**: Number of triangles sharing each vertex
- **Memory Footprint**: Balance between quality and performance
- **UV Distortion**: Texture mapping accuracy across surface

## What's Different from Previous Branch (11-basic-geometry-factories)

### New Additions:
- **Parametric Cylinder**: Advanced radial surface generation with configurable parameters
- **Tessellated Plane**: Grid-based subdivision demonstrating mesh density control
- **Extended GeometryFactory**: Advanced primitive methods with sophisticated parameter control
- **Enhanced UV Mapping**: Cylindrical and planar coordinate projection techniques
- **Tessellation Concepts**: Systematic mesh subdivision and density control
- **Advanced Normal Calculation**: Proper surface normals for complex parametric surfaces
- **Educational Progression**: Extended from 5 to 7 primitive types with advanced concepts
- **Mathematical Demonstrations**: Parametric equations and coordinate system transformations

### Enhanced Features:
- **Interactive Tessellation**: Real-time visualization of mesh density effects
- **Advanced Parameter Control**: Configurable sectors, stacks, and subdivision levels
- **Complex Surface Generation**: Mathematical construction of non-trivial surfaces
- **UV Mapping Techniques**: Multiple coordinate projection methods
- **Mesh Complexity Analysis**: Visual demonstration of tessellation trade-offs

### What Stayed the Same:
- Essential test framework architecture (Tests.h/cpp)
- All previous primitive types and tests
- ImGui integration for interactive parameter control
- Real-time visual feedback and educational interface
- Core rendering systems (Shader, Renderer, VertexArray, etc.)

## Learning Experiments

### Advanced Tessellation Challenges:
1. **Variable Subdivision**:
   ```cpp
   // Create adaptive plane with higher density at center
   static Mesh* CreateAdaptivePlane(int centerDensity, int edgeDensity);
   // Implement logarithmic spacing for natural density falloff
   ```

2. **Cylinder Variations**:
   ```cpp
   // Experiment with different cylinder configurations
   auto tallCylinder = GeometryFactory::CreateCylinder(16, 4, 2.0f, 0.3f);  // Tall and thin
   auto shortCylinder = GeometryFactory::CreateCylinder(32, 1, 0.5f, 1.0f); // Short and wide
   auto detailedCylinder = GeometryFactory::CreateCylinder(64, 8, 1.0f, 0.5f); // High detail
   ```

3. **UV Mapping Experiments**:
   ```cpp
   // Modify UV coordinates for different texture effects
   void ExperimentWithUVMapping() {
       // Tiled mapping (UV > 1.0)
       float u = float(sector) / sectors * 4.0f;  // 4x texture repeat

       // Stretched mapping
       float v = pow(float(stack) / stacks, 2.0f); // Non-linear height mapping

       // Spiral mapping
       float spiralU = fmod(u + v * 8.0f, 1.0f);  // Spiral texture pattern
   }
   ```

### Performance Analysis Challenges:
1. **Tessellation Trade-offs**:
   ```cpp
   // Compare performance vs quality for different subdivision levels
   struct TessellationTest {
       int subdivisions;
       int vertexCount;
       int triangleCount;
       float renderTime;
       float qualityScore;
   };
   ```

2. **Memory Usage Analysis**:
   ```cpp
   // Calculate memory footprint for different primitive configurations
   size_t CalculateMemoryUsage(int sectors, int stacks) {
       int vertices = (sectors + 1) * (stacks + 1);
       int indices = sectors * stacks * 6;
       return vertices * sizeof(Vertex) + indices * sizeof(unsigned int);
   }
   ```

### Advanced Geometry Extensions:
1. **Parametric Surfaces**:
   ```cpp
   // Implement additional parametric surfaces
   static Mesh* CreateTorus(float majorRadius, float minorRadius, int sectors, int stacks);
   static Mesh* CreateCone(float radius, float height, int sectors);
   static Mesh* CreateEllipsoid(float radiusX, float radiusY, float radiusZ, int sectors, int stacks);
   ```

2. **Procedural Generation**:
   ```cpp
   // Height-based terrain generation
   static Mesh* CreateTerrain(int width, int height, float heightScale);
   // Noise-based surface displacement
   static Mesh* CreateNoisyPlane(int subdivisions, float noiseScale);
   ```

## Common Issues & Solutions

### "Cylinder appears inside-out"
**Normal Vector Problems:**
- **Winding order**: Ensure counter-clockwise vertex order for outward-facing triangles
- **Normal calculation**: Verify normals point outward from cylinder axis
- **Face culling**: Check GL_CULL_FACE settings for proper visibility
- **Matrix transformations**: Ensure no negative scaling that flips normals

### "Tessellated plane has gaps or overlaps"
**Index Generation Issues:**
- **Vertex ordering**: Maintain consistent triangle winding across tessellation
- **Index calculation**: Verify quad-to-triangle decomposition mathematics
- **Boundary conditions**: Check edge cases at subdivision boundaries
- **Grid alignment**: Ensure proper vertex grid indexing

### "UV coordinates cause texture distortion"
**Coordinate Mapping Problems:**
- **Seam handling**: Address texture seams at UV coordinate discontinuities
- **Aspect ratio**: Match UV mapping to texture aspect ratio
- **Coordinate ranges**: Ensure UV coordinates stay within [0,1] unless tiling intended
- **Projection method**: Choose appropriate mapping (cylindrical vs planar vs spherical)

### "Performance degrades with high tessellation"
**Optimisation Strategies:**
- **Level of detail**: Implement distance-based tessellation reduction
- **Vertex caching**: Optimise index order for GPU vertex cache efficiency
- **Instancing**: Use instanced rendering for repeated geometry
- **Culling**: Implement frustum culling to avoid rendering off-screen geometry

## What's Next?

Building on this advanced geometry foundation, future branches will explore:
- **Camera Systems (Branch 13-14)**: Perspective and orthographic cameras with advanced controls
- **Mesh Loading (Branch 15)**: External model file parsing and complex mesh handling
- **Textured Meshes (Branch 16)**: Advanced material systems and multi-texture support
- **Render Batching (Branch 17-18)**: Performance optimisation and instanced rendering
- **Procedural Generation**: Terrain systems and algorithmic geometry creation
- **Advanced Tessellation**: GPU-based tessellation shaders and adaptive subdivision

## Resources for Deeper Learning

### Advanced Geometry and Mathematics:
- [Real-Time Rendering by Möller, Haines & Hoffman](https://www.realtimerendering.com/) - Chapters 13-15: Advanced Geometry
- [Geometric Modeling by Michael Mortenson](https://www.amazon.com/Geometric-Modeling-Michael-E-Mortenson/dp/0831133090)
- [3D Math Primer for Graphics and Game Development](https://gamemath.com/) - Parametric surface mathematics

### Tessellation and Subdivision:
- [Subdivision Surfaces in Computer Graphics](https://graphics.stanford.edu/~mdfisher/subdivision.html)
- [GPU Tessellation and Geometry Shaders](https://www.opengl.org/wiki/Tessellation)
- [Mesh Processing Techniques](https://www.cs.cmu.edu/~kmcrane/Projects/ModelingWithDiscreteGeometry/)

### UV Mapping and Parameterisation:
- [Texture Mapping Techniques](https://learnopengl.com/Getting-started/Textures)
- [UV Mapping Theory and Practice](https://blender.stackexchange.com/questions/33896/uv-mapping-explanation)
- [Advanced Texture Coordinate Generation](https://www.khronos.org/opengl/wiki/Texture_Coordinate_Generation)

### Performance and Optimisation:
- [GPU Performance Analysis](https://developer.nvidia.com/gpugems/gpugems/part-v-performance-and-practicalities)
- [Vertex Cache Optimisation](https://tomforsyth1000.github.io/papers/fast_vert_cache_opt.html)
- [Level of Detail Techniques](https://www.gamasutra.com/view/feature/131553/runtime_mesh_level_of_detail.php)

## Debug Tips

### Debugging Parametric Generation:
```cpp
// Validate parametric equations
void DebugParametricGeneration() {
    for (int sector = 0; sector <= sectors; ++sector) {
        float theta = 2.0f * PI * (float(sector) / sectors);
        std::cout << "Sector " << sector << ": theta=" << theta
                  << ", x=" << cos(theta) << ", z=" << sin(theta) << std::endl;
    }
}
```

### Debugging Tessellation:
```cpp
// Verify tessellation connectivity
void DebugTessellation(int subdivisionsX, int subdivisionsY) {
    int verticesX = subdivisionsX + 1;
    int expectedVertices = verticesX * (subdivisionsY + 1);
    int expectedTriangles = subdivisionsX * subdivisionsY * 2;

    std::cout << "Expected vertices: " << expectedVertices << std::endl;
    std::cout << "Expected triangles: " << expectedTriangles << std::endl;
    std::cout << "Expected indices: " << expectedTriangles * 3 << std::endl;
}
```

### UV Coordinate Validation:
```cpp
// Check UV coordinate ranges
void ValidateUVCoordinates(const std::vector<Vertex>& vertices) {
    float minU = 1.0f, maxU = 0.0f, minV = 1.0f, maxV = 0.0f;

    for (const auto& vertex : vertices) {
        minU = std::min(minU, vertex.texCoords[0]);
        maxU = std::max(maxU, vertex.texCoords[0]);
        minV = std::min(minV, vertex.texCoords[1]);
        maxV = std::max(maxV, vertex.texCoords[1]);
    }

    std::cout << "UV Range: U[" << minU << ", " << maxU << "], V[" << minV << ", " << maxV << "]" << std::endl;
}
```

---

This branch establishes advanced geometry generation capabilities, providing the mathematical and algorithmic foundation for complex surface construction, tessellation control, and sophisticated UV mapping techniques essential for production-quality 3D graphics applications.