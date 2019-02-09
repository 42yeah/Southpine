//
//  bus.hpp
//  Southpine
//
//  Created by 周昊 on 2019/2/2.
//  Copyright © 2019 aiofwa. All rights reserved.
//

#ifndef bus_hpp
#define bus_hpp

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <iostream>


struct bus {
    double prevX, prevY;
    float sensivity;
    float yaw, pitch;
    
    glm::vec3 eye, front, up, wants;
    glm::mat4 view, perspec;
    
    GLFWwindow *win;
    float delta;
    float camSpeed;
    
    glm::vec3 sun;
    glm::vec3 sunColor;
    
    
    // default configs
    GLuint chSheet;
    int texWidth, texHeight;
    int sheetWidth, sheetHeight;
    
    
    bool GOES_BY_PRESSING_W;
};


bus &bus();

#endif /* bus_hpp */
