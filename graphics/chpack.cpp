//
//  chpack.cpp
//  Southpine
//
//  Created by 周昊 on 2019/2/6.
//  Copyright © 2019 aiofwa. All rights reserved.
//


#include "chpack.hpp"
#include "utility.hpp"
#include <map>
#include <iostream>
#include "../memory/bus.hpp"


CHPack::CHPack(int x, int y) {
    this->pos = { 0.0f, 0.0f, 0.0f };
    this->VAO = genRect();
    
    // back means x * width, y * height
    // right means ...
    // front means ...
    this->botX = x * bus().texWidth + 1;
    this->botY = y * bus().texHeight + 1;
    
    float buffer[] = {
        -0.5f, 0.0f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.5f, 0.0f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, 0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        0.5f, 0.0f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    };
    
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO);
    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(buffer), buffer, GL_DYNAMIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 11, nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 11, (void *) (sizeof(float) * 3));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 11, (void *) (sizeof(float) * 6));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 11, (void *) (sizeof(float) * 9));
    
    this->x = x;
    this->y = y;
}



void CHPack::draw(GLuint prog, GLuint dir) {
    glm::mat4 model(1.0f);
    model = glm::translate(model, this->pos);
    
    int rdir = dir;
    if (dir < 4) { model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); }
    else { dir = 1; }
    
    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    float *memMap = (float *) glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    
    // ensure dir <= 3
    if (dir > 3) { dir = 3; }
    else if (dir < 0) { dir = 0; }
    
    // reserve 
    float additW = 0.0f;
    float additH = dir * bus().texHeight;
    if (dir == 3) { additH = 0.0f; }
    
    
    if (dir < 3) {
        memMap[9] = this->botX + additW;
        memMap[10] = this->botY + additH;
        memMap[20] = this->botX + bus().texWidth + additW;
        memMap[21] = this->botY + additH;
        memMap[31] = this->botX + additW;
        memMap[32] = this->botY + bus().texHeight + additH;
        memMap[42] = this->botX + additW;
        memMap[43] = this->botY + bus().texHeight + additH;
        memMap[53] = this->botX + bus().texWidth + additW;
        memMap[54] = this->botY + bus().texHeight + additH;
        memMap[64] = this->botX + bus().texWidth + additW;
        memMap[65] = this->botY + additH;
    } else {
        memMap[9] = this->botX + bus().texWidth + additW;
        memMap[10] = this->botY + additH;
        memMap[20] = this->botX + additW;
        memMap[21] = this->botY + additH;
        memMap[31] = this->botX + bus().texWidth + additW;
        memMap[32] = this->botY + bus().texHeight + additH;
        memMap[42] = this->botX + bus().texWidth + additW;
        memMap[43] = this->botY + bus().texHeight + additH;
        memMap[53] = this->botX + additW;
        memMap[54] = this->botY + bus().texHeight + additH;
        memMap[64] = this->botX + additW;
        memMap[65] = this->botY + additH;
    }
    
    
    for (int i = 0; i < 6; i++) {
        memMap[i * 11 + 9] /= bus().sheetWidth;
        memMap[i * 11 + 10] /= bus().sheetHeight;
    }
    
    glUnmapBuffer(GL_ARRAY_BUFFER);
    
    
    glUseProgram(prog);
    
    glUniformMatrix4fv(glGetUniformLocation(prog, "model"), 1, GL_FALSE, glm::value_ptr(model));
    
    glm::mat4 viewMat = bus().view;
    for (int row = 0; row < 3; row += 2) {
        for (int col = 0; col < 3; col++) {
            viewMat[row][col] = (row == col) ? 1 : 0;
        }
    }
    
    glUniformMatrix4fv(glGetUniformLocation(prog, "overrideView"), 1, GL_FALSE, glm::value_ptr(viewMat));
    
    glUniform1i(glGetUniformLocation(prog, "dir"), rdir);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bus().chSheet);
    glUniform1i(glGetUniformLocation(prog, "tex"), 0);
    
    glBindVertexArray(this->VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

