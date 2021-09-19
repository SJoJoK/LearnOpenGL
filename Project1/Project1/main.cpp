#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "mesh.h"
#include "model.h"
#include "camera.h"
#include "shader.h"
#include "stb_image.h"
using namespace glm;
using namespace std;
const int SCR_WIDTH = 1600, SCR_HEIGHT = 1200;
const int SHADOW_WIDTH = 1600, SHADOW_HEIGHT = 1200;
ostream& operator<<(std::ostream& out, const glm::vec4& v)
{
    out << '(' << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ')' << endl;
    return out;
}
ostream& operator<<(std::ostream& out, const glm::vec3& v)
{
    out << '(' << v.x << ", " << v.y << ", " << v.z << ')' << endl;
    return out;
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path);
GLFWwindow* my_init();
Camera camera(glm::vec3(0.0f, 0.0f, 10.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;
struct Material_Arr
{
    float ambient[3] = { 1.0f,0.5f,0.31f };
    float diffuse[3] = { 1.0f,0.5f,0.31f };
    float specular[3] = { 0.5f,0.5f,0.5f };
    float shininess = 32.0f;
};

struct Light_Arr
{
    float ambient[3] = { 0.2f,0.2f,0.2f };
    float diffuse[3] = { 0.5f,0.5f,0.5f };
    float specular[3] = { 1.0f,1.0f,1.0f };
};

struct PointLight_Arr {
    vec3 position = vec3(10.f, 10.f, 10.f);
    bool bulb_on = true;
    float bulb_radius = 3.f;
    float bulb_degree = 300.f;
    float bulb_height = 1.0f;
    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;

    float ambient[3] = { 0.05f,0.05f,0.05f };
    float diffuse[3] = { 0.8f,0.8f,0.8f };
    float specular[3] = { 1.0f,1.0f,1.0f };
};

struct DirLight_Arr {
    float direction[3] = { -0.2f,-1.0f,-0.3f };
    float ambient[3] = { 0.05f,0.05f,0.05f };
    float diffuse[3] = { 0.4f,0.4f,0.4f };
    float specular[3] = { 0.5f,0.5f,0.5f };
};

float bulb_vertices[] = {
    // positions          // normals           // texture coords
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
};

float quad_vertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
};
int main()
{
    glfwInit();
    const char* glsl_version = "#version 330";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    Shader NPRShader("vshader.glsl", "NPRshader.glsl");
    Shader bulbShader("vshader.glsl", "bulbshader.glsl");
    Shader depthShader("depthshader.glsl", "nullshader.glsl");
    Shader postShader("postVshader.glsl", "postFshader.glsl");
    unsigned int bulb_VBO;
    unsigned int bulb_VAO;
    glGenBuffers(1, &bulb_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, bulb_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(bulb_vertices), bulb_vertices, GL_STATIC_DRAW);
    glGenVertexArrays(1, &bulb_VAO);
    glBindVertexArray(bulb_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, bulb_VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // screen quad VAO
    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), &quad_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glm::mat4 identity;
    glm::mat4 model;
    glm::mat4 projection;
    glm::mat4 view;
    glm::mat4 normal_mat;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    stbi_set_flip_vertically_on_load(true);
    Model ourModel("D:\\Assets\\backpack\\backpack.obj");
    Material_Arr material_arr;
    Light_Arr light_arr;
    PointLight_Arr plight_arr;
    DirLight_Arr dlight_arr;
    bool gamma_on = false;
    int render_mode = 0;

    GLuint depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    GLuint depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
        SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        //Input
        processInput(window);
        //Render
        
        //UI
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        //my window
        {
            ImGui::Begin("Scene Editor", 0, ImGuiWindowFlags_AlwaysAutoResize);
            ImGui::BulletText("Bulb Attribute");
            ImGui::Checkbox("isOn ", &(plight_arr.bulb_on));
            ImGui::SliderFloat("Radius ", &(plight_arr.bulb_radius), 0.f, 10.f);
            ImGui::SliderFloat("Degree ", &(plight_arr.bulb_degree), 0.f, 360.f);
            ImGui::SliderFloat("Height ", &(plight_arr.bulb_height), 0.f, 10.f);
            ImGui::DragFloat3("bAmbient ", plight_arr.ambient, 0.05f, 0, 1);
            ImGui::DragFloat3("bDiffuse ", plight_arr.diffuse, 0.05f, 0, 1);
            ImGui::DragFloat3("bSpecular ", plight_arr.specular, 0.05f, 0, 1);
            ImGui::BulletText("Sun Attribute");
            ImGui::DragFloat3("sdirection ", dlight_arr.direction, 0.05f, -1, 1);
            ImGui::DragFloat3("sAmbient ", dlight_arr.ambient, 0.05f, 0, 1);
            ImGui::DragFloat3("sDiffuse ", dlight_arr.diffuse, 0.05f, 0, 1);
            ImGui::DragFloat3("sSpecular ", dlight_arr.specular, 0.05f, 0, 1);
            ImGui::BulletText("Material Attribute");
            ImGui::SliderFloat("shininess ", &(material_arr.shininess), 0.f, 100.f);
            ImGui::BulletText("Display Attribute");
            ImGui::Checkbox("Gamma Correction ", &gamma_on);
            //选择渲染方式
            {
                if (ImGui::Selectable("Render", render_mode == 0))
                    render_mode = 0;
                if (ImGui::Selectable("Normal", render_mode == 1))
                    render_mode = 1;
            }
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }
        ImGui::Render();
        //Clear
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        plight_arr.position = vec3(plight_arr.bulb_radius * cos(glm::radians(plight_arr.bulb_degree)),
                                   plight_arr.bulb_height,
                                   -1 * plight_arr.bulb_radius * sin(glm::radians(plight_arr.bulb_degree)));

        // 1. 首选渲染深度贴图
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        model = identity;
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.f, 1.f, 1.f));	// it's a bit too big for our scene, so scale it down
        depthShader.use();
        float near_plane = 0.1f, far_plane = 30.f;
        glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
        glm::mat4 lightView = glm::lookAt(//camera.Position,
            -2.f*glm::normalize(vec3(dlight_arr.direction[0], dlight_arr.direction[1], dlight_arr.direction[2])), 
            vec3(0.f),
            vec3(0.f,1.f,0.f));
        glm::mat4 lightSpaceMatrix = lightProjection * lightView;
        depthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
        depthShader.setMat4("model", model);
        ourModel.Draw(depthShader);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // 2. 像往常一样渲染场景，但这次使用深度贴图
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        postShader.use();
        glBindVertexArray(quadVAO);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        //ShaderProgram & VAO||EBO 
        NPRShader.use();  
        NPRShader.setVec3("viewPos", camera.Position);
        NPRShader.setVec3("material.diffuse", material_arr.diffuse);
        NPRShader.setVec3("material.specular", material_arr.specular);
        NPRShader.setFloat("material.shininess", material_arr.shininess);
        if (plight_arr.bulb_on)
        {
            NPRShader.setVec3("pointLight.ambient", plight_arr.ambient);
            NPRShader.setVec3("pointLight.diffuse", plight_arr.diffuse);
            NPRShader.setVec3("pointLight.specular", plight_arr.specular);
        }
        else
        {
            NPRShader.setVec3("pointLight.ambient", vec3(0.f));
            NPRShader.setVec3("pointLight.diffuse", vec3(0.f));
            NPRShader.setVec3("pointLight.specular", vec3(0.f));
        }
        NPRShader.setVec3("pointLight.position", plight_arr.position);
        NPRShader.setFloat("pointLight.constant", plight_arr.constant);
        NPRShader.setFloat("pointLight.linear", plight_arr.linear);
        NPRShader.setFloat("pointLight.quadratic", plight_arr.quadratic);
        NPRShader.setVec3("dirLight.direction", dlight_arr.direction);
        NPRShader.setVec3("dirLight.ambient", dlight_arr.ambient);
        NPRShader.setVec3("dirLight.diffuse", dlight_arr.diffuse);
        NPRShader.setVec3("dirLight.specular", dlight_arr.specular);
        NPRShader.setBool("gammaOn", gamma_on);
        NPRShader.setInt("renderMode", render_mode);
        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        NPRShader.setMat4("projection", projection);
        NPRShader.setMat4("view", view);

        // render the loaded model
        glm::mat4 model = identity;
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.f, 1.f, 1.f));	// it's a bit too big for our scene, so scale it down
        NPRShader.setMat4("model", model);
        NPRShader.setMat4("normal_mat", transpose(inverse(model)));
        //ourModel.Draw(NPRShader);

        if (plight_arr.bulb_on)
        {
            bulbShader.use();
            bulbShader.setVec3("bulbColor", plight_arr.diffuse);
            bulbShader.setMat4("projection", projection);
            bulbShader.setMat4("view", view);
            model = identity;
            model = glm::translate(model, plight_arr.position);
            model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
            bulbShader.setMat4("model", model);
            glBindVertexArray(bulb_VAO);
            //glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        //PollEvents and Swap
        glfwPollEvents();
        glfwSwapBuffers(window);
    }
    glfwTerminate();
    return 0;
}
// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

        lastX = xpos;
        lastY = ypos;

        camera.ProcessMouseMovement(xoffset, yoffset);
    }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE)
    {
        firstMouse = true;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    return;
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}