#ifndef WINDOW_H
#define WINDOW_H

struct GLFWwindow;

GLFWwindow* createWindow(int width = 800, int height = 800, const char* title = "OpenGL Window");
void terminate(GLFWwindow* window);
void clearWindow();
void updateWindow(GLFWwindow* window);

#endif