//
//  shader.cpp
//  Southpine
//
//  Created by 周昊 on 2019/2/2.
//  Copyright © 2019 aiofwa. All rights reserved.
//

#include "shader.hpp"
#include <fstream>
#include <iostream>
#include <sstream>


GLuint shader(GLuint type, std::string path) {
    std::fstream reader(path);
    std::stringstream ss;
    
    ss << reader.rdbuf();
    std::string src = ss.str();
    const char *raw = src.c_str();
    
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &raw, nullptr);
    glCompileShader(shader);
    
    char log[512] = { 0 };
    glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
    
    std::cout << "shader: " << log << std::endl;
    
    return shader;
}



GLuint program(std::string vpath, std::string fpath) {
    GLuint vshader = shader(GL_VERTEX_SHADER, vpath);
    GLuint fshader = shader(GL_FRAGMENT_SHADER, fpath);
    
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vshader);
    glAttachShader(prog, fshader);
    
    glLinkProgram(prog);
    
    char log[512] = { 0 };
    glGetProgramInfoLog(prog, sizeof(log), nullptr, log);
    
    std::cout << "program: " << log << std::endl;
    
    return prog;
}
