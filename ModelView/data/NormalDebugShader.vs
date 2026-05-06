#version 150

in vec3 iVertices;
in vec3 iNormals;
in vec3 iTangents;
in vec3 iBiNormals;

out Vertex
{
    vec3 voNormal;
    vec3 voTangents;
    vec3 voBiNormals;
    vec4 voColor;
} voVertice;


void main()
{
    gl_Position = vec4(iVertices, 1.0);

    voVertice.voNormal = iNormals;
    voVertice.voTangents = iTangents;
    voVertice.voBiNormals = iBiNormals;
    voVertice.voColor =  vec4(1.0, 1.0, 0.0, 1.0);
}