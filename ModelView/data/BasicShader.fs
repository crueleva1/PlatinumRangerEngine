#version 150
 
uniform vec4 uColor;
uniform bool uCustomColor;

out vec4 oColor;
 
void main()
{
    if (uCustomColor) {
        oColor = uColor;
        return;
    }
    oColor = vec4(1.0, 1.0, 1.0, 1.0);
}