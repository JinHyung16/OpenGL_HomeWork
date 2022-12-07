//201821107 미디어학과 김진형 과제6

#define GLEW_STATIC
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <vector>
#include "j3a.hpp"
#include "toys.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;
using namespace glm;

Program program; //about rendering
Program shadowProg; //about shadow

GLuint verBuf = 0;
GLuint normBuf = 0;
GLuint tcoordBuf = 0;
GLuint verArray = 0;
GLuint triBuf = 0;
GLuint texID = 0;
GLuint bumpID = 0;

GLuint fbo = 0;
GLuint shadowTex = 0;
GLuint shadowDepth = 0;

const float PI = 3.14f;

vec3 lightPos = vec3(3, 5, 5);
vec3 lightInt = vec3(1, 1, 1);
vec3 color = vec3(1, 0.3f, 0);
vec3 ambInt = vec3(0.1f, 0.1f, 0.1f);
float shin = 20;

float theta = 0;
float pit = 0;
float fovy = 60;
float camDistance = 1;

vec3 cameraPos = vec3(0, 0, 5);

double preX;
double preY;

const int shadowMapSize = 1024;

void CursorCallBack(GLFWwindow* window, double xpos, double ypos)
{
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1))
    {
        theta -= (xpos - preX) / width * PI;
        pit -= (ypos - preY) / height * PI;

        if (theta <= -90)
        {
            theta = -90;
        }
        else if (theta >= 90)
        {
            theta = 90;
        }
        printf("%f\n", theta);
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
    glEnable(GL_DEPTH_TEST);
    GLuint loc;
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    mat4 modelMat = mat4(1);

    //make a shadow buffer
    mat4 shadowMVP = ortho(-2.f, 2.f, -2.f, 2.f, 0.01f, 10.f) * lookAt(lightPos, vec3(0), vec3(0, 1, 0)) * modelMat;

    glUseProgram(shadowProg.programID);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, shadowMapSize, shadowMapSize);
    glClearColor(1, 1, 1, 1); //검은 배경 지우기
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    loc = glGetUniformLocation(shadowProg.programID, "shadowMVP");
    glUniformMatrix4fv(loc, 1, false, glm::value_ptr(shadowMVP));
    glBindVertexArray(verArray);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triBuf);
    glDrawElements(GL_TRIANGLES, nTriangles[0] * 3, GL_UNSIGNED_INT, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    vec3 cam = vec3(rotate(theta, vec3(0, 1, 0)) * rotate(pit, vec3(1, 0, 0)) * vec4(cameraPos * camDistance, 1));
    mat4 viewMat = lookAt(cam, vec3(0, 0, 0), vec3(0, 1, 0));
    mat4 projMat = perspective(fovy * PI / 180, width / (float)height, 0.1f, 100.f);

    glViewport(0, 0, width, height);
    glClearColor(0, 0, 0.2f, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_FRAMEBUFFER_SRGB);

    glUseProgram(program.programID);
    loc = glGetUniformLocation(program.programID, "modelMat");
    glUniformMatrix4fv(loc, 1, false, glm::value_ptr(modelMat));
    loc = glGetUniformLocation(program.programID, "viewMat");
    glUniformMatrix4fv(loc, 1, false, glm::value_ptr(viewMat));
    loc = glGetUniformLocation(program.programID, "projMat");
    glUniformMatrix4fv(loc, 1, false, glm::value_ptr(projMat));


    loc = glGetUniformLocation(program.programID, "lightPos");
    glUniform3fv(loc, 1, glm::value_ptr(lightPos));
    loc = glGetUniformLocation(program.programID, "lightInt");
    glUniform3fv(loc, 1, glm::value_ptr(lightInt));
    loc = glGetUniformLocation(program.programID, "ambInt");
    glUniform3fv(loc, 1, glm::value_ptr(ambInt));
    loc = glGetUniformLocation(program.programID, "color");
    glUniform3fv(loc, 1, glm::value_ptr(diffuseColor[0]));
    loc = glGetUniformLocation(program.programID, "specularColor");
    glUniform3fv(loc, 1, glm::value_ptr(specularColor[0]));
    loc = glGetUniformLocation(program.programID, "shininess");
    glUniform1f(loc, shininess[0]);

    loc = glGetUniformLocation(program.programID, "cam");
    glUniform3fv(loc, 1, glm::value_ptr(cam));

    //texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texID);
    loc = glGetUniformLocation(program.programID, "colorTexture");
    glUniform1i(loc, 0);

    //bumpMap
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, bumpID);
    loc = glGetUniformLocation(program.programID, "bumpTexture");
    glUniform1i(loc, 1);

    //shadowMap
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, shadowTex);
    loc = glGetUniformLocation(program.programID, "shadowMap");
    glUniform1i(loc, 2);
    loc = glGetUniformLocation(program.programID, "shadowMVP");
    glUniformMatrix4fv(loc, 1, false, value_ptr(shadowMVP));

    glBindVertexArray(verArray);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triBuf);
    //점의 갯수가 늘수록 숫자도 바뀌어야함
    glDrawElements(GL_TRIANGLES, nTriangles[0] * 3, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);
}

