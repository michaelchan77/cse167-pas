#include "hw3.h"
#include "3rdparty/glad.h" // needs to be included before GLFW!
#include "3rdparty/glfw/include/GLFW/glfw3.h"
#include "flexception.h"
#include "hw3_scenes.h"

#include <iostream>

using namespace hw3;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

struct CallbackData {
    Scene *scene = nullptr;
    Matrix4x4f org_cam_to_world;
    float yaw = 0.f, pitch = 0.f;
    double last_x = 0, last_y = 0;
    bool first_mouse = true;
    float camera_speed = 1.f;
};

void hw_3_1(const std::vector<std::string> &params) {
    // HW 3.1: Open a window using GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Balboa", NULL, NULL);
    if (window == NULL) {
        Error("Failed to create GLFW window");
        return;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        Error("Failed to initialize GLAD");
        return;
    }
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    while(!glfwWindowShouldClose(window)) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
}

void hw_3_2(const std::vector<std::string> &params) {
    // HW 3.2: Render a single 2D triangle
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Balboa", NULL, NULL);
    if (window == NULL) {
        Error("Failed to create GLFW window");
        return;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        Error("Failed to initialize GLAD");
        return;
    }
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    const char *vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "uniform float rot;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = vec4(aPos.x * cos(rot) - aPos.y * sin(rot),\n"
        "                      aPos.x * sin(rot) + aPos.y * cos(rot), aPos.z, 1.0);\n"
        "}\0";
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    const char *fragmentShaderSource = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "   FragColor = vec4(1.0, 0.5, 0.2, 1.0);\n"
        "}\0";
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glUseProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    while(!glfwWindowShouldClose(window)) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        float t = glfwGetTime();
        glUniform1f(glGetUniformLocation(shaderProgram, "rot"), t);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    CallbackData* data =
        (CallbackData*)glfwGetWindowUserPointer(window);
    if (data->first_mouse) {
        data->last_x = xpos;
        data->last_y = ypos;
        data->first_mouse = false;
        return;
    }

    float xoffset = xpos - data->last_x;
    float yoffset = data->last_y - ypos; 
    data->last_x = xpos;
    data->last_y = ypos;

    float sensitivity = 0.005f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    data->yaw += xoffset;
    data->pitch =
        std::clamp(data->pitch + yoffset, -89.f, 89.f);

    Matrix4x4f rot_yaw(
        cos(data->yaw), 0.f, -sin(data->yaw), 0.f,
                   0.f, 1.f,             0.f, 0.f,
        sin(data->yaw), 0.f,  cos(data->yaw), 0.f,
                   0.f, 0.f,             0.f, 1.f
    );
    Matrix4x4f rot_pitch(
        1.f,              0.f,               0.f, 0.f,
        0.f, cos(data->pitch), -sin(data->pitch), 0.f,
        0.f, sin(data->pitch),  cos(data->pitch), 0.f,
        0.f,              0.f,               0.f, 1.f
    );

    Matrix4x4f &to_world = data->scene->camera.cam_to_world;
    to_world = data->org_cam_to_world * rot_yaw * rot_pitch;
}

void process_input(GLFWwindow *window, float delta_time) {
    CallbackData* data =
        (CallbackData*)glfwGetWindowUserPointer(window);
    float camera_speed = static_cast<float>(
        0.1f * data->scene->camera.z_far * delta_time);
    Matrix4x4f &to_world = data->scene->camera.cam_to_world;
    Vector3f d = normalize(Vector3f{
        to_world(0, 2), to_world(1, 2), to_world(2, 2)
    });
    Vector3f r = normalize(Vector3f{
        to_world(0, 0), to_world(1, 0), to_world(2, 0)
    });
    Vector3f p = Vector3f{
        to_world(0, 3), to_world(1, 3), to_world(2, 3)
    };

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        p -= camera_speed * d;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        p += camera_speed * d;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        p -= camera_speed * r;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        p += camera_speed * r;
    }
    to_world(0, 3) = data->org_cam_to_world(0, 3) = p.x;
    to_world(1, 3) = data->org_cam_to_world(1, 3) = p.y;
    to_world(2, 3) = data->org_cam_to_world(2, 3) = p.z;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        data->scene->camera.s -= Real(0.01);
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        data->scene->camera.s += Real(0.01);
    }
}

