#include "application_solar.hpp"
#include "launcher.hpp"

#include "utils.hpp"
#include "shader_loader.hpp"
#include "model_loader.hpp"
#include "texture_loader.hpp"
#include "pixel_data.hpp"

#include <glbinding/gl/gl.h>
// use gl definitions from glbinding 
using namespace gl;

// dont load gl bindings from glfw
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <math.h>



ApplicationSolar::ApplicationSolar(std::string const& resource_path)
 :Application{resource_path} {

  // enable transparency
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDisable(GL_CULL_FACE);
  
  initializeSkybox();
  initializeShaderPrograms();
  initializeGeometry();
  initializeStars();
  initializeFramebuffer();
  //glEnable(GL_TEXTURE_2D);
  //glEnable(GL_LIGHTING);

  // planets
  // {name, size, speed_of_rotation, distance_to_origin, instrinct rotation, color}
  planet sonne{"Sonne",3.0f, 0.0f, 0.0f, 0.1f, {1.0f,0.75f,0.0f}};
  planet merkur{"Merkur",log(4.9f)*0.1f, 87.0f*0.00005f, log(5.8f)*2.5f, {0.5f,0.5f,0.5f}};
  planet venus{"Venus",log(12.0f)*0.1f, 200.0f*0.00005f, log(10.8f)*2.5f, {1.0f,0.9f,0.3f}};
  planet erde{"Erde",log(13.0f)*0.1f, 365.0f*0.00005f, log(15.0f)*2.5f, {0.05f,0.1f,1.0f}};
  planet mars{"Mars",log(6.8f)*0.1f, 700.0f*0.00005f, log(22.8f)*2.5f, {0.9f,0.5f,0.3f}};
  planet jupiter{"Jupiter",log(142.0f)*0.1f, 450.0f*0.00005f, log(77.8f)*2.5f, {0.8f,0.5f,0.3f}};
  planet saturn{"Saturn",log(120.0f)*0.1f, 50.0f*0.00005f, log(190.0f)*2.5f, {0.8f,0.8f,0.3f}};
  planet uranus{"Uranus",log(51.0f)*0.1f, 140.0f*0.00005f, log(435.0f)*2.5f, {0.6f,0.8f,1.0f}};
  planet neptun{"Neptun",log(48.0f)*0.1f, 300.0f*0.00005f, log(1000.0f)*2.5f, {0.0f,0.1f,0.8f}};
  planet pluto{"Pluto",log(2.3f)*0.1f, 160.0f*0.00005f, log(6000.0f)*2.5f, {0.8f,0.5f,0.4f}};
  planet mond{"Mond",log(3.4f)*0.1f, 365.0f*0.00005f, log(15.0f)*2.5f, {0.7f,0.7f,0.7f}};
  // planet pointer
  auto p_sonne    = std::make_shared<planet>(sonne);
  auto p_merkur   = std::make_shared<planet>(merkur);
  auto p_venus    = std::make_shared<planet>(venus);
  auto p_erde     = std::make_shared<planet>(erde);
  auto p_mars     = std::make_shared<planet>(mars);
  auto p_jupiter  = std::make_shared<planet>(jupiter);
  auto p_saturn   = std::make_shared<planet>(saturn);
  auto p_uranus   = std::make_shared<planet>(uranus);
  auto p_neptun   = std::make_shared<planet>(neptun);
  auto p_pluto    = std::make_shared<planet>(pluto);
  auto p_mond     = std::make_shared<planet>(mond);
  // vector with planet pointers
  all_planets.insert(std::end(all_planets),{p_sonne, p_merkur, p_venus,
    p_erde, p_mars, p_jupiter, p_saturn, p_uranus, p_neptun, p_pluto, p_mond});

  //initializeTextures(12, 0);
  // initialize textures and bind to texture objects
  for (unsigned int i = 0; i < (sizeof(all_textures)/sizeof(*all_textures)-2); ++i) {
    initializeTextures(i, 0);
  }
  // earth at night texture
  initializeSkyboxTex();
  initializeTextures(11, 1);
  initializeNormalMaps();

  initializeOrbits();

  std::cout << "Realistic shader activated.\n";
}

