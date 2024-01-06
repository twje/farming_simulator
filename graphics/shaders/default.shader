// This shader uses the fixed-function pipeline conventions (versions 3.3 and above), but they are still 
// available for compatibility.

#type vertex

// Transform from model space to screen space
void main()
{
    // Transform vertex position from model space to clip space
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex; // gl_ModelViewProjectionMatrix: Model-View-Projection matrix

    // Apply texture transformations and assign to texture coordinates
    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0; // gl_TextureMatrix[0]: Texture transformation matrix

    // Pass vertex color to fragment shader
    gl_FrontColor = gl_Color; // gl_Color: Vertex color
}

#type fragment

uniform sampler2D texture;

void main()
{
    // Sample the texture color using the texture coordinates from the vertex shader
    vec4 texColor = texture2D(texture, gl_TexCoord[0].xy);

    // Combine the texture color with the vertex color
    gl_FragColor = texColor * gl_Color;
}