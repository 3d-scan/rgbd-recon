#ifndef RECON_POINTS_HPP
#define RECON_POINTS_HPP

#include "reconstruction.hpp"

#include <globjects/Buffer.h>
#include <globjects/Program.h>
#include <globjects/VertexArray.h>

namespace kinect{

  class ReconPoints : public Reconstruction {

  public:
    ReconPoints(CalibrationFiles const& cfs, CalibVolumes const* cv, gloost::BoundingBox const&  bbox);
    ~ReconPoints();

    void draw() override;

  private:
    globjects::VertexArray*              m_point_grid;
    globjects::Buffer*                  m_point_buffer;

    globjects::Program*                  m_program;
  };
}

#endif // #ifndef RECON_POINTS_HPP