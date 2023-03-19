#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 12) out;

struct VSout{
    vec3 FragPos;
    vec3 norm;
    vec2 TexCoords;
    int instanceID;
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

float when_gt(float x, float y){
    return max(sign(x-y), 0.0f);
}

float nrand(vec3 uv)
{
    return fract(sin(dot(uv, vec3(12.9898, 78.233, 45.5386))) * 43758.5453);
}
float nrand(float uv)
{
    return fract(sin(uv * 12.9898) * 43758.5453);
}

vec2 randPoint(vec3 offset){
    float u1 = nrand(vs_out[0].FragPos + offset); //sample U[0,1]
    float u2 = nrand(vs_out[0].FragPos + offset + 10 * vec3(1));
    float gt = when_gt(u1 + u2, 1) ;
    u1 = (1-gt) * (u1) + gt * (1 - u1);
    float gtu2 = when_gt(u2, 0.5) ;
    u2 = (1-gt) * (u2) + gt * (1 - u2);


    return vec2(u1,u2);
}

void drawPyramid(vec3 pos, vec3 up, vec3 dir, float size){
    vec4 sPos = projection * view * vec4(pos,1);
    sPos = sPos / sPos.w;
    if(sPos.x < -1.3 || sPos.x > 1.3 || sPos.y < -1.3 || sPos.y > 1.3 || sPos.z < 0 || sPos.z > 0.98f) return;
    vec3 left = normalize(cross(dir, up));
    vec3 front = normalize(cross(left, up));
    vec3 norm = up;
    float rand = nrand(pos);
    vec3 topOffset = up * size + vec3(sin(dt + rand*3), cos(dt+rand*3),0)*size * 0.1;
    gs_out.FragPos = pos + topOffset;
    gl_Position = projection * view * vec4(gs_out.FragPos,1);
    gs_out.norm = norm;
    gs_out.TexCoords = vec2(0.5f, 1.866f);
    EmitVertex();  


    vec3 offset = left * (size * 2.f / 3.f);
    gs_out.FragPos = pos + offset;
    gl_Position = projection * view * vec4(gs_out.FragPos,1);
    gs_out.norm = norm;
    gs_out.TexCoords = vec2(1.577f, 0.f);
    EmitVertex();  

    offset = -offset;
    gs_out.FragPos = pos + offset;
    gl_Position = projection * view * vec4(gs_out.FragPos,1);
    gs_out.norm = norm;
    gs_out.TexCoords = vec2(-0.577f, 0.f);
    EmitVertex();  
    EndPrimitive();

    gs_out.FragPos = pos + topOffset;
    gl_Position = projection * view * vec4(gs_out.FragPos,1);
    gs_out.norm = norm;
    gs_out.TexCoords = vec2(0.5f, 1.866f);
    EmitVertex();  


    offset = front * (size * 2.f / 3.f);
    gs_out.FragPos = pos + offset;
    gl_Position = projection * view * vec4(gs_out.FragPos,1);
    gs_out.norm = norm;
    gs_out.TexCoords = vec2(1.577f, 0.f);
    EmitVertex();  

    offset = -offset;
    gs_out.FragPos = pos + offset;
    gl_Position = projection * view * vec4(gs_out.FragPos,1);
    gs_out.norm = norm;
    gs_out.TexCoords = vec2(-0.577f, 0.f);
    EmitVertex(); 
    EndPrimitive();

    left = normalize(cross(-dir, up));
    front = normalize(cross(left, up));
    gs_out.FragPos = pos + topOffset;
    gl_Position = projection * view * vec4(gs_out.FragPos,1);
    gs_out.norm = norm;
    gs_out.TexCoords = vec2(0.5f, 1.866f);
    EmitVertex();  


    offset = left * (size * 2.f / 3.f);
    gs_out.FragPos = pos + offset;
    gl_Position = projection * view * vec4(gs_out.FragPos,1);
    gs_out.norm = norm;
    gs_out.TexCoords = vec2(-0.577f, 0.f);
    EmitVertex();  

    offset = -offset;
    gs_out.FragPos = pos + offset;
    gl_Position = projection * view * vec4(gs_out.FragPos,1);
    gs_out.norm = norm; 
    gs_out.TexCoords = vec2(1.577f, 0.f);
    EmitVertex();  
    EndPrimitive();


    gs_out.FragPos = pos + topOffset;
    gl_Position = projection * view * vec4(gs_out.FragPos,1);
    gs_out.norm = norm;
    gs_out.TexCoords = vec2(0.5f, 1.866f);
    EmitVertex();  


    offset = front * (size * 2.f / 3.f);
    gs_out.FragPos = pos + offset;
    gl_Position = projection * view * vec4(gs_out.FragPos,1);
    gs_out.norm = norm;
    gs_out.TexCoords = vec2(-0.577f, 0.f);
    EmitVertex();  

    offset = -offset;
    gs_out.FragPos = pos + offset;
    gl_Position = projection * view * vec4(gs_out.FragPos,1);
    gs_out.norm = norm;
    gs_out.TexCoords = vec2(1.577f, 0.f);
    EmitVertex(); 
    EndPrimitive();
}

void main() {    
    vec3 offset = vec3(0);
    vec4 offsetP = (projection * view * vec4(offset,0));

    /*
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
    */
    for(int i = 0; i < 1; i++){
        vec2 r = randPoint((i + vs_out[0].instanceID) * vec3(5,10,3) );
        vec3 a = vs_out[1].FragPos - vs_out[0].FragPos;
        vec3 b = vs_out[2].FragPos - vs_out[0].FragPos;
        vec3 pos = vs_out[0].FragPos + a * r.x + b * r.y;
        a = vs_out[1].norm - vs_out[0].norm;
        b = vs_out[2].norm - vs_out[0].norm;
        vec3 norm = normalize(vs_out[0].norm + a * r.x + b * r.y);
        drawPyramid(pos, norm, vec3(nrand(i),nrand(i+10),nrand(i+100)), 1);
    }
    //drawPyramid(vs_out[0].FragPos, normalize(vs_out[0].norm), vec3(1,0,0), 0.1);
}  