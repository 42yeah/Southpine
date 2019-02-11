//
//  main.cpp
//  Southpine
//
//  Created by 周昊 on 2019/2/2.
//  Copyright © 2019 aiofwa. All rights reserved.
//


#include <iostream>
#include "../ext/glad/glad.h"
#include <GLFW/glfw3.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../graphics/shader.hpp"
#include "../graphics/utility.hpp"
#include "../graphics/chpack.hpp"
#include "../memory/bus.hpp"



void recalcFront() {
    bus().front.x = cos(glm::radians(bus().pitch)) * cos(glm::radians(bus().yaw));
    bus().front.y = sin(glm::radians(bus().pitch));
    bus().front.z = cos(glm::radians(bus().pitch)) * sin(glm::radians(bus().yaw));
    bus().front = glm::normalize(bus().front);
}



void mouseMoved(GLFWwindow *window, double x, double y) {
    double dx, dy;
    
    dx = x - bus().prevX;
    dy = bus().prevY - y;
    
    dx *= bus().sensivity;
    dy *= bus().sensivity;
    
    if (!(bus().wants == glm::vec3(0.0f, bus().sun.y, 0.0f))) {
        bus().yaw += dx;
        bus().pitch += dy;
    }
    
    if (bus().pitch >= 89.9f) { bus().pitch = 89.9f; }
    if (bus().pitch <= -89.9f) { bus().pitch = -89.9f; }
    
    if (bus().yaw >= 360.0f) { bus().yaw -= 360.0f; }
    if (bus().yaw <= -360.0f) { bus().yaw += 360.0f; }
    
    recalcFront();

    bus().prevX = x;
    bus().prevY = y;
}



void winResized(GLFWwindow *win, int w, int h) {
    bus().perspec = glm::perspective(glm::radians(45.0f), aspect(win), 0.01f, 200.0f);
}



bool pressed(int key) {
    return glfwGetKey(bus().win, key);
}



