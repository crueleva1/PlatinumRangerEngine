#version 150

layout(triangles) in;
layout(line_strip, max_vertices=8) out;

uniform mat4 uMVPMatrix;
uniform float uNormalLength;

in Vertex
{
    vec3 voNormal;
    vec3 voTangents;
    vec3 voBiNormals;
    vec4 voColor;
} voVertice[];

out vec4 goVertexColor;

void main()
{  
    //------ 3 lines for the 3 vertex normals
    //
    for (int nIndex = 0; nIndex < gl_in.length(); nIndex++)
    {
        vec3 kP = gl_in[nIndex].gl_Position.xyz;
        vec3 kN = voVertice[nIndex].voNormal;
        vec3 kT = voVertice[nIndex].voTangents;
        vec3 kB = voVertice[nIndex].voBiNormals;
        
        vec4 kTransformP = uMVPMatrix * vec4(kP, 1.0);;

        gl_Position = kTransformP;
        goVertexColor = vec4(1.0, 0.0, 0.0, 1.0);
        EmitVertex();
        
        gl_Position = uMVPMatrix * vec4(kP + kN * uNormalLength, 1.0);
        goVertexColor = vec4(1.0, 0.0, 0.0, 1.0);
        //goVertexColor = voVertice[nIndex].voColor;
        EmitVertex();

        gl_Position = kTransformP;
        goVertexColor = vec4(0.0, 1.0, 0.0, 1.0);
        EmitVertex();

        gl_Position = uMVPMatrix * vec4(kP + kT * uNormalLength, 1.0);
        goVertexColor = vec4(0.0, 1.0, 0.0, 1.0);
        EmitVertex();

        gl_Position = kTransformP;
        goVertexColor = vec4(0.0, 0.0, 1.0, 1.0);
        EmitVertex();

        gl_Position = uMVPMatrix * vec4(kP + kB * uNormalLength, 1.0);
        goVertexColor = vec4(0.0, 0.0, 1.0, 1.0);
        EmitVertex();

        EndPrimitive();
    }
  
    /*
    //------ One line for the face normal
    //
    vec3 kP0 = gl_in[0].gl_Position.xyz;
    vec3 kP1 = gl_in[1].gl_Position.xyz;
    vec3 kP2 = gl_in[2].gl_Position.xyz;
    
    vec3 kV0 = kP0 - kP1;
    vec3 kV1 = kP2 - kP1;
    
    vec3 kN = cross(kV1, kV0);
    kN = normalize(kN);
    
    // Center of the triangle
    vec3 kP = (kP0+kP1+kP2) / 3.0;
    
    gl_Position = uMVPMatrix * vec4(kP, 1.0);
    goVertexColor = vec4(1, 0, 0, 1);
    EmitVertex();
    
    gl_Position = uMVPMatrix * vec4(kP + kN * uNormalLength, 1.0);
    goVertexColor = vec4(1, 0, 0, 1);
    EmitVertex();
    EndPrimitive();
    */
}