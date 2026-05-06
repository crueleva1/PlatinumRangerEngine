#version 410

uniform sampler2D uPosSampler;
uniform sampler2D uNormalSampler;
uniform sampler2D uDiffuseSampler;
uniform samplerCube uCubeDepthMap;

uniform vec3 uCameraPos;
uniform vec3 uLightPos;
uniform vec3 uLightDiffuse;
uniform vec3 uLightAmbient;
uniform vec3 uLightSpecular;
uniform float uLightDimmer;
uniform float uLightRange;
uniform bool uEnableCubeDepthMap;

in vec4 voPosition;

layout (location = 0) out vec4 oColor;

#define DF_EPSILON 0.00001

float CalculShadowFactor(vec3 _kLightDirection, float _fAttenuation)
{
    float fZB = texture(uCubeDepthMap, _kLightDirection).r;

    float fDistance = length(_kLightDirection);

    float fValue = (fZB + 5.0) - fDistance;

    if (fZB == 0.0)
        return 1.0;

    if (fValue > 0.0)
        return 1.0;
    else
        return 1.0 - _fAttenuation;
}

void main()
{
    vec2 vTextureCoord = (voPosition.xy / voPosition.w) * vec2(0.5, 0.5) + vec2(0.5, 0.5);

    vec4 kColor;
    vec4 kNormal;
    vec4 kModelPos;
    vec3 kPackedNormal;
    vec3 kOrgLightDir;
    vec3 kLightDir;
    vec3 kCameraDir;
    
    kModelPos = texture2D(uPosSampler, vTextureCoord);
    kColor = texture2D(uDiffuseSampler, vTextureCoord);
    kPackedNormal = texture2D(uNormalSampler, vTextureCoord).rgb;
    kPackedNormal = (kPackedNormal - vec3(0.5, 0.5, 0.5)) * vec3(2.0, 2.0, 2.0);
    kNormal = vec4(normalize(kPackedNormal), 0.0);
    kOrgLightDir = uLightPos - kModelPos.xyz;
    kLightDir = normalize(kOrgLightDir);
    kCameraDir = normalize(uCameraPos - kModelPos.xyz);
    vec3 kHalfVec = normalize(kCameraDir + kLightDir);
    float fDistance = distance(uLightPos, kModelPos.xyz);
    //float fAttenuation = ((uLightRange - fDistance) * uLightDimmer) / (1.0 + fDistance + fDistance * fDistance);
    float fAttenuation = (1.0 - (fDistance / uLightRange)) * uLightDimmer;
    if (uEnableCubeDepthMap) {
        fAttenuation *= CalculShadowFactor(-kOrgLightDir, fAttenuation);
    }
    //float fAttenuation = uLightRange / (uLightRange + uLightDimmer * fDistance);

    vec4 kAmbientLight = kColor * vec4(uLightAmbient, 1.0);
    vec4 kDiffuseLight = vec4(uLightDiffuse, 1.0) * clamp(dot(vec4(kLightDir.xyz, 0.0), kNormal), 0.0, 1.0);
    vec4 kSpecularLight = vec4(uLightSpecular, 1.0) * pow(clamp(dot(vec4(kHalfVec.xyz, 0.0), kNormal), 0.0, 1.0), 30.0) * kColor.a;

    oColor = (kDiffuseLight + kSpecularLight) * kColor * fAttenuation + kAmbientLight * fAttenuation;
}
