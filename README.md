# Branch: 07-texturing

## Learning Objective
Master texture mapping and UV coordinate systems to apply detailed surface patterns to 3D objects. This branch introduces texture creation, sampling, coordinate systems, and filtering modes - fundamental techniques for creating realistic and visually appealing 3D graphics.

## What You'll Build
Five rotating 3D cubes with alternating procedural textures - checkerboard and gradient patterns. The implementation demonstrates texture binding, UV coordinate mapping, and shader-based texture sampling with colour blending.

![Expected Result](docs/images/07-texturing.png)
*A 960x540 window displaying 5 rotating cubes with alternating checkerboard and gradient textures*

## Key Concepts

### Core Concepts Learned:
- **Texture Mapping**: Applying 2D images to 3D surface geometry
- **UV Coordinates**: 2D texture coordinate system (U=horizontal, V=vertical)
- **Texture Sampling**: Reading colour values from textures in fragment shaders
- **Procedural Textures**: Generating texture patterns algorithmically
- **Texture Parameters**: Wrapping modes and filtering options
- **Texture Units**: Managing multiple textures in OpenGL
- **Texture Blending**: Combining texture colours with vertex colours
- **Texture Memory Management**: Creating, binding, and deleting textures

### OpenGL Functions Introduced:
- `glGenTextures()` - Generate texture object IDs
- `glBindTexture()` - Bind texture as current active texture
- `glTexImage2D()` - Upload texture data to GPU memory
- `glTexParameteri()` - Set texture filtering and wrapping parameters
- `glUniform1i()` - Set texture sampler uniform values
- `glDeleteTextures()` - Clean up texture memory

### GLSL Functions Introduced:
- `texture()` - Sample colour from texture at UV coordinates
- `sampler2D` - GLSL texture sampler uniform type

### Technical Terms:
- **UV Coordinates**: 2D texture space coordinates, typically ranging 0.0 to 1.0
- **Texture Atlas**: Single texture containing multiple sub-textures
- **Texture Unit**: Hardware slot for binding textures (GL_TEXTURE0, GL_TEXTURE1, etc.)
- **Texture Filtering**: Method for interpolating between texture pixels (nearest, linear)
- **Texture Wrapping**: Behaviour at texture boundaries (repeat, clamp, mirror)
- **Texel**: Single pixel in a texture (texture element)
- **Mipmapping**: Pre-computed lower resolution versions for distant objects
- **Anisotropic Filtering**: Advanced filtering for textures viewed at steep angles

## Code Architecture

### File Structure
```
src/
├── CMakeHelloWorld.cpp    # Main application with textured 3D cubes
└── CMakeHelloWorld.h      # Header file (if needed)

Dependencies:
├── OpenGL 3.3+           # Texture mapping support
└── GLM Library           # 3D mathematics and transformations
```

### Texture Pipeline
```
Texture Creation → GPU Upload → Shader Binding → UV Sampling → Colour Output
       ↓              ↓            ↓             ↓            ↓
   glGenTextures   glTexImage2D   glBindTexture  texture()   FragColor
```

### Vertex Data Structure
```
Each vertex: Position (XYZ) + Colour (RGB) + UV (UV) = 8 floats
Vertex layout: [X, Y, Z, R, G, B, U, V]
Total cube data: 8 vertices × 8 floats = 64 floats
```

## What's Different from Previous Branch (06-depth-testing)

### New Additions:
- **UV texture coordinates** added to vertex data (2 floats per vertex)
- **Procedural texture generation** with checkerboard and gradient patterns
- **Texture sampler uniforms** in fragment shader for texture access
- **Texture binding and management** in render loop
- **Texture filtering parameters** (nearest vs linear interpolation)
- **Multiple texture support** with alternating patterns per cube
- **Texture memory cleanup** in application shutdown

### What Stayed the Same:
- 3D cube geometry with 8 vertices and indexed rendering
- Depth testing and backface culling optimisations
- MVP transformation pipeline and camera setup
- Multiple cube rendering with individual transformations
- Keyboard input controls for interactive movement

## Understanding the Code

### UV Coordinate Mapping

**Vertex Data with UV Coordinates:**
```cpp
float vertices[] = {
    // Position        // Colour             // UV
    // X      Y     Z     R     G     B      U     V
    -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f,  // Bottom-left
     0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,  // Bottom-right
     0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,  // Top-right
    -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  0.0f, 1.0f,  // Top-left
};
```

**Vertex Attribute Configuration:**
```cpp
// Position attribute (location = 0)
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);

// Colour attribute (location = 1)
glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

// Texture coordinate attribute (location = 2)
glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
```

