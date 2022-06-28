//201821107 미디어학과 김진형 과제1

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <math.h>
#include "toys.h"

using namespace std;

//step 값 따라 모양 결정됨
const float step = 10000;
//theta 범위에 따른 각도
const float angle = 3.14 * 2.f;

//중점 및 반지름
float posX = 0.f;
float posY = 0.f;
float radius = 0.8f;

Program program;
GLuint verBuf = 0;
GLuint verArray = 0;
GLuint triBuf = 0;

void render(GLFWwindow* window)
{
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glClearColor(0, 0, 0.2f, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(program.programID);
    glBindVertexArray(verArray);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triBuf);
    // 점의 갯수가 늘수록 숫자도 바뀌어야함
    glDrawElements(GL_TRIANGLES, step * 3, GL_UNSIGNED_INT, 0);
    glfwSwapBuffers(window);
}

void init()
{
    program.loadShaders("shader.vert", "shader.frag");

    vector<glm::vec3> vert;
    vert.push_back(glm::vec3(posX, posY, 0));

    for (int i = 0; i <= step; i++)
    {
        posX = radius * cos(angle * i / step);
        posY = radius * sin(angle * i / step);
        vert.push_back(glm::vec3(posX, posY, 0));
    }

    vector<glm::u32vec3> triangles;
    for (int i = 0; i <= step; i++)
    {
        triangles.push_back(glm::u32vec3(0, i + 1, i + 2));
    }

    //Set a vertex
    glGenBuffers(1, &verBuf);
    glBindBuffer(GL_ARRAY_BUFFER, verBuf);
    glBufferData(GL_ARRAY_BUFFER, vert.size() * sizeof(glm::vec3), vert.data(), GL_STATIC_DRAW);

    glGenVertexArrays(1, &verArray);
    glBindVertexArray(verArray);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, verBuf);
    glVertexAttribPointer(0, 3, GL_FLOAT, 0, 0, 0);

    //Set a Triangles
    glGenBuffers(1, &triBuf);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triBuf);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangles.size() * sizeof(glm::u32vec3), triangles.data(), GL_STATIC_DRAW);
}

int main(void)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    if (!glfwInit())
    {
        return -1;
    }
    GLFWwindow* window = glfwCreateWindow(800, 800, "CircleTest", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glewInit();
    glfwSwapInterval(10);
    init();
    while (!glfwWindowShouldClose(window))
    {
        render(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
}