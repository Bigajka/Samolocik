#version 330 core

in vec3 FragPos;
in vec3 NormalWorld;
in vec3 ObjectColor;

out vec4 FragColor;

uniform vec3 ViewPos;
uniform vec3 uColor;

void main()
{
    vec3 baseColor = ObjectColor * uColor;

    vec3 lightPos = vec3(0.0, 6.5, 0.0);

    vec3 viewPos = vec3(0.0, 0.0, 3.0);

    vec3 norm = normalize(NormalWorld);

    vec3 lightDir = normalize(lightPos - FragPos);

    float diff = max(dot(norm, lightDir), 0.0);

    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 reflectDir = reflect(-lightDir, norm);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);

    vec3 ambient = 0.2 * ObjectColor;

    vec3 diffuse = 0.8 * diff * ObjectColor;

    vec3 specular = 0.35 * spec * vec3(1.0);

    vec3 result = ambient + diffuse + specular;

    FragColor = vec4(result, 1.0);
}
