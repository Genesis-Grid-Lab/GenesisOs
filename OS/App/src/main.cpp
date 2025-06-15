#include <UrbanEngine.h>
#include "Core/EntryPoint.h"
#include "OSApp.h"

UE::Application* UE::CreateApplication(UE::ApplicationCommandLineArgs args){
  return new OSApp(args);
}

// #include <GLFW/glfw3.h>
// #include <stdio.h>
// int main() {
//     if (!glfwInit()) { printf("glfwInit failed\n"); return -1; }
//     GLFWwindow* win = glfwCreateWindow(640, 480, "Test", NULL, NULL);
//     if (!win) { printf("window failed\n"); return -1; }
//     glfwMakeContextCurrent(win);
//     printf("OpenGL loaded: %s\n", glGetString(GL_VERSION));
//     return 0;
// }

