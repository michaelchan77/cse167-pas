#include "hw3.h"
#include "3rdparty/glad.h" // needs to be included before GLFW!
#include "3rdparty/glfw/include/GLFW/glfw3.h"
#include "hw3_scenes.h"

using namespace hw3;

// glfw callback function resizes viewport when window size is changed
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// glfw exit upon escape key
void processInput(GLFWwindow *window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void hw_3_1(const std::vector<std::string> &params) {
    // HW 3.1: Open a window using GLFW

    const unsigned int WIDTH = 800;
    const unsigned int HEIGHT = 600;

    // glfw initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // for mac os x
    
    // glfw create window
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    // glad load OpenGL function pointers
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }
    // render loop
    while(!glfwWindowShouldClose(window)) {
        // input
        processInput(window);

        // render
        glClearColor(0.7f, 0.35f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // swap buffers and check IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return;
}

void hw_3_2(const std::vector<std::string> &params) {
    // HW 3.2: Render a single 2D triangle
}

void hw_3_3(const std::vector<std::string> &params) {
    // HW 3.3: Render a scene
    if (params.size() == 0) {
        return;
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;
}

void hw_3_4(const std::vector<std::string> &params) {
    // HW 3.4: Render a scene with lighting
    if (params.size() == 0) {
        return;
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;
}
