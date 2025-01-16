// Shader-ul de fragment / Fragment shader 
#version 330

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

    // fara difuzie si specularitate :(
    
    // Culoarea finala 
    vec3 emission=lightColor;
    vec3 result = emission + (ambient_term);
	out_Color = vec4(result, 1.0f);
}