// upload planets
void ApplicationSolar::upload_planet_transforms(std::shared_ptr<planet> planet, std::string shadermode, texture_object obj, float number) const {

  // main rotation
  glm::fmat4 model_matrix = glm::rotate(glm::fmat4{}, float(glfwGetTime())*planet->m_rot_speed, glm::fvec3{0.0f, 1.0f, 0.0f});
  // main distance (distance to origin)
  model_matrix = glm::translate(model_matrix, glm::fvec3{0.0f, 0.0f, planet->m_origin_dis});
  // intrinsic rotation
  model_matrix = glm::rotate(model_matrix, float(glfwGetTime())*planet->m_intr_rot_speed, glm::fvec3{0.0f, 1.0f, 0.0f});
  
  // if moon, rotate around earth
  if (planet->m_name == "Mond") {
    // moon rotation
    model_matrix = glm::rotate(model_matrix, float(glfwGetTime()), glm::fvec3{0.0f, 1.0f, 0.0f});
    // moon distance (distance to earth)
    model_matrix = glm::translate(model_matrix, glm::fvec3{0.0f, 0.0f, 0.5f});
  }

  // scale the planet
  model_matrix = glm::scale(model_matrix, glm::fvec3{planet->m_size,planet->m_size,planet->m_size});

  // color
  glUniform3fv(m_shaders.at(shadermode).u_locs.at("PlanetColor"), 1,
    glm::value_ptr(planet->m_color));

  // planet number
  glUniform1f(m_shaders.at(shadermode).u_locs.at("PlanetNumber"), GLfloat(number));

  // shader it
  glUniformMatrix4fv(m_shaders.at(shadermode).u_locs.at("ModelMatrix"),
                     1, GL_FALSE, glm::value_ptr(model_matrix));

  // realistic shader with texture
  if (shadermode == "planet") {
    //std::cout << "TexNumber: " << texNumber << std::endl;
    // activate shader
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, obj.handle);
    // get location of sampler uniform
    glUniform1i(glGetUniformLocation(m_shaders.at(shadermode).handle, "Texture"), GLint(0));

    // normal uniform
    glm::fmat4 normal_matrix = glm::inverseTranspose(glm::inverse(m_view_transform) * model_matrix);
    glUniformMatrix4fv(m_shaders.at(shadermode).u_locs.at("NormalMatrix"),
                       1, GL_FALSE, glm::value_ptr(normal_matrix));
  }


  // bind the VAO to draw
  glBindVertexArray(planet_object.vertex_AO);

  // draw bound vertex array using bound shader
  glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);
}

void ApplicationSolar::upload_sun(std::shared_ptr<planet> sun, std::string shadermode, texture_object obj) const {

  // intrinsic rotation
  glm::fmat4 model_matrix = glm::rotate(glm::fmat4{}, float(glfwGetTime())*sun->m_intr_rot_speed, glm::fvec3{0.0f, 1.0f, 0.0f});

  // scale sun
  model_matrix = glm::scale(model_matrix, glm::fvec3{sun->m_size,sun->m_size,sun->m_size});

  std::string mode = "";
  if (shadermode == "planet") {
    mode = "sun";
  }
  if (shadermode == "cel") {
    mode = "sun_cel";
  }

  // shader it
  glUniformMatrix4fv(m_shaders.at(mode).u_locs.at("ModelMatrix"),
                     1, GL_FALSE, glm::value_ptr(model_matrix));

  //glUseProgram(m_shaders.at("sun").handle);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, obj.handle);
  // get location of sampler uniform
  glUniform1i(glGetUniformLocation(m_shaders.at(mode).handle, "Texture"), GLint(0));

  // extra matrix for normal transformation to keep them orthogonal to surface
  //glm::fmat4 normal_matrix = glm::inverseTranspose(glm::inverse(m_view_transform) * model_matrix);
  //glUniformMatrix4fv(m_shaders.at("sun").u_locs.at("NormalMatrix"),
  //                   1, GL_FALSE, glm::value_ptr(normal_matrix));

  // bind the VAO to draw
  glBindVertexArray(planet_object.vertex_AO);

  // draw bound vertex array using bound shader
  glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);
}

void ApplicationSolar::upload_skybox(texture_object obj) const {

  // remove depth
  glDepthMask(GL_FALSE);

  // activate texture
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, obj.handle);
  glBindVertexArray(skybox_object.vertex_AO);
  // get location of sampler uniform
  glUniform1i(glGetUniformLocation(m_shaders.at("skybox").handle, "TextureSky"), GLint(0));

  // draw 
  glDrawArrays(GL_TRIANGLES, 0, 36);

  // reactive depth
  glDepthMask(GL_TRUE);
}

void ApplicationSolar::upload_orbits(std::shared_ptr<planet> planet) const {

  // shader it
  glm::fmat4 moon_matrix{};
  if (planet->m_name == "Mond") {
    // if moon, set moon orbit with rotation
    moon_matrix = glm::rotate(moon_matrix, float(glfwGetTime())*planet->m_rot_speed, glm::fvec3{0.0f, 1.0f, 0.0f});
    moon_matrix = glm::translate(moon_matrix, glm::fvec3{0.0f, 0.0f, planet->m_origin_dis});
    // shader it
    glUniform1f(m_shaders.at("orbit").u_locs.at("OrbitDistance"), 0.5f);
  }
  else {
    // shader it
    glUniform1f(m_shaders.at("orbit").u_locs.at("OrbitDistance"), planet->m_origin_dis);
  }
  // shader it
  glUniformMatrix4fv(m_shaders.at("orbit").u_locs.at("MoonMatrix"),
                     1, GL_FALSE, glm::value_ptr(moon_matrix));

  // bind the VAO to draw
  glBindVertexArray(orbit_object.vertex_AO);

  // draw bound vertex array using bound shader
  glDrawArrays(orbit_object.draw_mode, 0, orbit_object.num_elements);
}

