#include "application_indexed.hpp"
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

ApplicationIndexed::ApplicationIndexed(std::string const& resource_path)
 :Application{resource_path}
 ,m_vertex_bo{0}
 ,m_index_bo{0}
{
  initializeGeometry();
}

ApplicationIndexed::~ApplicationIndexed() {
  glDeleteBuffers(1, &m_vertex_bo);
  glDeleteBuffers(1, &m_index_bo);
}

void ApplicationIndexed::initializeGeometry() {
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

// generate index buffer
  // index data
  std::vector<std::uint8_t> indices {
    0, 1, 2
  };

  // generate generic buffer
  glGenBuffers(1, &m_index_bo);
  // bind this to current vao as an index buffer
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_bo);
  // configure currently bound array buffer
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(std::uint8_t) * indices.size(), indices.data(), GL_STATIC_DRAW);
}

void ApplicationIndexed::render() const {
  glm::fmat4 model_matrix = glm::rotate(glm::fmat4{}, float(glfwGetTime()), glm::fvec3{0.0f, 1.0f, 0.0f});
  model_matrix = glm::translate(glm::fmat4{1.0f}, glm::fvec3{0.0f, 0.0f, -1.0f}) * model_matrix;
  // upload modelview matrix
  glMatrixMode(GL_MODELVIEW);
  glLoadMatrixf(glm::value_ptr(model_matrix));

  // size of one vertex in bytes, 3 floats position, 3 floats color
  GLsizei stride = GLsizei(sizeof(float) * 6);

 // bind this as vertex data source
  glBindBuffer(GL_ARRAY_BUFFER, m_vertex_bo);
 // bind this as index data source
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_bo);
 
  // activate vertex position data from vbo
  glEnableClientState(GL_COLOR_ARRAY);
  // vertex position have 3 float components, stride and no offset
  glVertexPointer(3, GL_FLOAT, stride, (GLvoid*)0);

  // activate vertex color data from vbo
  glEnableClientState(GL_VERTEX_ARRAY);             
  // colors have 3 float components, stride and lie behind position vector 
  glColorPointer(3, GL_FLOAT, stride, (GLvoid*)(sizeof(float) * 3));

  // draw triangle from indices
  glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, 0);        
}

void ApplicationIndexed::updateProjection() {
  // upload projection matrix
  glMatrixMode(GL_PROJECTION);
  glLoadMatrixf(glm::value_ptr(m_view_projection));
}

// exe entry point
int main(int argc, char* argv[]) {
  Launcher::run<ApplicationIndexed>(argc, argv);
}