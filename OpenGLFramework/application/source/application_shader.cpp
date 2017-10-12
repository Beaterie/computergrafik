#include "application_shader.hpp"
#include "launcher.hpp"

#include "utils.hpp"
#include "shader_loader.hpp"
#include "model_loader.hpp"

#include <glbinding/gl/gl.h>
// use gl definitions from glbinding 
using namespace gl;

//dont load gl bindings from glfw
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

ApplicationShader::ApplicationShader(std::string const& resource_path)
 :Application{resource_path}
 ,m_program{0}
{
  initializeShaderPrograms();
}

GLuint createShader(std::string const& file_path, GLenum shader_type) {
  // allocate shader
  GLuint shader = glCreateShader(shader_type);

  std::string shader_source{utils::read_file(file_path)};
  // glshadersource expects array of c-strings
  const char* shader_chars = shader_source.c_str();
  // define source
  glShaderSource(shader, 1, &shader_chars, 0);
  // compile source
  glCompileShader(shader);

  return shader;
}

ApplicationShader::~ApplicationShader() {
  // free program
  glDeleteProgram(m_program);
}

// load shader programs
void ApplicationShader::initializeShaderPrograms() {
  // allocate program
  m_program = glCreateProgram();

  // load and compile vert and frag shader
  GLuint vertex_shader = createShader(m_resource_path + "/shaders/emulation.vert", GL_VERTEX_SHADER);
  GLuint fragment_shader = createShader(m_resource_path + "/shaders/emulation.frag", GL_FRAGMENT_SHADER);

  // attach the shaders to the program
  glAttachShader(m_program, vertex_shader);
  glAttachShader(m_program, fragment_shader);

  // link shaders
  glLinkProgram(m_program);

  // detach shaders
  glDetachShader(m_program, vertex_shader);
  glDetachShader(m_program, fragment_shader);
  // and free them
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);

  // bind program
  glUseProgram(m_program);
}

void ApplicationShader::render() const {
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

void ApplicationShader::updateProjection() {
  // upload projection matrix
  glMatrixMode(GL_PROJECTION);
  glLoadMatrixf(glm::value_ptr(m_view_projection));
}

// exe entry point
int main(int argc, char* argv[]) {
  Launcher::run<ApplicationShader>(argc, argv);
}