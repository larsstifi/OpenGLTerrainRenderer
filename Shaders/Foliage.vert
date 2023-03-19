#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

struct VSout{
    vec3 FragPos;
    vec3 norm;
    vec2 TexCoords;
    int instanceID;
};
out VSout vs_out;



void main()
{
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    vs_out.norm =  normalize(mat3(model) * aNorm); 
    vs_out.TexCoords = aTexCoord;
    vs_out.instanceID = gl_InstanceID;
}
