#shader vertex // Specifies the vertex shader section
#version 330 core // Using GLSL version 3.30, compatible with OpenGL 3.3


layout(location = 0) in vec3 aPosition; //vertex positon 
layout(location = 1) in vec3 aNormal;   //Vertex Normal
layout(location = 2) in vec2 aTextureCoords; //Texture Coordinates

out vec2 v_TexCoords; //output to the fragment shader

void main()
{
    v_TexCoords = aTextureCoords;
    gl_Position = vec4(aPosition, 1.0);
};





#shader fragment // Specifies the fragment shader section
#version 330 core // Using GLSL version 3.30, compatible with OpenGL 3.3

in vec2  v_TexCoords;
out vec4 FragColor;


uniform sampler2D u_Texture;
uniform vec2      u_Texel;
uniform int       u_Effect;
uniform float     u_Opacity;


//see vision and image processing ppt
//this function will apply edge detection using a sobel filter
//detect edges by computing the gradient in both the horizontal and vertcal directions
//a high gradient means there is an edge, while a low gradient will indicate there is no edge. 

vec4 applyEdgeDetection()
{
    vec3 kernelX[3] = vec3[](vec3(-1,0,1), 
                          vec3(-2,0, 2),
                          vec3(-1,0,1));

    vec3 kernelY[3] = vec3[](vec3(-1,-2,-1), 
                          vec3( 0, 0, 0),
                          vec3( 1, 2, 1));

    //gradient in the x directions
    vec3 colourX = vec3(0.0);
    //gradient in the y directions
    vec3 colourY = vec3(0.0);


    for(int y = -1; y <= 1; y++)
    {
        for(int x = -1; x <= 1; x++)
        {
            vec3 sample = texture(u_Texture, v_TexCoords + u_Texel * vec2(x, y)).rgb;

            colourX += kernelX[y + 1][x + 1] * sample;
            colourY += kernelY[y + 1][x + 1] * sample;

        }
    }

    //the magnitude of the gradient that is used to highlight edges
    float edgeStrength = length(colourX) + length(colourY);
    return vec4(vec3(edgeStrength), 1.0);


}


//see more about this in the powerpoint called post processing using fragment shader

//fnctin to invert the colours of the texture 
vec4 applyColourInversion()
{
    vec4 color = texture(u_Texture, v_TexCoords);
    return vec4(1.0 - color.rgb, 1.0);
}


//see more about this in the powerpoint called post processing using fragment shader
//function will apply a greyscale effect
//convert the image to greyscale based on luminace formula 

vec4 applyGreyScale()
{
    vec4 colour = texture(u_Texture, v_TexCoords);

    float grey = dot(colour.rgb, vec3(0.299, 0.587, 0.114));

    return vec4(vec3(grey), 1.0);
}


//blur the image by averaging the surrounding pixels

//we're going apply a 3x3 kernal that will be used to give a higher weight to the center pixel and the surrounding pixels'


vec4 applyBlur() //a gaussian blur 
{
    vec3 blur = vec3(0.0);

    float kernel[9] = float[](1.0 / 16, 2.0 / 16, 1.0 / 16,
                              2.0 / 16, 4.0 / 16, 2.0 / 16,
                              1.0 / 16, 2.0 / 16, 1.0 / 16);

    int index = 0;
    for(int y = -1; y <= 1; y++)
    {
        for(int x = -1; x<= 1; x++)
        {
            vec3 sample = texture(u_Texture, v_TexCoords + u_Texel * vec2(x, y)).rgb;
            blur += sample * kernel[index++];
        }
    }
    return vec4(blur, 1.0);
}


//this is a function to sharpen the image
//enhance the edges by booting the center pixel and 
//reducing contributuions from the surrounding pixels
vec4 applySharpen()
{
    vec3 kernel[3] = vec3[](vec3(-1, -1, -1),
                            vec3(-1,  9, -1),
                            vec3(-1, -1, -1)); //sharpen kernel

    vec3 sharp = vec3(0.0);


    for(int y = -1; y <= 1; y++)
    {
        for(int x = -1; x<= 1; x++)
        {
            vec3 sample = texture(u_Texture, v_TexCoords + u_Texel * vec2(x, y)).rgb;
            sharp += sample * kernel[y + 1][x + 1];
        }
    }
    return vec4(sharp, 1.0);
}




//a function to apply a blur effect
//increase the brightness for pixels that are above a luminance threshhold 

vec4 applyBloom()
{
    vec4 colour= texture(u_Texture, v_TexCoords); //get the texture colour
    float brightness = dot(colour.rgb, vec3(0.2126, 0.7152, 0.0722)); //luminance 
    if(brightness > 0.7) //if the brightness is higher than 0.7, enhance the brightness by simulating it
    {
        return vec4(colour.rgb * 1.5, 1.0);

    }
    return colour;
}


void main()
{
    vec4 baseColour = texture(u_Texture, v_TexCoords); 
    vec4 result = baseColour;

    if(u_Effect == 0)
    {
        
    }
    else if(u_Effect == 1)
    {
        result = applyEdgeDetection();
    }
    else if(u_Effect == 2)
    {
        result = applyColourInversion();
    }
    else if(u_Effect == 3)
    {
        result = applyBlur();
    }
    else if(u_Effect == 4)
    {
        result = applySharpen();
    }    
    else if(u_Effect == 5)
    {
        result = applyBloom();
    }    
    else if(u_Effect == 6)
    {
        result = applyGreyScale();
    }    
    else if(u_Effect == 7)
    {
    //venezuela :)))))))
        if(v_TexCoords.y < 0.33)
        {       
            result = vec4(1.0, 0.0, 0.0, 1.0);
        }
        else if(v_TexCoords.y > 0.33 && v_TexCoords.y < 0.66)
        {
            result = vec4(0.0, 0.0, 1.0, 1.0);
        }
         else
        { 
            result = vec4(1.0, 1.0, 0.0, 1.0);
        }
    }
    else if (u_Effect == 8)
    {

        //Romania :))) 
        if(v_TexCoords.x < 0.33)
        {
            result = vec4(0.0, 0.0, 1.0, 1.0);
        }
        else if(v_TexCoords.x > 0.33 && v_TexCoords.x < 0.66)
        {
            result = vec4(1.0, 1.0, 0.0, 1.0);
        }
        else
        {
            result = vec4(1.0, 0.0, 0.0, 1.0);
        }
    }else if (u_Effect == 9)
    {
       //central square inverted
       if(v_TexCoords.x > 0.33 && v_TexCoords.x < 0.66 
         && v_TexCoords.y > 0.33 && v_TexCoords.y < 0.66)
        {
             result = applyColourInversion();
        }
        else
        {
            result = applyBlur();
        }
    }

    FragColor = mix(baseColour, result, u_Opacity);
}









