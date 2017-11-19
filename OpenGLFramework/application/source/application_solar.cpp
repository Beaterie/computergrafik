#include "application_solar.hpp"
#include "launcher.hpp"

#include "utils.hpp"
#include "shader_loader.hpp"
#include "model_loader.hpp"

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
 :Application{resource_path}
 ,planet_object{}
 {
  initializeGeometry();
  initializeStars();

  // planets
  // {size, speed_of_rotation, distance_to_origin, [moon, instrinct rotation,] color}
  planet sonne{1.5f, 0.0f, 0.0f, {1.0f,0.8f,0.0f}};
  planet merkur{log(4.9f)*0.1f, 87.0f*0.00005f, log(5.8f)*2.5f, {0.4f,0.2f,1.0f}};
  planet venus{log(12.0f)*0.1f, 200.0f*0.00005f, log(10.8f)*2.5f, {0.9f,0.2f,1.0f}};
  planet erde{log(13.0f)*0.1f, 365.0f*0.00005f, log(15.0f)*2.5f, {0.05f,0.1f,1.0f}};
  planet mars{log(6.8f)*0.1f, 700.0f*0.00005f, log(22.8f)*2.5f, {0.8f,0.1f,0.0f}};
  planet jupiter{log(142.0f)*0.1f, 450.0f*0.00005f, log(77.8f)*2.5f, {0.8f,0.0f,0.0f}};
  planet saturn{log(120.0f)*0.1f, 50.0f*0.00005f, log(143.0f)*2.5f, {0.5f,1.0f,0.0f}};
  planet uranus{log(51.0f)*0.1f, 140.0f*0.00005f, log(358.0f)*2.5f, {0.0f,0.3f,0.3f}};
  planet neptun{log(48.0f)*0.1f, 300.0f*0.00005f, log(800.0f)*2.5f, {0.0f,0.1f,0.8f}};
  planet pluto{log(2.3f)*0.1f, 160.0f*0.00005f, log(2000.0f)*2.5f, {1.0f,1.0f,0.0f}};
  planet mond{log(3.4f)*0.1f, 365.0f*0.00005f, log(15.0f)*2.5f, true, {0.5f,0.5f,0.5f}};
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

  initializeOrbits();
  initializeShaderPrograms();
}

// upload planets
void ApplicationSolar::upload_planet_transforms(std::shared_ptr<planet> planet) const {

  // main rotation
  glm::fmat4 model_matrix = glm::rotate(glm::fmat4{}, float(glfwGetTime())*planet->m_rot_speed, glm::fvec3{0.0f, 1.0f, 0.0f});
  // main distance (distance to origin)
  model_matrix = glm::translate(model_matrix, glm::fvec3{0.0f, 0.0f, planet->m_origin_dis});
  // intrinsic rotation
  model_matrix = glm::rotate(model_matrix, float(glfwGetTime())*planet->m_intr_rot_speed, glm::fvec3{0.0f, 1.0f, 0.0f});
  
  // if moon, rotate around earth
  if (planet->m_moon == true) {
    // moon rotation
    model_matrix = glm::rotate(model_matrix, float(glfwGetTime()), glm::fvec3{0.0f, 1.0f, 0.0f});
    // moon distance (distance to earth)
    model_matrix = glm::translate(model_matrix, glm::fvec3{0.0f, 0.0f, 0.5f});
  }

  // scale the planet
  model_matrix = glm::scale(model_matrix, glm::fvec3{planet->m_size,planet->m_size,planet->m_size});

  // color
  glUniform3fv(m_shaders.at("planet").u_locs.at("PlanetColor"), 1,
    glm::value_ptr(planet->m_color));

  // shader it
  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ModelMatrix"),
                     1, GL_FALSE, glm::value_ptr(model_matrix));

  // extra matrix for normal transformation to keep them orthogonal to surface
  glm::fmat4 normal_matrix = glm::inverseTranspose(glm::inverse(m_view_transform) * model_matrix);
  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("NormalMatrix"),
                     1, GL_FALSE, glm::value_ptr(normal_matrix));

  // bind the VAO to draw
  glBindVertexArray(planet_object.vertex_AO);

  // draw bound vertex array using bound shader
  glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);
};

void ApplicationSolar::upload_orbits(std::shared_ptr<planet> planet) const {

  // shader it
  glm::fmat4 moon_matrix{};
  if (planet->m_moon) {
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
  
  // bind shader to upload uniforms
  glUseProgram(m_shaders.at("planet").handle);
  
  // load planets
  for (unsigned int i = 0; i < all_planets.size(); ++i) {
    upload_planet_transforms(all_planets[i]);
  }

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

  std::cout << "updating..\n";

  // upload matrix to gpu
  glUseProgram(m_shaders.at("planet").handle);
  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ViewMatrix"),
                      1, GL_FALSE, glm::value_ptr(view_matrix));

  // sun position
  glUniform3fv(m_shaders.at("planet").u_locs.at("SunPosition"), 1,
    glm::value_ptr(glm::fvec3{view_matrix * glm::fvec4{0.0f,0.0f,0.0f,1.0f}}));

  // upload matrix to gpu
  glUseProgram(m_shaders.at("star").handle);
  glUniformMatrix4fv(m_shaders.at("star").u_locs.at("ViewMatrix"),
                      1, GL_FALSE, glm::value_ptr(view_matrix));

  // upload matrix to gpu
  glUseProgram(m_shaders.at("orbit").handle);
  glUniformMatrix4fv(m_shaders.at("orbit").u_locs.at("ViewMatrix"),
                      1, GL_FALSE, glm::value_ptr(view_matrix));
}

