# Branch: 10-textures-and-raycasting

## Learning Objective
Master 2D texture mapping, sampling techniques, and fundamental ray casting algorithms through interactive demonstrations. This branch builds upon the test harness foundation to explore advanced graphics programming concepts including texture operations, UV coordinate systems, and geometric intersection calculations.

## What You'll Build
An interactive test suite with three focused demonstrations: basic framebuffer clearing, comprehensive 2D texture operations, and ray casting techniques. Each test provides real-time parameter adjustment and visual feedback to understand the underlying graphics programming concepts.

![Expected Result](docs/images/10-textures-and-raycasting.png)
*A 960x540 window with an ImGui interface demonstrating texture mapping and ray casting concepts*

## Key Concepts

### Core Concepts Learned:
- **2D Texture Mapping**: Understanding UV coordinate systems and texture sampling
- **Texture Operations**: Loading, binding, and manipulating texture data
- **Texture Filtering**: Linear vs nearest neighbour sampling techniques
- **Texture Wrapping**: Repeat, clamp, and mirror wrapping modes
- **Ray Casting Fundamentals**: Ray-object intersection algorithms
- **Geometric Calculations**: Vector mathematics for 3D graphics
- **Interactive Parameter Testing**: Real-time adjustment of graphics parameters
- **Visual Debugging**: Using colour coding and visual feedback for algorithm validation

### Test Demonstrations:
- **testClearColour**: Fundamental framebuffer clearing and colour management
- **testTexture2D**: Comprehensive 2D texture operations and sampling techniques
- **testRayCasting**: Ray-sphere and ray-plane intersection algorithms

### Technical Features:
- **Texture Loading**: Dynamic texture creation and GPU upload
- **UV Coordinate Manipulation**: Real-time UV coordinate transformation
- **Ray Generation**: Mouse-based ray casting from camera position
- **Intersection Visualisation**: Real-time feedback for ray-object intersections
- **Parameter Controls**: Interactive sliders for texture and ray casting parameters

## Code Architecture

### File Structure
```
src/
├── CMakeHelloWorld.cpp       # Test harness application with focused demonstrations
├── Shader.h/.cpp             # Shader compilation and uniform management
├── Mesh.h/.cpp               # Vertex data and geometry abstraction
├── Renderer.h/.cpp           # High-level rendering commands
├── VertexArray.h/.cpp        # VAO wrapper
├── VertexBuffer.h/.cpp       # VBO wrapper
├── IndexBuffer.h/.cpp        # EBO wrapper
├── VertexBufferLayout.h      # Vertex attribute layout helper
├── tests/
│   ├── Tests.h/.cpp          # Base test framework classes
│   ├── testClearColour.h/.cpp # Basic framebuffer clearing demonstration
│   ├── testTexture2D.h/.cpp  # 2D texture mapping and sampling techniques
│   └── testRayCasting.h/.cpp # Ray casting algorithms and intersection testing
└── vendor/
    └── imgui/                # Dear ImGui library for interactive GUI

Dependencies:
├── OpenGL 3.3+              # Graphics API
├── GLM Library              # Vector and matrix mathematics
├── GLFW                     # Window management
├── GLEW                     # OpenGL extension loading
└── Dear ImGui               # Interactive parameter controls
```

### Test Demonstrations Overview

**testClearColour - Fundamental Concepts:**
- Basic framebuffer clearing operations
- Colour buffer, depth buffer, and stencil buffer management
- Interactive colour selection and animation
- OpenGL clear operations and state management

**testTexture2D - Texture Mapping:**
- 2D texture creation and GPU upload
- UV coordinate systems and texture sampling
- Texture filtering modes (nearest, linear)
- Texture wrapping modes (repeat, clamp, mirror)
- Real-time texture parameter adjustment
- Procedural texture generation and manipulation

**testRayCasting - Geometric Algorithms:**
- Ray generation from camera/mouse position
- Ray-sphere intersection calculations
- Ray-plane intersection algorithms
- Vector mathematics for 3D graphics
- Real-time intersection visualisation
- Interactive geometry parameter adjustment

### Educational Progression

**Learning Path:**
1. **Clear Operations** → Understanding fundamental GPU operations
2. **Texture Mapping** → 2D graphics and UV coordinate systems
3. **Ray Casting** → 3D mathematics and geometric calculations

