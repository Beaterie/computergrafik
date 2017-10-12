#include "shader_loader.hpp"
#include "utils.hpp"

#include <glbinding/gl/functions.h>
// use gl definitions from glbinding 
using namespace gl;

namespace shader_loader {

GLuint shader(std::string const& file_path, GLenum shader_type) {
  GLuint shader = 0;
  shader = glCreateShader(shader_type);

  std::string shader_source{utils::read_file(file_path)};
  // glshadersource expects array of c-strings
  const char* shader_chars = shader_source.c_str();
  glShaderSource(shader, 1, &shader_chars, 0);

  glCompileShader(shader);

  // check if compilation was successfull
  GLint success = 0;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if(success == 0) {
    // get log length
    GLint log_size = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_size);
    // get log
    GLchar* log_buffer = (GLchar*)malloc(sizeof(GLchar) * log_size);
    glGetShaderInfoLog(shader, log_size, &log_size, log_buffer);
    // output errors
    utils::output_log(log_buffer, utils::file_name(file_path));
    // free broken shader
    glDeleteShader(shader);
    free(log_buffer);

    throw std::logic_error("Compilation of " + file_path);
  }

  return shader;
}

GLuint program(std::string const& vertex_path, std::string const& fragment_path) {
  GLuint program = glCreateProgram();

  // load and compile vert and frag shader
  GLuint vertex_shader = shader(vertex_path, GL_VERTEX_SHADER);
  GLuint fragment_shader = shader(fragment_path, GL_FRAGMENT_SHADER);

  // attach the shaders to the program
  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  // link shaders
  glLinkProgram(program);

  // check if linking was successfull
  GLint success = 0;
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if(success == 0) {
    // get log length
    GLint log_size = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_size);
    // get log
    GLchar* log_buffer = (GLchar*)malloc(sizeof(GLchar) * log_size);
    glGetProgramInfoLog(program, log_size, &log_size, log_buffer);
    // output errors
    utils::output_log(log_buffer, utils::file_name(vertex_path) + " & " + utils::file_name(fragment_path));
    // free broken program
    glDeleteProgram(program);
    free(log_buffer);

    throw std::logic_error("Linking of " + vertex_path + " & " + fragment_path);
  }
  // detach shaders
  glDetachShader(program, vertex_shader);
  glDetachShader(program, fragment_shader);
  // and free them
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);

  return program;
}

GLuint program(std::string const& vertex_path, std::string const& geometry_path, std::string const& fragment_path) {
  GLuint program = glCreateProgram();

  // load and compile vert and frag shader
  GLuint vertex_shader = shader(vertex_path, GL_VERTEX_SHADER);
  GLuint geometry_shader = shader(geometry_path, GL_GEOMETRY_SHADER);
  GLuint fragment_shader = shader(fragment_path, GL_FRAGMENT_SHADER);

  // attach the shaders to the program
  glAttachShader(program, vertex_shader);
  glAttachShader(program, geometry_shader);
  glAttachShader(program, fragment_shader);
  // link shaders
  glLinkProgram(program);

  // check if linking was successfull
  GLint success = 0;
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if(success == 0) {
    // get log length
    GLint log_size = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_size);
    // get log
    GLchar* log_buffer = (GLchar*)malloc(sizeof(GLchar) * log_size);
    glGetProgramInfoLog(program, log_size, &log_size, log_buffer);
    // output errors
    utils::output_log(log_buffer, utils::file_name(vertex_path) + " & " + utils::file_name(geometry_path) + " & " + utils::file_name(fragment_path));
    // free broken program
    glDeleteProgram(program);
    free(log_buffer);

    throw std::logic_error("Linking of " + vertex_path + " & " + geometry_path + " & " + fragment_path);
  }
  // detach shaders
  glDetachShader(program, vertex_shader);
  glDetachShader(program, geometry_shader);
  glDetachShader(program, fragment_shader);
  // and free them
  glDeleteShader(vertex_shader);
  glDeleteShader(geometry_shader);
  glDeleteShader(fragment_shader);

  return program;
}

};