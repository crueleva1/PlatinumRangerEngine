#version 410


uniform vec4 uColor;
uniform sampler2D uDiffuse;
uniform sampler2D uOpacity;
uniform sampler2D uNormals;
uniform sampler2D uMetallic;
uniform sampler2D uRoughness;
uniform vec2 uMetallicRoughnessParam;
uniform bool uHasMask;
uniform bool uHasTexture;

in vec4 voWorldPosition;
in vec3 voNormal;
in vec3 voTangent;
in vec2 voTextureCoord;

layout (location = 0) out vec4 oColor;
layout (location = 1) out vec4 oWorldPosition;
layout (location = 2) out vec4 oWorldNormal;
layout (location = 3) out vec4 oMetallicRoughness;

void main()
{
    if (uHasTexture) {
        oColor = texture2D(uDiffuse, voTextureCoord);
        if (uHasMask) {
            oColor.a = texture2D(uOpacity, voTextureCoord).r;
        }
    }
    else {
        oColor = uColor;
    }
    float nAlpha = 0.0;
    if (oColor.a > 0.8)
        nAlpha = 1.0;
    oWorldPosition = vec4(voWorldPosition.xyz, nAlpha);
    mat3 kTBN;
    kTBN[0] = voTangent;
    kTBN[1] = cross(voNormal, voTangent);
    kTBN[2] = voNormal;
    vec3 kFinalNormal = texture2D(uNormals, voTextureCoord).rgb * 2.0 - vec3(1.0);
    // Packed negative normal 1~-1 to 0.5~-0.5
    oWorldNormal = vec4(kTBN * normalize(kFinalNormal) * vec3(0.5, 0.5, 0.5) + vec3(0.5, 0.5, 0.5), nAlpha);

    oMetallicRoughness.a = nAlpha;
    oMetallicRoughness.r = texture2D(uMetallic, voTextureCoord).r * uMetallicRoughnessParam.x;
    oMetallicRoughness.g = texture2D(uRoughness, voTextureCoord).r * uMetallicRoughnessParam.y;
}