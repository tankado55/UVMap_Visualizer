#shader vertex
#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 uv;

out vec2 texCoords;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Proj;

void main()
{
   texCoords = uv;
   mat4 mvp = u_Proj * u_View * u_Model;
   gl_Position = mvp * vec4(pos, 1.0);
};

#shader fragment
#version 330 core

out vec4 color;

in vec2 texCoords;

uniform sampler2D u_Texture;
uniform float u_TextureGridMode;
uniform float u_TextureColorMode;

const vec4 plainColor = vec4(1.0);

vec4 gridColor(vec2 p, vec4 defaultColor)
{
    const float nCells = 30;
    vec2 q = fract(p * nCells);
    if (q.x < 0.1) return vec4(0.8, 0.0, 0.0, 1.0);
    if (q.y < 0.1) return vec4(0.0, 0.5, 0.0, 1.0);
    return defaultColor;
}

void main()
{
    vec4 colTexture = texture(u_Texture, texCoords).rgba;
    colTexture = mix(plainColor, colTexture, u_TextureColorMode);
    vec4 gridTexture = gridColor(texCoords, colTexture);

    color = mix(
        colTexture,
        gridTexture,
        u_TextureGridMode
    );
};