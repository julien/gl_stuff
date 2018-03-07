#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <time.h>
#include "utils.h"

int g_viewport_width = 1024;
int g_viewport_height = 768;

unsigned const VERTEX_SIZE = 9;
unsigned const int MAX_BATCH = 10922;
unsigned const int VERTEX_DATA_SIZE = VERTEX_SIZE * MAX_BATCH * 6;
unsigned const int VERTICES_PER_QUAD = 4;

GLfloat view_matrix[16] = {
    2.0f / (float) g_viewport_width, 0.0f, 0.0f, 0.0f,
    0.0f, -2.0f / (float) g_viewport_height, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 0.0f, 0.0f
};

GLfloat vertexData[VERTEX_DATA_SIZE];
GLfloat vPositionData[VERTEX_DATA_SIZE];
GLfloat vColorData[VERTEX_DATA_SIZE];
unsigned int count = 0;

void draw_img(float x, float y, float w, float h, float r,
               float tx, float ty, float sx, float sy,
               float u0, float v0, float u1, float v1) {

    float x0 = x;
    float y0 = y;
    float x1 = x + w;
    float y1 = y + h;
    float x2 = x;
    float y2 = y + h;
    float x3 = x + w;
    float y3 = y;
    unsigned int offset = 0;


    offset = count * VERTEX_SIZE;

    // rotation | translation | scale | position | uv | color
    // Vertex 1
    vPositionData[offset++] = r;
    vPositionData[offset++] = tx;
    vPositionData[offset++] = ty;
    vPositionData[offset++] = sx;
    vPositionData[offset++] = sy;
    vPositionData[offset++] = x0;
    vPositionData[offset++] = y0;
    vPositionData[offset++] = u0;
    vPositionData[offset++] = v0;

    // Vertex 2
    vPositionData[offset++] = r;
    vPositionData[offset++] = tx;
    vPositionData[offset++] = ty;
    vPositionData[offset++] = sx;
    vPositionData[offset++] = sy;
    vPositionData[offset++] = x1;
    vPositionData[offset++] = y1;
    vPositionData[offset++] = u1;
    vPositionData[offset++] = v1;

    // Vertex 3
    vPositionData[offset++] = r;
    vPositionData[offset++] = tx;
    vPositionData[offset++] = ty;
    vPositionData[offset++] = sx;
    vPositionData[offset++] = sy;
    vPositionData[offset++] = x2;
    vPositionData[offset++] = y2;
    vPositionData[offset++] = u0;
    vPositionData[offset++] = v1;

    // Vertex 4
    vPositionData[offset++] = r;
    vPositionData[offset++] = tx;
    vPositionData[offset++] = ty;
    vPositionData[offset++] = sx;
    vPositionData[offset++] = sy;
    vPositionData[offset++] = x3;
    vPositionData[offset++] = y3;
    vPositionData[offset++] = u1;
    vPositionData[offset++] = v0;

    if (++count >= MAX_BATCH) {
        printf("match batch reached\n");
        count = 0;
    }
}


int main() {
    srand(time(NULL));

    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(
                             g_viewport_width, g_viewport_height, "  ", NULL, NULL);

    GLFWmonitor *mon = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = glfwGetVideoMode(mon);

    int wx = (int)((mode->width - g_viewport_width) * 0.5);
    int wy = (int)((mode->height - g_viewport_height) * 0.5);

    glfwSetWindowPos(window, wx, wy);
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    glewInit();

    GLuint sp = create_program("vert.glsl", "frag.glsl");
    glUseProgram(sp);

    // TODO: use indices + GL_ARRAY_ELEMENT_BUFFER


    // Buffers
    // rotation|translation|scale|position|uv|color
    int fsize = sizeof(GLfloat);
    int stride = fsize * 5;

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint r_vbo, t_vbo, s_vbo, p_vbo, uv_vbo, c_vbo;
    glGenBuffers(1, &r_vbo);
    glGenBuffers(1, &t_vbo);
    glGenBuffers(1, &s_vbo);
    glGenBuffers(1, &p_vbo);
    glGenBuffers(1, &uv_vbo);
    glGenBuffers(1, &c_vbo);

    // rotation
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, stride, NULL);
    // translation
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride * 2, (void*) 2);
    // scale
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride * 4, (void*) 6);
    // position
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, stride * 6, (void*) 10);
    // uv
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, stride * 8, (void*) 14);
    // color
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, stride * 10, (void*) 18);

    // Rendering settings
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GLint u_matrix = glGetUniformLocation(sp, "u_matrix");
    glUniformMatrix4fv(u_matrix, 1, GL_FALSE, view_matrix);

    // GLint u_image = glGetUniformLocation(sp, "u_image");
    // GLuint tex;
    // load_texture("dude.png", &tex, 0);
    // glUniform1i(u_image, 0);
    // dont forget to set glActiveTexture

    glClearColor(0.0, 0.0, 0.0, 1.0);
    draw_img(10, 10, 500, 500, 0, 0, 0, 1, 1, 0, 0, 10, 10);

    while (!glfwWindowShouldClose(window)) {
        // static double previous_seconds = glfwGetTime();
        // double current_seconds = glfwGetTime();
        // double elapsed_seconds = current_seconds - previous_seconds;
        // previous_seconds = current_seconds;

        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        glViewport(0, 0, w, h);

        glClear(GL_COLOR_BUFFER_BIT);

        // printf("count: %d\n", count);

        glBindVertexArray(vao);
        glBufferData(GL_ARRAY_BUFFER, count * VERTEX_SIZE * sizeof(GLfloat),
                     vPositionData, GL_DYNAMIC_DRAW);
        glDrawArrays(GL_TRIANGLES, 0, 1 * VERTICES_PER_QUAD);

        glfwPollEvents();
        if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE)) {
            glfwSetWindowShouldClose(window, 1);
        }
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}
