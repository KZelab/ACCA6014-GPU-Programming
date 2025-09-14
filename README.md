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
├── CMakeHelloWorld.cpp       # Test harness main application with ImGui integration
├── Shader.h/.cpp             # Shader compilation and uniform management
├── Mesh.h/.cpp               # Vertex data and geometry abstraction
├── Renderer.h/.cpp           # High-level rendering commands
├── VertexArray.h/.cpp        # VAO wrapper (from main branch)
├── VertexBuffer.h/.cpp       # VBO wrapper (from main branch)
├── IndexBuffer.h/.cpp        # EBO wrapper (from main branch)
├── VertexBufferLayout.h      # Vertex attribute layout helper
├── tests/
│   ├── Tests.h/.cpp          # Base test framework classes
└── vendor/
    └── imgui/                # Dear ImGui library for interactive GUI
        ├── imgui.h/.cpp
        ├── imgui_impl_glfw.h/.cpp
        ├── imgui_impl_opengl3.h/.cpp
        └── ... (additional ImGui files)

Dependencies:
├── OpenGL 3.3+              # Graphics API
├── GLM Library              # Mathematics
├── GLFW                     # Window management
├── GLEW                     # OpenGL extension loading
└── Dear ImGui               # Immediate mode GUI
```

### Test Framework Architecture

The test harness follows a modular, extensible design:

**Main Application (CMakeHelloWorld.cpp):**
- Initialises GLFW, GLEW, and ImGui systems
- Creates and manages the test menu system
- Provides main rendering loop with delta time calculation
- Handles test navigation and GUI integration

**Test Base Classes (Tests.h/cpp):**
- Abstract Tests class defining common interface (Update, Render, RenderGUI)
- TestMenu class managing test registration and selection
- Template-based registration system for type-safe test creation

**Individual Test Classes:**
- TestShader: Demonstrates shader compilation, uniform setting, and multiple shader programs
- TestMesh: Shows mesh creation, factory methods, and geometry management
- TestRenderer: Validates OpenGL state management and rendering pipeline

**ImGui Integration:**
- Real-time parameter adjustment through sliders, colour pickers, and checkboxes
- Interactive test selection via button-based menu system
- Organised GUI panels with feature descriptions and controls

### Test Registration System

**Registration Process:**
```cpp
// In main application
TestMenu->RegisterTest<TestShader>("Shader Abstraction", window);
TestMenu->RegisterTest<TestMesh>("Mesh Abstraction", window);
TestMenu->RegisterTest<TestRenderer>("Renderer Abstraction", window);
```

**Template-Based Creation:**
- Type-safe test instantiation with constructor parameters
- Automatic memory management and cleanup
- Extensible design for adding new test types

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

### Test Framework Implementation

**Header (Shader.h):**
```cpp
class Shader {
private:
    std::string m_Filepath;
    unsigned int m_RendererID;
    std::unordered_map<std::string, int> m_uniformLocationCache;

public:
    Shader(const std::string& filepath);  // File-based constructor
    Shader(const char* vertexSrc, const char* fragmentSrc);  // String-based
    ~Shader();  // RAII cleanup

    void Bind() const;
    void Unbind() const;
    void setUniformMat4f(const std::string& name, const glm::mat4& matrix);
    // ... other uniform setters
};
```

**Key Features:**
- **Uniform Caching**: Avoids expensive glGetUniformLocation calls
- **Multiple Constructors**: Support for file-based and string-based shaders
- **RAII**: Automatic glDeleteProgram in destructor
- **Error Handling**: Comprehensive shader compilation error reporting

### Mesh Class Architecture

**Vertex Structure:**
```cpp
struct Vertex {
    float position[3];   // XYZ coordinates
    float colour[3];     // RGB colour values
    float texCoords[2];  // UV texture coordinates
};
```

**Mesh Management:**
```cpp
class Mesh {
private:
    unsigned int m_VAO, m_VBO, m_EBO;
    std::vector<Vertex> m_Vertices;
    std::vector<unsigned int> m_Indices;

public:
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    ~Mesh();  // RAII cleanup

