#shader vertex
#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 a_Normal;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Proj;
uniform mat3 u_NormalMatrix;

out vec2 texCoords;
out vec3 normal;
out vec3 fragPos;

void main()
{
   texCoords = uv;
   normal = u_NormalMatrix * a_Normal;
   fragPos = pos;

   mat4 mvp = u_Proj * u_View * u_Model;
   gl_Position = mvp * vec4(pos, 1.0);
};


#shader fragment
#version 330 core

struct Material {
    float shininess;
};

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

out vec4 color;

in vec2 texCoords;
in vec3 normal;
in vec3 fragPos;

uniform sampler2D u_Texture;
uniform float u_TextureGridMode;
uniform float u_TextureColorMode;
uniform DirLight u_DirLight;
uniform vec3 u_ViewPos;
uniform Material material;

const vec4 plainColor = vec4(1.0);

// function prototypes
vec4 calcGridColor(vec2 p, vec4 defaultColor);
vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec4 materialDiffuse);


void main()
{
    vec3 norm = normalize(normal);
    vec3 viewDir = normalize(u_ViewPos - fragPos);
    vec4 colTexture = texture(u_Texture, texCoords).rgba;
    colTexture = mix(plainColor, colTexture, u_TextureColorMode);
    vec4 gridTexture = calcGridColor(texCoords, colTexture);

    vec4 diffuse = mix(
        colTexture,
        gridTexture,
        u_TextureGridMode
    );
    vec4 dirLight = vec4(calcDirLight(u_DirLight, norm, viewDir, diffuse), 1.0);
    color = dirLight;
};

vec4 calcGridColor(vec2 p, vec4 defaultColor)
{
    const float nCells = 30;
    vec2 q = fract(p * nCells);
    if (q.x < 0.1) return vec4(0.8, 0.0, 0.0, 1.0);
    if (q.y < 0.1) return vec4(0.0, 0.5, 0.0, 1.0);
    return defaultColor;
}

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec4 materialDiffuse)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 ambient = light.ambient * vec3(materialDiffuse);
    vec3 diffuse = light.diffuse * diff * vec3(materialDiffuse);
    vec3 specular = light.specular * spec;
    //if (diff == 0.0)
    //{
    //    return vec3(1.0, 0.0, 0.0);
    //}
    return (ambient + diffuse + specular);
    //return vec4(1.0,1.0,1.0,1.0);
}