void ApplicationSolar::render() const {

  std::string shadermode = "";

  glDepthMask(GL_FALSE);
  glUseProgram(m_shaders.at("skybox").handle);
  glActiveTexture(GL_TEXTURE0);
  // bind Texture Object to 2d texture binding point of unit
  glBindTexture(GL_TEXTURE_CUBE_MAP, all_texture_objects[12].handle);
  glUniform1i(glGetUniformLocation(m_shaders.at("skybox").handle, "TextureSky"), GLint(0));
  glBindVertexArray(skybox_object.vertex_AO);

  //glEnable(GL_CULL_FACE);
  //glCullFace(GL_FRONT);
  //glDisable(GL_DEPTH_TEST);
  glDrawElements(skybox_object.draw_mode, skybox_object.num_elements, model::INDEX.type, NULL);
  //glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);

  // bind shader to upload uniforms
  //glUseProgram(m_shaders.at("skybox").handle);
  // load skybox
  //upload_skybox(all_texture_objects[12]);

  // check which shader should be used (realistic or cel shader)
  if (celshading) {
    shadermode = "cel";
    // bind shader to upload uniforms
    glUseProgram(m_shaders.at("sun_cel").handle);
    // sun position
    glUniform3fv(m_shaders.at("sun_cel").u_locs.at("SunPosition"), 1,
    glm::value_ptr(glm::fvec3{0.0f,0.0f,0.0f}));
  }
  else {
    shadermode = "planet";
    // bind shader to upload uniforms
    glUseProgram(m_shaders.at("sun").handle);
    // sun position
    glUniform3fv(m_shaders.at("sun").u_locs.at("SunPosition"), 1,
    glm::value_ptr(glm::fvec3{0.0f,0.0f,0.0f}));
  }

  // load sun
  upload_sun(all_planets[0], shadermode, all_texture_objects[0]);

  // bind shader to upload uniforms
  glUseProgram(m_shaders.at(shadermode).handle);
  // sun position
  glUniform3fv(m_shaders.at(shadermode).u_locs.at("SunPosition"), 1,
      glm::value_ptr(glm::fvec3{0.0f,0.0f,0.0f}));

  // activate shader
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, all_texture_objects[11].handle);
  // get location of sampler uniform
  glUniform1i(glGetUniformLocation(m_shaders.at(shadermode).handle, "TextureNight"), GLint(1));

  // load planets
  for (unsigned int i = 1; i < all_planets.size(); ++i) {
    upload_planet_transforms(all_planets[i], shadermode, all_texture_objects[i], float(i));
  }

  // load mars normal map
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, normaltex_obj.handle);
  glUniform1i(glGetUniformLocation(m_shaders.at(shadermode).handle, "NormalMap"), GLint(2));

  // bind shader to upload uniforms
  glUseProgram(m_shaders.at("star").handle);
  // bind the VAO to draw
  glBindVertexArray(star_object.vertex_AO);
  // draw bound vertex array using bound shader
  glDrawArrays(star_object.draw_mode, 0, star_object.num_elements);

  // bind shader to upload uniforms
  glUseProgram(m_shaders.at("orbit").handle);
  // load orbits
  for (unsigned int i = 0; i < all_planets.size(); ++i) {
    upload_orbits(all_planets[i]);
  }
}

void ApplicationSolar::updateView() {
  // vertices are transformed in camera space, so camera transform must be inverted
  glm::fmat4 view_matrix = glm::inverse(m_view_transform);

  // upload matrix to gpu
  glUseProgram(m_shaders.at("skybox").handle);
  glUniformMatrix4fv(m_shaders.at("skybox").u_locs.at("ViewMatrix"),
                      1, GL_FALSE, glm::value_ptr(view_matrix));

  glUseProgram(m_shaders.at("planet").handle);
  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ViewMatrix"),
                      1, GL_FALSE, glm::value_ptr(view_matrix));

  glUseProgram(m_shaders.at("star").handle);
  glUniformMatrix4fv(m_shaders.at("star").u_locs.at("ViewMatrix"),
                      1, GL_FALSE, glm::value_ptr(view_matrix));

  glUseProgram(m_shaders.at("orbit").handle);
  glUniformMatrix4fv(m_shaders.at("orbit").u_locs.at("ViewMatrix"),
                      1, GL_FALSE, glm::value_ptr(view_matrix));

  glUseProgram(m_shaders.at("sun").handle);
  glUniformMatrix4fv(m_shaders.at("sun").u_locs.at("ViewMatrix"),
                      1, GL_FALSE, glm::value_ptr(view_matrix));

  glUseProgram(m_shaders.at("sun_cel").handle);
  glUniformMatrix4fv(m_shaders.at("sun_cel").u_locs.at("ViewMatrix"),
                      1, GL_FALSE, glm::value_ptr(view_matrix));

  glUseProgram(m_shaders.at("cel").handle);
  glUniformMatrix4fv(m_shaders.at("cel").u_locs.at("ViewMatrix"),
                      1, GL_FALSE, glm::value_ptr(view_matrix));
}

