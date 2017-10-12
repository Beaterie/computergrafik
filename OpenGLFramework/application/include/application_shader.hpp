#ifndef APPLICATION_SHADER_HPP
#define APPLICATION_SHADER_HPP

#include "application.hpp"

// gpu representation of model
class ApplicationShader : public Application {
 public:
  // allocate and initialize objects
  ApplicationShader(std::string const& resource_path);
  ~ApplicationShader();

  // update projection matrix
  void updateProjection();
  // draw all objects
  void render() const;

 protected:
  void initializeShaderPrograms();

  GLuint m_program;
};

#endif