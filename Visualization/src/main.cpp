#include <iostream>
#include <stdio.h>
#include <math.h>

#include "glad/glad.h"
#include "glfw3.h"
#include "stb_image.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Shader.hpp"
#include "Cube.hpp"
#include "ObjectCollection.hpp"

float resolution[2] = {800.0f, 600.0f};
float asprat = resolution[0] / resolution[1];

const char *vertexShaderPath = "resources/shaders/vshader.glsl";
const char *fragmentShaderPath = "resources/shaders/fshader.glsl";

float deltaTime = 0.0f;
float lastFrame = 0.0f;

const float CAMERA_MAX_SPEED = 25.0f;
const float CAMERA_ACCEL = 0.10f;
const float CAMERA_DECEL_FACTOR = 0.95f;
const float CAMERA_SENSITIVITY = 0.1f;
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, -3.0f);
glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 1.0f);
glm::vec3 cameraUp = glm::cross(cameraDirection, glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), cameraDirection)));
glm::vec3 cameraSpeed = glm::vec3(0.0f, 0.0f, 0.0f);
bool firstMouse = true;
float lastX = resolution[0]/2.0;
float lastY = resolution[1]/2.0;
float yaw = 90.0f;
float pitch = 0.0f;

glm::mat4 viewmat, projmat, transform;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    resolution[0] = width;
    resolution[1] = height;
    asprat = resolution[0] / resolution[1];
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    xoffset *= CAMERA_SENSITIVITY;
    yoffset *= CAMERA_SENSITIVITY;

    yaw += xoffset;
    pitch += yoffset;

    pitch = glm::clamp(pitch, -89.0f, 89.0f);

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraDirection = glm::normalize(direction);
    std::cout << "POS: " << glm::to_string(cameraPos) << " PITCH/YAW: " << pitch << " | " << yaw << " | DIR: " << glm::to_string(cameraDirection) << std::endl;
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        if (cameraSpeed[2] < 0) { cameraSpeed[2] *= CAMERA_DECEL_FACTOR; }
        cameraSpeed[2] = cameraSpeed[2] + CAMERA_ACCEL;
    } else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        if (cameraSpeed[2] > 0) { cameraSpeed[2] *= CAMERA_DECEL_FACTOR; }
        cameraSpeed[2] = cameraSpeed[2] - CAMERA_ACCEL;
    } else { cameraSpeed[2] *= CAMERA_DECEL_FACTOR; }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        if (cameraSpeed[0] > 0) { cameraSpeed[0] *= CAMERA_DECEL_FACTOR; }
        cameraSpeed[0] = cameraSpeed[0] - CAMERA_ACCEL;
    } else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        if (cameraSpeed[0] < 0) { cameraSpeed[0] *= CAMERA_DECEL_FACTOR; }
        cameraSpeed[0] = cameraSpeed[0] + CAMERA_ACCEL;
    } else { cameraSpeed[0] *= CAMERA_DECEL_FACTOR; }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        if (cameraSpeed[1] < 0) { cameraSpeed[1] *= CAMERA_DECEL_FACTOR; }
        cameraSpeed[1] = cameraSpeed[1] + CAMERA_ACCEL;
    } else if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        if (cameraSpeed[1] > 0) { cameraSpeed[1] *= CAMERA_DECEL_FACTOR; }
        cameraSpeed[1] = cameraSpeed[1] - CAMERA_ACCEL;
    } else { cameraSpeed[1] *= CAMERA_DECEL_FACTOR; }
    cameraSpeed = glm::step(0.00001f, glm::abs(cameraSpeed)) * cameraSpeed;
    cameraSpeed = glm::clamp(cameraSpeed, -CAMERA_MAX_SPEED, CAMERA_MAX_SPEED);
}

int loadTexture(const char *texturePath) {
    stbi_set_flip_vertically_on_load(true);
    int twidth, theight, tchan;
    unsigned char *tdata = stbi_load(texturePath, &twidth, &theight, &tchan, 0);
    unsigned int texture;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // Texture wrapping: repeat
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // Use mipmaps for distant textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Use linear filtering for close objects
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, twidth, theight, 0, GL_RGB, GL_UNSIGNED_BYTE, tdata);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(tdata);
    return texture;
}

int main(int argc, char** argv) {
    std::cout << "Hello, world" << std::endl;
    
    // Initialize GLFW and configure it to use version 3.3 with OGL Core Profile
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Create the Window object and set it to the current Context
    GLFWwindow* window = glfwCreateWindow(resolution[0], resolution[1], "Modular Robotics", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // Register window callbacks
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    // Invoke GLAD to load addresses of OpenGL functions in the GPU drivers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Establish the Viewport
    glViewport(0, 0, resolution[0], resolution[1]);
    // Enable z-buffer depth testing and transparency
    glEnable(GL_DEPTH_TEST);

    Shader shader = Shader(vertexShaderPath, fragmentShaderPath);
    int texture = loadTexture("resources/textures/face_debug.png");
    unsigned int VAO = _createCubeVAO();

    ObjectCollection* cubes = new ObjectCollection(&shader, VAO, texture);
    cubes->addObj(new Cube(1.0f, 0.0f, 0.0f));  // Bottom layer
    cubes->addObj(new Cube(0.0f, 0.0f, 0.0f));
    cubes->addObj(new Cube(0.0f, 0.0f, -1.0f));
    cubes->addObj(new Cube(0.0f, 1.0f, -1.0f));  // Middle layer part 1
    cubes->addObj(new Cube(0.0f, 1.0f, -2.0f));
    cubes->addObj(new Cube(1.0f, 1.0f, -2.0f));
    cubes->addObj(new Cube(1.0f, 2.0f, -2.0f)); // Top layer
    cubes->addObj(new Cube(2.0f, 2.0f, -2.0f));
    cubes->addObj(new Cube(2.0f, 2.0f, -1.0f));
    cubes->addObj(new Cube(2.0f, 1.0f, -1.0f));  // Middle layer part 2
    cubes->addObj(new Cube(2.0f, 1.0f, 0.0f));
    cubes->addObj(new Cube(1.0f, 1.0f, 0.0f));

    projmat;
    viewmat = glm::mat4(1.0f);
    transform = glm::mat4(1.0f);
    projmat = glm::perspective(glm::radians(45.0f), asprat, 0.1f, 100.0f);
    transform = glm::scale(transform, glm::vec3(0.9f, 0.9f, 0.9f));


    while(!glfwWindowShouldClose(window)) {
        // -- Input ---
        
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        cameraPos += (cameraSpeed.z * cameraDirection * deltaTime);
        cameraPos += (cameraSpeed.x * glm::cross(cameraDirection, cameraUp) * deltaTime);
        cameraPos += (cameraSpeed.y * cameraUp * deltaTime);
        viewmat = glm::lookAt(cameraPos, cameraPos + cameraDirection, cameraUp);

        // -- Rendering --
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // std::cout << glm::to_string(viewmat) << std::endl;
        cubes->drawAll();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);

    glfwTerminate();
    std::cout << "Goodbye, world\n";
    return 0;
}