    void Bind() const;
    static Mesh* CreateCube();  // Factory method
};
```

**Advantages:**
- **Type Safety**: Structured vertex data instead of raw float arrays
- **Memory Management**: Automatic VAO/VBO/EBO cleanup
- **Factory Pattern**: Easy creation of common geometries
- **Reusability**: Same mesh can be drawn multiple times

### Renderer Class Architecture

**Error Checking Macros:**
```cpp
#define ASSERT(x) if (!(x)) __debugbreak();

#define GlCall(x) glClearError();\
    x;\
    ASSERT(glLogCall(#x, __FILE__, __LINE__))
```

**Rendering Interface:**
```cpp
class Renderer {
public:
    void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;
    void Clear() const;
    void ClearColour_Black() const;
    void ClearColour_White() const;
};
```

**Benefits:**
- **Error Detection**: Automatic OpenGL error checking and reporting
- **High-Level Interface**: Simple Draw() calls instead of manual OpenGL
- **Debugging Support**: File and line number reporting for errors
- **State Management**: Centralized rendering state control

### Refactored Main Loop

**Before (Monolithic):**
```cpp
// Manual OpenGL calls scattered throughout main()
glGenBuffers(1, &VBO);
glBindBuffer(GL_ARRAY_BUFFER, VBO);
glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
glUseProgram(shaderProgram);
glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(cubeMvp));
glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
```

**After (Abstracted):**
```cpp
// Clean object-oriented interface
Mesh* cubeMesh = Mesh::CreateCube();
Shader shader(vertexShader, fragmentShader);
Renderer renderer;

// In render loop:
shader.Bind();
cubeMesh->Bind();
shader.setUniformMat4f("u_MVP", cubeMvp);
glDrawElements(GL_TRIANGLES, cubeMesh->GetIndexCount(), GL_UNSIGNED_INT, 0);
```

## Try These Experiments

### Beginner Challenges:
1. **Add new uniform setters**:
   ```cpp
   // In Shader class, add:
   void setUniform3f(const std::string& name, const glm::vec3& vector) {
       GlCall(glUniform3fv(getUniformLocation(name), 1, &vector[0]));
   }
   ```

2. **Create different mesh types**:
   ```cpp
   // Add to Mesh class:
   static Mesh* CreateTriangle();
   static Mesh* CreateQuad();
   static Mesh* CreateSphere(int subdivisions);
   ```

3. **Add renderer state methods**:
   ```cpp
   // In Renderer class:
   void EnableWireframe() const;
   void SetViewport(int x, int y, int width, int height) const;
   ```

### Intermediate Challenges:
1. **Implement shader hot-reloading**:
   ```cpp
   class Shader {
       void ReloadFromFile();  // Re-read and recompile shader file
       bool CheckFileModified();  // Check if file changed on disk
   };
   ```

2. **Add mesh transformation support**:
   ```cpp
   class Mesh {
       void Transform(const glm::mat4& matrix);  // Apply transformation to vertices
       void Scale(float factor);
       void Translate(const glm::vec3& offset);
   };
   ```

3. **Create material system**:
   ```cpp
   struct Material {
       glm::vec3 ambient, diffuse, specular;
       float shininess;
       unsigned int diffuseTexture, normalTexture;
   };
   ```

### Advanced Challenges:
1. **Implement render batching**:
   - Group similar objects to reduce draw calls
   - Sort by shader, then by texture, then by mesh
   - Use instanced rendering for identical objects

2. **Add memory profiling**:
   - Track OpenGL memory usage
   - Monitor vertex buffer and texture memory
   - Implement resource pooling

3. **Create scene graph**:
   - Hierarchical object relationships
   - Automatic transformation inheritance
   - Efficient frustum culling

## Common Issues & Solutions

### "Linker errors with new classes"
**Build Configuration Problems:**
- **Missing source files**: Ensure all .cpp files are in Visual Studio project
- **Include path issues**: Check header file paths and dependencies
- **Circular includes**: Use forward declarations in headers
- **Static library linking**: Verify OpenGL libraries are linked correctly

### "Runtime crashes in destructors"
**Resource Management Issues:**
- **Double deletion**: Ensure resources aren't deleted multiple times
- **Use after destruction**: Don't use objects after they're destroyed
- **OpenGL context**: Ensure OpenGL context exists when destructors run
- **Exception safety**: Use RAII consistently throughout codebase

### "Performance regression with abstraction"
**Overhead Concerns:**
- **Virtual function calls**: Keep hot paths non-virtual where possible
- **Excessive state changes**: Batch similar rendering operations
- **Memory allocations**: Pre-allocate containers and reuse objects
- **Debug vs Release**: Ensure compiler optimizations are enabled

### "Debugging abstracted OpenGL calls"
**Error Tracking Issues:**
- **GlCall macro**: Use consistently for all OpenGL calls
- **Error context**: Check which specific call failed
- **State validation**: Verify OpenGL state before operations
- **Resource tracking**: Monitor resource creation and deletion

## What's Next?

In the next branch (`09-test-harness`), we'll:
- **Implement testing framework** from the main branch architecture
- **Add unit tests** for our abstracted classes
- **Create integration tests** for rendering pipeline
- **Set up automated testing** for graphics code validation
- **Learn testing patterns** specific to graphics programming
- **Establish quality assurance** practices for continued development

## Resources for Deeper Learning

### Essential Reading:
- [Effective C++ by Scott Meyers](https://www.amazon.com/Effective-Specific-Improve-Programs-Designs/dp/0321334876)
- [Clean Code by Robert Martin](https://www.amazon.com/Clean-Code-Handbook-Software-Craftsmanship/dp/0132350884)
- [Game Engine Architecture by Jason Gregory](https://www.gameenginebook.com/)

### Video Tutorials:
- [The Cherno C++ Series](https://www.youtube.com/playlist?list=PLlrATfBNZ98dudnM48yfGUldqGD0S4FFb)
- [OpenGL Abstraction Patterns](https://www.youtube.com/results?search_query=opengl+abstraction+patterns)

### Deep Dive Topics:
- RAII and modern C++ resource management
- Design patterns in game engine architecture
- OpenGL debugging and profiling techniques
- Memory management in graphics applications
- Component-based entity systems
- Render graph architecture

### Tools for Development:
- **Visual Studio Debugger**: Step through abstracted object lifetimes
- **RenderDoc**: Analyze abstracted rendering calls
- **Application Verifier**: Detect resource leaks and errors

## Debug Tips

### Checking Object Lifetime:
```cpp
// Add logging to constructors/destructors
Shader::Shader(const std::string& filepath) {
    std::cout << "Creating shader: " << filepath << std::endl;
    // ... construction code
}

Shader::~Shader() {
    std::cout << "Destroying shader: " << m_Filepath << std::endl;
    GlCall(glDeleteProgram(m_RendererID));
}
```

### Validating OpenGL State:
```cpp
// Add state validation methods
void Renderer::ValidateState() const {
    GLint currentProgram;
    glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
    std::cout << "Current shader program: " << currentProgram << std::endl;
}
```

### Memory Leak Detection:
```cpp
// Track resource creation/deletion
static int shaderCount = 0;

Shader::Shader(...) {
    ++shaderCount;
    std::cout << "Shader count: " << shaderCount << std::endl;
}

Shader::~Shader() {
    --shaderCount;
    std::cout << "Shader count: " << shaderCount << std::endl;
}
```

### Common Abstraction Mistakes:
- Creating objects without proper OpenGL context
- Forgetting to call Bind() before using objects
- Not handling OpenGL errors consistently
- Mixing raw OpenGL calls with abstracted classes
- Improper resource cleanup ordering

---