**Interactive Features:**
- Real-time parameter sliders for immediate visual feedback
- Colour pickers for intuitive colour selection
- Mouse interaction for ray casting demonstrations
- Educational descriptions explaining each concept

## What's Different from Previous Branch (09-test-harness)

### New Additions:
- **testTexture2D**: Comprehensive 2D texture mapping and sampling demonstration
- **testRayCasting**: Interactive ray casting algorithms and intersection testing
- **Advanced Graphics Concepts**: Moving beyond basic rendering to computational graphics
- **Mathematical Visualisation**: Real-time display of geometric calculations
- **Interactive Algorithm Testing**: Parameter adjustment for texture and ray casting operations
- **Educational Progression**: Building from fundamental concepts to advanced techniques

### What Stayed the Same:
- Essential test framework architecture (Tests.h/cpp)
- testClearColour as foundational demonstration
- ImGui integration for interactive parameter control
- Real-time visual feedback and educational interface
- Clean project structure and modular design

## Understanding the Code

### testClearColour Implementation

**Core Functionality:**
```cpp
// Basic framebuffer clearing with different buffer types
void testClearColour::Render() {
    if (m_ClearDepth && m_ClearStencil) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    } else if (m_ClearDepth) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    } else {
        glClear(GL_COLOR_BUFFER_BIT);
    }
}
```

**Interactive Features:**
- Colour presets for quick testing
- Animated colour transitions
- Buffer clearing options (colour, depth, stencil)
- Real-time colour parameter adjustment

### testTexture2D Implementation

**Core Texture Operations:**
```cpp
// Texture loading and binding in constructor
m_Texture = std::make_unique<Texture>(R"(res/Textures/1.png)");
m_Texture->Bind(); // Activate texture unit 0
m_Shader->setUniform1i("u_Texture", 0); // Link shader uniform to texture unit 0

// Vertex data with UV coordinates
float positions[] = {
   -50.0f, -50.0f, 0.0f, 0.0f, // Bottom-left with UV (0,0)
    50.0f, -50.0f, 1.0f, 0.0f, // Bottom-right with UV (1,0)
    50.0f,  50.0f, 1.0f, 1.0f, // Top-right with UV (1,1)
   -50.0f,  50.0f, 0.0f, 1.0f  // Top-left with UV (0,1)
};
```

**Interactive Features:**
- Real-time translation of textured quads using ImGui sliders
- Multiple textured objects rendered simultaneously
- UV coordinate mapping from 2D texture to geometry
- Orthographic projection for clear 2D visualization

### testRayCasting Implementation

**Ray Generation and Mathematics:**
```cpp
// Calculate ray direction from mouse position
glm::vec3 TestRayCasting::CalculateRayDirection(float mouseX, float mouseY) {
    // Convert mouse position to NDC
    float x = (2.0f * mouseX) / width - 1.0f;
    float y = 1.0f - (2.0f * mouseY) / height; // Invert Y-axis

    glm::vec4 rayClip(x, y, -1.0f, 1.0f);
    glm::vec4 rayEye = glm::inverse(projectionMatrix) * rayClip;
    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);
    glm::vec3 rayWorld = glm::vec3(glm::inverse(viewMatrix) * rayEye);
    return glm::normalize(rayWorld);
}

// Ray-sphere intersection test
bool TestRayCasting::RayIntersectsSphere(const glm::vec3& rayOrigin,
    const glm::vec3& rayDirection, const glm::vec3& sphereCenter, float sphereRadius) {
    glm::vec3 oc = rayOrigin - sphereCenter;
    float a = glm::dot(rayDirection, rayDirection);
    float b = 2.0f * glm::dot(oc, rayDirection);
    float c = glm::dot(oc, oc) - sphereRadius * sphereRadius;
    float discriminant = b * b - 4 * a * c;
    return discriminant > 0; // Intersection if discriminant > 0
}
```

**3D Interactive Features:**
- WASD camera movement through 3D space
- Mouse-based ray casting from camera position
- Real-time sphere intersection detection and highlighting
- 3D sphere generation with parametric equations
- Visual feedback through colour changes (green for selected, red for unselected)

## Implementation Details

### Texture Class Integration
Both testTexture2D and testRayCasting rely on the existing graphics abstraction classes:

