#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "window.h"
#include "particle.h"
#include "orbital.h"

// ======================================================================================
// Here we define the quantum numbers and parameters for the orbital we want to visualize
int principal_quantum_number = 3;
int azimuthal_quantum_number = 1; 
int magnetic_quantum_number = 0;
int number_of_samples = 60000;
float bohr_radius = 0.35f;
// ======================================================================================

int window_width  = 800;
int window_height = 800;
const char* window_title = "An atom";

Vec3 proton_color = {1.0f, 0.1f, 0.1f};
Vec3 electron_cloud_color = {0.2f, 0.5f, 1.0f};

int main() {
    GLFWwindow* window = createWindow(window_width, window_height, window_title);
    if (!window) {
        return -1;
    }

    Particle proton({0.0f, 0.0f, 0.0f}, proton_color, 0.08f);
    OrbitalCloud electron_cloud(electron_cloud_color);

    electron_cloud.generateHydrogenState(principal_quantum_number, azimuthal_quantum_number, magnetic_quantum_number, number_of_samples, bohr_radius);

    while (!glfwWindowShouldClose(window)) {
        clearWindow();

        glPushMatrix();

        proton.draw(32);
        electron_cloud.draw(2.0f);

        glPopMatrix();

        updateWindow(window);
    }

    terminate(window);
    return 0;
}