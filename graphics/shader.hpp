//
//  shader.hpp
//  Southpine
//
//  Created by 周昊 on 2019/2/2.
//  Copyright © 2019 aiofwa. All rights reserved.
//

#ifndef shader_hpp
#define shader_hpp

#include <iostream>
#include "../ext/glad/glad.h"


GLuint shader(GLuint type, std::string path);

GLuint program(std::string vpath, std::string fpath);


#endif /* shader_hpp */
