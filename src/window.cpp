#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "window.h"
#include <cmath>
#include <iostream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Camera camera;

void mouse_callback(GLFWwindow* window, double mouse_x, double mouse_y) {
    if (camera.is_first_mouse) {
        camera.last_mouse_x   = mouse_x;
        camera.last_mouse_y   = mouse_y;
        camera.is_first_mouse = false;
        return;
    }

    double mouse_x_offset = mouse_x - camera.last_mouse_x;
    double mouse_y_offset = camera.last_mouse_y - mouse_y; 

    camera.last_mouse_x = mouse_x;
    camera.last_mouse_y = mouse_y;

    float mouse_sensitivity = 0.3f;
    mouse_x_offset *= mouse_sensitivity;
    mouse_y_offset *= mouse_sensitivity;

    camera.rotation_y += static_cast<float>(mouse_x_offset);
    camera.rotation_x += static_cast<float>(mouse_y_offset);

    if (camera.rotation_x > 89.0f)
        camera.rotation_x = 89.0f;
    if (camera.rotation_x < -89.0f)
        camera.rotation_x = -89.0f;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    float zoom_speed = 0.2f;
    camera.zoom -= static_cast<float>(yoffset) * zoom_speed;
    
    if (camera.zoom > -0.5f)
        camera.zoom = -0.5f;
    if (camera.zoom < -20.0f)
        camera.zoom = -20.0f;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    float aspect_ratio  = static_cast<float>(width) / static_cast<float>(height ? height : 1);
    float field_of_view = 45.0f;
    float f             = 1.0f / tanf(field_of_view * static_cast<float>(M_PI) / 360.0f);
    float near_plane    = 0.1f;
    float far_plane     = 100.0f;

    glFrustum(-aspect_ratio * near_plane / f, aspect_ratio * near_plane / f,
              -near_plane / f, near_plane / f,
              near_plane, far_plane);

    glMatrixMode(GL_MODELVIEW);
}

GLFWwindow* createWindow(int width, int height, const char* title) {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return nullptr;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

    GLFWwindow* window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return nullptr;
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    framebuffer_size_callback(window, width, height);

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    float light_position[] = {1.0f, 1.0f, 1.0f, 0.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    float ambient_light[]  = {0.4f, 0.4f, 0.4f, 1.0f};
    float diffuse_light[]  = {1.0f, 1.0f, 1.0f, 1.0f};
    float specular_light[] = {1.0f, 1.0f, 1.0f, 1.0f};

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_light);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_light);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular_light);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_POINT_SMOOTH);

    return window;
}

void terminate(GLFWwindow* window) {
    if (window) {
        glfwDestroyWindow(window);
    }
    glfwTerminate();
}

void clearWindow() {
    glClearColor(0.05f, 0.05f, 0.08f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // Apply camera transformations
    glTranslatef(0.0f, 0.0f, camera.zoom);
    glRotatef(camera.rotation_x, 1.0f, 0.0f, 0.0f);
    glRotatef(camera.rotation_y, 0.0f, 1.0f, 0.0f);
}

void updateWindow(GLFWwindow* window) {
    glfwSwapBuffers(window);
    glfwPollEvents();
}