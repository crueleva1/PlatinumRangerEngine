#version 410

uniform mat4 uWorldMatrix;
uniform vec4 uColor;
uniform sampler2D uDiffuse;
uniform sampler2D uOpacity;
uniform sampler2D uHeight;
uniform bool uHasMask;
uniform bool uHasTexture;

in vec4 voWorldPosition;
in vec3 voWorldNormal;
in vec2 voTextureCoord;

layout (location = 0) out vec4 oColor;
layout (location = 1) out vec4 oWorldPosition;
layout (location = 2) out vec4 oWorldNormal;

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
    vec3 kHeightedNormal = voWorldNormal;
    kHeightedNormal.y += (texture2D(uHeight, voTextureCoord).r - 0.5);
    // Packed negative normal 1~-1 to 0.5~-0.5
    oWorldNormal = vec4(normalize(vec4(uWorldMatrix * vec4(kHeightedNormal, 0.0)).xyz) * vec3(0.5, 0.5, 0.5) + vec3(0.5, 0.5, 0.5), nAlpha);
}