int main(int argc, const char * argv[]) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 8);
    
    GLFWwindow *window = glfwCreateWindow(800, 600, "Southpine", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    gladLoadGL();
    
    
    // init gl stuff
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouseMoved);
    glfwSetWindowSizeCallback(window, winResized);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    
    
    // configure bus.
    double curX, curY;
    glfwGetCursorPos(window, &curX, &curY);
    bus().prevX = curX;
    bus().prevY = curY;
    bus().sensivity = 0.07f;
    bus().yaw = 270.0;
    bus().pitch = 0.0f;
    bus().eye = { 0.0f, 1.0f, 2.0f };
    bus().front = { 0.0f, -1.0f, 0.0f };
    bus().up = { 0.0f, 1.0f, 0.0f };
    bus().wants = bus().eye;
    bus().win = window;
    bus().delta = 0.0f;
    bus().camSpeed = 10.0f;
    bus().GOES_BY_PRESSING_W = false;
    
    bus().texWidth = 16;
    bus().texHeight = 16;
    bus().chSheet = loadTex("assets/chars.png", &bus().sheetWidth, &bus().sheetHeight);
    
    // environments
    bus().sun = { 0.0f, 30.0f, 0.0f };
    bus().sunColor = { 0.392f, 0.352f, 0.01f };
    
    // configure camera.
    glm::mat4 defaultModel(1.0f);
    bus().perspec = bus().view = glm::mat4(1.0f);

    bus().view = glm::lookAt(bus().eye, bus().eye + bus().front, bus().up);
    bus().perspec = glm::perspective(glm::radians(45.0f), aspect(window), 0.01f, 200.0f);
    mouseMoved(window, curX, curY);
    
    GLuint uMatrices;
    glGenBuffers(1, &uMatrices);
    glBindBuffer(GL_UNIFORM_BUFFER, uMatrices);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2, nullptr, GL_DYNAMIC_DRAW);
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, uMatrices, 0, sizeof(glm::mat4) * 2);
    
    GLuint uSun;
    glGenBuffers(1, &uSun);
    glBindBuffer(GL_UNIFORM_BUFFER, uSun);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::vec4) * 3, nullptr, GL_DYNAMIC_DRAW);
    glBindBufferRange(GL_UNIFORM_BUFFER, 1, uSun, 0, sizeof(glm::vec4) * 3);
    
    GLuint field = genField();
    GLuint fieldProg = program("shaders/vworld.glsl", "shaders/fworld.glsl");
    glUniformBlockBinding(fieldProg, glGetUniformBlockIndex(fieldProg, "Matrices"), 0);
    glUniformBlockBinding(fieldProg, glGetUniformBlockIndex(fieldProg, "Sun"), 1);
    GLuint chProg = program("shaders/vch.glsl", "shaders/fch.glsl");
    glUniformBlockBinding(chProg, glGetUniformBlockIndex(chProg, "Matrices"), 0);
    glUniformBlockBinding(chProg, glGetUniformBlockIndex(chProg, "Sun"), 1);
    
    
    std::vector<CHPack> chs;
    for (int i = 0; i < 5000; i++) {
        CHPack guy = CHPack(i % 13, i * 3);
        guy.pos = { (sinf(i) * i) / 100.0f, 0.5f, (cosf(i) * i) / 100.0f };
        chs.push_back(guy);
    }
    
    // handle events, update, render, swap
    float prev = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        // calc delta
        float now = glfwGetTime();
        bus().delta = now - prev;
        prev = now;
        
        // handle events
        glfwPollEvents();
        
        
        // and moving around
        if (pressed(GLFW_KEY_W)) {
            if (!bus().GOES_BY_PRESSING_W) {
                bus().wants += bus().front * bus().delta * bus().camSpeed;
            }
        } else if (bus().GOES_BY_PRESSING_W) {
            bus().GOES_BY_PRESSING_W = false;
        }
        if (pressed(GLFW_KEY_S)) {
            bus().wants -= bus().front * bus().delta * bus().camSpeed;
        }
        if (pressed(GLFW_KEY_A)) {
            bus().wants += glm::normalize(glm::cross(bus().up, bus().front)) * bus().delta * bus().camSpeed;
        }
        if (pressed(GLFW_KEY_D)) {
            bus().wants -= glm::normalize(glm::cross(bus().up, bus().front)) * bus().delta * bus().camSpeed;
        }
        if (pressed(GLFW_KEY_SPACE)) {
            bus().wants.y += bus().camSpeed * 10.0f * bus().delta;
        }
        
        if (bus().wants.y >= bus().sun.y) {
            bus().wants = { 0.0f, bus().sun.y, 0.0f };
            if (pressed(GLFW_KEY_W)) {
                bus().GOES_BY_PRESSING_W = true;
            }
        } else if (bus().wants.y <= 0.5f) { bus().wants.y = 0.5f; }
        
        
        // update
        glm::vec3 deltaPos = bus().wants - bus().eye;
        bus().eye += deltaPos * bus().camSpeed * bus().delta;
        bus().view = glm::lookAt(bus().eye, bus().eye + bus().front, bus().up);
        
        // however, if wants == orthopos, then switch
        if (bus().wants == glm::vec3(0.0f, bus().sun.y, 0.0f)) {
            float dPitch = -89.9f - bus().pitch;
            float dYaw = 270.0f - bus().yaw;
            
            bus().pitch += dPitch * bus().delta * bus().camSpeed;
            bus().yaw += dYaw * bus().delta * bus().camSpeed;
            recalcFront();
        }
        
        
        glBindBuffer(GL_UNIFORM_BUFFER, uMatrices);
        char *VPDat = (char *) glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
        memcpy(VPDat, glm::value_ptr(bus().view), sizeof(glm::mat4));
        memcpy(VPDat + sizeof(glm::mat4), glm::value_ptr(bus().perspec), sizeof(glm::mat4));
        glUnmapBuffer(GL_UNIFORM_BUFFER);
        
        
        glBindBuffer(GL_UNIFORM_BUFFER, uSun);
        char *sunDat = (char *) glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
        memcpy(sunDat, glm::value_ptr(bus().sun), sizeof(glm::vec3));
        memcpy(sunDat + sizeof(glm::vec4), glm::value_ptr(bus().sunColor), sizeof(glm::vec3));
        memcpy(sunDat + sizeof(glm::vec4) * 2, glm::value_ptr(bus().eye), sizeof(glm::vec3));
        glUnmapBuffer(GL_UNIFORM_BUFFER);
        
        // render
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glUseProgram(fieldProg);
        glUniformMatrix4fv(glGetUniformLocation(fieldProg, "model"), 1, GL_FALSE, glm::value_ptr(defaultModel));
        
        glBindVertexArray(field);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        
        glm::mat4 chMod(1.0f);
        chMod = glm::translate(chMod, glm::vec3(0.0f, 0.1f, 0.0f));
        glUseProgram(chProg);
        glUniformMatrix4fv(glGetUniformLocation(chProg, "model"), 1, GL_FALSE, glm::value_ptr(defaultModel));
        
        for (int i = 0; i < chs.size(); i++) {
            if (bus().wants == glm::vec3(0.0f, bus().sun.y, 0.0f)) {
                chs[i].draw(chProg, 4);
            } else {
                chs[i].draw(chProg, 1);
            }
        }
        
        
        glfwSwapBuffers(window);
    }
    
    
    return 0;
}


