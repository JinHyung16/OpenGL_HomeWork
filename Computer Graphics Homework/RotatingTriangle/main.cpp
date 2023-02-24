//201821107 미디어학과 김진형 과제2

#define GLEW_STATIC
#include <GL/glew.h>
#include<glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include "toys.h"
#include <cmath>

using namespace std;
using namespace glm;

Program program;
GLuint verBuf = 0;
GLuint verArray = 0;
GLuint triBuf = 0;

float rotateAngle = 0.f;

void render(GLFWwindow* window)
{
    GLuint loc;
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glClearColor(0, 0, 0.2f, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(program.programID);

    //위치 받아서 넘기기
    loc = glGetUniformLocation(program.programID, "transf");
    mat3 transf = mat3(cos(rotateAngle), sin(rotateAngle), 0, (-1) * sin(rotateAngle), cos(rotateAngle), 0, 0, 0, 1);
    glUniformMatrix3fv(loc, 1, false, value_ptr(transf));

    glBindVertexArray(verArray);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triBuf);
    //점의 갯수가 늘수록 숫자도 바뀌어야함
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
    glfwSwapBuffers(window);
}

void init()
{
    program.loadShaders("shader.vert", "shader.frag");

    vector<vec3> vert;
    vert.push_back(vec3(0, 0.7, 0));
    vert.push_back(vec3(-0.7, -0.7, 0));
    vert.push_back(vec3(0.7, -0.7, 0));


    vector<u32vec3> triangles;
    triangles.push_back(u32vec3(0, 1, 2));

    //set a vertex
    glGenBuffers(1, &verBuf);
    glBindBuffer(GL_ARRAY_BUFFER, verBuf);
    glBufferData(GL_ARRAY_BUFFER, vert.size() * sizeof(glm::vec3), vert.data(), GL_STATIC_DRAW);

    glGenVertexArrays(1, &verArray);
    glBindVertexArray(verArray);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, verBuf);
    glVertexAttribPointer(0, 3, GL_FLOAT, 0, 0, 0);

    //set a Triangles
    glGenBuffers(1, &triBuf);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triBuf);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangles.size() * sizeof(u32vec3), triangles.data(), GL_STATIC_DRAW);
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
    GLFWwindow* window = glfwCreateWindow(800, 800, "RotationTest", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glewInit();
    //glfwSwapInterval 값을 0으로 가게 할수록 빨라짐
    glfwSwapInterval(1);
    init();
    while (!glfwWindowShouldClose(window))
    {
        //회전 시키기
        rotateAngle += (5.0f / 180.f * glm::pi<float>());
        render(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
}