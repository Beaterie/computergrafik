## OpenGLFramework
is a small cross-platform framework for learning OpenGL programming

[Installation](https://github.com/wobakj/OpenGLFramework/wiki/Installation)

[Troubleshooting](https://github.com/wobakj/OpenGLFramework/wiki/Troubleshooting)

### Features
* launcher encapsulating window and context management 
* example applications for usage of basic OpenGL objects
* png & tga texture loading
* obj model loading
* GLSL shader loading and error checking
* runtime OpenLG error checking
* live shader reloading by pressing _R_

### Examples
toggle compilation with cmake option _BUILD_EXAMPLES_ 
* **Immediate Mode** - application_fixed.cpp
* **Vertex Buffer Object** - application_vbo.cpp
* **Index Buffer Object** - application_indexed.cpp
* **Shader Program** - application_shader.cpp
* **Shader Uniforms** - application_uniforms.cpp
* **Vertex Array Object** - application_vao.cpp

### Tested Platforms
* **Linux** - makefile
* **Windows** - MSVC 2013
* **MacOS** - CLion, XCode, makefile

### Libraries
* [**GLFW**](http://www.glfw.org/) for window and context creation
* [**glbinding**](https://github.com/cginternals/glbinding) for OpenGL function binding
* [**GLM**](glm.g-truc.net/) for mathematics
* [**tinyobjloader**](http://syoyo.github.io/tinyobjloader/) for model loading
* [**stb_image**](https://github.com/nothings/stb) for image loading

### License
this framework is licensed undet the GPL v3
* **GLFW** is licensed under the zlib/libpng license
* **glbinding** is licensed under the MIT license
* **glm** is licensed under the Happy Bunny License (Modified MIT)
* **tinyobjloader** is licensed under 2 clause BSD
* **stb_image** is public domain
