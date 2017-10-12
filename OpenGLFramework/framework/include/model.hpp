#ifndef MODEL_HPP
#define MODEL_HPP

#include <glbinding/gl/types.h>

#include <map>
#include <vector>
// use gl definitions from glbinding 
using namespace gl;

// holds vertex information and triangle indices
struct model {

  //flag type to combine attributes
  typedef int attrib_flag_t;
  // type holding info about a vertex/model attribute
  struct attribute {

    attribute(attrib_flag_t f, GLsizei s, GLsizei c, GLenum t)
     :flag{f}
     ,size{s}
     ,components{c}
     ,type{t}
    {}

    // conversion to flag type for use as enum
    operator attrib_flag_t const&() const{
      return flag;
    }

    // ugly enum to use as flag, must be unique power of two
    attrib_flag_t flag;
    // size in bytes
    GLsizei size;
    // number of scalar components
    GLint components;
    // Gl type
    GLenum type;
    // offset from element beginning
    GLvoid* offset;
  };

  // holds all possible vertex attributes, for iteration
  static std::vector<attribute> const VERTEX_ATTRIBS;
  // symbolic values to access valuesin vector by name
  static attribute const& POSITION;
  static attribute const& NORMAL;
  static attribute const& TEXCOORD;
  static attribute const& TANGENT;
  static attribute const& BITANGENT;
  // is not a vertex attribute, so not stored in VERTEX_ATTRIBS
  static attribute const  INDEX;
  
  model();
  model(std::vector<GLfloat> const& databuff, attrib_flag_t attribs, std::vector<GLuint> const& trianglebuff = std::vector<GLuint>{});

  std::vector<GLfloat> data;
  std::vector<GLuint> indices;
  // byte offsets of individual element attributes
  std::map<attrib_flag_t, GLvoid*> offsets;
  // size of one vertex element in bytes
  GLsizei vertex_bytes;
  std::size_t vertex_num;
};

#endif