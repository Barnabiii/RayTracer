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
#define TEXTURE_PATH "container.jpg"

typedef struct {
    const float* vertices;
    size_t vertexSize;
    const unsigned int* indices;
    size_t indexSize;
} MeshData;

typedef struct {
    GLuint texture;
    GLuint shader;
} MeshMaterial;

typedef struct {
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    GLsizei indexCount;
} MeshBuffers;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

char* get_shader_content(const char* fileName) {
    FILE *fp = fopen(fileName, "rb");
    if (!fp) return NULL;

    if (fseek(fp, 0, SEEK_END) != 0) { fclose(fp); return NULL; }
    long size = ftell(fp);
    if (size < 0) { fclose(fp); return NULL; }
    rewind(fp);

    char* shaderContent = malloc((size_t)size + 1);
    if (!shaderContent) { fclose(fp); return NULL; }

    size_t read = fread(shaderContent, 1, (size_t)size, fp);
    shaderContent[read] = '\0';
    fclose(fp);
    return shaderContent;
}

GLuint createShader(const char* vertexShaderSource, const char* fragmentShaderSource) {
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    glDetachShader(program, vertexShader);
    glDeleteShader(vertexShader);
    glDetachShader(program, fragmentShader);
    glDeleteShader(fragmentShader);

    return program;
}

GLuint loadTexture(char* texture_path) {
    GLuint texture;
    glGenTextures(1, &texture);  
    glBindTexture(GL_TEXTURE_2D, texture);  

    int width, height, nrChannels;
    unsigned char *data = stbi_load(texture_path, &width, &height, &nrChannels, 0); 
    if(!data) {
        fprintf(stderr,"Failed to load Texture");
        glDeleteTextures(1, &texture);
        return -1;
    }
    GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
    return texture;
}

void bindBuffers(MeshBuffers* mbuf, MeshData* data) {
    glBindVertexArray(mbuf->vao);
    
    glBindBuffer(GL_ARRAY_BUFFER, mbuf->vbo);
    glBufferData(GL_ARRAY_BUFFER, data->vertexSize, data->vertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mbuf->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, data->indexSize, data->indices, GL_STATIC_DRAW); 

    
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
}

MeshBuffers CreateGPUMesh(MeshData* data) {
    MeshBuffers m = {0};
    glGenVertexArrays(1, &m.vao);
    glGenBuffers(1, &m.vbo);
    glGenBuffers(1, &m.ebo);
    
    bindBuffers(&m, data);
    m.indexCount = data->indexSize / sizeof(unsigned int);
    return m;
}

void drawMesh(MeshBuffers* mbuf, MeshMaterial* mMat) {

    glBindTexture(GL_TEXTURE_2D, mMat->texture);

    glUseProgram(mMat->shader);

    glBindVertexArray(mbuf->vao);
    //glDrawArrays(GL_TRIANGLES, 1, 3);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

}

int main(void) {
    if(!glfwInit()) {
        fprintf(stderr,"Failed to initialize glfw");
        return -1;
    }

    // Request an OpenGL 3.3 Core context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Triangle (C)", NULL, NULL);
    if (window == NULL) {
        fprintf(stderr,"Failed to create GLFW window");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window); 

    if(!gladLoadGL()) {
        fprintf(stderr,"Failed to initialize GLAD");
        glfwTerminate();
        return -1;
    }

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

    const char* vertexShaderSource = get_shader_content(VERTEX_SHADER_PATH);
    const char* fragmentShaderSource = get_shader_content(FRAG_SHADER_PATH);
    if (!vertexShaderSource || !fragmentShaderSource) {
    fprintf(stderr, "Failed to load shader files\n");
        if (vertexShaderSource) free((void*)vertexShaderSource);
        if (fragmentShaderSource) free((void*)fragmentShaderSource);
        return -1;
    }

    GLuint texture = loadTexture(TEXTURE_PATH);
    if (texture == -1) {
        fprintf(stderr, "Texture load failed\n");
    }

    GLuint shaderProgram = createShader(vertexShaderSource,fragmentShaderSource);

    MeshData quad = {vertices, sizeof(vertices), indices, sizeof(indices)};
    MeshBuffers quadbuf = CreateGPUMesh(&quad);
    MeshMaterial quadMat = {0};
    quadMat.texture = texture;
    quadMat.shader = shaderProgram;

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        drawMesh(&quadbuf,&quadMat);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &quadbuf.vao);
    glDeleteBuffers(1, &quadbuf.vbo);
    glDeleteBuffers(1, &quadbuf.ebo);
    glDeleteTextures(1, &texture);
    glDeleteProgram(shaderProgram);
    free((void*)vertexShaderSource);
    free((void*)fragmentShaderSource);
    glfwTerminate();
    return 0;
}
