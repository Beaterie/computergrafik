#include "application_uniform.hpp"
#include "launcher.hpp"

#include "utils.hpp"

#include <glbinding/gl/gl.h>
// use gl definitions from glbinding 
using namespace gl;

//dont load gl bindings from glfw
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

ApplicationUniform::ApplicationUniform(std::string const& resource_path)
 :Application{resource_path}
 ,m_ul_model_view{-1}
 ,m_ul_projection{-1}
{
  initializeShaderPrograms();
}

// load shader programs
void ApplicationUniform::initializeShaderPrograms() {
  // store shader program objects in container
  m_shaders.emplace("uniform", shader_program{m_resource_path + "shaders/uniform.vert",
                                              m_resource_path + "shaders/emulation.frag"});
}

void ApplicationUniform::render() const {
  glm::fmat4 model_matrix = glm::rotate(glm::fmat4{}, float(glfwGetTime()), glm::fvec3{0.0f, 1.0f, 0.0f});
  model_matrix = glm::translate(glm::fmat4{1.0f}, glm::fvec3{0.0f, 0.0f, -1.0f}) * model_matrix;
  // upload modelview matrix
  glUniformMatrix4fv(m_ul_model_view, 1, GL_FALSE, glm::value_ptr(model_matrix));

  // draw triangle
  glBegin(GL_TRIANGLES);
    // first vertex
    glVertex3f(0.5, -0.5, 0.0);
    glColor3f(1.0, 0.0, 0.0);
    // second vertex
    glVertex3f(-0.5, -0.5, 0.0);
    glColor3f(0.0, 1.0, 0.0);
    // third vertes
    glVertex3f(0.0, 0.5, 0.0);
    glColor3f(0.0, 0.0, 1.0);
  glEnd();
}

void ApplicationUniform::updateProjection() {
  // upload matrix to gpu
  glUniformMatrix4fv(m_ul_projection, 1, GL_FALSE, glm::value_ptr(m_view_projection));
}

// callback after shader reloading
void ApplicationUniform::uploadUniforms() {
  // bind new shader
  glUseProgram(m_shaders.at("uniform").handle);
   // load matrix uniform locations
  m_ul_model_view = glGetUniformLocation(m_shaders.at("uniform").handle, "ModelViewMatrix");
  m_ul_projection = glGetUniformLocation(m_shaders.at("uniform").handle, "ProjectionMatrix");
  // reupload projection
  updateProjection();
}

// exe entry point
int main(int argc, char* argv[]) {
  Launcher::run<ApplicationUniform>(argc, argv);
}