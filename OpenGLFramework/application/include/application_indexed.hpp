#ifndef APPLICATION_INDEXED_HPP
#define APPLICATION_INDEXED_HPP

#include "application.hpp"

// gpu representation of model
class ApplicationIndexed : public Application {
 public:
  // allocate and initialize objects
  ApplicationIndexed(std::string const& resource_path);
  ~ApplicationIndexed();

  // update projection matrix
  void updateProjection();
  // draw all objects
  void render() const;
 
 private:
  void initializeGeometry();
  // vertex buffer object
  GLuint m_vertex_bo;
  // index buffer object
  GLuint m_index_bo;
};

#endif