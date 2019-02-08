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
    
    bus().yaw += dx;
    
    if (!(bus().wants == glm::vec3(0.0f, bus().sun.y, 0.0f))) {
        bus().pitch += dy;
    }
    
    if (bus().pitch >= 89.9f) { bus().pitch = 89.9f; }
    if (bus().pitch <= -89.9f) { bus().pitch = -89.9f; }
    
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
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, uMatrices);
    
    GLuint field = genField();
    GLuint fieldProg = program("shaders/vworld.glsl", "shaders/fworld.glsl");
    glUniformBlockBinding(fieldProg, glGetUniformBlockIndex(fieldProg, "Matrices"), 0);
    GLuint chProg = program("shaders/vch.glsl", "shaders/fch.glsl");
    glUniformBlockBinding(chProg, glGetUniformBlockIndex(chProg, "Matrices"), 0);
    
    CHPack steve(0, 0);
    steve.pos = glm::vec3(0.0, 2.0f, 0.0f);
    
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
            bus().wants += bus().front * bus().delta * bus().camSpeed;
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
        } else if (bus().wants.y <= 1.0f) { bus().wants.y = 1.0f; }
        
        
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
        char *VPdat = (char *) glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
        memcpy(VPdat, glm::value_ptr(bus().view), sizeof(glm::mat4));
        memcpy(VPdat + sizeof(glm::mat4), glm::value_ptr(bus().perspec), sizeof(glm::mat4));
        glUnmapBuffer(GL_UNIFORM_BUFFER);
        
        
        // render
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glUseProgram(fieldProg);
        glUniformMatrix4fv(glGetUniformLocation(fieldProg, "model"), 1, GL_FALSE, glm::value_ptr(defaultModel));
        glUniform3fv(glGetUniformLocation(fieldProg, "sunPos"), 1, glm::value_ptr(bus().sun));
        glUniform3fv(glGetUniformLocation(fieldProg, "sunColor"), 1, glm::value_ptr(bus().sunColor));
        glUniform3fv(glGetUniformLocation(fieldProg, "eye"), 1, glm::value_ptr(bus().eye));
        
        glBindVertexArray(field);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        
        glm::mat4 chMod(1.0f);
        chMod = glm::translate(chMod, glm::vec3(0.0f, 0.1f, 0.0f));
        glUseProgram(chProg);
        glUniformMatrix4fv(glGetUniformLocation(chProg, "model"), 1, GL_FALSE, glm::value_ptr(defaultModel));
        glUniform3fv(glGetUniformLocation(chProg, "sunPos"), 1, glm::value_ptr(bus().sun));
        glUniform3fv(glGetUniformLocation(chProg, "sunColor"), 1, glm::value_ptr(bus().sunColor));
        glUniform3fv(glGetUniformLocation(chProg, "eye"), 1, glm::value_ptr(bus().eye));
        
        steve.draw(chProg);
        
        
        glfwSwapBuffers(window);
    }
    
    
    return 0;
}


