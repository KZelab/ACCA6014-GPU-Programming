# Branch: 05-transformations

## Learning Objective
Learn 3D transformations and the graphics pipeline by implementing Model-View-Projection matrices. This introduces coordinate systems, matrix mathematics, and interactive controls that form the foundation of all 3D graphics applications.

## What You'll Build
An animated, interactive square that continuously rotates and responds to keyboard input. The square can be moved with WASD keys and scaled with Q/E, all rendered with proper 3D perspective projection.

![Expected Result](docs/images/05-animated-square.png)
*A 960x540 window displaying a rotating rainbow square that responds to keyboard input in 3D space*

## Key Concepts

### Core Concepts Learned:
- **Transformation Matrices**: Mathematical tools for moving, rotating, and scaling objects
- **Model-View-Projection (MVP)**: The standard 3D graphics pipeline
- **Coordinate Systems**: Local/Model, World, View, and Clip coordinates
- **Matrix Multiplication**: How transformations combine and order matters
- **Uniform Variables**: Passing data from CPU to GPU shaders
- **Interactive Controls**: Real-time input affecting 3D transformations
- **Perspective Projection**: Creating realistic 3D depth perception

### OpenGL Functions Introduced:
- `glGetUniformLocation()` - Find uniform variable locations in shaders
- `glUniformMatrix4fv()` - Upload matrix data to GPU
- `glfwGetKey()` - Check keyboard input state
- `glfwGetTime()` - Get elapsed time for animations
- `glm::perspective()` - Create perspective projection matrix
- `glm::lookAt()` - Create view/camera matrix
- `glm::translate()`, `glm::rotate()`, `glm::scale()` - Transformation functions

### GLM Library Functions:
- `glm::mat4()` - 4x4 matrix creation
- `glm::radians()` - Convert degrees to radians
- `glm::value_ptr()` - Get pointer to matrix data for OpenGL
- Matrix multiplication with `*` operator
- Vector creation with `glm::vec3()`

### Technical Terms:
- **MVP Matrix**: Combined transformation from model to screen coordinates
- **Model Matrix**: Local object space to world space transformation
- **View Matrix**: World space to camera/eye space transformation
- **Projection Matrix**: 3D to 2D screen projection transformation
- **Uniform**: Global shader variable set from CPU
- **Identity Matrix**: Neutral transformation (no change)
- **Homogeneous Coordinates**: 4D vectors for 3D transformations

## Code Architecture

### File Structure
```
src/
├── CMakeHelloWorld.cpp    # Main application with 3D transformations
└── CMakeHelloWorld.h      # Header file (if needed)

Dependencies:
├── GLM Library            # Mathematics for 3D transformations
└── OpenGL 3.3+           # Matrix uniform support
```

### Transformation Pipeline
```
Local Coordinates → Model Matrix → World Coordinates
       ↓
View Matrix → Camera Coordinates → Projection Matrix → Clip Coordinates
       ↓
Perspective Division → Normalized Device Coordinates → Screen Coordinates
```

### Matrix Multiplication Order
The order of matrix multiplication is crucial:
```cpp
MVP = Projection * View * Model
```
This reads right-to-left: Model → View → Projection

## What's Different from Previous Branch (04-indexed-rendering)

### New Additions:
- **GLM mathematics library** for 3D vector and matrix operations
- **MVP matrix uniform** passed to vertex shader each frame
- **Perspective projection** creating realistic 3D depth
- **View/camera matrix** positioning virtual camera in 3D space
- **Dynamic model transformations** with rotation animation
- **Keyboard input handling** for interactive movement and scaling
- **Time-based animation** using `glfwGetTime()`

### What Stayed the Same:
- Indexed square geometry (4 vertices, 6 indices)
- Vertex attributes (position + colour)
- Fragment shader (unchanged)
- VBO/VAO/EBO setup and cleanup
- Basic render loop structure

## Understanding the Code

### The Graphics Pipeline Transformation

