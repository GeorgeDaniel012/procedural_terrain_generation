// Shader-ul de fragment / Fragment shader 
#version 330

//in vec3 ex_Color; 
out vec4 out_Color;

vec3 lightColor;
vec3 objectColor;
float ambientStrength;

uniform bool isDark;

void main(void)
{
    // determining color of sphere
    if (!isDark) {
        objectColor = vec3(1.0, 0.85, 0.13);
        lightColor = vec3(0.7f, 0.693f, 0.448f);
        ambientStrength = 0.3f;
    } else {
        objectColor = vec3(0.6, 0.6, 0.6);
        lightColor = vec3(0.6f, 0.6f, 0.6f);
        ambientStrength = 0.1f;
    }

    // Ambient
	vec3 ambient_light = ambientStrength * lightColor;  // ambient_light=ambientStrength*lightColor 
    vec3 ambient_term = ambient_light * objectColor; 

	// Diffuse
    /*
    vec3 norm = normalize(Normal); // vectorul s 
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
       */
    // Culoarea finala 
    vec3 emission=lightColor;
    vec3 result = emission + (ambient_term);
	out_Color = vec4(result, 1.0f);
    //out_Color = vec4(objectColor, 1.0f);
}