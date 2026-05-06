#version 410

uniform sampler2D uPosSampler;
uniform sampler2D uNormalSampler;
uniform sampler2D uDiffuseSampler;
uniform sampler2D uMetallicRoughness;
uniform samplerCube uCubeEnvMap;

uniform vec3 uCameraPos;
uniform vec3 uLightPos; // Transformed
uniform vec3 uLightDiffuse;
uniform vec3 uLightAmbient;
uniform vec3 uLightSpecular;
uniform vec3 uEnvWorldPos;
uniform float uLightDimmer;
uniform float uLightRange;
uniform float uLightRim;
uniform int uPipelineSwitch;
uniform bool uEnableCubeEnvMap;

in vec4 voPosition;

layout (location = 0) out vec4 oColor;

#define PI 3.1415926

#define eSpecularPipeline_Phong 0x00000001
#define eSpecularPipeline_Blinn 0x00000002
#define eSpecularPipeline_Cook  0x00000004
#define eCookPipeline_Blinn     0x00000008
#define eCookPipeline_Beckann   0x00000010
#define eCookPipeline_GGX       0x00000020

float saturate(float _fValue)
{
    return clamp(_fValue, 0.0, 1.0);
}

float PhongDiffuse()
{
    return (1.0 / PI);
}


// compute fresnel specular factor for given base specular and product
// product could be NdV or VdH depending on used technique
vec3 FresnelFactor(vec3 _kF0, float _fProduct)
{
    return mix(_kF0, vec3(1.0), pow(1.01 - _fProduct, 5.0));
}


// following functions are copies of UE4
// for computing cook-torrance specular lighting terms
float DBlinn(float _fRoughness, float _fNdH)
{
    float fMul1 = _fRoughness * _fRoughness;
    float fMul2 = fMul1 * fMul1;
    float fPow = 2.0 / fMul2 - 2.0;
    return (fPow + 2.0) / (2.0 * PI) * pow(_fNdH, fPow);
}

float DBeckmann(float _fRoughness, float _fNdH)
{
    float fMul1 = _fRoughness * _fRoughness;
    float fMul2 = fMul1 * fMul1;
    float fNdH2 = _fNdH * _fNdH;
    return exp((fNdH2 - 1.0) / (fMul2 * fNdH2)) / (PI * fMul2 * fNdH2 * fNdH2);
}

float DGGX(float _fRoughness, float _fNdH)
{
    float fMul1 = _fRoughness * _fRoughness;
    float fMul2 = fMul1 * fMul1;
    float fD = (_fNdH * fMul2 - _fNdH) * _fNdH + 1.0;
    return fMul2 / (PI * fD * fD);
}

float GSchlick(float _fRoughness, float _fNdV, float _fNdL)
{
    float fk = _fRoughness * _fRoughness * 0.5;
    float fV = _fNdV * (1.0 - fk) + fk;
    float fL = _fNdL * (1.0 - fk) + fk;
    return 0.25 / (fV * fL);
}


// simple phong specular calculation with normalization
vec3 PhongSpecular(vec3 _kV, vec3 _kL, vec3 _kN, vec3 _kSpecular, float _fRoughness)
{
    vec3 kR = reflect(-_kL, _kN);
    float fSpec = max(0.0, dot(_kV, kR));

    float k = 0.0;
    if (_fRoughness == 0.0)
        k = 10000.0;
    else
        k = 1.999 / (_fRoughness * _fRoughness);

    return min(1.0, 3.0 * 0.0398 * k) * pow(fSpec, min(10000.0, k)) * _kSpecular;
}

// simple blinn specular calculation with normalization
vec3 BlinnSpecular(float _fNdH, vec3 _kSpecular, float _fRoughness)
{
    float k = 0.0;
    if (_fRoughness == 0.0)
        k = 10000.0;
    else
        k = 1.999 / (_fRoughness * _fRoughness);
    
    return min(1.0, 3.0 * 0.0398 * k) * pow(_fNdH, min(10000.0, k)) * _kSpecular;
}

// cook-torrance specular calculation                      
vec3 CookTorranceSpecular(float _fNdL, float _fNdV, float _fNdH, vec3 _kSpecular, float _fRoughness)
{
    float fD = 0.0;
    if ((uPipelineSwitch & eCookPipeline_Blinn) != 0) {
        fD = DBlinn(_fRoughness, _fNdH);
    }
    else if ((uPipelineSwitch & eCookPipeline_Beckann) != 0) {
        fD = DBeckmann(_fRoughness, _fNdH);
    }
    else if ((uPipelineSwitch & eCookPipeline_GGX) != 0) {
        fD = DGGX(_fRoughness, _fNdH);
    }

    float fG = GSchlick(_fRoughness, _fNdV, _fNdL);

    float fRim = mix(1.0 - _fRoughness * uLightRim * 0.9, 1.0, _fNdV);

    return (1.0 / fRim) * _kSpecular * fG * fD;
}


