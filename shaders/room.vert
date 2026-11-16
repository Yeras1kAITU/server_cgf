#version 330 core
layout(location=0) in vec3 aPosition;
layout(location=1) in vec3 aNormal;
layout(location=2) in vec2 aUV;
layout(location=3) in mat4 aModel;
layout(location=7) in vec4 aColorKind;
layout(location=8) in vec4 aParameters;
uniform mat4 uView;
uniform mat4 uProjection;
out vec3 worldPosition;
out vec3 worldNormal;
out vec2 uv;
flat out vec4 colorKind;
flat out vec4 parameters;
void main() {
    vec4 position=aModel*vec4(aPosition,1.0);
    worldPosition=position.xyz;
    worldNormal=mat3(transpose(inverse(aModel)))*aNormal;
    uv=aUV;
    colorKind=aColorKind;
    parameters=aParameters;
    gl_Position=uProjection*uView*position;
}