void ApplicationSolar::updateProjection() {
  // upload matrix to gpu
  glUseProgram(m_shaders.at("skybox").handle);
  glUniformMatrix4fv(m_shaders.at("skybox").u_locs.at("ProjectionMatrix"),
                     1, GL_FALSE, glm::value_ptr(m_view_projection));  

  glUseProgram(m_shaders.at("planet").handle);
  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ProjectionMatrix"),
                     1, GL_FALSE, glm::value_ptr(m_view_projection));

  glUseProgram(m_shaders.at("star").handle);
  glUniformMatrix4fv(m_shaders.at("star").u_locs.at("ProjectionMatrix"),
                     1, GL_FALSE, glm::value_ptr(m_view_projection));

  glUseProgram(m_shaders.at("orbit").handle);
  glUniformMatrix4fv(m_shaders.at("orbit").u_locs.at("ProjectionMatrix"),
                     1, GL_FALSE, glm::value_ptr(m_view_projection));

  glUseProgram(m_shaders.at("sun").handle);
  glUniformMatrix4fv(m_shaders.at("sun").u_locs.at("ProjectionMatrix"),
                     1, GL_FALSE, glm::value_ptr(m_view_projection));

  glUseProgram(m_shaders.at("sun_cel").handle);
  glUniformMatrix4fv(m_shaders.at("sun_cel").u_locs.at("ProjectionMatrix"),
                     1, GL_FALSE, glm::value_ptr(m_view_projection));

  glUseProgram(m_shaders.at("cel").handle);
  glUniformMatrix4fv(m_shaders.at("cel").u_locs.at("ProjectionMatrix"),
                     1, GL_FALSE, glm::value_ptr(m_view_projection));
}

// update uniform locations
void ApplicationSolar::uploadUniforms() {
  updateUniformLocations();  
  updateView();
  updateProjection();
}

// handle key input
void ApplicationSolar::keyCallback(int key, int scancode, int action, int mods) {

  // Two-direction-navigation is possible:
  // 1. press button for the first direction
  // 2. press and hold button for the second direction
  // 3. press any other key to reset the navigation
  // 4. start again with 1.

  // Also possible with three directions.

  // Example:
  // 1. press W
  // 2. press and hold SPACE
  // --> you fly to the top and zoom in
  // 3. press any other key to reset the navigation
  

  if (action == GLFW_REPEAT || action == GLFW_PRESS) {

    // zoom in (W)
    if (key == GLFW_KEY_W) {
      press_W = -0.25f;
      press_S = 0.0f;
    }
    // zoom out (S)
    else if (key == GLFW_KEY_S) {
      press_S = 0.25f;
      press_W = 0.0f;
    }
    // fly left (A)
    else if (key == GLFW_KEY_A) {
      press_A = -0.25f;
      press_D = 0.0f;
    }
    // fly right (D)
    else if (key == GLFW_KEY_D) {
      press_D = 0.25f;
      press_A = 0.0f;
    }
    // fly to the bottom (E)
    else if (key == GLFW_KEY_E) {
      press_E = -0.25f;
      press_SPACE = 0.0f;
    }
    // fly to the top (SPACE)
    else if (key == GLFW_KEY_SPACE) {
      press_SPACE = 0.25f;
      press_E = 0.0f;
    }
    // reset all navigation (every other key)
    else {
      press_W = 0.0f;
      press_A = 0.0f;
      press_S = 0.0f;
      press_D = 0.0f;
      press_E = 0.0f;
      press_SPACE = 0.0f;
    }

    
    // change direction
    m_view_transform = glm::translate(m_view_transform, glm::fvec3{
      press_A + press_D,
      press_SPACE + press_E,
      press_W + press_S });
    
    updateView();
  }

  if (action == GLFW_PRESS) {
    // press key 1 for realistic shader
    if (key == GLFW_KEY_1) {
      celshading = false;
      std::cout << "Realistic shader activated.\n";
    }
    // press key 2 for cel shading
    else if (key == GLFW_KEY_2) {
      celshading = true;
      std::cout << "Cel shader activated.\n";
    }
  }
}

// handle delta mouse movement input
void ApplicationSolar::mouseCallback(double pos_x, double pos_y) {
  // camera rotation
  m_view_transform = glm::rotate(m_view_transform, float(pos_x)*0.01f, glm::fvec3{0.0f,1.0f,0.0f});
  m_view_transform = glm::rotate(m_view_transform, float(pos_y)*0.01f, glm::fvec3{1.0f,0.0f,0.0f});

  updateView();
}

