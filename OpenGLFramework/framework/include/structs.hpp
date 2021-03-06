#ifndef STRUCTS_HPP
#define STRUCTS_HPP

#include <map>
#include <glbinding/gl/gl.h>
#include <vector>
#include <glm/gtc/type_ptr.hpp>
// use gl definitions from glbinding 
using namespace gl;

// gpu representation of model
struct model_object {
  // vertex array object
  GLuint vertex_AO = 0;
  // vertex buffer object
  GLuint vertex_BO = 0;
  // index buffer object
  GLuint element_BO = 0;
  // primitive type to draw
  GLenum draw_mode = GL_NONE;
  // indices number, if EBO exists
  GLsizei num_elements = 0;
};

// gpu representation of texture
struct texture_object {
  // handle of texture object
  GLuint handle = 0;
  // binding point
  GLenum target = GL_NONE;
};

// shader handle and uniform storage
struct shader_program {
  shader_program(std::string const& vertex, std::string const& fragment)
   :vertex_path{vertex}
   ,fragment_path{fragment}
   ,handle{0}
   {}

  // path to shader source
  std::string vertex_path; 
  std::string fragment_path; 
  // object handle
  GLuint handle;
  // uniform locations mapped to name
  std::map<std::string, GLint> u_locs{};
};

struct planet {
  planet():
  m_name{"default"},
  m_size{1.0f},
  m_rot_speed{1.0f},
  m_origin_dis{1.0f},
  m_intr_rot_speed{0.5f},
  m_color{0.0f,1.0f,0.0f}
  {}

  planet(std::string name, float size, float speed, float distance, glm::fvec3 color):
  m_name{name},
  m_size{size},
  m_rot_speed{speed},
  m_origin_dis{distance},
  m_intr_rot_speed{0.5f},
  m_color{color}
  {}

  planet(std::string name, float size, float speed, float distance, float selfrot, glm::fvec3 color):
  m_name{name},
  m_size{size},
  m_rot_speed{speed},
  m_origin_dis{distance},
  m_intr_rot_speed{selfrot},
  m_color{color}
  {}

  // planet name
  std::string m_name;
  // size
  float m_size;
  // rotation speed
  float m_rot_speed;
  // distance to origin
  float m_origin_dis;
  // intrinsic rotation speed
  float m_intr_rot_speed;
  // planet color
  glm::fvec3 m_color;
};

#endif