**Texture Loading:**
```cpp
// In testTexture2D constructor
m_Texture = std::make_unique<Texture>(R"(res/Textures/1.png)");
m_Texture->Bind();
m_Shader->setUniform1i("u_Texture", 0);
```

**Vertex Array Setup:**
```cpp
// Common pattern used in both tests
m_VAO = std::make_unique<VertexArray>();
m_VBO = std::make_unique<VertexBuffer>(vertices.data(), vertexSize);
VertexBufferLayout layout;
layout.Push<float>(3); // Position attributes
m_VAO->AddBuffer(*m_VBO, layout);
```

### Shader Integration
Both tests use the existing Shader class for uniform management:

```cpp
// 2D texture test uses orthographic projection
m_Proj = glm::ortho(0.0f, 960.0f, 0.0f, 540.f, -1.0f, 1.0f);
shader.setUniformMat4f("u_MVP", m_Proj * m_View * model);

// 3D ray casting test uses perspective projection
projectionMatrix = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
shader.setUniformMat4f("projection", projectionMatrix);
```

## Learning Experiments

### Texture Mapping Challenges:
1. **Modify UV coordinates**:
   ```cpp
   // In testTexture2D, experiment with different UV mappings:
   float positions[] = {
       -50.0f, -50.0f, 0.0f, 0.0f,    // Try different UV values
        50.0f, -50.0f, 2.0f, 0.0f,    // Values > 1.0 show texture wrapping
        50.0f,  50.0f, 2.0f, 2.0f,    // Experiment with texture repetition
       -50.0f,  50.0f, 0.0f, 2.0f
   };
   ```

2. **Add texture filtering controls**:
   - Add ImGui controls to switch between GL_NEAREST and GL_LINEAR
   - Experiment with different texture wrapping modes
   - Try GL_CLAMP_TO_EDGE vs GL_REPEAT

3. **Multiple texture units**:
   - Load a second texture and bind to texture unit 1
   - Modify shader to blend between two textures
   - Add ImGui slider to control blend factor

### Ray Casting Enhancements:
1. **Add different object types**:
   ```cpp
   // Extend the Object struct to support different shapes
   enum ObjectType { SPHERE, BOX, PLANE };
   struct Object {
       glm::vec3 position;
       ObjectType type;
       union {
           float radius;        // For spheres
           glm::vec3 dimensions; // For boxes
       };
   };
   ```

2. **Implement ray-box intersection**:
   ```cpp
   bool TestRayCasting::RayIntersectsBox(const glm::vec3& rayOrigin,
       const glm::vec3& rayDirection, const glm::vec3& boxMin, const glm::vec3& boxMax);
   ```

3. **Add intersection distance calculation**:
   - Calculate and display the distance to intersection point
   - Show intersection coordinates in ImGui
   - Implement closest object selection when multiple intersections occur

### Combined Challenges:
1. **Textured spheres in ray casting**:
   - Apply textures to the ray casting spheres
   - Calculate UV coordinates for sphere surfaces
   - Show different textures on each object

2. **3D texture positioning**:
   - Modify testTexture2D to work in 3D space
   - Add camera controls similar to ray casting test
   - Combine 2D texture techniques with 3D perspective

## Common Issues & Solutions

### "Texture not displaying correctly"
**Texture Loading Problems:**
- **File path issues**: Ensure texture files exist in res/Textures/ directory
- **Image format**: Verify PNG files are properly formatted
- **UV coordinate problems**: Check vertex data includes correct texture coordinates
- **Texture binding**: Ensure texture is bound before rendering

### "Ray casting not detecting intersections"
**Mathematical Issues:**
- **Ray direction calculation**: Verify mouse-to-world coordinate conversion
- **Matrix inversions**: Check that view and projection matrices are valid
- **Sphere positioning**: Ensure objects are positioned within camera view
- **Discriminant calculation**: Verify ray-sphere intersection math

### "ImGui controls not responsive"
**GUI Integration Problems:**
- **Event handling**: Ensure GLFW events are processed before ImGui
- **Context binding**: Verify ImGui context is current during GUI rendering
- **Slider ranges**: Check that slider min/max values are appropriate
- **Variable updates**: Ensure GUI changes affect the actual rendering variables

