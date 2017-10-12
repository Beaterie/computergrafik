#ifndef MODEL_LOADER_HPP
#define MODEL_LOADER_HPP

#include "model.hpp"

#include "tiny_obj_loader.h"

namespace model_loader {

model obj(std::string const& path, model::attrib_flag_t import_attribs = model::POSITION);

}

#endif