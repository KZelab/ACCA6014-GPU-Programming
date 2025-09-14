# Branch: 04-indexed-rendering

## Learning Objective
Learn efficient geometry rendering using Index Buffer Objects (EBO) to eliminate duplicate vertices. This introduces indexed rendering, which is fundamental for creating complex 3D models efficiently.

## What You'll Build
A colourful square made from two triangles, but using only 4 vertices instead of 6! This demonstrates how indexed rendering eliminates duplicate vertex data and makes complex geometry more memory-efficient.

![Expected Result](docs/images/04-indexed-square.png)
*A 960x540 window displaying a rainbow-coloured square with 4 corners: red, green, blue, and yellow*

## Key Concepts

### Core Concepts Learned:
- **Index Buffer Objects (EBO)**: Efficient storage of vertex indices
- **Indexed vs Array Rendering**: `glDrawElements()` vs `glDrawArrays()`
- **Vertex Reuse**: Eliminating duplicate vertices in complex geometry
- **Memory Efficiency**: Reducing GPU memory usage for large models
- **Triangle Topology**: Understanding how indices define triangle faces
- **Element Array Buffers**: OpenGL buffer type for storing indices

### OpenGL Functions Introduced:
- `glGenBuffers()` for Element Buffer Objects
- `glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ...)` - Bind index buffer
- `glBufferData(GL_ELEMENT_ARRAY_BUFFER, ...)` - Upload index data
- `glDrawElements()` - Draw using indexed rendering
- `glDeleteBuffers()` for EBO cleanup

### Technical Terms:
- **Index/Element Buffer Object (EBO)**: Buffer storing vertex indices
- **Index**: Integer pointing to a vertex in the vertex array
- **Triangle List**: Drawing mode using every 3 indices as one triangle
- **Vertex Reuse**: Same vertex used by multiple triangles
- **Memory Bandwidth**: Data transfer efficiency between CPU and GPU
- **Winding Order**: Direction triangles face (clockwise/counter-clockwise)

## Code Architecture

### File Structure
```
src/
├── CMakeHelloWorld.cpp    # Main application with indexed square
└── CMakeHelloWorld.h      # Header file (if needed)
```

### Memory Layout Comparison

**Without Indexing (6 vertices for square):**
```
Triangle 1: V0(-0.5,-0.5), V1(0.5,-0.5), V2(0.5,0.5)
Triangle 2: V3(0.5,0.5), V4(-0.5,0.5), V5(-0.5,-0.5)

Note: V0=V5, V2=V3 (duplicates!)
Memory: 6 vertices × 6 floats = 36 floats
```

**With Indexing (4 unique vertices + 6 indices):**
```
Vertices: V0(-0.5,-0.5), V1(0.5,-0.5), V2(0.5,0.5), V3(-0.5,0.5)
Indices: [0,1,2] [2,3,0]

Memory: 4 vertices × 6 floats + 6 indices = 30 floats (17% savings!)
```

### Index Array Explanation
```cpp
unsigned int indices[] = {
    0, 1, 2,   // First triangle: bottom-left → bottom-right → top-right
    2, 3, 0    // Second triangle: top-right → top-left → bottom-left
};
```

The magic happens when vertex 0 and vertex 2 are reused by both triangles!

## What's Different from Previous Branch (03-vertex-attributes)

### New Additions:
- **Square geometry** instead of triangle (4 vertices, 2 triangles)
- **Index Buffer Object (EBO)** for storing vertex indices
- **Index array** defining triangle connectivity
- **glDrawElements()** for indexed rendering
- **Memory-efficient vertex layout** eliminating duplicates
- **EBO cleanup** in resource management

### What Stayed the Same:
- Vertex attributes (position + colour)
- Shader programs (vertex and fragment)
- VAO configuration for vertex attributes
- Interleaved vertex data layout
- Basic render loop structure

## Understanding the Code

### Why Use Indexed Rendering?

