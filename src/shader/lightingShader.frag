// inplement pointlight, spotlight or paralight
#version 330 core
struct Material {
    sampler2D A_D;
    sampler2D S;
    float shininess;
};
struct PointLight {
    vec3 position;

    vec3 A, D, S;
    // attenuation parameters
    // Kc === 1
    float Kl, Kq;
};
struct SpotLight {
    vec3 position;
    vec3 direction;
    float innerCosPhi; // 0 < outer < inner < 1
    float outerCosPhi;

    vec3 A, D, S;
    float Kl, Kq; //Kc = 1
};
struct DirLight {
    vec3 direction;

    vec3 A, D, S;
};

// output
out vec4 fragColor;

// fragment propreties
uniform Material object;
in vec3 normal;
in vec3 fragPos;
in vec2 texCood;

// environment propreties
uniform vec3 viewPos;
uniform DirLight dlight;
uniform PointLight plight[4];
uniform SpotLight slight;

vec3 calcDirLight(DirLight light, vec3 norm, vec3 viewDir);
vec3 calcPointLight(PointLight light, vec3 norm, vec3 viewDir);
vec3 calcSpotLight(SpotLight light, vec3 norm, vec3 viewDir);
// --------------------------------------------------------
void main()
{
    // calculate shared variable
    vec3 norm = normalize(normal);
    vec3 viewDir = normalize(viewPos - fragPos);

    vec3 result = calcDirLight(dlight, norm, viewDir);
    for(int i = 0; i < 4; ++i)
        result += calcPointLight(plight[i], norm, viewDir);
    result += calcSpotLight(slight, norm, viewDir);

    fragColor = vec4(result, 0);
}

/**
 * calculate Parallel Light result
 * *********************/
vec3 calcDirLight(DirLight light, vec3 norm, vec3 viewDir)
{
    // diffuse light
    vec3 lightDir = normalize(light.direction);
    float diff = max(0, dot(lightDir, norm));

    // specular light
    float spec = pow(max(0, dot(viewDir, reflect(-lightDir, norm))), object.shininess);

    return  vec3(texture(object.A_D, texCood)) * (light.A + diff * light.D) // ambient & diffuse
            + vec3(texture(object.S, texCood)) * spec * light.S;
}

/**
 * calculate PointLight result
 * *********************/
vec3 calcPointLight(PointLight light, vec3 norm, vec3 viewDir)
{
    // diffuse light
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(0, dot(lightDir, norm));

    // specular light
    float spec = pow(max(0, dot(viewDir, reflect(-lightDir, norm))), object.shininess);
    
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.f/(1.f + (light.Kl + light.Kq * distance) * distance);

    return  attenuation * (     // attenuation
            vec3(texture(object.A_D, texCood)) * (light.A + diff * light.D) // ambient & diffuse
            + vec3(texture(object.S, texCood)) * spec * light.S
            );
}

/**
 * calculate SpotLight result
 * *********************/
vec3 calcSpotLight(SpotLight light, vec3 norm, vec3 viewDir)
{
    // calculate whether fragment is in spot
    vec3 frag2light = light.position - fragPos;
    vec3 lightDir = normalize(frag2light);
    float cosTheta = dot(lightDir, normalize(-light.direction));
    
    //attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.f/(1.f + (light.Kl + light.Kq * distance) * distance);

    if(cosTheta > light.outerCosPhi) // inside the spot || in the ring
    {
        // diffuse light
        float diff = max(0, dot(lightDir, norm));

        // specular light
        float spec = pow(max(0, dot(viewDir, reflect(-lightDir, norm))), object.shininess);

        // intensity of the spot
        float intensity;
        if(cosTheta > light.innerCosPhi) // totally inside the spot
            intensity = 1.f;
        else // in the ring
            intensity = (cosTheta - light.outerCosPhi) / (light.innerCosPhi - light.outerCosPhi);
        
        return  attenuation * (intensity * (
                        vec3(texture(object.A_D, texCood)) * diff * light.D // ambient & diffuse
                        + vec3(texture(object.S, texCood)) * spec * light.S
                        )
                + vec3(texture(object.A_D, texCood)) * light.A
                );
    }
    else // outside the spot
    {
        return attenuation * vec3(texture(object.A_D, texCood)) * light.A;
    }
}