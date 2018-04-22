#include "recon_mvt.hpp"

#include "calibration_files.hpp"
#include "screen_quad.hpp"
#include <KinectCalibrationFile.h>
#include "CalibVolumes.hpp"

#include <Matrix.h>
#include <glm/gtc/type_precision.hpp>
#include <globjects/VertexAttributeBinding.h>
#include <globjects/Shader.h>

namespace kinect{

ReconMVT::ReconMVT(CalibrationFiles const& cfs, CalibVolumes const* cv, gloost::BoundingBox const&  bbox)
 :Reconstruction(cfs, cv, bbox)
 ,m_va_pass_depth()
 ,m_va_pass_accum()
 ,m_tri_grid{new globjects::VertexArray()}
 ,m_tri_buffer{new globjects::Buffer()}
 ,m_program_accum{new globjects::Program()}
 ,m_program_normalize{new globjects::Program()}
{
  m_program_accum->attach(
    globjects::Shader::fromFile(GL_VERTEX_SHADER,   "glsl/mvt_accum.vs"),
    globjects::Shader::fromFile(GL_FRAGMENT_SHADER, "glsl/mvt_accum.fs"),
    globjects::Shader::fromFile(GL_GEOMETRY_SHADER, "glsl/mvt_accum.gs")
  );

  m_program_accum->setUniform("kinect_colors",1);
  // use unprocessed depths
  m_program_accum->setUniform("kinect_depths",40);
  m_program_accum->setUniform("kinect_normals", 4);
  m_program_accum->setUniform("depth_map_curr",14);
  m_program_accum->setUniform("cv_min_d", 0.5f);
  m_program_accum->setUniform("cv_max_d", 4.5f);
  m_program_accum->setUniform("epsilon", 0.075f);
  m_program_accum->setUniform("min_length", m_min_length);
  m_program_accum->setUniform("cv_xyz", m_cv->getXYZVolumeUnits());
  m_program_accum->setUniform("cv_uv", m_cv->getUVVolumeUnits());
  m_program_accum->setUniform("tex_size_inv", glm::fvec2{1.0f / m_tex_width, 1.0f / m_tex_height});

  m_program_normalize->attach(
     globjects::Shader::fromFile(GL_VERTEX_SHADER,   "glsl/trigrid_normalize.vs")
    ,globjects::Shader::fromFile(GL_FRAGMENT_SHADER, "glsl/trigrid_normalize.fs")
    );
  m_program_normalize->setUniform("color_map",15);
  m_program_normalize->setUniform("depth_map",16);

  std::vector<glm::fvec2> data{};
  float stepX = 1.0f/m_tex_width;
  float stepY = 1.0f/m_tex_height;
  for(unsigned y = 0; y < m_tex_width; ++y){
    for(unsigned x = 0; x < m_tex_height; ++x){
      data.emplace_back( (x+0.5) * stepX, (y + 0.5) * stepY );
      data.emplace_back( (x+1.5) * stepX, (y + 0.5) * stepY );
      data.emplace_back( (x+0.5) * stepX, (y + 1.5) * stepY );

      data.emplace_back( (x+1.5) * stepX, (y + 0.5) * stepY );
      data.emplace_back( (x+1.5) * stepX, (y + 1.5) * stepY );
      data.emplace_back( (x+0.5) * stepX, (y + 1.5) * stepY );
    }
  }

  m_tri_buffer->setData(data, GL_STATIC_DRAW);

  m_tri_grid->enable(0);
  m_tri_grid->binding(0)->setAttribute(0);
  m_tri_grid->binding(0)->setBuffer(m_tri_buffer, 0, sizeof(glm::fvec2));
  m_tri_grid->binding(0)->setFormat(2, GL_FLOAT);

  reload();
  
  resize(600, 480);
}

ReconMVT::~ReconMVT() {
  m_tri_grid->destroy();
  m_tri_buffer->destroy();
  m_program_accum->destroy();
  m_program_normalize->destroy();
}

void ReconMVT::draw(){
  std::cerr << "ReconMVT::draw() is not implemented any more!" << std::endl;
  return;
  // calculate img_to_eye for this view
  gloost::Matrix projection_matrix;
  glGetFloatv(GL_PROJECTION_MATRIX, projection_matrix.data());
  gloost::Matrix viewport_translate;
  viewport_translate.setIdentity();
  viewport_translate.setTranslate(1.0,1.0,1.0);
  gloost::Matrix viewport_scale;
  viewport_scale.setIdentity();

  glm::uvec4 viewport_vals{getViewport()};
  viewport_scale.setScale(viewport_vals[2] * 0.5, viewport_vals[3] * 0.5, 0.5f);
  gloost::Matrix image_to_eye =  viewport_scale * viewport_translate * projection_matrix;
  image_to_eye.invert();

  // cull in the geometry shader
  glDisable(GL_CULL_FACE);
// pass 1 goes to depth buffer only
  m_va_pass_depth->enable(0, false);

  m_program_accum->use();
  m_program_accum->setUniform("stage", 0u);

  for(unsigned layer = 0; layer < m_num_kinects; ++layer){
    m_program_accum->setUniform("layer",  layer);

    m_tri_grid->drawArrays(GL_TRIANGLES, 0, m_tex_width * m_tex_height * 6);
  }

  m_va_pass_depth->disable();

// pass 2 goes to accumulation buffer
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND); 
  glBlendFuncSeparateEXT(GL_ONE,GL_ONE,GL_ONE,GL_ONE);
  glBlendEquationSeparateEXT(GL_FUNC_ADD, GL_FUNC_ADD);
  m_va_pass_accum->enable();
  m_program_accum->setUniform("stage", 1u);
  m_program_accum->setUniform("viewportSizeInv", glm::fvec2(1.0f/m_va_pass_depth->getWidth(), 1.0f/m_va_pass_depth->getHeight()));
  m_program_accum->setUniform("img_to_eye_curr", image_to_eye);
  
  m_va_pass_depth->bindToTextureUnitDepth(14);

  for(unsigned layer = 0; layer < m_num_kinects; ++layer){
    m_program_accum->setUniform("layer",  layer);

    m_tri_grid->drawArrays(GL_TRIANGLES, 0, m_tex_width * m_tex_height * 6);
  }

  m_program_accum->release();
  m_va_pass_accum->disable();
  glDisable(GL_BLEND);

// normalize pass outputs best quality color and depth to framebuffer of parent renderstage
  glEnable(GL_DEPTH_TEST);

  m_program_normalize->use();
  m_program_normalize->setUniform("texSizeInv", glm::fvec2(1.0f/m_va_pass_depth->getWidth(), 1.0f/m_va_pass_depth->getHeight()));
  m_program_normalize->setUniform("offset"    , glm::fvec2(1.0f * viewport_vals[0],                    1.0f * viewport_vals[1]));
  
  m_va_pass_accum->bindToTextureUnitRGBA(15);
  m_va_pass_depth->bindToTextureUnitDepth(16);
  
  ScreenQuad::draw();

  m_program_normalize->release();
}

void ReconMVT::resize(std::size_t width, std::size_t height) {
  m_va_pass_depth = std::unique_ptr<ViewArray>{new ViewArray(width, height, 1)};
  m_va_pass_accum = std::unique_ptr<ViewArray>{new ViewArray(width, height, 1)};
}

}