void hw_3_3(const std::vector<std::string> &params) {
    // HW 3.3: Render a scene
    if (params.size() == 0) {
        return;
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    int width = scene.camera.resolution.x;
    int height = scene.camera.resolution.y;
    GLFWwindow* window = glfwCreateWindow(width, height, "Balboa", NULL, NULL);
    if (window == NULL) {
        Error("Failed to create GLFW window");
        return;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        Error("Failed to initialize GLAD");
        return;
    }
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    CallbackData callback_data{&scene, scene.camera.cam_to_world};
    glfwSetWindowUserPointer(window, &callback_data);

    const char *vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec3 aColor;\n"
        "uniform mat4 projection;\n"
        "uniform mat4 view;\n"
        "uniform mat4 model;\n"
        "out vec3 vertexColor;\n"
        "void main()\n"
        "{\n"
        "   vertexColor = aColor;\n"
        "   gl_Position = projection * view * model * vec4(aPos, 1);\n"
        "}\0";
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    int success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        char infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "Vertex shader compilation failed\n" << infoLog << std::endl;
    };

    const char *fragmentShaderSource = "#version 330 core\n"
        "in vec3 vertexColor;\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "   FragColor = vec4(vertexColor, 1.0);\n"
        "}\0";
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        char infoLog[512];
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "Fragment shader compilation failed\n" << infoLog << std::endl;
    };

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glUseProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    std::vector<uint32_t> VAOs;
    VAOs.resize(scene.meshes.size());

    for (int i = 0; i < (int)scene.meshes.size(); i++) {
        glGenVertexArrays(1, &VAOs[i]);
        glBindVertexArray(VAOs[i]);
        unsigned int VBO_vertex;
        glGenBuffers(1, &VBO_vertex);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_vertex);
        glBufferData(GL_ARRAY_BUFFER,
                     sizeof(Vector3f) * scene.meshes[i].vertices.size(),
                     scene.meshes[i].vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0 /* layout index */,
                              3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        unsigned int VBO_color;
        glGenBuffers(1, &VBO_color);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_color);
        glBufferData(GL_ARRAY_BUFFER,
                     sizeof(Vector3f) * scene.meshes[i].vertex_colors.size(),
                     scene.meshes[i].vertex_colors.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(1 /* layout index */,
                              3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);

        unsigned int EBO;
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     sizeof(Vector3i) * scene.meshes[i].faces.size(),
                     scene.meshes[i].faces.data(), GL_STATIC_DRAW);
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_FRAMEBUFFER_SRGB);
    float last_time = (float)glfwGetTime();
    while(!glfwWindowShouldClose(window)) {
        float current_time = (float)glfwGetTime();
        float delta_time = current_time - last_time;
        last_time = current_time;
        process_input(window, delta_time);

        // Setup camera transformation matrix
        Real a = Real(width) / Real(height);
        Real s = scene.camera.s;
        Real zfar = scene.camera.z_far;
        Real znear = scene.camera.z_near;
        Matrix4x4f projection;
        projection(0, 0) = 1.f / (a * s);
        projection(1, 1) = 1.f / s;
        projection(2, 2) = -zfar / (zfar - znear);
        projection(2, 3) = -zfar * znear / (zfar - znear);
        projection(3, 2) = -1.f;

        glClearColor(scene.background.x,
                     scene.background.y,
                     scene.background.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Matrix4x4f view = inverse(scene.camera.cam_to_world);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"),
                           1, GL_FALSE /* transpose */, projection.ptr());
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"),
                           1, GL_FALSE /* transpose */, view.ptr());

        glUseProgram(shaderProgram);
        for (int i = 0; i < (int)scene.meshes.size(); i++) {
            const Matrix4x4f &model = scene.meshes[i].model_matrix;
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"),
                               1, GL_FALSE /* transpose */, model.ptr());
            glBindVertexArray(VAOs[i]);
            glDrawElements(GL_TRIANGLES, 3 * scene.meshes[i].faces.size(), GL_UNSIGNED_INT, 0);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
}

