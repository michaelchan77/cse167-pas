#include "hw3.h"
#include "3rdparty/glad.h" // needs to be included before GLFW!
#include "3rdparty/glfw/include/GLFW/glfw3.h"
#include "hw3_scenes.h"
#include "hw3_shader.h"

using namespace hw3;

// settings
const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 800;

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

    // build and compile shader program
    // --------------------------------
    Shader ourShader("/Users/michael/Documents/GitHub/cse167-pas/src/hw3-2_shader.vs", 
                     "/Users/michael/Documents/GitHub/cse167-pas/src/hw3-2_shader.fs");
    
    // set up vertex data/buffers and configure vertex attributes
    // ----------------------------------------------------------
    // store vertex data in memory on graphics card
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f,  0.5f, 0.0f
    };
    unsigned int indices[] = {
        0, 1, 2   // first triangle
    };  
   
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind vertex array object first, then bind and set vertex buffers and configure vertex attributes
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // unbind once registered
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    // call to draw in wireframe polygons
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    // render loop
    // -----------
    while(!glfwWindowShouldClose(window)) {
        // input
        processInput(window);

        // render
        glClearColor(0.7f, 0.35f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        // activate shader
        ourShader.use();
        glBindVertexArray(VAO);
        // update shader uniform
        double timeValue = glfwGetTime();
        const float PI = float(c_PI);
        float angle = sin(timeValue) * PI + PI;
        Matrix4x4f R = Matrix4x4f::identity();
        R(0,0) = std::cos(angle);
        R(0,1) = -std::sin(angle);
        R(1,0) = std::sin(angle);
        R(1,1) = std::cos(angle);
        ourShader.setMat4("rotate", R);

        // glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

        // glfw swap buffers and check IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    // de-allocate all resources once they've outlived their purpose:
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return;
}

void hw_3_3(const std::vector<std::string> &params) {
    // HW 3.3: Render a scene
    if (params.size() == 0) {
        return;
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    // hw_2_4
    // ------
    int width = scene.camera.resolution.x;
    int height = scene.camera.resolution.y;
    float a = float(width) / height; // aspect ratio
    float s = scene.camera.s; // scaling factor of the view frustrum
    float z_near = scene.camera.z_near; // distance of the near clipping plane
    float z_far = scene.camera.z_far; // distance of the far clipping plane

    Matrix4x4f V = inverse(scene.camera.cam_to_world); // world_to_camera
    Matrix4x4f P =  Matrix4x4::identity(); // camera_to_clip
    P(0, 0) = 1.0f / (a * s);
    P(1, 1) = 1.0f / s;
    P(2, 2) = -(z_far) / (z_far - z_near);
    P(2, 3) = -(z_far * z_near) / (z_far - z_near);
    P(3, 2) = -1;
    P(3, 3) = 0;

    // glfw initialize and configure
    // -----------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // for mac os x
    
    // glfw create window
    // ------------------
    GLFWwindow* window = glfwCreateWindow(width, height, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    // glad load OpenGL function pointers
    // ----------------------------------
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_FRAMEBUFFER_SRGB);

    // build and compile shader program
    // --------------------------------
    Shader ourShader("/Users/michael/Documents/GitHub/cse167-pas/src/hw3-3_shader.vs", 
                     "/Users/michael/Documents/GitHub/cse167-pas/src/hw3-2_shader.fs");
    
    // set up vertex data/buffers and configure vertex attributes
    // ----------------------------------------------------------
    // generate VAO/VBO/EBOs for each mesh
    int n = scene.meshes.size();
    std::vector<unsigned int> VAOs(n), VBOs(n), EBOs(n);

    glGenVertexArrays(VAOs.size(), VAOs.data());
    glGenBuffers(VBOs.size(), VBOs.data());
    glGenBuffers(EBOs.size(), EBOs.data());

    for (int i = 0; i < n; i++) {
        // bind vertex array object first
        glBindVertexArray(VAOs[i]);

        // bind and set vertex buffers and configure vertex attributes
        glBindBuffer(GL_ARRAY_BUFFER, VBOs[i]);
        glBufferData(GL_ARRAY_BUFFER, 
                     scene.meshes[i].vertices.size() * sizeof(Vector3f), 
                     scene.meshes[i].vertices.data(), 
                     GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[i]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
                     scene.meshes[i].faces.size() * sizeof(Vector3f), 
                     scene.meshes[i].faces.data(), 
                    GL_STATIC_DRAW);
        
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        
        // unbind once registered
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    
    // render loop
    // -----------
    while(!glfwWindowShouldClose(window)) {
        // input
        processInput(window);

        // render
        // ------
        glClearColor(0.7f, 0.35f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // activate shader
        ourShader.use();

        for (int i = 0; i < n; i++) {
            Matrix4x4f M = scene.meshes[i].model_matrix; // object_to_world
            // update shader uniforms
            ourShader.setMat4("model", M);
            ourShader.setMat4("view", V);
            ourShader.setMat4("projection", P);
            // render mesh
            glBindVertexArray(VAOs[i]);
            glDrawElements(GL_TRIANGLES, scene.meshes[i].faces.size() * sizeof(Vector3f), GL_UNSIGNED_INT, 0);   // glDrawArrays(GL_TRIANGLES, 0, 3);
        }

        // glfw swap buffers and check IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    // de-allocate all resources once they've outlived their purpose:
    glDeleteVertexArrays(VAOs.size(), VAOs.data());
    glDeleteBuffers(VBOs.size(), VBOs.data());
    glDeleteBuffers(EBOs.size(), EBOs.data());

    glfwTerminate();
    return;
}

void hw_3_4(const std::vector<std::string> &params) {
    // HW 3.4: Render a scene with lighting
    if (params.size() == 0) {
        return;
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;
}
