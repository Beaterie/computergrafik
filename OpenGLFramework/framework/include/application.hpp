#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include "structs.hpp"

#include <glm/gtc/type_precision.hpp>

#include <map>

// gpu representation of model
class Application {
 public:
  // allocate and initialize objects
  Application(std::string const& resource_path);
  // free
  virtual ~Application();

  // update uniform locations and values
  inline virtual void uploadUniforms() {};
  // update projection matrix
  void setProjection(glm::fmat4 const& projection_mat);
  virtual void updateProjection() = 0;
  // react to key input
  inline virtual void keyCallback(int key, int scancode, int action, int mods) {};
  //handle delta mouse movement input
  inline virtual void mouseCallback(double pos_x, double pos_y) {};

  // give shader programs to launcher
  virtual std::map<std::string, shader_program>& getShaderPrograms();
  // draw all objects
  virtual void render() const = 0;

 protected:
  void updateUniformLocations();

  std::string m_resource_path; 

  glm::fmat4 m_view_transform;
  glm::fmat4 m_view_projection;

  // container for the shader programs
  std::map<std::string, shader_program> m_shaders{};
};

#endif