### "Performance issues with 3D rendering"
**Optimization Concerns:**
- **Depth testing**: Enable GL_DEPTH_TEST for proper 3D rendering
- **Vertex buffer size**: Consider the number of vertices in sphere generation
- **Draw call frequency**: Avoid redundant OpenGL state changes
- **Matrix calculations**: Cache frequently used transformation matrices

## What's Next?

Building on this foundation of 2D textures and ray casting, future branches will explore:
- **Advanced Lighting Models**: Implementing Phong, Blinn-Phong, and PBR shading
- **3D Texture Mapping**: Cube maps, normal mapping, and displacement mapping
- **Ray Tracing Extensions**: Reflection, refraction, and global illumination
- **Performance Optimization**: Frustum culling, level-of-detail, and instanced rendering
- **Advanced Ray Casting**: Acceleration structures like BVH or octrees
- **Material Systems**: Complex material properties and multi-pass rendering

## Resources for Deeper Learning

### Computer Graphics Fundamentals:
- [Real-Time Rendering by Möller, Haines & Hoffman](https://www.realtimerendering.com/)
- [Fundamentals of Computer Graphics by Shirley & Marschner](https://www.amazon.com/Fundamentals-Computer-Graphics-Steve-Marschner/dp/1482229390)
- [Ray Tracing in One Weekend Series](https://raytracing.github.io/)

### OpenGL and Graphics Programming:
- [LearnOpenGL](https://learnopengl.com/) - Comprehensive OpenGL tutorial series
- [OpenGL Programming Guide](https://www.opengl.org/documentation/books/#the_opengl_programming_guide_the_official_guide_to_learning_opengl_version_4_5_with_spir_v)
- [GPU Gems Series](https://developer.nvidia.com/gpugems/gpugems) - Advanced GPU programming techniques

### Mathematical Foundations:
- [3D Math Primer for Graphics and Game Development](https://gamemath.com/)
- [Mathematics for 3D Game Programming and Computer Graphics](https://www.amazon.com/Mathematics-Programming-Computer-Graphics-Third/dp/1435458869)
- [GLM Documentation](https://glm.g-truc.net/) - Learn the mathematical operations used in this project

### Interactive Learning:
- [Shadertoy](https://www.shadertoy.com/) - Experiment with fragment shaders online
- [GeoGebra 3D](https://www.geogebra.org/3d) - Visualize 3D mathematical concepts
- [Ray Casting Visualizations](https://www.cs.cornell.edu/courses/cs4620/2013fa/lectures/03raytracing.pdf)

### Development Tools:
- **RenderDoc**: Frame capture and analysis for OpenGL debugging
- **Nsight Graphics**: NVIDIA's graphics debugging and profiling tool
- **Visual Studio Graphics Diagnostics**: Built-in graphics debugging capabilities

## Debug Tips

### Debugging Texture Issues:
```cpp
// Check texture loading in testTexture2D constructor
if (!m_Texture) {
    std::cout << "Failed to load texture: res/Textures/1.png" << std::endl;
}

// Verify texture binding
GLint currentTexture;
glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTexture);
std::cout << "Current bound texture: " << currentTexture << std::endl;
```

### Debugging Ray Casting Math:
```cpp
// Validate ray direction in TestRayCasting::Update()
std::cout << "Ray Origin: (" << rayOrigin.x << ", " << rayOrigin.y << ", " << rayOrigin.z << ")" << std::endl;
std::cout << "Ray Direction: (" << rayDirection.x << ", " << rayDirection.y << ", " << rayDirection.z << ")" << std::endl;

// Check intersection calculations
float discriminant = b * b - 4 * a * c;
std::cout << "Discriminant: " << discriminant << " (>0 means intersection)" << std::endl;
```

### Matrix Validation:
```cpp
// Print matrices for debugging transformations
void PrintMatrix(const glm::mat4& matrix, const std::string& name) {
    std::cout << name << ":" << std::endl;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            std::cout << matrix[i][j] << " ";
        }
        std::cout << std::endl;
    }
}
```

### Common Issues:
- **UV coordinates outside [0,1]**: Can cause texture sampling issues
- **Incorrect matrix multiplication order**: MVP should be Projection * View * Model
- **Missing depth testing**: Essential for proper 3D rendering in ray casting
- **ImGui slider ranges**: Ensure min/max values allow meaningful interaction

---

