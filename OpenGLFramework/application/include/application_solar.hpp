#ifndef APPLICATION_SOLAR_HPP
#define APPLICATION_SOLAR_HPP

#include "application.hpp"
#include "model.hpp"
#include "structs.hpp"
#include <memory>

// gpu representation of model
class ApplicationSolar : public Application {
 public:
  // allocate and initialize objects
  ApplicationSolar(std::string const& resource_path);
  // free allocated objects
  ~ApplicationSolar();

  // update uniform locations and values
  void uploadUniforms();
  // update projection matrix
  void updateProjection();
  // react to key input
  void keyCallback(int key, int scancode, int action, int mods);
  // handle delta mouse movement input
  void mouseCallback(double pos_x, double pos_y);

  // draw all objects
  void render() const;

  // upload planets
  void upload_planet_transforms(std::shared_ptr<planet> planet) const;

 protected:
  void initializeShaderPrograms();
  void initializeGeometry();
  void updateView();

  // cpu representation of model
  model_object planet_object;
  // vector with planets
  std::vector<std::shared_ptr<planet>> all_planets;
  
  float press_W = 0.0f;
  float press_S = 0.0f;
  float press_A = 0.0f;
  float press_D = 0.0f;
  float press_E = 0.0f;
  float press_SPACE = 0.0f;
};

#endif
