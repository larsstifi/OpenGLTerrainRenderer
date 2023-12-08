#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 60) out;

struct VSout{
    vec3 FragPos;
    vec3 norm;
    vec2 TexCoords;
};
in VSout vs_out[];
struct GSout{
    vec3 FragPos;
    vec3 norm;
    vec2 TexCoords;
};
out GSout gs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 viewPos;
uniform float dt;

void GenerateLine(int index)
{
    gl_Position =  gl_in[index].gl_Position;
    gs_out.norm = vs_out[index].norm;
    gs_out.TexCoords = vs_out[index].TexCoords;
    gs_out.FragPos = vs_out[index].FragPos;
    EmitVertex();
    gl_Position = (gl_in[index].gl_Position +  vec4(normalize(mat3(projection) * mat3(view) * vs_out[index].norm), 0.0) * 1);
    gs_out.norm = vs_out[index].norm;
    gs_out.TexCoords = vs_out[index].TexCoords;
    gs_out.FragPos = vs_out[index].FragPos;
    EmitVertex();
    EndPrimitive();
}

void main()
{
    GenerateLine(0); // first vertex normal
    GenerateLine(1); // second vertex normal
    GenerateLine(2); // third vertex normal
}  
/*
void main() {    
    vec3 offset = vec3(0);
    vec4 offsetP = (projection * view * vec4(offset,0));

    gl_Position = gl_in[0].gl_Position + offsetP;    // 1:bottom-left
    gs_out.FragPos = vs_out[0].FragPos + offset;
    gs_out.norm = vs_out[0].norm;
    gs_out.TexCoords = vs_out[0].TexCoords;
    EmitVertex();  
    gl_Position = gl_in[1].gl_Position + offsetP;    // 1:bottom-left
    gs_out.FragPos = vs_out[1].FragPos + offset;
    gs_out.norm = vs_out[1].norm;
    gs_out.TexCoords = vs_out[1].TexCoords;
    EmitVertex();  
    gl_Position = gl_in[2].gl_Position + offsetP;    // 1:bottom-left
    gs_out.FragPos = vs_out[2].FragPos + offset;
    gs_out.norm = vs_out[2].norm;
    gs_out.TexCoords = vs_out[2].TexCoords;
    EmitVertex();  
}  
*/