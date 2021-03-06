#version 140
#extension GL_ARB_shading_language_include : require
#extension GL_ARB_shader_storage_buffer_object : require

// input
in vec3 in_Position; 

uniform mat4 gl_ModelViewMatrix;
uniform mat4 gl_ProjectionMatrix;

out vec3 geo_Position;
out uint geo_Id;

#include </bricks.glsl>

void main() {
  geo_Position = in_Position;
  geo_Id = bricks_occupied[gl_InstanceID];
  gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * vec4(to_world(in_Position, index_3d(geo_Id)), 1.0);
}