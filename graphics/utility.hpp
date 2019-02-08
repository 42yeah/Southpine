//
//  utility.hpp
//  Southpine
//
//  Created by 周昊 on 2019/2/2.
//  Copyright © 2019 aiofwa. All rights reserved.
//

#ifndef utility_hpp
#define utility_hpp

#include "../ext/glad/glad.h"
#include <GLFW/glfw3.h>
#include <map>
#include <iostream>


GLuint testTriangle();

GLuint testCube();

GLuint genField();

GLuint genRect();

GLuint loadTex(std::string path, int *oWidth = nullptr, int *oHeight = nullptr);


float aspect(GLFWwindow *window);


#endif /* utility_hpp */
