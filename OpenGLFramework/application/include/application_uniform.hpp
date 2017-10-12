#ifndef APPLICATION_UNIFORM_HPP
#define APPLICATION_UNIFORM_HPP

#include "application.hpp"

#include <glm/gtc/type_precision.hpp>

// gpu representation of model
class ApplicationUniform : public Application {
 public:
  // allocate and initialize objects
  ApplicationUniform(std::string const& resource_path);

  // update uniform locations and values
  void uploadUniforms();
  // update projection matrix
  void updateProjection();
  // draw all objects
  void render() const;

 protected:
  void initializeShaderPrograms();
  // uniform locations
  GLint m_ul_model_view;
  GLint m_ul_projection;
};

#endif