**1. Model Matrix (Local → World Space):**
```cpp
glm::mat4 model = glm::mat4(1.0f);  // Identity matrix (no transformation)
model = glm::rotate(model, currentTime, glm::vec3(0.0f, 0.0f, 1.0f));
```
Transforms vertices from the object's local coordinate system to world coordinates.

**2. View Matrix (World → Camera Space):**
```cpp
glm::mat4 view = glm::lookAt(
    glm::vec3(0.0f, 0.0f, 3.0f),   // Camera position (3 units back)
    glm::vec3(0.0f, 0.0f, 0.0f),   // Look at origin
    glm::vec3(0.0f, 1.0f, 0.0f)    // Up vector (Y-axis)
);
```
Simulates placing a camera in the 3D world.

**3. Projection Matrix (Camera → Screen Space):**
```cpp
glm::mat4 projection = glm::perspective(
    glm::radians(45.0f),    // 45-degree field of view
    960.0f / 540.0f,        // Aspect ratio (width/height)
    0.1f,                   // Near clipping plane
    100.0f                  // Far clipping plane
);
```
Creates perspective distortion - objects further away appear smaller.

### Matrix Uniform Upload

```cpp
// Get uniform location (do this once, outside render loop)
int mvpLocation = glGetUniformLocation(shaderProgram, "u_MVP");

// Upload matrix each frame (inside render loop)
glm::mat4 mvp = projection * view * model;
glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(mvp));
```

### Keyboard Controls Implementation

```cpp
if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    model = glm::translate(model, glm::vec3(0.0f, 0.001f, 0.0f));  // Small upward movement
}
```
Small incremental movements accumulate over frames for smooth motion.

## Try These Experiments

### Beginner Challenges:
1. **Change rotation axis** - Rotate around different axes:
   ```cpp
   // Rotate around X-axis (like a wheel)
   model = glm::rotate(model, currentTime, glm::vec3(1.0f, 0.0f, 0.0f));

   // Rotate around Y-axis (like a spinning top)
   model = glm::rotate(model, currentTime, glm::vec3(0.0f, 1.0f, 0.0f));
   ```

2. **Modify rotation speed**:
   ```cpp
   float rotationSpeed = 2.0f;  // 2x faster
   model = glm::rotate(model, currentTime * rotationSpeed, glm::vec3(0.0f, 0.0f, 1.0f));
   ```

3. **Change camera position**:
   ```cpp
   // Camera looking from the side
   glm::mat4 view = glm::lookAt(
       glm::vec3(3.0f, 0.0f, 0.0f),   // Camera to the right
       glm::vec3(0.0f, 0.0f, 0.0f),   // Still looking at origin
       glm::vec3(0.0f, 1.0f, 0.0f)    // Up vector unchanged
   );
   ```

### Intermediate Challenges:
1. **Add oscillating scale animation**:
   ```cpp
   float scaleAmount = 1.0f + 0.3f * sin(currentTime * 2.0f);  // Oscillate between 0.7 and 1.3
   model = glm::scale(model, glm::vec3(scaleAmount, scaleAmount, 1.0f));
   ```

2. **Create orbital motion**:
   ```cpp
   float radius = 1.5f;
   glm::vec3 orbitPosition = glm::vec3(
       radius * cos(currentTime),
       radius * sin(currentTime),
       0.0f
   );
   model = glm::translate(model, orbitPosition);
   ```

3. **Implement mouse look camera**:
   ```cpp
   // Get mouse position and convert to camera angles
   double mouseX, mouseY;
   glfwGetCursorPos(window, &mouseX, &mouseY);

   float yaw = (mouseX / 960.0f) * 2.0f * 3.14159f;     // Full circle
   float pitch = (mouseY / 540.0f) * 3.14159f - 1.57f;  // +/- 90 degrees

   glm::vec3 cameraPos = glm::vec3(
       3.0f * cos(pitch) * cos(yaw),
       3.0f * sin(pitch),
       3.0f * cos(pitch) * sin(yaw)
   );

   view = glm::lookAt(cameraPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
   ```