### Procedural Texture Generation

**Checkerboard Pattern:**
```cpp
unsigned int CreateCheckerboardTexture() {
    const int width = 8, height = 8;
    unsigned char textureData[width * height * 3];

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            bool isWhite = ((x + y) % 2) == 0;
            unsigned char color = isWhite ? 255 : 64;

            int index = (y * width + x) * 3;
            textureData[index + 0] = color; // Red
            textureData[index + 1] = color; // Green
            textureData[index + 2] = color; // Blue
        }
    }

    // Upload to GPU and set parameters...
}
```

**Gradient Pattern:**
```cpp
unsigned int CreateGradientTexture() {
    const int width = 16, height = 16;
    unsigned char textureData[width * height * 3];

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            unsigned char red = (255 * x) / (width - 1);   // Horizontal gradient
            unsigned char blue = (255 * y) / (height - 1);  // Vertical gradient
            unsigned char green = 128;                       // Constant

            int index = (y * width + x) * 3;
            textureData[index + 0] = red;
            textureData[index + 1] = green;
            textureData[index + 2] = blue;
        }
    }

    // Upload to GPU and set parameters...
}
```

### Shader Texture Sampling

**Vertex Shader (passes UV coordinates):**
```glsl
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColour;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 u_MVP;

out vec3 vertexColour;
out vec2 texCoord;

void main() {
    gl_Position = u_MVP * vec4(aPos, 1.0);
    vertexColour = aColour;
    texCoord = aTexCoord;   // Pass UV to fragment shader
}
```

**Fragment Shader (samples texture):**
```glsl
#version 330 core
in vec3 vertexColour;
in vec2 texCoord;

uniform sampler2D u_Texture;
out vec4 FragColor;

void main() {
    vec4 texColor = texture(u_Texture, texCoord);  // Sample texture
    FragColor = texColor * vec4(vertexColour, 1.0);  // Blend with vertex colour
}
```

### Texture Binding in Render Loop

```cpp
for (unsigned int i = 0; i < 5; i++) {
    // Alternate between textures for visual variety
    if (i % 2 == 0) {
        glBindTexture(GL_TEXTURE_2D, checkerboardTexture);
    } else {
        glBindTexture(GL_TEXTURE_2D, gradientTexture);
    }
    glUniform1i(textureLocation, 0);  // Use texture unit 0

    // Transform and draw cube...
}
```

## Try These Experiments

### Beginner Challenges:
1. **Modify UV coordinates** to scale textures:
   ```cpp
   // Double texture repeat by scaling UV coordinates
   -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f,  // Scale to 2.0, 2.0
    0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  2.0f, 0.0f,
    0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  2.0f, 2.0f,
   -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  0.0f, 2.0f,
   ```

2. **Change texture filtering**:
   ```cpp
   // Switch from nearest (pixelated) to linear (smooth)
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   ```

3. **Experiment with wrapping modes**:
   ```cpp
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
   ```

### Intermediate Challenges:
1. **Create animated UV coordinates**:
   ```cpp
   // In vertex shader
   vec2 animatedUV = aTexCoord + vec2(u_Time * 0.1, 0.0);  // Scrolling texture
   texCoord = animatedUV;
   ```

2. **Add more procedural patterns**:
   ```cpp
   unsigned int CreateCircleTexture() {
       // Create concentric circles pattern
       for (int y = 0; y < height; y++) {
           for (int x = 0; x < width; x++) {
               float centerX = width * 0.5f;
               float centerY = height * 0.5f;
               float distance = sqrt((x - centerX) * (x - centerX) + (y - centerY) * (y - centerY));
               unsigned char intensity = (unsigned char)(255 * (sin(distance * 0.5f) * 0.5f + 0.5f));
           }
       }
   }
   ```

3. **Implement texture atlasing**:
   ```cpp
   // Use different UV regions for different cube faces
   vec2 atlasUV = aTexCoord * 0.25;  // Scale to quarter size
   if (faceID == 1) atlasUV.x += 0.25;  // Offset for different faces
   if (faceID == 2) atlasUV.y += 0.25;
   ```

### Advanced Challenges:
1. **Load textures from image files**:
   - Integrate image loading library (stb_image, SOIL, etc.)
   - Support PNG, JPG, BMP formats
   - Handle different colour formats (RGB, RGBA)

2. **Implement multi-texturing**:
   - Bind multiple textures simultaneously
   - Blend between different texture layers
   - Use alpha channels for transparency