void main()
{
    vec2 vTextureCoord = (voPosition.xy / voPosition.w) * vec2(0.5, 0.5) + vec2(0.5, 0.5);
    // World Pos
    vec4 kModelPos = texture2D(uPosSampler, vTextureCoord);

    // light attenuation
    vec3 kLVec = uLightPos - kModelPos.xyz;
    vec3 kVVec = uCameraPos - kModelPos.xyz;
    vec3 kHVec = kLVec + kVVec;
    //float fLightAttenuation = 20.0 / dot(kLVec, kLVec);
    float fLightAttenuation = (1.0 - (distance(uLightPos, kModelPos.xyz) / uLightRange)) * uLightDimmer;


    kLVec = normalize(kLVec);
    kVVec = normalize(kVVec);
    kHVec = normalize(kHVec);

    // normal Map
    vec3 kNormal = texture2D(uNormalSampler, vTextureCoord).rgb;
    kNormal = (kNormal - vec3(0.5, 0.5, 0.5)) * vec3(2.0, 2.0, 2.0);
    kNormal = normalize(kNormal);

    // compute material reflectance
    float fNdL = max(0.0, dot(kNormal, kLVec));
    float fNdV = max(0.001, dot(kNormal, kVVec));
    float fNdH = max(0.001, dot(kNormal, kHVec));
    float fHdV = max(0.001, dot(kHVec, kVVec));
    float fLdV = max(0.001, dot(kLVec, kVVec));

    // Albedo
    vec4 kColor = texture2D(uDiffuseSampler, vTextureCoord);
    // Metallic and roughness
    vec2 kMetallicRoughness = texture2D(uMetallicRoughness, vTextureCoord).rg;

    // material params
    float fMetallic = kMetallicRoughness.x;
    float fRoughness = kMetallicRoughness.y;

    // mix between metal and non-metal material, for non-metal
    // constant base specular factor of 0.04 grey is used
    vec3 kSpecular = mix(vec3(0.04), kColor.rgb, fMetallic);

    vec3 kSpecFresnel;
    vec3 kSpecreFinal;

    if ((uPipelineSwitch & eSpecularPipeline_Phong) != 0) {  // specular reflectance with PHONG
        kSpecFresnel = FresnelFactor(kSpecular, fNdV);
        kSpecreFinal = PhongSpecular(kVVec, kLVec, kNormal, kSpecFresnel, fRoughness);
    }
    else if ((uPipelineSwitch & eSpecularPipeline_Blinn) != 0) {  // specular reflectance with BLINN
        kSpecFresnel = FresnelFactor(kSpecular, fHdV);
        kSpecreFinal = BlinnSpecular(fNdH, kSpecFresnel, fRoughness);
    }
    else { // specular reflectance with COOK-TORRANCE
        kSpecFresnel = FresnelFactor(kSpecular, fHdV);
        kSpecreFinal = CookTorranceSpecular(fNdL, fNdV, fNdH, kSpecFresnel, fRoughness);
    }

    kSpecreFinal *= vec3(fNdL);

    vec3 kDiffuseRef;
    if (!uEnableCubeEnvMap)
        kDiffuseRef = (vec3(1.0) - kSpecFresnel) * fNdL;
    else
        kDiffuseRef = (vec3(1.0) - kSpecFresnel) * PhongDiffuse() * fNdL;

    // compute lighting
    vec3 kReflectedLight;
    vec3 kDiffuseLight; // initial value == constant ambient light
    vec3 kEnvDiffuse = vec3(0.0);
    vec3 kEnvSpec = vec3(0.0);
    if (uEnableCubeEnvMap) {
        vec3 kEnvDir = reflect(-kVVec, kNormal);
        kEnvDiffuse = texture(uCubeEnvMap, kEnvDir).xyz;
        kEnvDiffuse *= kDiffuseRef;
        kEnvSpec = kEnvDiffuse * FresnelFactor(kSpecular, fNdV);
        
    }

    // point light
    vec3 kLightColor = (uLightDiffuse + uLightAmbient) * fLightAttenuation;
    kReflectedLight = kSpecreFinal * kLightColor * (uLightSpecular + kEnvSpec);
    kDiffuseLight = kDiffuseRef * kLightColor + kEnvDiffuse * fLightAttenuation;

    // final result
    vec3 kResult = kDiffuseLight * mix(kColor.rgb, vec3(0.0), fMetallic) +  kReflectedLight;

    oColor = vec4(kResult, kColor.a);
}