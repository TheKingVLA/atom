#ifndef WINDOW_H
#define WINDOW_H

struct GLFWwindow;

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