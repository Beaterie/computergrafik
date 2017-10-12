#ifndef APPLICATION_VBO_HPP
#define APPLICATION_VBO_HPP

#include "application.hpp"

// gpu representation of model
class ApplicationVbo : public Application {
 public:
  // allocate and initialize objects
  ApplicationVbo(std::string const& resource_path);
  ~ApplicationVbo();

  // update projection matrix
  void updateProjection();
  // draw all objects
  void render() const;
 
 private:
  void initializeGeometry();
  // vertex buffer object
  GLuint m_vertex_bo;
};

#endif