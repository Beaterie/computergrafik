#ifndef APPLICATION_VAO_HPP
#define APPLICATION_VAO_HPP

#include "application.hpp"

// gpu representation of model
class ApplicationVao : public Application {
 public:
  // allocate and initialize objects
  ApplicationVao(std::string const& resource_path);
  ~ApplicationVao();

  // update uniform locations and values
  void uploadUniforms();
  // update projection matrix
  void updateProjection();
  // draw all objects
  void render() const;

 protected:
  void initializeShaderPrograms();
  void initializeGeometry();

  // vertex array object
  GLuint m_vertex_ao;
  // vertex buffer object
  GLuint m_vertex_bo;
  // index buffer object
  GLuint m_index_bo;
};

#endif