//201821107 미디어학과 김진형 과제3

#define GLEW_STATIC
#include <GL/glew.h>
#include<glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <vector>
#include "toys.h"
#include <cmath>
#include "j3a.hpp"

using namespace std;
using namespace glm;

Program program;
GLuint verBuf = 0;
GLuint verArray = 0;
GLuint triBuf = 0;

const float PI = 3.14f;

float theta = 0;
float pit = 0;
float fovy = 60;
float camDistance = 1;

vec3 cameraPos = vec3(0, 0, 5);

double preX;
double preY;

void CursorCallBack(GLFWwindow* window, double xpos, double ypos)
{
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1))
    {
        theta -= (xpos - preX) / width * PI;
        pit -= (ypos - preY) / height * PI;

        if (pit <= -90)
        {
            pit = -90;
        }
        else if (pit >= 90)
        {
            pit = 90;
        }
        printf("%f\n", pit);
    }
    preX = xpos;
    preY = ypos;
}

void ScrollCallBack(GLFWwindow* window, double xoffset, double yoffset)
{
    //zoom 기능
    fovy = fovy * pow(1.01, yoffset);
    if (fovy <= 10)
    {
        fovy = 10;
    }
    else if (fovy >= 170)
    {
        fovy = 170;
    }
    printf("%f\n", fovy);
    //dolly 기능
    //camDistance = camDistance * pow(1.01, yoffset);
}

void render(GLFWwindow* window)
{
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    vec3 cam = vec3(rotate(theta, vec3(0, 1, 0)) * rotate(pit, vec3(1, 0, 0)) * vec4(cameraPos * camDistance, 1));
    mat4 viewMat = lookAt(cam, vec3(0, 0, 0), vec3(0, 1, 0));
    mat4 proMat = perspective(fovy * PI / 180, width / (float)height, 0.1f, 100.f);
    mat4 modelMat = mat4(1);

    glViewport(0, 0, width, height);
    glClearColor(0, 0, 0.2f, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(program.programID);

    GLuint loc;
    //위치 받아서 넘기기
    loc = glGetUniformLocation(program.programID, "modelMat");
    glUniformMatrix4fv(loc, 1, false, value_ptr(modelMat));
    loc = glGetUniformLocation(program.programID, "viewMat");
    glUniformMatrix4fv(loc, 1, false, value_ptr(viewMat));
    loc = glGetUniformLocation(program.programID, "proMat");
    glUniformMatrix4fv(loc, 1, false, value_ptr(proMat));

    glBindVertexArray(verArray);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triBuf);
    //점의 갯수가 늘수록 숫자도 바뀌어야함
    glDrawElements(GL_TRIANGLES, nTriangles[0] * 3, GL_UNSIGNED_INT, 0);
    glfwSwapBuffers(window);
}

void init()
{
    program.loadShaders("shader.vert", "shader.frag");
    loadJ3A("/OpenGLTest/Archive/dwarf.j3a"); // if you change the 3D modeling, plz modify here
    //set a vertex
    glGenBuffers(1, &verBuf);
    glBindBuffer(GL_ARRAY_BUFFER, verBuf);
    glBufferData(GL_ARRAY_BUFFER, nVertices[0] * sizeof(glm::vec3), vertices[0], GL_STATIC_DRAW);

    glGenVertexArrays(1, &verArray);
    glBindVertexArray(verArray);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, verBuf);
    glVertexAttribPointer(0, 3, GL_FLOAT, 0, 0, 0);

    //set a Triangles
    glGenBuffers(1, &triBuf);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triBuf);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, nTriangles[0] * sizeof(glm::u32vec3), triangles[0], GL_STATIC_DRAW);
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

    GLFWwindow* window = glfwCreateWindow(800, 800, "ViewingTest", nullptr, nullptr);

    glfwSetCursorPosCallback(window, CursorCallBack);
    glfwSetScrollCallback(window, ScrollCallBack);

    glfwMakeContextCurrent(window);
    glewInit();
    //glfwSwapInterval 값을 0으로 가게 할수록 빨라짐
    glfwSwapInterval(1);
    init();
    while (!glfwWindowShouldClose(window))
    {
        render(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
}