**Problem with Array Rendering:**
To create a square with `glDrawArrays()`, you need 6 vertices (2 triangles × 3 vertices):
```cpp
float vertices[] = {
    // Triangle 1
    -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,  // Bottom-left
     0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,  // Bottom-right
     0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f,  // Top-right

    // Triangle 2 (duplicates bottom-left and top-right!)
     0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f,  // Top-right (duplicate)
    -0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 0.0f,  // Top-left
    -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f   // Bottom-left (duplicate)
};
```

**Solution with Indexed Rendering:**
Only store unique vertices and use indices to reference them:
```cpp
float vertices[] = {
    // Only 4 unique vertices needed!
    -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,  // 0: Bottom-left
     0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,  // 1: Bottom-right
     0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f,  // 2: Top-right
    -0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 0.0f   // 3: Top-left
};

unsigned int indices[] = {
    0, 1, 2,   // Triangle 1: reuse vertices 0, 1, 2
    2, 3, 0    // Triangle 2: reuse vertices 2, 3, 0
};
```

### EBO Binding and VAO State

The EBO becomes part of the VAO state when bound:
```cpp
glBindVertexArray(VAO);           // Start VAO state recording
// ... configure vertex attributes ...
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);  // EBO becomes part of VAO
glBufferData(GL_ELEMENT_ARRAY_BUFFER, ...);  // Upload indices
```

Later, when rendering:
```cpp
glBindVertexArray(VAO);           // Automatically binds associated EBO too!
glDrawElements(...);              // Uses the EBO bound to this VAO
```

### glDrawElements Parameters
```cpp
glDrawElements(mode, count, type, indices);
```
- **mode**: `GL_TRIANGLES` (draw triangles)
- **count**: 6 (number of indices to process)
- **type**: `GL_UNSIGNED_INT` (index data type)
- **indices**: 0 (offset into index array)

## Try These Experiments

### Beginner Challenges:
1. **Change square colours** - Modify vertex colours:
   ```cpp
   // Make a sunset square
   -0.5f, -0.5f, 0.0f,  1.0f, 0.5f, 0.0f,  // Orange
    0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.5f,  // Pink
    0.5f,  0.5f, 0.0f,  0.5f, 0.0f, 1.0f,  // Purple
   -0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 0.5f   // Light yellow
   ```

2. **Create different shapes** - Modify indices for different triangle arrangements:
   ```cpp
   // Draw only one triangle
   unsigned int indices[] = { 0, 1, 2 };
   // Change glDrawElements count to 3
   ```

3. **Reverse winding order** - Flip triangle orientation:
   ```cpp
   unsigned int indices[] = {
       0, 2, 1,   // Reversed first triangle
       2, 0, 3    // Reversed second triangle
   };
   ```

### Intermediate Challenges:
1. **Create a hexagon** using indexed rendering:
   ```cpp
   // 7 vertices: center + 6 around edge
   float vertices[] = {
        0.0f,  0.0f, 0.0f,  1.0f, 1.0f, 1.0f,  // 0: Center - White
        0.5f,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  // 1: Right - Red
        0.25f, 0.43f, 0.0f, 1.0f, 0.5f, 0.0f,  // 2: Top-right - Orange
        // ... continue with 6 edge vertices
   };

   unsigned int indices[] = {
       0, 1, 2,  // Triangle 1
       0, 2, 3,  // Triangle 2
       // ... 6 triangles total
   };
   ```

2. **Create a wireframe mode**:
   ```cpp
   // Before rendering loop
   glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

   // After rendering loop (to restore)
   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
   ```

3. **Dynamic index modification** - Change indices at runtime:
   ```cpp
   // In render loop, alternate between square and triangles
   static bool showSquare = true;
   if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
       showSquare = !showSquare;
   }

   int indexCount = showSquare ? 6 : 3;  // 6 for square, 3 for triangle
   glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
   ```

### Advanced Challenges:
1. **Triangle strips and fans** - Use different primitive types
2. **Index buffer sub-ranges** - Draw different parts of the same model
3. **Multiple index buffers** - Different detail levels for same geometry
4. **16-bit indices** - Use `GL_UNSIGNED_SHORT` for smaller models