// load shader programs
void ApplicationSolar::initializeShaderPrograms() {
  // store shader program objects in container
  m_shaders.emplace("skybox", shader_program{m_resource_path + "shaders/skybox.vert",
                                           m_resource_path + "shaders/skybox.frag"}); 

  m_shaders.emplace("planet", shader_program{m_resource_path + "shaders/planet.vert",
                                           m_resource_path + "shaders/planet.frag"});

  m_shaders.emplace("star", shader_program{m_resource_path + "shaders/starlight.vert",
                                           m_resource_path + "shaders/starlight.frag"});

  m_shaders.emplace("orbit", shader_program{m_resource_path + "shaders/orbit.vert",
                                           m_resource_path + "shaders/orbit.frag"});

  m_shaders.emplace("sun", shader_program{m_resource_path + "shaders/sun.vert",
                                           m_resource_path + "shaders/sun.frag"});

  m_shaders.emplace("cel", shader_program{m_resource_path + "shaders/celshading.vert",
                                           m_resource_path + "shaders/celshading.frag"});

  m_shaders.emplace("sun_cel", shader_program{m_resource_path + "shaders/sun_cel.vert",
                                           m_resource_path + "shaders/sun_cel.frag"});

  m_shaders.emplace("screenquad", shader_program{m_resource_path + "shaders/screenquad.vert",
                                           m_resource_path + "shaders/screenquad.frag"});

  // request uniform locations for shader program
  m_shaders.at("skybox").u_locs["ProjectionMatrix"] = -1;
  m_shaders.at("skybox").u_locs["ViewMatrix"] = -1;
  m_shaders.at("skybox").u_locs["TextureSky"] = -1;

  m_shaders.at("planet").u_locs["ModelMatrix"] = -1;
  m_shaders.at("planet").u_locs["ViewMatrix"] = -1;
  m_shaders.at("planet").u_locs["ProjectionMatrix"] = -1;
  m_shaders.at("planet").u_locs["NormalMatrix"] = -1;
  m_shaders.at("planet").u_locs["SunPosition"] = -1;
  m_shaders.at("planet").u_locs["PlanetColor"] = -1;
  m_shaders.at("planet").u_locs["Texture"] = -1;
  m_shaders.at("planet").u_locs["TextureNight"] = -1;
  m_shaders.at("planet").u_locs["NormalMap"] = -1;
  m_shaders.at("planet").u_locs["PlanetNumber"] = -1;

  m_shaders.at("star").u_locs["ViewMatrix"] = -1;
  m_shaders.at("star").u_locs["ProjectionMatrix"] = -1;

  m_shaders.at("orbit").u_locs["OrbitDistance"] = -1;
  m_shaders.at("orbit").u_locs["MoonMatrix"] = -1;
  m_shaders.at("orbit").u_locs["ViewMatrix"] = -1;
  m_shaders.at("orbit").u_locs["ProjectionMatrix"] = -1;

  //m_shaders.at("sun").u_locs["NormalMatrix"] = -1;
  m_shaders.at("sun").u_locs["ModelMatrix"] = -1;
  m_shaders.at("sun").u_locs["ViewMatrix"] = -1;
  m_shaders.at("sun").u_locs["ProjectionMatrix"] = -1;
  m_shaders.at("sun").u_locs["SunPosition"] = -1;
  m_shaders.at("sun").u_locs["Texture"] = -1;

  m_shaders.at("sun_cel").u_locs["ModelMatrix"] = -1;
  m_shaders.at("sun_cel").u_locs["ViewMatrix"] = -1;
  m_shaders.at("sun_cel").u_locs["ProjectionMatrix"] = -1;
  m_shaders.at("sun_cel").u_locs["SunPosition"] = -1;

  //m_shaders.at("cel").u_locs["NormalMatrix"] = -1;
  m_shaders.at("cel").u_locs["ModelMatrix"] = -1;
  m_shaders.at("cel").u_locs["ViewMatrix"] = -1;
  m_shaders.at("cel").u_locs["ProjectionMatrix"] = -1;
  m_shaders.at("cel").u_locs["SunPosition"] = -1;
  m_shaders.at("cel").u_locs["PlanetColor"] = -1;
  //m_shaders.at("cel").u_locs["NormalMatrix"] = -1;
  //m_shaders.at("cel").u_locs["NormalMap"] = -1;
  m_shaders.at("cel").u_locs["PlanetNumber"] = -1;

  m_shaders.at("screenquad").u_locs["blur"] = -1;
  m_shaders.at("screenquad").u_locs["greyscale"] = -1;
  m_shaders.at("screenquad").u_locs["ColorTexture"] = -1;
}

// generate stars
void ApplicationSolar::initializeStars() {

  // generate 5000 random stars
  for (int i = 0; i < 5000; ++i) {
    // position
    float x = rand()%100-50;
    float y = rand()%100-50;
    float z = rand()%100-50;
    // color
    float r = rand()%100*0.01f;
    float g = rand()%100*0.01f;
    float b = rand()%100*0.01f;
    // push to vector
    all_stars.insert(std::end(all_stars),{x,y,z,r,g,b});
  }

  // generate vertex array object
  glGenVertexArrays(1, &star_object.vertex_AO);
  // bind the array for attaching buffers
  glBindVertexArray(star_object.vertex_AO);

  // generate generic buffer
  glGenBuffers(1, &star_object.vertex_BO);
  // bind this as an vertex array buffer containing all attributes
  glBindBuffer(GL_ARRAY_BUFFER, star_object.vertex_BO);
  // configure currently bound array buffer
  glBufferData(GL_ARRAY_BUFFER, GLsizeiptr((sizeof(float) * all_stars.size())), all_stars.data(), GL_STATIC_DRAW);

  // activate first attribute on gpu
  glEnableVertexAttribArray(0);
  // second attribute is 3 floats with no offset & stride
  glVertexAttribPointer(0, // index
  3, // number of components
  GL_FLOAT, // datatype
  GL_FALSE, // normalize?
  6 * sizeof(float), // stride
  0); // offset

  // activate second attribute on gpu
  glEnableVertexAttribArray(1);
  // second attribute is 3 floats with no offset & stride
  glVertexAttribPointer(1, // index
  3, // number of components
  GL_FLOAT, // datatype
  GL_FALSE, // normalize?
  6 * sizeof(float), // stride
  (void*) (3 * sizeof(float))); // offset

  star_object.draw_mode = GL_POINTS;
  star_object.num_elements = GLsizei(all_stars.size() / 6);
}

