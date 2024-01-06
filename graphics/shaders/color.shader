#type vertex

void main() 
{
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
    gl_FrontColor = gl_Color;
}

#type fragment

uniform sampler2D texture;

void main()
{    
    vec4 texColor = texture2D(texture, gl_TexCoord[0].st);
    gl_FragColor = mix(texColor, gl_Color, texColor.a);    
}