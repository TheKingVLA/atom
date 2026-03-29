#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "window.h"
#include "particle.h"
#include "orbital.h"

int width  = 800;
int height = 800;
const char* title = "An atom";

int main() {
    GLFWwindow* window = createWindow(width, height, title);
    if (!window) {
        return -1;
    }

    Particle proton({0.0f, 0.0f, 0.0f}, {1.0f, 0.1f, 0.1f}, 0.08f);

    OrbitalCloud cloud({0.2f, 0.5f, 1.0f});
    cloud.generate1s(40000, 0.35f);

    while (!glfwWindowShouldClose(window)) {
        clearWindow();

        glPushMatrix();

        // optional slow rotation so the cloud feels 3D
        static float angle = 0.0f;
        angle += 0.15f;
        glRotatef(angle, 0.0f, 1.0f, 0.0f);

        proton.draw(32);
        cloud.draw(2.0f);

        glPopMatrix();

        updateWindow(window);
    }

    terminate(window);
    return 0;
}