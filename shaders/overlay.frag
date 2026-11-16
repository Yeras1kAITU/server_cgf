#version 330 core
in vec2 uv;
out vec4 fragmentColor;
uniform sampler2D uHUD;
void main(){fragmentColor=texture(uHUD,uv);}