// load models
void ApplicationSolar::initializeGeometry() {
  // new sphere model by LEON HUTANS:
  // this model fixes the texture bug of wrong orientation
  model planet_model = model_loader::obj(m_resource_path + "models/sphere_new_fromcube.obj", model::NORMAL | model::TEXCOORD | model::TANGENT);

  // generate vertex array object
  glGenVertexArrays(1, &planet_object.vertex_AO);
  // bind the array for attaching buffers
  glBindVertexArray(planet_object.vertex_AO);

  // generate generic buffer
  glGenBuffers(1, &planet_object.vertex_BO);
  // bind this as an vertex array buffer containing all attributes
  glBindBuffer(GL_ARRAY_BUFFER, planet_object.vertex_BO);
  // configure currently bound array buffer
  glBufferData(GL_ARRAY_BUFFER, GLsizeiptr((sizeof(float) * planet_model.data.size())), planet_model.data.data(), GL_STATIC_DRAW);

  // activate first attribute on gpu
  glEnableVertexAttribArray(0);
  // first attribute is 3 floats with no offset & stride
  glVertexAttribPointer(0, model::POSITION.components, model::POSITION.type, GL_FALSE, planet_model.vertex_bytes, planet_model.offsets[model::POSITION]);
  
  // activate second attribute on gpu
  glEnableVertexAttribArray(1);
  // second attribute is 3 floats with no offset & stride
  glVertexAttribPointer(1, model::NORMAL.components, model::NORMAL.type, GL_FALSE, planet_model.vertex_bytes, planet_model.offsets[model::NORMAL]);

  // activate third attribute on gpu
  glEnableVertexAttribArray(2);
  // third attribute is 2 floats with no offset & stride
  glVertexAttribPointer(2, model::TEXCOORD.components, model::TEXCOORD.type, GL_FALSE, planet_model.vertex_bytes, planet_model.offsets[model::TEXCOORD]);

  // activate fourth attribute on gpu
  glEnableVertexAttribArray(3);
  // third attribute is 2 floats with no offset & stride
  glVertexAttribPointer(3, model::TANGENT.components, model::TANGENT.type, GL_FALSE, planet_model.vertex_bytes, planet_model.offsets[model::TANGENT]);

  // generate generic buffer
  glGenBuffers(1, &planet_object.element_BO);
  // bind this as an vertex array buffer containing all attributes
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planet_object.element_BO);
  // configure currently bound array buffer
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, GLsizeiptr((unsigned long)model::INDEX.size * planet_model.indices.size()), planet_model.indices.data(), GL_STATIC_DRAW);

  // store type of primitive to draw
  planet_object.draw_mode = GL_TRIANGLES;
  // transfer number of indices to model object 
  planet_object.num_elements = GLsizei(planet_model.indices.size());
}

void ApplicationSolar::initializeSkybox() {
  // load box model
  model skybox_model = model_loader::obj(m_resource_path + "models/box.obj", model::NORMAL);

  // generate vertex array object
  glGenVertexArrays(1, &skybox_object.vertex_AO);
  // bind the array for attaching buffers
  glBindVertexArray(skybox_object.vertex_AO);

  // generate generic buffer
  glGenBuffers(1, &skybox_object.vertex_BO);
  // bind this as an vertex array buffer containing all attributes
  glBindBuffer(GL_ARRAY_BUFFER, skybox_object.vertex_BO);
  // configure currently bound array buffer
  glBufferData(GL_ARRAY_BUFFER, GLsizeiptr((sizeof(float) * skybox_model.data.size())), skybox_model.data.data(), GL_STATIC_DRAW);

  // activate first attribute on gpu
  glEnableVertexAttribArray(0);
  // first attribute is 3 floats with no offset & stride
  glVertexAttribPointer(0, model::POSITION.components, model::POSITION.type, GL_FALSE, skybox_model.vertex_bytes, skybox_model.offsets[model::POSITION]);

   // generate generic buffer
  glGenBuffers(1, &skybox_object.element_BO);
  // bind this as an vertex array buffer containing all attributes
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skybox_object.element_BO);
  // configure currently bound array buffer
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, GLsizeiptr((unsigned long)model::INDEX.size * skybox_model.indices.size()), skybox_model.indices.data(), GL_STATIC_DRAW);

  // store type of primitive to draw
  skybox_object.draw_mode = GL_TRIANGLES;
  // transfer number of indices to model object 
  skybox_object.num_elements = GLsizei(skybox_model.indices.size());
}

