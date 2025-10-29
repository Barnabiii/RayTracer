#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"

#define VERTEX_SHADER_PATH "shader/vertex_shader.glsl"
#define FRAG_SHADER_PATH "shader/fragment_shader.glsl"

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

char* get_shader_content(const char* fileName)
{
    FILE *fp;
    long size = 0;
    char* shaderContent;
    
    /* Read File to get size */
    fp = fopen(fileName, "rb");
    if(fp == NULL) {
        return "";
    }
    fseek(fp, 0L, SEEK_END);
    size = ftell(fp)+1;
    fclose(fp);

    /* Read File for Content */
    fp = fopen(fileName, "r");
    shaderContent = memset(malloc(size), '\0', size);
    fread(shaderContent, 1, size-1, fp);
    fclose(fp);

    return shaderContent;
}

int main(void) {
    const char* vertexShaderSource = get_shader_content(VERTEX_SHADER_PATH);
    const char* fragmentShaderSource = get_shader_content(FRAG_SHADER_PATH);

    glfwInit();

    // Request an OpenGL 3.3 Core context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Triangle (C)", NULL, NULL);
    if (window == NULL) {
        fprintf(stderr,"Failed to create GLFW window");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window); 

    gladLoadGL(); // init GLAD which loads OpenGL

    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);



    float vertices[] = {
        // positions            // colors              // texture coords
         0.5f,  0.5f,  0.0f,    1.0f,  0.0f,  0.0f,    1.0f,  1.0f, // top right
        -0.5f,  0.5f,  0.0f,    0.0f,  1.0f,  0.0f,    0.0f,  1.0f, // top left 
        -0.5f, -0.5f,  0.0f,    0.0f,  0.0f,  1.0f,    0.0f,  0.0f, // bottom left
         0.5f, -0.5f,  0.0f,    0.0f,  0.0f,  0.0f,    1.0f,  0.0f  // bottom right
    };

    unsigned int indices[] = {
        0,1,3,  //first triangle
        1,2,3   //second triangle
    };

    // -------------------- Compile and Link shaders (Vertex and Fragment) into a Shader Program --------------------
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Delete the individual shaders; they’re now linked into the program
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // -------------------- Generating a Texture --------------------
    unsigned int texture;
    glGenTextures(1, &texture);  
    glBindTexture(GL_TEXTURE_2D, texture);  

    int width, height, nrChannels;
    unsigned char *data = stbi_load("container.jpg", &width, &height, &nrChannels, 0); 
    if(!data) {
        fprintf(stderr,"Failed to load Texture");
        return -1;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    // -------------------- Create VBO and VAO --------------------
    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); 


    
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6* sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0);

    // -------------------- Render Loop --------------------
    while (!glfwWindowShouldClose(window)) {
        // Clear the screen
        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindTexture(GL_TEXTURE_2D, texture);

        glUseProgram(shaderProgram);

        glBindVertexArray(VAO);
        //glDrawArrays(GL_TRIANGLES, 1, 3);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // -------------------- Cleanup --------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}
