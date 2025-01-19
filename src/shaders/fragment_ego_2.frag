#version 300 es
precision mediump float;

in vec2 fragmentTexCoord;
in vec3 fragmentNormal;
in vec3 frag_pos; // 插值後的片段位置

out vec4 screenColor;

uniform sampler2D material;
// uniform vec3 lightPosition; // 點光源位置

// uniform float constant;     // 常數項 (衰減參數)
// uniform float linear;       // 線性項 (衰減參數)
// uniform float quadratic;    // 二次項 (衰減參數)

void main()
{
    float constant = 0.5;
    float linear = 0.8;
    float quadratic = 0.532;

    // point light 1
    vec3 lightPosition = vec3(5.0, 1.0, 2.0);

    // 計算法線
    vec3 normal = normalize(fragmentNormal);
   
    // 計算光線方向
    vec3 lightDir = normalize(lightPosition - frag_pos);

    // 漫反射光強度
    float diff1 = max(dot(normal, lightDir), 0.0);

    // 計算距離並應用衰減
    float distance = length(lightPosition - frag_pos);
    float attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));

    // 結合光照強度與衰減
    diff1 = diff1 * attenuation;

    // point light 2
    vec3 lightPosition2 = vec3(2.0, -2.0, 1.0);

    // 計算法線
    vec3 normal2 = normalize(fragmentNormal);
   
    // 計算光線方向
    vec3 lightDir2 = normalize(lightPosition2 - frag_pos);

    // 漫反射光強度
    float diff2 = max(dot(normal2, lightDir2), 0.0);

    // 計算距離並應用衰減
    float distance2 = length(lightPosition2 - frag_pos);
    float attenuation2 = 1.0 / (constant + linear * distance2 + quadratic * (distance2 * distance2));

    // 結合光照強度與衰減
    diff2 = diff2 * attenuation2;

    // 取材質顏色並應用光照
    vec3 materialColor = texture(material, fragmentTexCoord).rgb;
    vec3 outColor = diff1 * vec3(1.0f, 0.0f, 0.0f) * 1.5;
    vec3 outColor2 = diff2 * vec3(1.0f, 0.0f, 0.0f) * 1.5;

    // materialColor *= 0.8;
    screenColor = vec4(materialColor.x + outColor.x, materialColor.y - (outColor.x * 0.7), materialColor.z - outColor.x, 1.0);
    screenColor = vec4(screenColor.x + outColor2.x, screenColor.y - (outColor2.x * 0.7), screenColor.z - outColor2.x, 1.0);
  
}