void ApplicationSolar::initializeNormalMaps() {
  pixel_data mars = texture_loader::file(m_resource_path + "textures/" + "2k_mars_normal.png");

  glActiveTexture(GL_TEXTURE2);
  glGenTextures(1, &normaltex_obj.handle);
  glBindTexture(GL_TEXTURE_2D, normaltex_obj.handle);

  // define mandatory sampling parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // pixel transfer
  glTexImage2D(GL_TEXTURE_2D,0,mars.channels,GLsizei(mars.width),
               GLsizei(mars.height),0,mars.channels,mars.channel_type,mars.ptr());
}

void ApplicationSolar::initializeSkyboxTex() {
  pixel_data up = texture_loader::file(m_resource_path + "textures/" + "skybox_up.png");
  pixel_data down = texture_loader::file(m_resource_path + "textures/" + "skybox_down.png");
  pixel_data left = texture_loader::file(m_resource_path + "textures/" + "skybox_left.png");
  pixel_data right = texture_loader::file(m_resource_path + "textures/" + "skybox_right.png");
  pixel_data front = texture_loader::file(m_resource_path + "textures/" + "skybox_front.png");
  pixel_data back = texture_loader::file(m_resource_path + "textures/" + "skybox_back.png");
  std::vector<pixel_data> skyboxTex{up,down,left,right,front,back};

  glActiveTexture(GL_TEXTURE0);
  glGenTextures(1, &all_texture_objects[12].handle);
  glBindTexture(GL_TEXTURE_CUBE_MAP, all_texture_objects[12].handle);
  
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X,0,right.channels,GLsizei(right.width),
                GLsizei(right.height-1),0,right.channels,right.channel_type,right.ptr());  
  glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X,0,left.channels,GLsizei(left.width),
                GLsizei(left.height-1),0,left.channels,left.channel_type,left.ptr());           
  glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y,0,up.channels,GLsizei(up.width),
                GLsizei(up.height-1),0,up.channels,up.channel_type,up.ptr());
  glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,0,down.channels,GLsizei(down.width),
                GLsizei(down.height-1),0,down.channels,down.channel_type,down.ptr()); 
  glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,0,back.channels,GLsizei(back.width),
                GLsizei(back.height-1),0,back.channels,back.channel_type,back.ptr());
  glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z,0,front.channels,GLsizei(front.width),
                GLsizei(front.height-1),0,front.channels,front.channel_type,front.ptr());
}

void ApplicationSolar::initializeTextures(unsigned int num, unsigned int unit_num) {
  // texture all
  pixel_data pixData = texture_loader::file(m_resource_path + "textures/" + all_textures[num]);

  // activate Texture Unit to which to bind texture 
  glActiveTexture(GL_TEXTURE0 + unit_num);
  // generate Texture Object
  glGenTextures(1, &all_texture_objects[num].handle);

  // bind Texture Object to 2d texture binding point of unit
  glBindTexture(GL_TEXTURE_2D, all_texture_objects[num].handle);

  // define mandatory sampling parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // pixel transfer
  glTexImage2D(GL_TEXTURE_2D,0,pixData.channels,GLsizei(pixData.width),
               GLsizei(pixData.height),0,pixData.channels,pixData.channel_type,pixData.ptr());
}

void ApplicationSolar::initializeScreenQuad() {

  screen_edges.insert(std::end(screen_edges), {
      -1.0,-1.0,0.0, 0.0,0.0,
      1.0,-1.0,0.0,  1.0,0.0,
      1.0,1.0,0.0,   1.0,1.0,
      -1.0,1.0,0.0,  0.0,1.0
    });

  model screen_quad_model = model{screen_edges, (model::TEXCOORD | model::POSITION), {1}};

  // generate vertex array object
  glGenVertexArrays(1, &screen_quad_object.vertex_AO);
  // bind the array for attaching buffers
  glBindVertexArray(screen_quad_object.vertex_AO);

  // generate generic buffer
  glGenBuffers(1, &screen_quad_object.vertex_BO);
  // bind this as an vertex array buffer containing all attributes
  glBindBuffer(GL_ARRAY_BUFFER, screen_quad_object.vertex_BO);
  // configure currently bound array buffer
  glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(sizeof(float) * screen_quad_model.data.size()), screen_quad_model.data.data(), GL_STATIC_DRAW);

  // activate first attribute on gpu
  glEnableVertexAttribArray(0);
  // first attribute is 3 floats with no offset & stride
  glVertexAttribPointer(0, // index
  3, // number of components
  GL_FLOAT, // datatype
  GL_FALSE, // normalize?
  5 * sizeof(float), // stride
  0); // offset

  // activate first attribute on gpu
  glEnableVertexAttribArray(1);
  // second attribute is 3 floats with offset & stride
  glVertexAttribPointer(1, // index
  2, // number of components
  GL_FLOAT, // datatype
  GL_FALSE, // normalize?
  5 * sizeof(float), // stride
  (void*) (3 * sizeof(float))); // offset
  
 
  screen_quad_object.draw_mode = GL_TRIANGLE_STRIP;
  screen_quad_object.num_elements = GLsizei(4);
  
}

