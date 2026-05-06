#version 410

uniform sampler2D uDiffuse;
uniform vec2 uResolution;
uniform float uWeight;
uniform float uRadius;
uniform int uBlurMode;
uniform bool uHorizontal;

in vec2 voTextureCoord;

out vec4 oColor;

vec4 blur5(sampler2D image, vec2 uv, vec2 resolution, vec2 direction) 
{
    float fWeight = uWeight;
    vec4 color = vec4(0.0);
    vec2 off1 = (vec2(1.3333333333333333) * direction) / resolution;
    color += texture2D(image, uv) * 0.29411764705882354;
    color += texture2D(image, uv + off1) * (0.35294117647058826 + fWeight);
    color += texture2D(image, uv - off1) * (0.35294117647058826 + fWeight);
    return color; 
}

vec4 blur9(sampler2D image, vec2 uv, vec2 resolution, vec2 direction)
{
    float fWeight = uWeight;
    vec4 color = vec4(0.0);
    vec2 off1 = (vec2(1.3846153846) * direction) / resolution;
    vec2 off2 = (vec2(3.2307692308) * direction) / resolution;
    color += texture2D(image, uv)        * (0.2270270270 + fWeight);
    color += texture2D(image, uv + off1) * (0.3162162162 + fWeight);
    color += texture2D(image, uv - off1) * (0.3162162162 + fWeight);
    color += texture2D(image, uv + off2) * (0.0702702703 + fWeight);
    color += texture2D(image, uv - off2) * (0.0702702703 + fWeight);
    return color;
}

vec4 blur13(sampler2D image, vec2 uv, vec2 resolution, vec2 direction)
{
    float fWeight = uWeight;
    vec4 color = vec4(0.0);
    vec2 off1 = (vec2(1.411764705882353) * direction) / resolution;
    vec2 off2 = (vec2(3.2941176470588234) * direction) / resolution;
    vec2 off3 = (vec2(5.176470588235294) * direction) / resolution;
    color += texture2D(image, uv)        * (0.1964825501511404   + fWeight);
    color += texture2D(image, uv + off1) * (0.2969069646728344   + fWeight);
    color += texture2D(image, uv - off1) * (0.2969069646728344   + fWeight);
    color += texture2D(image, uv + off2) * (0.09447039785044732  + fWeight);
    color += texture2D(image, uv - off2) * (0.09447039785044732  + fWeight);
    color += texture2D(image, uv + off3) * (0.010381362401148057 + fWeight);
    color += texture2D(image, uv - off3) * (0.010381362401148057 + fWeight);
    return color;
}

float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

vec4 blur(sampler2D image, vec2 uv, vec2 resolution, vec2 direction)
{             
    vec2 tex_offset = 1.0 / resolution; // gets size of single texel
    vec3 result = texture(image, uv).rgb * weight[0]; // current fragment's contribution
    for(int i = 1; i < 5; ++i) {
        vec2 offset = tex_offset * direction * i;
        result += texture(image, uv + offset).rgb * weight[i];
        result += texture(image, uv - offset).rgb * weight[i];
    }

    return vec4(result, 1.0);
}

void main()
{
    vec2 kDir;
    if (uHorizontal) {
        kDir = vec2(uRadius, 0.0);
    }
    else {
        kDir = vec2(0.0, uRadius);
    }

    if (uBlurMode == 2) {
        oColor = blur13(uDiffuse, voTextureCoord, uResolution, kDir);
        return;
    }
    else if (uBlurMode == 1) {
        oColor = blur9(uDiffuse, voTextureCoord, uResolution, kDir);
        return;
    }

    oColor = blur5(uDiffuse, voTextureCoord, uResolution, kDir);
}