#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shader.h"
#include <iostream>
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out vec3 ourColor;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"   ourColor = aColor;\n"
"}\0";
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 ourColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(ourColor, 1.0);\n"
"}\0";
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    float vertices[] = {
        // λ��              // ��ɫ
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // ����
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // ����
         0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // ����
    };

    //unsigned int indices[] = { // ע��������0��ʼ! 
    //    0, 1, 3, // ��һ��������
    //    1, 2, 3  // �ڶ���������
    //};
    int  success_shader;
    int  success_link;
    char infoLog[512];

    int nrAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
    std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;

    //VBO
    unsigned int VBO;
    glGenBuffers(1, &VBO);

    //VAO
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);

    //EBO
    unsigned int EBO;
    glGenBuffers(1, &EBO);

    // 1. ��VAO
    glBindVertexArray(VAO);
    // 2. �Ѷ������鸴�Ƶ������й�OpenGLʹ��
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // 3. �������ǵ��������鵽һ�����������У���OpenGLʹ��
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // 4. ���ö�������ָ��
    glVertexAttribPointer(0,//Ҫ���õĶ�������
        3,//�������ԵĴ�С��һ���м���ֵ
        GL_FLOAT,//��������
        GL_FALSE,//�Ƿ��һ
        6 * sizeof(float),//�����������Ķ���������֮��ļ��
        (void*)0//���ݾ��뻺����ʼλ�õ�ƫ����
    );
    glEnableVertexAttribArray(0);//������������
    glVertexAttribPointer(1,//Ҫ���õĶ�������
        3,//�������ԵĴ�С��һ���м���ֵ
        GL_FLOAT,//��������
        GL_FALSE,//�Ƿ��һ
        6 * sizeof(float),//�����������Ķ���������֮��ļ��
        (void*)(3*sizeof(float))//���ݾ��뻺����ʼλ�õ�ƫ����
    );
    glEnableVertexAttribArray(1);//������������

    Shader ourShader("vshader.vs", "fshader.fs");

    while (!glfwWindowShouldClose(window))
    {
        //Input
        processInput(window);

        //Render
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        //Clear
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        //ShaderProgram & VAO||EBO
        ourShader.use();

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);
        //PollEvents and Swap
        glfwPollEvents();
        glfwSwapBuffers(window);
    }
    glfwTerminate();
    return 0;
}