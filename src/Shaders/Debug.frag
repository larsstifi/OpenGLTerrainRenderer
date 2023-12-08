#version 330 core


struct GSout{
    vec3 FragPos;
    vec3 norm;
    vec2 TexCoords;
};
in GSout gs_out;

//textures
uniform sampler2D texture1;




//Lights
uniform vec3 viewPos;

out vec4 FragColor;


void main()
{   
    FragColor = vec4(gs_out.norm, 1);
}