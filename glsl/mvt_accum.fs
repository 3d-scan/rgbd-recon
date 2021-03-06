#version 140
#extension GL_ARB_shading_language_include : require
#extension GL_ARB_gpu_shader5 : enable
uniform uint stage;
// used by accumulation pass
uniform sampler2DArray kinect_colors;
uniform sampler2DArray kinect_normals;
uniform sampler2DArray depth_map_curr;
uniform uint layer;

uniform mat4 img_to_eye_curr;
uniform vec2 viewportSizeInv;
uniform float epsilon;
uniform mat4 gl_NormalMatrix;

#include </shading.glsl>

#include </inc_bbox_test.glsl>
///////////////////////////////////////////////////////////////////////////////
// input
///////////////////////////////////////////////////////////////////////////////
in vec2  texture_coord;
in vec3  pos_es;
in vec3  pos_cs;
in float sq_area_cs;

in float depth;
in float lateral_quality;
in vec3  normal_es;

out vec4 gl_FragColor;
///////////////////////////////////////////////////////////////////////////////
// main
///////////////////////////////////////////////////////////////////////////////
void main() {

  if(!in_bbox(pos_cs))
    discard;

  // to cull away borders of the rgb camera view
  if(texture_coord.s > 0.99 || texture_coord.s < 0.01 ||
    texture_coord.t > 0.99 || texture_coord.t < 0.01) {
      discard;
  }

  vec3 normal = -normalize(normal_es);
  // backface culling
  if ( dot ( normal, normalize(pos_es) ) > 0.0 ) {
   discard;
  }

  //float quality = 1.0/(depth * depth);
  float quality = lateral_quality/depth;
  //float packed_normal = pack_vec3(normal);

  if(stage > 0u){ // accumulation pass write color and quality if within epsilon
    vec3  coords = vec3(gl_FragCoord.xy * viewportSizeInv, 0.0 /*here layer is always 0*/);
    float depth_curr = texture(depth_map_curr, coords).r;
    vec4  position_curr = img_to_eye_curr * vec4(gl_FragCoord.xy + vec2(0.5,0.5),depth_curr,1.0);
    vec3  position_curr_es = (position_curr / position_curr.w).xyz;
    if(epsilon < length(position_curr_es - pos_es)){
      discard;
    }

    if (g_shade_mode == 3u) {
      gl_FragColor = vec4(camera_colors[layer] * quality, quality);
    }
    else {
      vec3 color = texture(kinect_colors, vec3(texture_coord, float(layer))).rgb;
      gl_FragColor = vec4(shade(pos_es, normal, color) * quality, quality);
    }
  }
}