// generate orbits
void ApplicationSolar::initializeOrbits() {

  // set default circle
  for (int i = 0; i < 360; ++i) {
    float x = 1*cos(i*2*3.1415926f/180);
    float y = 1*sin(i*2*3.1415926f/180);
    float z = 0;
    // color (white)
    //float r = 1;
    //float g = 1;
    //float b = 1;
    all_orbits.insert(std::end(all_orbits), {x,y,z});
  }
  
  //std::cout << "number of floats: " << all_orbits.size() << std::endl;

  // generate vertex array object
  glGenVertexArrays(1, &orbit_object.vertex_AO);
  // bind the array for attaching buffers
  glBindVertexArray(orbit_object.vertex_AO);

  // generate generic buffer
  glGenBuffers(1, &orbit_object.vertex_BO);
  // bind this as an vertex array buffer containing all attributes
  glBindBuffer(GL_ARRAY_BUFFER, orbit_object.vertex_BO);
  // configure currently bound array buffer
  glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(sizeof(float) * all_orbits.size()), all_orbits.data(), GL_STATIC_DRAW);

  // activate first attribute on gpu
  glEnableVertexAttribArray(0);
  // second attribute is 3 floats with no offset & stride
  glVertexAttribPointer(0, // index
  3, // number of components
  GL_FLOAT, // datatype
  GL_FALSE, // normalize?
  3 * sizeof(float), // stride
  0); // offset

  // activate second attribute on gpu
  //glEnableVertexAttribArray(1);
  // second attribute is 3 floats with no offset & stride
  /*glVertexAttribPointer(1, // index
  3, // number of components
  GL_FLOAT, // datatype
  GL_FALSE, // normalize?
  6 * sizeof(float), // stride
  (void*) (3 * sizeof(float))); // offset*/

  orbit_object.draw_mode = GL_LINE_LOOP;
  orbit_object.num_elements = 360;
  //std::cout << "number of orbit_points: " << orbit_object.num_elements << std::endl;
}

void ApplicationSolar::initializeFramebuffer() {
  //glClear(GL_COLOR_BUFFER_BIT);
  //glClear(GL_DEPTH_BUFFER_BIT);

  // ---------------------------- Texture OBJECT ----------------------------
  
  // activate Texture Unit to which to bind texture 
  glActiveTexture(GL_TEXTURE3);
  // generate Texture Object
  glGenTextures(1, &texturebuff.handle);
  // bind Texture Object to 2d texture binding point of unit
  glBindTexture(GL_TEXTURE_2D, texturebuff.handle);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,GLsizei(1000u),GLsizei(750u),0,GL_RGB,GL_UNSIGNED_BYTE,NULL);


  // ---------------------------- Renderbuffer OBJECT ----------------------------
 
  // generate Renderbuffer Object
  glGenRenderbuffers(1, &renderbuff.handle);
  // bind RBO for formatting
  glBindRenderbuffer(GL_RENDERBUFFER, renderbuff.handle);
  // specify RBO properties:
  //  - format, e.g. GL_DEPTH_COMPONENT24
  //  - horizontal resolution
  //  - vertical resolution
  glRenderbufferStorage(GL_RENDERBUFFER,
    GL_DEPTH_COMPONENT24,
    1000u,
    750u);


  // ---------------------------- Framebuffer OBJECT ----------------------------

  // generate Frame Buffer Object
  glGenFramebuffers(1, &framebuff.handle);
  // bind FBO for configuration
  glBindFramebuffer(GL_FRAMEBUFFER, framebuff.handle);
  // specify Texture Object attachments
  //  - to color or depth attachment
  //  - texture handle
  //  - highest level of texture (no mipmap)
  glFramebufferTexture(GL_FRAMEBUFFER,
    GL_COLOR_ATTACHMENT0,
    texturebuff.handle,
    0);
  // specify Renderbuffer Object attachments
  //  - to depth or stencil attachment
  //  - value must be GL_RENDERBUFFER
  //  - handle of RBO
  glFramebufferRenderbuffer(GL_FRAMEBUFFER,
    GL_DEPTH_ATTACHMENT,
    GL_RENDERBUFFER,
    renderbuff.handle);

  // create array containing enums representing color attachments
  GLenum draw_buffers[1] = {GL_COLOR_ATTACHMENT0};
  // set these color attachments to receive fragments
  glDrawBuffers(1, draw_buffers);

  // get the FBO status
  GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  // compare return value with the valid status value
  if (status != GL_FRAMEBUFFER_COMPLETE) {
    std::cout << "ERROR :)" << status << std::endl;
  } else {
    std::cout << status << std::endl;
  }
}

ApplicationSolar::~ApplicationSolar() {
  glDeleteBuffers(1, &planet_object.vertex_BO);
  glDeleteBuffers(1, &planet_object.element_BO);
  glDeleteVertexArrays(1, &planet_object.vertex_AO);

  glDeleteBuffers(1, &star_object.vertex_BO);
  glDeleteVertexArrays(1, &star_object.vertex_AO);

  glDeleteBuffers(1, &orbit_object.vertex_BO);
  glDeleteVertexArrays(1, &orbit_object.vertex_AO);

  glDeleteTextures(0, &all_texture_objects[0].handle);
  glDeleteTextures(1, &all_texture_objects[1].handle);
}

// exe entry point
int main(int argc, char* argv[]) {
  Launcher::run<ApplicationSolar>(argc, argv);
}