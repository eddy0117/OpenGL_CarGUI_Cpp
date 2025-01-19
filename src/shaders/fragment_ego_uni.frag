#version 300 es
precision mediump float;

in vec2 fragmentTexCoord;
in vec3 fragmentNormal;
in vec3 frag_pos;

out vec4 screenColor;

uniform sampler2D material;

// 可自行調整最大光源數上限
const int MAX_LIGHTS = 50;

// 光源數量
uniform int numLights;

// 光源位置陣列
uniform vec3 lightPositions[MAX_LIGHTS];

// 衰減參數
// uniform float constantAtten;
// uniform float linearAtten;
// uniform float quadraticAtten;

void main()
{
    // 取材質顏色
    vec3 normal = normalize(fragmentNormal);
    vec3 baseColor = texture(material, fragmentTexCoord).rgb;
    vec3 finalColor = baseColor;
    float constant = 0.5;
    float linear = 0.8;
    float quadratic = 0.532;
    // 累積所有光源
    for (int i = 0; i < numLights; i++) {
        vec3 lightDir = normalize(lightPositions[i] - frag_pos);
        float diff = max(dot(normal, lightDir), 0.0);

        float distance = length(lightPositions[i] - frag_pos);
        float attenuation = 1.0 / (constant + linear * distance
                                 + quadratic * distance * distance);
        diff *= attenuation;

        // 可依需求調整光照顏色、強度
        vec3 lightColor = diff * vec3(1.0, 0.0, 0.0) * 2.0;
        // vec3 lightColor2 = diff * vec3(1.0, 0.0, 0.0) * 1.0;
        finalColor = vec3(finalColor.x + lightColor.x, finalColor.y - (0.7 * lightColor.x), finalColor.z - lightColor.x);
        // finalColor = vec3(finalColor.x + (0.5 * lightColor2.x), finalColor.y - (0.7 * lightColor2.x), finalColor.z - lightColor2.x);
    }

    screenColor = vec4(finalColor, 1.0);
}