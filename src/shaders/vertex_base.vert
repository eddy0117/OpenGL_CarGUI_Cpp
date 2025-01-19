# version 300 es
precision lowp float;

layout (location=0) in vec3 vertexPos;
layout (location=1) in vec2 vertexTexCoord;     // UV
layout (location=2) in vec3 vertexNormal;
layout (location=3) in mat4 instance_matrix;  // 實例的變換矩陣

out vec2 fragmentTexCoord;
out vec3 fragmentNormal;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

void main() {
    // gl_Position: (x/w, y/w, z/w, w)
    gl_Position = projection * view * instance_matrix * vec4(vertexPos, 1.0);
    fragmentTexCoord = vertexTexCoord;
    fragmentNormal = (instance_matrix * vec4(vertexNormal, 0.0)).xyz;
}
