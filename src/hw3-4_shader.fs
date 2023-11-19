#version 330 core
out vec4 fragColor;

in vec3 color;
in vec2 texCoord;
in vec3 normal;
in vec3 fragPos;

uniform sampler2D ourTexture;
uniform vec3 lightColor;
uniform vec3 lightDir;
uniform vec3 viewPos;

void main()
{
    // choose to render with colors or textures
    bool usingColors = false; 

    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
    // diffuse
    vec3 norm = normalize(normal);
    vec3 lightDir_n = normalize(lightDir);
    float diff = max(dot(norm, lightDir_n), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 2);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 lightSum = ambient + diffuse + specular;

    if (usingColors) { // colors 
        fragColor = vec4(lightSum * color, 1.0f);
    } else { // textures
        fragColor = vec4(lightSum, 1.0f) * texture(ourTexture, texCoord);
    }    
}