void ApplicationSolar::updateProjection() {
  // upload matrix to gpu
  glUseProgram(m_shaders.at("planet").handle);
  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ProjectionMatrix"),
                     1, GL_FALSE, glm::value_ptr(m_view_projection));

  // upload matrix to gpu
  glUseProgram(m_shaders.at("star").handle);
  glUniformMatrix4fv(m_shaders.at("star").u_locs.at("ProjectionMatrix"),
                     1, GL_FALSE, glm::value_ptr(m_view_projection));

  // upload matrix to gpu
  glUseProgram(m_shaders.at("orbit").handle);
  glUniformMatrix4fv(m_shaders.at("orbit").u_locs.at("ProjectionMatrix"),
                     1, GL_FALSE, glm::value_ptr(m_view_projection));
}

// update uniform locations
void ApplicationSolar::uploadUniforms() {
  updateUniformLocations();
  
  // bind new shader
  // glUseProgram(m_shaders.at("planet").handle);
  
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

    // testing
    //std::cout << press_A << " " << press_D << " " << 
    //  press_SPACE << " " << press_E << " " << 
    //  press_W << " " << press_S << "\n";
    // std::cout << "Key: " << key;
    // std::cout << "\nScancode: " << scancode;
    // std::cout << "\nAction: " << action;
    // std::cout << "\nMods: " << mods << "\n";
    
    updateView();
  }
}

// handle delta mouse movement input
void ApplicationSolar::mouseCallback(double pos_x, double pos_y) {

 // std::cout << "Mouse values     : " << pos_x << "\t" << pos_y << std::endl;

  // camera rotation
  m_view_transform = glm::rotate(m_view_transform, float(pos_x)*0.01f, glm::fvec3{0.0f,1.0f,0.0f});
  m_view_transform = glm::rotate(m_view_transform, float(pos_y)*0.01f, glm::fvec3{1.0f,0.0f,0.0f});

  updateView();
}

// load shader programs
void ApplicationSolar::initializeShaderPrograms() {
  // store shader program objects in container
  m_shaders.emplace("planet", shader_program{m_resource_path + "shaders/simple.vert",
                                           m_resource_path + "shaders/simple.frag"});
  m_shaders.emplace("star", shader_program{m_resource_path + "shaders/starlight.vert",
                                           m_resource_path + "shaders/starlight.frag"});
  m_shaders.emplace("orbit", shader_program{m_resource_path + "shaders/orbit.vert",
                                           m_resource_path + "shaders/orbit.frag"});
  // request uniform locations for shader program
  m_shaders.at("planet").u_locs["NormalMatrix"] = -1;
  m_shaders.at("planet").u_locs["ModelMatrix"] = -1;
  m_shaders.at("planet").u_locs["ViewMatrix"] = -1;
  m_shaders.at("planet").u_locs["ProjectionMatrix"] = -1;
  m_shaders.at("planet").u_locs["SunPosition"] = -1;
  m_shaders.at("planet").u_locs["PlanetColor"] = -1;
  m_shaders.at("star").u_locs["ViewMatrix"] = -1;
  m_shaders.at("star").u_locs["ProjectionMatrix"] = -1;
  m_shaders.at("orbit").u_locs["OrbitDistance"] = -1;
  m_shaders.at("orbit").u_locs["MoonMatrix"] = -1;
  m_shaders.at("orbit").u_locs["ViewMatrix"] = -1;
  m_shaders.at("orbit").u_locs["ProjectionMatrix"] = -1;
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
  model planet_model = model_loader::obj(m_resource_path + "models/sphere.obj", model::NORMAL);

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
  
  std::cout << "number of floats: " << all_orbits.size() << std::endl;

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
  std::cout << "number of orbit_points: " << orbit_object.num_elements << std::endl;
}

ApplicationSolar::~ApplicationSolar() {
  glDeleteBuffers(1, &planet_object.vertex_BO);
  glDeleteBuffers(1, &planet_object.element_BO);
  glDeleteVertexArrays(1, &planet_object.vertex_AO);

  glDeleteBuffers(1, &star_object.vertex_BO);
  glDeleteVertexArrays(1, &star_object.vertex_AO);

  glDeleteBuffers(1, &orbit_object.vertex_BO);
  glDeleteVertexArrays(1, &orbit_object.vertex_AO);
}

// exe entry point
int main(int argc, char* argv[]) {
  Launcher::run<ApplicationSolar>(argc, argv);
}