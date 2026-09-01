#version 460 core

layout(location = 0) in vec3 normal;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 cameraPositionCameraSpace;

layout(set = 0, binding = 0) uniform sampler2D tex;

layout(location = 0) out vec4 FragColor;

vec3 lightPos = vec3(1.0, 5.0, -3.0);
vec3 lightColor = vec3(1.0, 1.0, 1.0);

float toSRGB(float x) {
if (x <= 0.0031308)
        return 12.92 * x;
    else
        return 1.055 * pow(x, (1.0/2.4)) - 0.055;
}

vec3 sRGB(vec3 c) {
    return vec3(toSRGB(c.x), toSRGB(c.y), toSRGB(c.z));
}

float toLinear(float x) {
    if (x <= 0.04045)
        return x / 12.92;
    else
        return pow((x + 0.055) / 1.055, 2.4);
}

vec3 toLinearVec3(vec3 c) {
    return vec3(toLinear(c.x), toLinear(c.y), toLinear(c.z));
}

vec3 fogColor = vec3(0.25, 0.25, 0.25);
const float minFogRadius = 5.0;
const float maxFogRadius = 10.0;

void main()
{
    float lightAngle = max(dot(normalize(normal), normalize(lightPos)), 0.0);
    vec4 phongColor = texture(tex, texCoord) * vec4((0.3 + 0.7 * lightAngle) * lightColor, 1.0);

    float distanceFog = length(-cameraPositionCameraSpace);
    float fogFactor = (distanceFog - minFogRadius) / (maxFogRadius - minFogRadius);
    fogFactor = clamp(fogFactor, 0.0, 1.0);
    vec3 linearFogColor = toLinearVec3(fogColor);
    
    FragColor.rgb = mix(phongColor.rgb, linearFogColor, fogFactor);

    FragColor.a = phongColor.a;
    FragColor.rgb = sRGB(FragColor.rgb);
}