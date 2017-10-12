#ifndef LAUNCHER_HPP
#define LAUNCHER_HPP

#include "application.hpp"

#include <string>

// forward declarations
class Application;
class GLFWwindow;

class Launcher {
 public:
  template<typename T>
  static void run(int argc, char* argv[]) {
    Launcher launcher{argc, argv};
    launcher.run<T>();
  }

 private:

  Launcher(int argc, char* argv[]);
  // run application
  template<typename T>
  void run(){
    initialize();

    m_application = new T{m_resource_path};

    mainLoop();
  }
  
  // create window and set callbacks
  void initialize();
  // start main loop
  void mainLoop();
  // update viewport and field of view
  void update_projection(GLFWwindow* window, int width, int height);
  // load shader programs and update uniform locations
  void update_shader_programs(bool throwing);
  // handle key input
  void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
  //handle mouse movement input
  void mouse_callback(GLFWwindow* window, double pos_x, double pos_y);

  // calculate fps and show in window title
  void show_fps();
  // free resources
  void quit(int status);

  // vertical field of view of camera
  const float m_camera_fov;

  // initial window dimensions
  const unsigned m_window_width;
  const unsigned m_window_height;
  // the rendering window
  GLFWwindow* m_window;

  // variables for fps computation
  double m_last_second_time;
  unsigned m_frames_per_second;

  // path to the resource folders
  std::string m_resource_path;

  Application* m_application;
};
#endif