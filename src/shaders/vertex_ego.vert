# version 300 es
precision lowp float;

layout (location=0) in vec3 vertexPos;
layout (location=1) in vec2 vertexTexCoord;
layout (location=2) in vec3 vertexNormal;

out vec2 fragmentTexCoord;
out vec3 fragmentNormal;
out vec3 frag_pos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{   
    vec4 world_pos = model * vec4(vertexPos, 1.0);
    frag_pos = world_pos.xyz;
    gl_Position = projection * view * world_pos;
    fragmentTexCoord = vertexTexCoord;
    fragmentNormal = (model * vec4(vertexNormal, 0.0)).xyz;
}