## Common Issues & Solutions

### "Square not rendering or appears wrong"
**Possible Causes & Solutions:**
- **Wrong index count**: Ensure `glDrawElements` count matches your indices array size
- **Index out of range**: Check all indices are valid (0 to vertex count - 1)
- **EBO not bound**: EBO must be bound while VAO is active
- **Wrong index type**: Match `glDrawElements` type parameter to your indices

### "Memory access errors or crashes"
**Index Array Issues:**
- **Out-of-bounds indices**: Index 5 with only 4 vertices will crash
- **Wrong indices data type**: Using `int` indices but specifying `GL_UNSIGNED_SHORT`
- **Buffer size mismatch**: `sizeof(indices)` doesn't match actual array size

### "Performance not improving with indexing"
**Optimization Notes:**
- **Small geometry**: Indexing overhead may outweigh benefits for simple shapes
- **Cache misses**: Random index access patterns can reduce GPU efficiency
- **Vertex reuse ratio**: Benefits increase with more shared vertices

### "Triangles facing wrong direction"
**Winding Order Issues:**
- **Backface culling**: Enable with `glEnable(GL_CULL_FACE)` to see the effect
- **Clockwise vs counter-clockwise**: OpenGL default front face is counter-clockwise
- **Index order matters**: [0,1,2] vs [0,2,1] creates opposite-facing triangles

## What's Next?

In the next branch (`05-transformations`), we'll:
- Learn about **transformation matrices** for moving, rotating, and scaling objects
- Understand **Model-View-Projection (MVP)** pipeline
- Implement **interactive transformations** with keyboard input
- Create **animated geometry** using time-based transformations
- Explore **coordinate systems** and **matrix mathematics**

## Resources for Deeper Learning

### Essential Reading:
- [LearnOpenGL - Elements Buffer Objects](https://learnopengl.com/Getting-started/Hello-Triangle)
- [OpenGL Wiki - Vertex Rendering](https://www.khronos.org/opengl/wiki/Vertex_Rendering)
- [OpenGL Specification - Drawing Commands](https://registry.khronos.org/OpenGL/specs/gl/glspec46.core.pdf)

### Video Tutorials:
- [The Cherno OpenGL - Index Buffers](https://www.youtube.com/watch?v=MXNMC1YAxVQ)
- [OpenGL Tutorial - Indexed Drawing](https://www.youtube.com/watch?v=WMiggUPst-Q)

### Deep Dive Topics:
- GPU memory bandwidth optimization
- Vertex cache optimization and mesh optimization
- Triangle strips, fans, and adjacency primitives
- Geometry instancing and indirect drawing
- Mesh compression techniques

### Tools for Development:
- **RenderDoc**: Analyze vertex and index buffer usage
- **GPU Memory Profilers**: Measure memory bandwidth improvements
- **Mesh Optimization Tools**: Tools like meshoptimizer for index optimization

## Debug Tips

### Checking Index Buffer State:
```cpp
GLint currentEBO;
glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &currentEBO);
std::cout << "Current EBO: " << currentEBO << std::endl;
```

### Verifying Index Data:
```cpp
// Print indices for debugging
for (int i = 0; i < 6; i++) {
    std::cout << "Index " << i << ": " << indices[i] << std::endl;
}
```

### Visualising Vertex Reuse:
```cpp
// Color vertices by usage count to see sharing
// Vertices used by 2 triangles will have different interpolation
```

### Common Index Buffer Mistakes:
- Forgetting to bind EBO before uploading data
- Using signed integers instead of unsigned for indices
- Calculating wrong byte size for `glBufferData()`
- Not updating draw call count when changing indices

---

**Congratulations!** You've successfully implemented indexed rendering, a fundamental optimization technique used in all modern 3D graphics applications. Understanding how to efficiently organize vertex data and eliminate duplicates is crucial for rendering complex models with millions of triangles. Every 3D game and application relies on these same principles!