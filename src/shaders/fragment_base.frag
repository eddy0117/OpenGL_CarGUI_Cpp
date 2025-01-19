# version 300 es
precision lowp float;

in vec2 fragmentTexCoord;
in vec3 fragmentNormal;

out vec4 screenColor;

uniform sampler2D material; // 不需要顯式傳值進來, 由於默認情況下 bindtexture 會綁到第 0 個 sampler

const vec3 sunDirection = normalize(vec3(-1.0, 0.0, 1.0));

void main()
{
    float lightStrength = max(0.6, dot(fragmentNormal, sunDirection)) * 1.6;
    // vec3 outColor = texture(material, fragmentTexCoord).rgb;
    // screenColor = vec4(lightStrength * outColor.x, lightStrength * outColor.y, outColor.z, 1.0);
    screenColor = vec4(lightStrength * texture(material, fragmentTexCoord).rgb, 1.0);
}
