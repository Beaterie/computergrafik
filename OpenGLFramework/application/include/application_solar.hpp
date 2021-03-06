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
  void upload_planet_transforms(std::shared_ptr<planet> planet, std::string shadermode, texture_object obj, float number) const;
  // upload sun
  void upload_sun(std::shared_ptr<planet> sun, std::string shadermode, texture_object obj) const;
  // upload orbits
  void upload_orbits(std::shared_ptr<planet> planet) const;
  // upload skybox
  void upload_skybox() const;

  void upload_lightframebuffer() const;
  // upload screen quad
  void upload_screenquad() const;

 protected:
  void initializeShaderPrograms();
  void initializeSkybox();
  void initializeGeometry();
  void initializeNormalMaps();
  void initializeSkyboxTex();
  void initializeTextures(unsigned int num, unsigned int unit_num);
  void initializeStars();
  void initializeOrbits();
  void initializeLightFramebuffer();
  void initializeFramebuffer();
  void initializeScreenQuad();
  void updateView();

  // cpu representation of model
  model_object planet_object{};
  model_object star_object{};
  model_object orbit_object{};
  model_object skybox_object{};
  model_object screen_quad_object{};

  // vectors with content
  std::vector<std::shared_ptr<planet>> all_planets;
  std::vector<float> all_stars;
  std::vector<float> all_orbits;
  std::vector<float> screen_edges;
  std::string all_textures[13] = {"4k_sun.png", "2k_mercury.png", "2k_venus.png",
    "2k_earth.png", "2k_mars.png", "2k_jupiter.png", "2k_saturn.png", "2k_uranus.png",
    "2k_neptune.png", "1k_pluto.png", "2k_moon.png", "2k_earth_nightmap.png", "skybox.png"};
  texture_object all_texture_objects[13] = {};
  texture_object normaltex_obj{};
  texture_object texturebuff{};
  texture_object renderbuff{};
  texture_object framebuff{};

  texture_object light_framebuff{};
  texture_object light_texturebuff{};
  texture_object light_renderbuff{};

  
  // navigator keys
  float press_W = 0.0f;
  float press_S = 0.0f;
  float press_A = 0.0f;
  float press_D = 0.0f;
  float press_E = 0.0f;
  float press_SPACE = 0.0f;

  // keys for modes
  bool celshading = false;
  bool greyscale = false;
  bool verMirror = false;
  bool horMirror = false;
  bool blur = false;
};

#endif