void init()
{
    shadowProg.loadShaders("shadow.vert", "shadow.frag");

    //about shadow
    glGenTextures(1, &shadowTex);
    glBindTexture(GL_TEXTURE_2D, shadowTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, 1024, 1024, 0, GL_RGB, GL_FLOAT, 0);
    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glGenTextures(1, &shadowDepth);
    glBindTexture(GL_TEXTURE_2D, shadowDepth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, 1024, 1024, 0, GL_RGB, GL_FLOAT, 0);
    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, shadowTex, 0);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowDepth, 0);
    GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, drawBuffers);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("FBO Error\n");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);

    program.loadShaders("shader.vert", "shader.frag");
    loadJ3A("/OpenGLTest/Archive/dwarf.j3a");
    int w, h, n;
    stbi_set_flip_vertically_on_load(true);
    void* buf = stbi_load(("/OpenGLTest/Archive/" + diffuseMap[0]).c_str(), &w, &h, &n, 4);
    printf("Image: %dx%dx%d\n", w, h, n);

    //set texID
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, buf);
    stbi_image_free(buf);

    //set bumpID
    buf = stbi_load(("/OpenGLTest/Archive/" + bumpMap[0]).c_str(), &w, &h, &n, 4);
    glGenTextures(1, &bumpID);
    glBindTexture(GL_TEXTURE_2D, bumpID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, buf);
    stbi_image_free(buf);

    //set a vertex
    glGenBuffers(1, &verBuf);
    glBindBuffer(GL_ARRAY_BUFFER, verBuf);
    glBufferData(GL_ARRAY_BUFFER, nVertices[0] * sizeof(glm::vec3), vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &normBuf);
    glBindBuffer(GL_ARRAY_BUFFER, normBuf);
    glBufferData(GL_ARRAY_BUFFER, nVertices[0] * sizeof(glm::vec3), normals[0], GL_STATIC_DRAW);

    glGenBuffers(1, &tcoordBuf);
    glBindBuffer(GL_ARRAY_BUFFER, tcoordBuf);
    glBufferData(GL_ARRAY_BUFFER, nVertices[0] * sizeof(glm::vec2), texCoords[0], GL_STATIC_DRAW);

    glGenVertexArrays(1, &verArray);
    glBindVertexArray(verArray);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, verBuf);
    glVertexAttribPointer(0, 3, GL_FLOAT, 0, 0, 0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, normBuf);
    glVertexAttribPointer(1, 3, GL_FLOAT, 0, 0, 0);

    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, tcoordBuf);
    glVertexAttribPointer(2, 2, GL_FLOAT, 0, 0, 0);

    //set a Triangles
    glGenBuffers(1, &triBuf);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triBuf);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, nTriangles[0] * sizeof(glm::u32vec3), triangles[0], GL_STATIC_DRAW);
}

int main(void)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    if (!glfwInit())
    {
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(1024, 1024, "ShadowMapping", nullptr, nullptr);

    glfwSetCursorPosCallback(window, CursorCallBack);
    glfwSetScrollCallback(window, ScrollCallBack);
    glfwMakeContextCurrent(window);

    //glfwSwapInterval 값을 0으로 가게 할수록 빨라짐
    glfwSwapInterval(1);
    glewInit();
    init();
    while (!glfwWindowShouldClose(window))
    {
        render(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
}