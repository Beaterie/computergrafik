#include "application_vbo.hpp"
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

#include <vector>

ApplicationVbo::ApplicationVbo(std::string const& resource_path)
 :Application{resource_path}
 ,m_vertex_bo{0}
{
  initializeGeometry();
}

ApplicationVbo::~ApplicationVbo() {
  glDeleteBuffers(1, &m_vertex_bo);
}

void ApplicationVbo::initializeGeometry() {
  std::vector<float> vertices{
    // first vertex
    0.5, -0.5, 0.0,
    1.0, 0.0, 0.0,
    // second vertex
    -0.5, -0.5, 0.0,
    0.0, 1.0, 0.0,
    // third vertes
    0.0, 0.5, 0.0,
    0.0, 0.0, 1.0,
  };

// generate vertex buffer
  // generate generic buffer
  glGenBuffers(1, &m_vertex_bo);
  // bind this as an vertex array buffer
  glBindBuffer(GL_ARRAY_BUFFER, m_vertex_bo);
  // configure currently bound array buffer
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
}

void ApplicationVbo::render() const {
  glm::fmat4 model_matrix = glm::rotate(glm::fmat4{}, float(glfwGetTime()), glm::fvec3{0.0f, 1.0f, 0.0f});
  model_matrix = glm::translate(glm::fmat4{1.0f}, glm::fvec3{0.0f, 0.0f, -1.0f}) * model_matrix;
  // upload modelview matrix
  glMatrixMode(GL_MODELVIEW);
  glLoadMatrixf(glm::value_ptr(model_matrix));

  // size of one vertex in bytes, 3 floats position, 3 floats color
  GLsizei stride = GLsizei(sizeof(float) * 6);

 // bind this as vertex data source
  glBindBuffer(GL_ARRAY_BUFFER, m_vertex_bo);
 
  // activate vertex position data from vbo
  glEnableClientState(GL_VERTEX_ARRAY);             
  // vertex position have 3 float components, stride and no offset
  glVertexPointer(3, GL_FLOAT, stride, (GLvoid*)0);

  // activate vertex color data from vbo
  glEnableClientState(GL_COLOR_ARRAY);
  // colors have 3 float components, stride and lie behind position vector 
  glColorPointer(3, GL_FLOAT, stride, (GLvoid*)(sizeof(float) * 3));

  // draw triangle
  glDrawArrays(GL_TRIANGLES, 0, 3);        
}

void ApplicationVbo::updateProjection() {
  // upload projection matrix
  glMatrixMode(GL_PROJECTION);
  glLoadMatrixf(glm::value_ptr(m_view_projection));
}

// exe entry point
int main(int argc, char* argv[]) {
  Launcher::run<ApplicationVbo>(argc, argv);
}