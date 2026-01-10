#version 330

in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;
in vec4 vertexColor;

uniform mat4 mvp;       
uniform float time; 

uniform float frequency;
uniform float amplitude;
uniform float speed;

out vec2 fragTexCoord;
out vec4 fragColor;

void main()
{
    vec3 p = vertexPosition;
    p.y += sin(p.x * frequency + time * speed) * amplitude;

    gl_Position = mvp * vec4(p, 1.0);

    fragTexCoord = vertexTexCoord;
    fragColor = vertexColor;
}