### Advanced Challenges:
1. **Multiple objects** - Render several squares with different transformations
2. **Hierarchical transformations** - Parent-child object relationships
3. **Smooth input handling** - Implement acceleration and deceleration
4. **Camera animation** - Automatic camera movement around the scene

## Common Issues & Solutions

### "Square not visible or distorted"
**Possible Causes & Solutions:**
- **Wrong matrix order**: Ensure `MVP = projection * view * model` (not the reverse)
- **Camera too close/far**: Adjust camera position in `glm::lookAt()`
- **Wrong field of view**: Try different angles in `glm::perspective()`
- **Aspect ratio mismatch**: Use actual window width/height ratio

### "Animation too fast or slow"
**Timing Issues:**
- **Frame rate dependent**: Use `deltaTime` instead of fixed increments
- **Rotation speed**: Multiply `currentTime` by a speed factor
- **Input sensitivity**: Adjust movement amounts in keyboard handling

### "Matrix uniform not updating"
**Uniform Upload Problems:**
- **Location not found**: Check `glGetUniformLocation()` return value (-1 = not found)
- **Wrong shader active**: Call `glUseProgram()` before `glUniformMatrix4fv()`
- **Shader compilation error**: Uniform might be optimised away if unused

### "Keyboard input not responsive"
**Input Handling Issues:**
- **Polling location**: Call `glfwPollEvents()` in render loop
- **Key state persistence**: Keys need to be held down, not just pressed once
- **Movement amounts**: Might be too small to see (increase values for testing)

## What's Next?

In the next branch (`06-depth-testing`), we'll:
- Learn about **depth buffers** and Z-testing for proper 3D rendering
- Understand **depth function** and **depth writing**
- Implement **multiple overlapping objects** that render correctly
- Create **3D cube geometry** instead of flat squares
- Explore **backface culling** and **winding order**

## Resources for Deeper Learning

### Essential Reading:
- [LearnOpenGL - Transformations](https://learnopengl.com/Getting-started/Transformations)
- [OpenGL Wiki - Uniform](https://www.khronos.org/opengl/wiki/Uniform_(GLSL))
- [GLM Documentation](https://glm.g-truc.net/0.9.9/index.html)

### Video Tutorials:
- [The Cherno OpenGL - Matrices](https://www.youtube.com/watch?v=LhQ85bkQpGk)
- [3Blue1Brown - Linear Transformations](https://www.youtube.com/watch?v=kYB8IZa5AuE)

### Deep Dive Topics:
- Linear algebra foundations for computer graphics
- Quaternions for rotation representation
- Matrix decomposition and inverse transformations
- Gimbal lock and rotation interpolation
- Camera control systems and movement patterns

### Tools for Development:
- **RenderDoc**: Visualise transformation matrices and uniform values
- **Linear Algebra Visualizers**: Understand matrix transformations graphically
- **GLM Documentation**: Complete reference for all mathematics functions

## Debug Tips

### Checking Uniform Locations:
```cpp
int mvpLocation = glGetUniformLocation(shaderProgram, "u_MVP");
if (mvpLocation == -1) {
    std::cout << "Error: u_MVP uniform not found!" << std::endl;
}
```

### Debugging Matrix Values:
```cpp
// Print matrix for debugging
glm::mat4 mvp = projection * view * model;
for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
        std::cout << mvp[i][j] << " ";
    }
    std::cout << std::endl;
}
```

### Verifying Transformations:
```cpp
// Test with identity matrix (should render normally)
glm::mat4 mvp = glm::mat4(1.0f);  // No transformation
glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(mvp));
```

### Common Matrix Mistakes:
- Forgetting to multiply matrices in correct order (P * V * M)
- Using degrees instead of radians in GLM functions
- Not updating uniforms every frame for animations
- Applying transformations in wrong coordinate space

---

**Congratulations!** You've successfully implemented 3D transformations and the graphics pipeline! Understanding the Model-View-Projection matrix chain is fundamental to all 3D graphics programming. Every 3D game, CAD application, and visualization tool uses these same mathematical principles to position and display objects in 3D space.