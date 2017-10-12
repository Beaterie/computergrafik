#include "model_loader.hpp"

// use floats and med precision operations
#include <glm/gtc/type_precision.hpp>
#include <glm/geometric.hpp>

#include <iostream>

namespace model_loader {

void generate_normals(tinyobj::mesh_t& model);

std::vector<glm::fvec3> generate_tangents(tinyobj::mesh_t const& model);

model obj(std::string const& name, model::attrib_flag_t import_attribs){
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;

  std::string err = tinyobj::LoadObj(shapes, materials, name.c_str());

  if (!err.empty()) {
    if (err[0] == 'W' && err[1] == 'A' && err[2] == 'R') {
      std::cerr << "tinyobjloader: " << err << std::endl;    
    }
    else {
      throw std::logic_error("tinyobjloader: " + err);    
    }
  }

  model::attrib_flag_t attributes{model::POSITION | import_attribs};

  std::vector<float> vertex_data;
  std::vector<unsigned> triangles;

  unsigned vertex_offset = 0;

  for (auto& shape : shapes) {
    tinyobj::mesh_t& curr_mesh = shape.mesh;
    // prevent MSVC warning due to Win BOOL implementation
    bool has_normals = (import_attribs & model::NORMAL) != 0;
    if(has_normals) {
      // generate normals if necessary
      if (curr_mesh.normals.empty()) {
        generate_normals(curr_mesh);
      }
    }

    bool has_uvs = (import_attribs & model::TEXCOORD) != 0;
    if(has_uvs) {
      if (curr_mesh.texcoords.empty()) {
        has_uvs = false;
        attributes ^= model::TEXCOORD;
        std::cerr << "Shape has no texcoords" << std::endl;
      }
    }

    bool has_tangents = import_attribs & model::TANGENT;
    std::vector<glm::fvec3> tangents;
    if (has_tangents) {
      if (!has_uvs) {
        has_tangents = false;
        attributes ^= model::TANGENT;
        std::cerr << "Shape has no texcoords" << std::endl;
      }
      else {
        tangents = generate_tangents(curr_mesh);
      }
    }

    // push back vertex attributes
    for (unsigned i = 0; i < curr_mesh.positions.size() / 3; ++i) {
      vertex_data.push_back(curr_mesh.positions[i * 3]);
      vertex_data.push_back(curr_mesh.positions[i * 3 + 1]);
      vertex_data.push_back(curr_mesh.positions[i * 3 + 2]);

      if (has_normals) {
        vertex_data.push_back(curr_mesh.normals[i * 3]);
        vertex_data.push_back(curr_mesh.normals[i * 3 + 1]);
        vertex_data.push_back(curr_mesh.normals[i * 3 + 2]);
      }

      if (has_uvs) {
        vertex_data.push_back(curr_mesh.texcoords[i * 2]);
        vertex_data.push_back(curr_mesh.texcoords[i * 2 + 1]);
      }

      if (has_tangents) {
        vertex_data.push_back(tangents[i].x);
        vertex_data.push_back(tangents[i].y);
        vertex_data.push_back(tangents[i].z);
      }
    }

    // add triangles
    for (unsigned i = 0; i < curr_mesh.indices.size(); ++i) {
      triangles.push_back(vertex_offset + curr_mesh.indices[i]);
    }

    vertex_offset += unsigned(curr_mesh.positions.size() / 3);
  }

  return model{vertex_data, attributes, triangles};
}

void generate_normals(tinyobj::mesh_t& model) {
  std::vector<glm::fvec3> positions(model.positions.size() / 3);

  for (unsigned i = 0; i < model.positions.size(); i+=3) {
    positions[i / 3] = glm::fvec3{model.positions[i], model.positions[i + 1], model.positions[i + 2]};
  }

  std::vector<glm::fvec3> normals(model.positions.size() / 3, glm::fvec3{0.0f});
  for (unsigned i = 0; i < model.indices.size(); i+=3) {
    glm::fvec3 normal = glm::cross(positions[model.indices[i+1]] - positions[model.indices[i]], positions[model.indices[i+2]] - positions[model.indices[i]]);

    normals[model.indices[i]] += normal;
    normals[model.indices[i+1]] += normal;
    normals[model.indices[i+2]] += normal;
  }

  model.normals.reserve(model.positions.size());
  for (unsigned i = 0; i < normals.size(); ++i) {
    glm::fvec3 normal = glm::normalize(normals[i]);
    model.normals[i * 3] = normal[0];
    model.normals[i * 3 + 1] = normal[1];
    model.normals[i * 3 + 2] = normal[2];
  }
}

std::vector<glm::fvec3> generate_tangents(tinyobj::mesh_t const& model) {
  // containers for vertex attributes
  std::vector<glm::fvec3> positions(model.positions.size() / 3);
  std::vector<glm::fvec3> normals(model.positions.size() / 3);
  std::vector<glm::fvec2> texcoords(model.positions.size() / 3);
  std::vector<glm::fvec3> tangents(model.positions.size() / 3, glm::fvec3{0.0f});

  // get vertex positions and texture coordinates from mesh_t
  for (unsigned i = 0; i < model.positions.size(); i+=3) {
    positions[i / 3] = glm::fvec3{model.positions[i],
                                 model.positions[i + 1],
                                 model.positions[i + 2]};
    normals[i / 3] = glm::fvec3{model.normals[i],
                                 model.normals[i + 1],
                                 model.normals[i + 2]};
  }
  for (unsigned i = 0; i < model.texcoords.size(); i+=2) {
    texcoords[i / 2] = glm::fvec2{model.texcoords[i], model.texcoords[i + 1]};
  }

  // calculate tangent for triangles
  for (unsigned i = 0; i < model.indices.size() / 3; i++) {
    // indices of vertices of this triangle
    unsigned indices[3] = {model.indices[i * 3],
                           model.indices[i * 3 + 1],
                           model.indices[i * 3 + 2]};
    // access an attribute of xth vert with vector access "attribute[indices[x]]"
    
    // calculate tangent for the triangle and add it to the accumulation tangents of the adjacent vertices
    // see generate_normals() for similar workflow 
  }
  // normalize and orthogonalize accumulated vertex tangents
  for (unsigned i = 0; i < tangents.size(); ++i) {
    // implement orthogonalization and normalization here
  }

  throw std::logic_error("Tangent creation not implemented yet");

  return tangents;
}

};