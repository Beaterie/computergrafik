#ifndef APPLICATION_FIXED_HPP
#define APPLICATION_FIXED_HPP

#include "application.hpp"

// gpu representation of model
class ApplicationFixed : public Application {
 public:
  // allocate and initialize objects
  ApplicationFixed(std::string const& resource_path);

  // update projection matrix
  void updateProjection();
  // draw all objects
  void render() const;
};

#endif