void hw_3_4(const std::vector<std::string> &params) {
    // HW 3.4: Render a scene with lighting
    if (params.size() == 0) {
        return;
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    int width = scene.camera.resolution.x;
    int height = scene.camera.resolution.y;
    GLFWwindow* window = glfwCreateWindow(width, height, "Balboa", NULL, NULL);
    if (window == NULL) {
        Error("Failed to create GLFW window");
        return;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        Error("Failed to initialize GLAD");
        return;
    }
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    CallbackData callback_data{&scene, scene.camera.cam_to_world};
    glfwSetWindowUserPointer(window, &callback_data);

    const char *vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec3 aColor;\n"
        "layout (location = 2) in vec3 aNormal;\n"
        "uniform mat4 projection;\n"
        "uniform mat4 view;\n"
        "uniform mat4 model;\n"
        "out vec3 vertexColor;\n"
        "out vec3 vertexNormal;\n"
        "out vec3 fragPos;"
        "void main()\n"
        "{\n"
        "   vertexColor = aColor;\n"
        "   vertexNormal = mat3(transpose(inverse(model))) * aNormal;\n"
        "   fragPos = vec3(model * vec4(aPos, 1));\n"
        "   gl_Position = projection * view * model * vec4(aPos, 1);\n"
        "}\0";
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    int success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        char infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "Vertex shader compilation failed\n" << infoLog << std::endl;
    };

    const char *fragmentShaderSource = "#version 330 core\n"
        "in vec3 vertexColor;\n"
        "in vec3 vertexNormal;\n"
        "in vec3 fragPos;\n"
        "uniform vec3 viewPos;\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "   vec3 lightColor = vec3(1.0, 1.0, 1.0);\n"
        "   vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));\n"
        "   float ambient = 0.1;\n"
        "   vec3 norm = normalize(vertexNormal);\n"
        "   float diff = max(dot(norm, lightDir), 0.0);\n"
        "   vec3 viewDir = normalize(viewPos - fragPos);\n"
        "   vec3 reflectDir = reflect(-lightDir, norm);\n"
        "   float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);\n"
        "   vec3 result = (ambient + diff + spec) * vertexColor;\n"
        "   FragColor = vec4(result, 1.0);\n"
        "}\0";
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        char infoLog[512];
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "Fragment shader compilation failed\n" << infoLog << std::endl;
    };

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glUseProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    std::vector<uint32_t> VAOs;
    VAOs.resize(scene.meshes.size());

    for (int i = 0; i < (int)scene.meshes.size(); i++) {
        glGenVertexArrays(1, &VAOs[i]);
        glBindVertexArray(VAOs[i]);
        unsigned int VBO_vertex;
        glGenBuffers(1, &VBO_vertex);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_vertex);
        glBufferData(GL_ARRAY_BUFFER,
                     sizeof(Vector3f) * scene.meshes[i].vertices.size(),
                     scene.meshes[i].vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0 /* layout index */,
                              3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        unsigned int VBO_color;
        glGenBuffers(1, &VBO_color);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_color);
        glBufferData(GL_ARRAY_BUFFER,
                     sizeof(Vector3f) * scene.meshes[i].vertex_colors.size(),
                     scene.meshes[i].vertex_colors.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(1 /* layout index */,
                              3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        unsigned int VBO_normal;
        glGenBuffers(1, &VBO_normal);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_normal);
        glBufferData(GL_ARRAY_BUFFER,
                     sizeof(Vector3f) * scene.meshes[i].vertex_normals.size(),
                     scene.meshes[i].vertex_normals.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(2 /* layout index */,
                              3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(2);

        unsigned int EBO;
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     sizeof(Vector3i) * scene.meshes[0].faces.size(),
                     scene.meshes[0].faces.data(), GL_STATIC_DRAW);
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_FRAMEBUFFER_SRGB);
    float last_time = (float)glfwGetTime();
    while(!glfwWindowShouldClose(window)) {
        float current_time = (float)glfwGetTime();
        float delta_time = current_time - last_time;
        last_time = current_time;
        process_input(window, delta_time);

        // Setup camera transformation matrix
        Real a = Real(width) / Real(height);
        Real s = scene.camera.s;
        Real zfar = scene.camera.z_far;
        Real znear = scene.camera.z_near;
        Matrix4x4f projection;
        projection(0, 0) = 1.f / (a * s);
        projection(1, 1) = 1.f / s;
        projection(2, 2) = -zfar / (zfar - znear);
        projection(2, 3) = -zfar * znear / (zfar - znear);
        projection(3, 2) = -1.f;

        glClearColor(scene.background.x,
                     scene.background.y,
                     scene.background.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Matrix4x4f view = inverse(scene.camera.cam_to_world);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"),
                           1, GL_FALSE /* transpose */, projection.ptr());
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"),
                           1, GL_FALSE /* transpose */, view.ptr());
        const Matrix4x4f &to_world = scene.camera.cam_to_world;
        glUniform3f(glGetUniformLocation(shaderProgram, "viewPos"),
                    to_world(0, 3), to_world(1, 3), to_world(2, 3));

        glUseProgram(shaderProgram);
        for (int i = 0; i < (int)scene.meshes.size(); i++) {
            const Matrix4x4f &model = scene.meshes[i].model_matrix;
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"),
                               1, GL_FALSE /* transpose */, model.ptr());
            glBindVertexArray(VAOs[i]);
            glDrawElements(GL_TRIANGLES, 3 * scene.meshes[i].faces.size(), GL_UNSIGNED_INT, 0);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
}
