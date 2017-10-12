#include "application_fixed.hpp"
#include "launcher.hpp"

#include <glbinding/gl/gl.h>
// use gl definitions from glbinding 
using namespace gl;

//dont load gl bindings from glfw
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

ApplicationFixed::ApplicationFixed(std::string const& resource_path)
 :Application{resource_path}
{}

void ApplicationFixed::render() const {
  glm::fmat4 model_matrix = glm::rotate(glm::fmat4{}, float(glfwGetTime()), glm::fvec3{0.0f, 1.0f, 0.0f});
  model_matrix = glm::translate(glm::fmat4{1.0f}, glm::fvec3{0.0f, 0.0f, -1.0f}) * model_matrix;
  // upload modelview matrix
  glMatrixMode(GL_MODELVIEW);
  glLoadMatrixf(glm::value_ptr(model_matrix));
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

void ApplicationFixed::updateProjection() {
  // upload projection matrix
  glMatrixMode(GL_PROJECTION);
  glLoadMatrixf(glm::value_ptr(m_view_projection));
}

// exe entry point
int main(int argc, char* argv[]) {
  Launcher::run<ApplicationFixed>(argc, argv);
}