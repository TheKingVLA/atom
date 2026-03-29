#ifndef WINDOW_H
#define WINDOW_H

struct GLFWwindow;

/**
 * @brief Camera struct for managing view transformations.
 */
struct Camera {
    float rotationX = 0.0f;  // Rotation around X axis (up/down)
    float rotationY = 0.0f;  // Rotation around Y axis (left/right)
    float zoom = -4.0f;      // Distance from center (negative = camera back)
    
    // Mouse tracking
    double lastMouseX = 0.0;
    double lastMouseY = 0.0;
    bool firstMouse = true;
};

/**
 * @brief Global camera object accessible from callbacks.
 */
extern Camera camera;

/**
 * @brief Mouse callback function for handling mouse movement.
 * @param window The GLFWwindow instance.
 * @param xpos The x-coordinate of the mouse position.
 * @param ypos The y-coordinate of the mouse position.
 */
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

/**
 * @brief Scroll callback function for handling mouse scrolling.
 * @param window The GLFWwindow instance.
 * @param xoffset The x-offset of the scroll.
 * @param yoffset The y-offset of the scroll.
 */
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

/**
 * @brief Creates an OpenGL window.
 * @param width The width of the window.
 * @param height The height of the window.
 * @param title The title of the window.
 * @return A pointer to the created GLFWwindow, or nullptr if creation failed.
 */
GLFWwindow* createWindow(int width = 800, int height = 800, const char* title = "OpenGL Window");

/**
 * @brief Terminates the specified GLFWwindow.
 * @param window A pointer to the GLFWwindow to terminate.
 */
void terminate(GLFWwindow* window);

/**
 * @brief Clears the specified GLFWwindow.
 * @param window A pointer to the GLFWwindow to clear.
 */
void clearWindow();

/**
 * @brief Updates the specified GLFWwindow by swapping buffers and polling events.
 * @param window A pointer to the GLFWwindow to update.
 */
void updateWindow(GLFWwindow* window);

#endif