#ifndef TEXTURE_LOADER_HPP
#define TEXTURE_LOADER_HPP

#include "pixel_data.hpp"

#include <string>

namespace texture_loader {
  pixel_data file(std::string const& file_name);
};

#endif