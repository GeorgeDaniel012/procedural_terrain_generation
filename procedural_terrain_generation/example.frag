// Shader-ul de fragment / Fragment shader 
#version 410 core

in float Height;
in vec3 FragPos;
in vec3 inViewPos;
in vec2 TexCoords;

vec3 objectColor;

out vec4 out_Color;

uniform sampler2D heightMap;
uniform float uTexelSize;
uniform float HEIGHT_SCALE;
uniform vec3 lightColor;
uniform vec3 inLightPos;

void main(void)
{
    float left  = texture(heightMap, TexCoords + vec2(-uTexelSize, 0.0)).r * HEIGHT_SCALE * 2.0 - 1.0;
    float right = texture(heightMap, TexCoords + vec2( uTexelSize, 0.0)).r * HEIGHT_SCALE * 2.0 - 1.0;
    float up    = texture(heightMap, TexCoords + vec2(0.0,  uTexelSize)).r * HEIGHT_SCALE * 2.0 - 1.0;
    float down  = texture(heightMap, TexCoords + vec2(0.0, -uTexelSize)).r * HEIGHT_SCALE * 2.0 - 1.0;
    vec3 normal = normalize(vec3(down - up, 2.0, left - right));

    float h = (Height + 16)/64.0f;
    objectColor = vec3(h, h, h);

    // Ambient
    float ambientStrength = 0.5f;
    vec3 ambient_light = ambientStrength * lightColor;  // ambient_light=ambientStrength*lightColor 
    vec3 ambient_term= ambient_light * objectColor; // ambient_material=objectColor
  	
    // Diffuse 
    vec3 norm = normalize(normal); // vectorul s 
    vec3 lightDir = normalize(inLightPos - FragPos); // vectorul L
    float diff = max(dot(norm, lightDir), 0.0); // 
    vec3 diffuse_light = lightColor; // diffuse_light=lightColor;
    vec3 diffuse_term = diff * diffuse_light * objectColor; // diffuse_material=objectColor
    
    // Specular
    float specularStrength = 0.5f;
    float shininess = 100.0f;
    vec3 viewDir = normalize(inViewPos - FragPos); // versorul catre observator
    vec3 reflectDir = normalize(reflect(-lightDir, norm));  // versorul vectorului R 
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess); 
    vec3 specular_light = specularStrength  * lightColor; // specular_light=specularStrength  * lightColor
    vec3 specular_term = spec * specular_light * objectColor;   // specular_material=objectColor
       
    // Culoarea finala 
      vec3 emission=vec3(0.0, 0.0, 0.0);
    //vec3 emission=vec3(1.0,0.8,0.4);
    vec3 result = emission + (ambient_term + diffuse_term + specular_term);
	out_Color = vec4(result, 1.0f);

}