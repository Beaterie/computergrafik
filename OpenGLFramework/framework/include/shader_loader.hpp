#ifndef SHADER_LOADER_HPP
#define SHADER_LOADER_HPP

#include <glbinding/gl/enum.h>
using namespace gl;

#include <string>

namespace shader_loader {
  // compile shader
  unsigned shader(std::string const& file_path, GLenum shader_type);
  // create program from vertex and fragment shader
  unsigned program(std::string const& vertex_name, std::string const& fragment_name);
  // create program from vertex, geometry and fragment shader
  unsigned program(std::string const& vertex_path, std::string const& geometry_path, std::string const& fragment_path);
};

#endif
