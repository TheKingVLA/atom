#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "window.h"
#include "particle.h"
#include "orbital.h"

// ======================================================================================
// Here we define the quantum numbers and parameters for the orbital we want to visualize
int n = 3;
int l = 1; 
int m = 0;
int numSamples = 60000;
float a0 = 0.35f;
// ======================================================================================

int width  = 800;
int height = 800;
const char* title = "An atom";

Vec3 proton_color = {1.0f, 0.1f, 0.1f};
Vec3 cloud_color = {0.2f, 0.5f, 1.0f};

int main() {
    GLFWwindow* window = createWindow(width, height, title);
    if (!window) {
        return -1;
    }

    Particle proton({0.0f, 0.0f, 0.0f}, proton_color, 0.08f);
    OrbitalCloud cloud(cloud_color);

    cloud.generateHydrogenState(n, l, m, numSamples, a0);

    while (!glfwWindowShouldClose(window)) {
        clearWindow();

        glPushMatrix();

        proton.draw(32);
        cloud.draw(2.0f);

        glPopMatrix();

        updateWindow(window);
    }

    terminate(window);
    return 0;
}