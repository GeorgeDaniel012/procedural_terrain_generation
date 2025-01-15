// Shader-ul de fragment / Fragment shader 
#version 410 core

in float Height;
in vec3 fragPos;
in vec3 inViewPos;
in vec2 TexCoords;

vec3 objectColor, lightColor;

out vec4 out_Color;

uniform sampler2D heightMap;
uniform float uTexelSize;
uniform float HEIGHT_SCALE;
uniform bool isDark;
uniform vec3 inLightPos;

void main(void)
{
    float left  = texture(heightMap, TexCoords + vec2(-uTexelSize, 0.0)).r * HEIGHT_SCALE * 2.0 - 1.0;
    float right = texture(heightMap, TexCoords + vec2( uTexelSize, 0.0)).r * HEIGHT_SCALE * 2.0 - 1.0;
    float up    = texture(heightMap, TexCoords + vec2(0.0,  uTexelSize)).r * HEIGHT_SCALE * 2.0 - 1.0;
    float down  = texture(heightMap, TexCoords + vec2(0.0, -uTexelSize)).r * HEIGHT_SCALE * 2.0 - 1.0;
    vec3 normal = normalize(vec3(down - up, 2.0, left - right));

    float h = (Height + 16)/128.0f;
    objectColor = vec3(0.016f + h, 0.34f + h, 0.15f + h);

    float ambientStrength;
    if (!isDark) {
        lightColor = vec3(0.6f, 0.593f, 0.348f);
        ambientStrength = 0.3f;
    } else {
        lightColor = vec3(0.6f, 0.6f, 0.6f);
        ambientStrength = 0.1f;
    }
    
    // Ambient
    vec3 ambient_light = ambientStrength * lightColor;  // ambient_light=ambientStrength*lightColor 
    vec3 ambient_term= ambient_light * objectColor; // ambient_material=objectColor
  	
    // Diffuse 
    vec3 norm = normalize(normal); // vectorul s 
    vec3 lightDir = normalize(inLightPos - fragPos); // vectorul L
    float diff = max(dot(norm, lightDir), 0.0); // 
    vec3 diffuse_light = lightColor; // diffuse_light=lightColor;
    vec3 diffuse_term = diff * diffuse_light * objectColor; // diffuse_material=objectColor
    
    // Specular
    float specularStrength = 0.5f;
    float shininess = 1.0f;
    vec3 viewDir = normalize(inViewPos - fragPos); // versorul catre observator
    vec3 reflectDir = normalize(reflect(-lightDir, norm));  // versorul vectorului R 
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess); 
    vec3 specular_light = specularStrength  * lightColor; // specular_light=specularStrength  * lightColor
    vec3 specular_term = spec * specular_light * objectColor;   // specular_material=objectColor
      
    // Culoarea finala 
    vec3 emission=vec3(0.0, 0.0, 0.0);
    vec3 result = emission + (ambient_term + diffuse_term + specular_term);
	out_Color = vec4(result, 1.0f);

}