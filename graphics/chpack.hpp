//
//  chpack.hpp
//  Southpine
//
//  Created by 周昊 on 2019/2/6.
//  Copyright © 2019 aiofwa. All rights reserved.
//

#ifndef chpack_hpp
#define chpack_hpp


#include "../ext/glad/glad.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


// an action-packed character.
class CHPack {
public:
    CHPack(int x, int y);
    
    void draw(GLuint prog);
    
    
    int x, y;
    int botX, botY;
    GLuint VAO, VBO;
    glm::vec3 pos;
};


#endif /* chpack_hpp */