3. **Add normal mapping preparation**:
   - Create tangent space vectors
   - Prepare for bump mapping and normal mapping
   - Calculate proper coordinate systems per face

## Common Issues & Solutions

### "Textures appear black or white"
**Texture Binding Problems:**
- **Texture not bound**: Ensure `glBindTexture()` is called before drawing
- **Uniform not set**: Check `glUniform1i(textureLocation, 0)` is called
- **Wrong texture unit**: Verify texture unit matches uniform value
- **Texture not generated**: Confirm `glGenTextures()` and `glTexImage2D()` succeed

### "Textures appear stretched or distorted"
**UV Coordinate Issues:**
- **Wrong UV values**: Ensure UV coordinates range from 0.0 to 1.0 for normal mapping
- **Incorrect stride**: Verify vertex attribute pointer stride includes UV data
- **UV attribute disabled**: Check `glEnableVertexAttribArray(2)` for UV attribute
- **Wrong offset**: Confirm UV offset calculation (6 * sizeof(float))

### "Textures appear pixelated or blurry"
**Filtering Parameter Issues:**
- **Wrong filter mode**: Use `GL_NEAREST` for pixelated, `GL_LINEAR` for smooth
- **Missing mipmap**: Consider `GL_LINEAR_MIPMAP_LINEAR` for distant objects
- **Texture size mismatch**: Use power-of-two dimensions for better compatibility

### "Application crashes during texture operations"
**Memory Management Problems:**
- **Invalid texture ID**: Check texture generation succeeded
- **Wrong texture format**: Ensure format matches data (RGB vs RGBA)
- **Memory leak**: Always call `glDeleteTextures()` in cleanup
- **Double deletion**: Don't delete the same texture twice

## What's Next?

In the next branch (`08-abstraction`), we'll:
- **Refactor code architecture** by breaking apart the monolithic source file
- **Create Shader class** for shader compilation and management
- **Implement Mesh class** for vertex data and rendering abstraction
- **Build Renderer class** for high-level rendering commands
- **Establish project structure** that scales to complex applications
- **Prepare foundation** for advanced features like lighting and materials

## Resources for Deeper Learning

### Essential Reading:
- [LearnOpenGL - Textures](https://learnopengl.com/Getting-started/Textures)
- [OpenGL Wiki - Texture](https://www.khronos.org/opengl/wiki/Texture)
- [Real-Time Rendering - Texture Mapping](http://www.realtimerendering.com/)

### Video Tutorials:
- [The Cherno OpenGL - Textures](https://www.youtube.com/watch?v=n4k7ANAFsIQ)
- [OpenGL Texture Mapping Fundamentals](https://www.youtube.com/results?search_query=opengl+texture+mapping)

### Deep Dive Topics:
- Texture compression formats (DXT, ETC, ASTC)
- Advanced filtering techniques (anisotropic filtering)
- Procedural texture generation algorithms
- Texture streaming and memory management
- UV unwrapping and texture painting workflows
- Texture arrays and 3D textures

### Tools for Development:
- **RenderDoc**: Inspect texture data and binding states
- **Texture Tools**: NVIDIA Texture Tools, AMD Compressonator
- **Image Libraries**: stb_image, SOIL, FreeImage for loading files

## Debug Tips

### Checking Texture State:
```cpp
// Verify texture was created successfully
GLuint textureID;
glGenTextures(1, &textureID);
if (textureID == 0) {
    std::cout << "Error: Failed to generate texture!" << std::endl;
}
```

### Debugging UV Coordinates:
```cpp
// Output UV coordinates as colours in fragment shader
FragColor = vec4(texCoord.x, texCoord.y, 0.0, 1.0);  // Red=U, Green=V
```

### Texture Parameter Verification:
```cpp
// Check current texture parameters
GLint wrapS, wrapT, minFilter, magFilter;
glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, &wrapS);
glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, &wrapT);
glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &minFilter);
glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, &magFilter);
```

### Common Texture Mistakes:
- Forgetting to bind texture before setting parameters
- Not enabling texture coordinate vertex attribute
- Using wrong texture unit in uniform (glUniform1i value)
- Incorrect UV coordinate ranges or vertex data layout
- Missing texture deletion causing memory leaks

---

**Congratulations!** You've successfully implemented texture mapping and UV coordinate systems! Texture mapping is fundamental to modern 3D graphics - every game, 3D application, and visualization tool relies on textures to create detailed, realistic surfaces. You now understand the complete pipeline from procedural texture generation to GPU sampling and rendering.