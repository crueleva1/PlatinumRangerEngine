#version 410

uniform sampler2D uDiffuse;
uniform vec2 uBrightnessContrast;
uniform float uBrightnessThreshold;

in vec2 voTextureCoord;

out vec4 oColor;

vec4 BrightnessContrast(vec3 _kColor, float _fBrightness, float _fContrast)
{
    vec3 kLumCoeff = vec3(0.2125, 0.7154, 0.0721);
    vec3 kAvgLumin = vec3(0.5, 0.5, 0.5);

    vec3 kIntensity = vec3(dot(_kColor + vec3(_fBrightness, _fBrightness, _fBrightness), kLumCoeff));
    vec3 kSatColor = mix(kIntensity, _kColor, 1.0);
    vec3 kContrast = mix(kAvgLumin, kSatColor, _fContrast);

    float fIntensity = dot(kContrast, kLumCoeff);
#if 0
    if (uBrightnessThreshold > fIntensity)
        fIntensity = 0.0f;
#endif
    return vec4(_kColor * fIntensity, 1.0);
}

void main()
{
    oColor = BrightnessContrast(texture2D(uDiffuse, voTextureCoord).rgb, uBrightnessContrast.x, uBrightnessContrast.y);
}