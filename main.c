#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <windows.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"

#define VERTEX_SHADER_PATH "shader/vertex_shader.glsl"
#define FRAG_SHADER_PATH "shader/fragment_shader.glsl"
#define COMPUTE_SHADER_PATH "shader/compute_shader.glsl"

const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 857;

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

void bindBuffers(MeshBuffers* mbuf, MeshData* data) {
    glBindVertexArray(mbuf->vao);
    
    glBindBuffer(GL_ARRAY_BUFFER, mbuf->vbo);
    glBufferData(GL_ARRAY_BUFFER, data->vertexSize, data->vertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mbuf->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, data->indexSize, data->indices, GL_STATIC_DRAW); 

    
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // UVs attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);

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

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "OpenGL Triangle (C)", NULL, NULL);
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

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    float vertices[] = {
    // positions           //UVs
    -1.0f, -1.0f,  0.0f,   0.0f,  0.0f,
    -1.0f,  1.0f,  0.0f,   0.0f,  1.0f,
     1.0f, -1.0f,  0.0f,   1.0f,  0.0f,
     1.0f,  1.0f,  0.0f,   1.0f,  1.0f
    };

    unsigned int indices[] = {
        0,1,2,  //first triangle
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

    GLuint screenTexture;
    glCreateTextures(GL_TEXTURE_2D, 1, &screenTexture);
    glTextureParameteri(screenTexture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(screenTexture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(screenTexture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(screenTexture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureStorage2D(screenTexture, 1, GL_RGBA32F, SCREEN_WIDTH, SCREEN_HEIGHT);
	glBindImageTexture(0, screenTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    GLuint screenShaderProgram = createShader(vertexShaderSource,fragmentShaderSource);

    MeshData quad = {vertices, sizeof(vertices), indices, sizeof(indices)};
    MeshBuffers quadbuf = CreateGPUMesh(&quad);

    //setup compute shader
    const char* computeShaderSource = get_shader_content(COMPUTE_SHADER_PATH);

    GLuint computeShader = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(computeShader, 1, &computeShaderSource, NULL);
    glCompileShader(computeShader);

    GLuint computeProgram = glCreateProgram();
    glAttachShader(computeProgram, computeShader);
    glLinkProgram(computeProgram);

    glDetachShader(computeProgram, computeShader);
    glDeleteShader(computeShader);

    double time = 0.0;
    double last_time = 0.0;
    long frameCount = 0;
    float avgFPS = 0;

    while (!glfwWindowShouldClose(window)) {
        Sleep(1);
        frameCount++;
        last_time = time;
        time = glfwGetTime();
        double delta = time-last_time;
        int fps = 1/delta;
        avgFPS =(((float)frameCount-1)/(float)frameCount)*avgFPS + fps/(float)frameCount;
        //printf("FPS: %d\n",fps);

        glUseProgram(computeProgram);
        glUniform1f(0, time);
        glDispatchCompute(ceil(SCREEN_WIDTH/8),ceil(SCREEN_HEIGHT/4),1);
        glMemoryBarrier(GL_ALL_BARRIER_BITS);

        glUseProgram(screenShaderProgram);
        glBindTextureUnit(0, screenTexture);
        glUniform1i(glGetUniformLocation(screenShaderProgram, "screen"), 0);
        glBindVertexArray(quadbuf.vao);
        glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(indices[0]), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    printf("avg framerate: %f\n",avgFPS);
    glDeleteVertexArrays(1, &quadbuf.vao);
    glDeleteBuffers(1, &quadbuf.vbo);
    glDeleteBuffers(1, &quadbuf.ebo);
    glDeleteTextures(1, &screenTexture);
    glDeleteProgram(screenShaderProgram);
    free((void*)vertexShaderSource);
    free((void*)fragmentShaderSource);
    free((void*)computeShaderSource);
    glfwTerminate();
    return 0;
}
