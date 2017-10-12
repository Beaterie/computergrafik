#include "application_vao.hpp"
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

#include <vector>

ApplicationVao::ApplicationVao(std::string const& resource_path)
 :Application{resource_path}
 ,m_vertex_ao{0}
 ,m_vertex_bo{0}
 ,m_index_bo{0}
{
  initializeShaderPrograms();
  initializeGeometry();
}
ApplicationVao::~ApplicationVao() {
  glDeleteBuffers(1, &m_vertex_bo);
  glDeleteBuffers(1, &m_index_bo);
  glDeleteVertexArrays(1, &m_vertex_ao);
}

// load shader programs
void ApplicationVao::initializeShaderPrograms() {
  // store shader program objects in container
  m_shaders.emplace("vao", shader_program{m_resource_path + "shaders/vao.vert",
                                          m_resource_path + "shaders/emulation.frag"});

  // request uniform locations for shader program
  m_shaders.at("vao").u_locs["ModelViewMatrix"] = -1;
  m_shaders.at("vao").u_locs["ProjectionMatrix"] = -1;
}

void ApplicationVao::initializeGeometry() {
  // interleaved vertex data
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
  // size of one vertex in bytes, 3 floats position, 3 floats color
  GLsizei stride = GLsizei(sizeof(float) * 6);

// generate vertex buffer
  // generate generic buffer
  glGenBuffers(1, &m_vertex_bo);
  // bind this as an vertex array buffer to vao
  glBindBuffer(GL_ARRAY_BUFFER, m_vertex_bo);
  // configure currently bound array buffer
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

// generate vertex array object and bind attributes
  // generate vertex array object
  glGenVertexArrays(1, &m_vertex_ao);
  // bind the array for attaching buffers
  glBindVertexArray(m_vertex_ao);

  // activate first attribute on gpu
  glEnableVertexAttribArray(0);
  // first attribute is 3 floats with no offset & stride
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
  // activate second attribute on gpu
  glEnableVertexAttribArray(1);
  // second attribute is 3 floats with no offset & stride
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)uintptr_t(sizeof(float) * 3));

// generate index buffer and bind to vao
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

void ApplicationVao::render() const {
  glm::fmat4 model_matrix = glm::rotate(glm::fmat4{}, float(glfwGetTime()), glm::fvec3{0.0f, 1.0f, 0.0f});
  model_matrix = glm::translate(glm::fmat4{1.0f}, glm::fvec3{0.0f, 0.0f, -1.0f}) * model_matrix;
  // upload modelview matrix
  glUniformMatrix4fv(m_shaders.at("vao").u_locs.at("ModelViewMatrix"),
                     1, GL_FALSE, glm::value_ptr(model_matrix));

// draw triangle
  // bind the VAO to draw
  glBindVertexArray(m_vertex_ao);
  // draw indexed bound vertex array using bound shader
  glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, NULL);
}

void ApplicationVao::updateProjection() {
  // upload matrix to gpu
  glUniformMatrix4fv(m_shaders.at("vao").u_locs.at("ProjectionMatrix"),
                     1, GL_FALSE, glm::value_ptr(m_view_projection));
}

// callback after shader reloading
void ApplicationVao::uploadUniforms() {
  updateUniformLocations();
  // bind new shader
  glUseProgram(m_shaders.at("vao").handle);
  // reupload projection
  updateProjection();
}

// exe entry point
int main(int argc, char* argv[]) {
  Launcher::run<ApplicationVao>(argc, argv);
}