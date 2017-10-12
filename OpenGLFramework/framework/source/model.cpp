#include "model.hpp"

#include <glbinding/gl/enum.h>

#include <cstdint>

std::vector<model::attribute> const model::VERTEX_ATTRIBS
 = {  
    /*POSITION*/{ 1 << 0, sizeof(float), 3, GL_FLOAT},
    /*NORMAL*/{   1 << 1, sizeof(float), 3, GL_FLOAT},
    /*TEXCOORD*/{ 1 << 2, sizeof(float), 2, GL_FLOAT},
    /*TANGENT*/{  1 << 3, sizeof(float), 3, GL_FLOAT},
    /*BITANGENT*/{1 << 4, sizeof(float), 3, GL_FLOAT}
 };

model::attribute const& model::POSITION = model::VERTEX_ATTRIBS[0];
model::attribute const& model::NORMAL = model::VERTEX_ATTRIBS[1];
model::attribute const& model::TEXCOORD = model::VERTEX_ATTRIBS[2];
model::attribute const& model::TANGENT = model::VERTEX_ATTRIBS[3];
model::attribute const& model::BITANGENT = model::VERTEX_ATTRIBS[4];
model::attribute const  model::INDEX{1 << 5, sizeof(unsigned),  1, GL_UNSIGNED_INT};

model::model()
 :data{}
 ,indices{}
 ,offsets{}
 ,vertex_bytes{0}
 ,vertex_num{0}
{}

model::model(std::vector<GLfloat> const& databuff, attrib_flag_t contained_attributes, std::vector<GLuint> const& trianglebuff)
 :data(databuff)
 ,indices(trianglebuff)
 ,offsets{}
 ,vertex_bytes{0}
 ,vertex_num{0}
{
  // number of components per vertex
  std::size_t component_num = 0;

  for (auto const& supported_attribute : model::VERTEX_ATTRIBS) {
    // check if buffer contains attribute
    if (supported_attribute.flag & contained_attributes) {
      // write offset, explicit cast to prevent narrowing warning
      offsets.insert(std::pair<attrib_flag_t, GLvoid*>{supported_attribute, (GLvoid*)uintptr_t(vertex_bytes)});
      // move offset pointer forward
      vertex_bytes += supported_attribute.size * supported_attribute.components;
      // increase number of components
      component_num += supported_attribute.components;
    }
  }
  // set number of vertice sin buffer
  vertex_num = data.size() / component_num;
}