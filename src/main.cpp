#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "window.h"
#include "particle.h"

#define M_PI 3.14159265358979323846

int width  = 800;
int height = 800;
const char* title = "An atom";

vector<float> proton_color   = {1.0f, 0.0f, 0.0f};  // Red 
vector<float> neutron_color  = {0.0f, 1.0f, 0.0f};  // Green 
vector<float> electron_color = {0.0f, 0.0f, 1.0f};  // Blue 
float proton_radius   = 0.1f;
float neutron_radius  = 0.1f;
float electron_radius = 0.05f;

int main() {
    GLFWwindow* window = createWindow(width, height, title);
    if (!window) {
        return -1;
    }

    float rotation_angle = 0.0f;

    while (!glfwWindowShouldClose(window)) {
        clearWindow();

        vector<float> pos = {0.0f, 0.0f, 0.0f};
        Particle p(pos, proton_color, proton_radius);
        p.draw(pos, proton_radius, 50);
        vector<float> pos2 = {0.5f, M_PI / 2, 0.0f};
        Particle n(pos2, neutron_color, neutron_radius);
        n.draw(pos2, neutron_radius, 50);
        
        updateWindow(window);
    }

    terminate(window);
    return 0;
}