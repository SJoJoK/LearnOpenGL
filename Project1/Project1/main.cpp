#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "imfilebrowser.h"
#include "mesh.h"
#include "model.h"
#include "camera.h"
#include "shader.h"
#include "stb_image.h"
#define RENDER 0
#define MODEL 0
#define NORMAL 1
#define AO 2
#define ALBEDO 3
#define DIFFUSE 3
#define SPECULAR 4
#define METALLIC 4
#define ROUGHNESS 5
#define MESH 6
using namespace glm;
using namespace std;
int SCR_WIDTH = 1200, SCR_HEIGHT = 900;
const int SHADOW_WIDTH = 3200, SHADOW_HEIGHT = 3200;
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
void get_depth_buffer(unsigned int& FBO, unsigned int& depthMap);
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

struct PBRLight_Arr {
    bool white = true;
    bool point = false;
    vec3 position;
    float direction[3] = { 1.f, -1.f, -1.f };
    float color[3] = { 1.f, 1.f, 1.f };
    float flux = 10.f;
    float radius = 3.f;
    float degree = 225.f;
    float height = 1.737f;
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
    Shader PBRShader("vshader.glsl", "PBRshader.glsl");
    Shader bulbShader("vshader.glsl", "bulbshader.glsl");
    Shader depthShader("depthshader.glsl", "nullshader.glsl");
    Shader postShader("postVshader.glsl", "postFshader.glsl");
    Shader* tureShader = &NPRShader;
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

    glm::mat4 identity;
    glm::mat4 model;
    glm::mat4 projection;
    glm::mat4 view;
    glm::mat4 normal_mat;
    glm::mat4 lightSpaceMatrix;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    stbi_set_flip_vertically_on_load(true);
    Model* ourModel = new Model();
    Material_Arr material_arr;
    PBRLight_Arr PBRlight_arr;
    Texture texture_albedo, texture_normal, texture_metallic, texture_roughness, texture_AO;
    bool sun_on = false;
    bool gamma_on = true;
    bool HDR_on = true;
    bool shadow_on = true;
    int render_mode = RENDER;
    int model_choose = -1;

    GLuint depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    GLuint depthMap, depthMap1;

    get_depth_buffer(depthMapFBO, depthMap);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

    ImGui::FileBrowser fileDialog;
    ourModel->loadModel("D:/Assets/backpack/backpack.obj");
    ourModel->loaded = true;
    texture_albedo.id = TextureFromFile("D:/Assets/backpack/diffuse.jpg");
    texture_metallic.id = TextureFromFile("D:/Assets/backpack/specular.jpg");
    texture_normal.id = TextureFromFile("D:/Assets/backpack/normal.png");
    texture_AO.id = TextureFromFile("D:/Assets/backpack/ao.jpg");
    texture_roughness.id = TextureFromFile("D:/Assets/backpack/roughness.jpg");


    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        //Input
        processInput(window);
        //Render
        
        //ImGui
        {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            //Scene Editor
            {
                ImGui::Begin("Scene Editor", 0, ImGuiWindowFlags_AlwaysAutoResize);
                ImGui::BulletText("PBRLight Attribute");
                ImGui::Checkbox("pWhite Light", &PBRlight_arr.white);
                ImGui::DragFloat3("pdirection ", PBRlight_arr.direction, 0.05f, -1, 1);
                ImGui::SliderFloat("Radius ", &(PBRlight_arr.radius), 0.f, 10.f);
                ImGui::SliderFloat("Degree ", &(PBRlight_arr.degree), 0.f, 360.f);
                ImGui::SliderFloat("Height ", &(PBRlight_arr.height), 0.f, 10.f);
                ImGui::DragFloat3("pColor ", PBRlight_arr.color, 0.5f, 0.f, 30.f);
                ImGui::SliderFloat("pFlux ", &(PBRlight_arr.flux), 0.f, 30.f);
                ImGui::BulletText("Material Attribute");
                ImGui::SliderFloat("shininess ", &(material_arr.shininess), 0.f, 100.f);
                ImGui::BulletText("Display Attribute");
                ImGui::Checkbox("Gamma Correction ", &gamma_on);
                ImGui::Checkbox("HDR ", &HDR_on);
                ImGui::Checkbox("Shadow ", &shadow_on);
                ImGui::BulletText("Render Mode");
                //选择渲染方式
                {
                    if (ImGui::Selectable("Render", render_mode == RENDER))
                        render_mode = RENDER;
                    if (ImGui::Selectable("Albedo", render_mode == ALBEDO))
                        render_mode = ALBEDO;
                    if (ImGui::Selectable("Metallic", render_mode == SPECULAR))
                        render_mode = SPECULAR;
                    if (ImGui::Selectable("Roughness", render_mode == ROUGHNESS))
                        render_mode = ROUGHNESS;
                    if (ImGui::Selectable("Normal", render_mode == NORMAL))
                        render_mode = NORMAL;
                    if (ImGui::Selectable("AO", render_mode == AO))
                        render_mode = AO;
                    if (ImGui::Selectable("Mesh", render_mode == MESH))
                        render_mode = MESH;
                }
                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
                ImGui::End();
            }
            //Model Editor
            {
                ImGui::Begin("Model Editor", 0, ImGuiWindowFlags_AlwaysAutoResize);
                if (ImGui::Button("Model"))
                {
                    fileDialog.Open();
                    model_choose = MODEL;
                }
                if (ImGui::Button("Albedo Map"))
                {
                    fileDialog.Open();
                    model_choose = ALBEDO;

                }
                if (ImGui::Button("Normal Map"))
                {
                    fileDialog.Open();
                    model_choose = NORMAL;

                }
                if (ImGui::Button("Specular Map"))
                {
                    fileDialog.Open();
                    model_choose = SPECULAR;
                }
                if (ImGui::Button("Roughness Map"))
                {
                    fileDialog.Open();
                    model_choose = ROUGHNESS;

                }
                if (ImGui::Button("AO Map"))
                {
                    fileDialog.Open();
                    model_choose = AO;

                }
                fileDialog.Display();

                if (fileDialog.HasSelected())
                {
                    string ab_path = fileDialog.GetSelected().string();
                    switch (model_choose)
                    {
                    case MODEL :
                    {
                        ourModel->loadModel(ab_path);
                        break;
                    }
                    case ALBEDO:
                    {
                        texture_albedo.id = TextureFromFile(ab_path);
                        break;
                    }
                    case METALLIC:
                    {
                        texture_metallic.id = TextureFromFile(ab_path);
                        break;
                    }
                    case NORMAL:
                    {
                        texture_normal.id = TextureFromFile(ab_path);
                        break;
                    }
                    case AO:
                    {
                        texture_AO.id = TextureFromFile(ab_path);
                        break;
                    }
                    case ROUGHNESS :
                    {
                        texture_roughness.id = TextureFromFile(ab_path);
                        break;
                    }
                    }
                    fileDialog.ClearSelected();
                }
            }
            ImGui::Render();
        }
        //Clear
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        //Handle Attribute
        {
            PBRlight_arr.position = vec3(PBRlight_arr.radius * cos(glm::radians(PBRlight_arr.degree)),
                PBRlight_arr.height,
                -1 * PBRlight_arr.radius * sin(glm::radians(PBRlight_arr.degree)));
            vec3 d = vec3(0.f) - (PBRlight_arr.position);

            if (PBRlight_arr.white)
            {
                PBRlight_arr.color[0] = 1.f;
                PBRlight_arr.color[1] = 1.f;
                PBRlight_arr.color[2] = 1.f;
            }
            static bool ffk = true;

            PBRlight_arr.direction[0] = d.x;
            PBRlight_arr.direction[1] = d.y;
            PBRlight_arr.direction[2] = d.z;


            projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
            view = camera.GetViewMatrix();
        }
        if (ourModel->loaded)
        {
            //Shadow Map
            {
                // 1. 首选渲染深度贴图
                glBindTexture(GL_TEXTURE_2D, depthMap);
                glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
                glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
                glClear(GL_DEPTH_BUFFER_BIT);
                model = identity;
                depthShader.use();
                float near_plane = 0.1f, far_plane = 30.f;
                glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
                glm::mat4 lightView = glm::lookAt(//camera.Position,
                        -glm::normalize(vec3(PBRlight_arr.direction[0], PBRlight_arr.direction[1], PBRlight_arr.direction[2])),
                        vec3(0.f),
                        vec3(0.f, 1.f, 0.f));
                lightSpaceMatrix = lightProjection * lightView;
                depthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
                depthShader.setMat4("model", model);
                ourModel->Draw(depthShader);
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                // 2. 像往常一样渲染场景，但这次使用深度贴图
                glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            }
            tureShader = &PBRShader;
            //ShaderProgram & VAO||EBO 
            tureShader->use();
            {
                tureShader->setVec3("viewPos", camera.Position);
                tureShader->setVec3("material.diffuse", material_arr.diffuse);
                tureShader->setVec3("material.specular", material_arr.specular);
                tureShader->setFloat("material.shininess", material_arr.shininess);
                tureShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
                tureShader->setMat4("light_PBR.lightSpaceMatrix", lightSpaceMatrix);
                tureShader->setVec3("light_PBR.direction", PBRlight_arr.direction[0], PBRlight_arr.direction[1], PBRlight_arr.direction[2]);
                tureShader->setBool("light_PBR.point", false);
                tureShader->setBool("ROUGH", true);
                tureShader->setBool("sRGBtexture", true);
                tureShader->setVec3("light_PBR.lightColor", PBRlight_arr.flux*vec3(PBRlight_arr.color[0], PBRlight_arr.color[1], PBRlight_arr.color[2]));
                tureShader->setBool("gammaOn", gamma_on);
                tureShader->setBool("HDROn", HDR_on);
                tureShader->setBool("shadowOn", shadow_on);
                tureShader->setInt("renderMode", render_mode);
                // view/projection transformations
                projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
                view = camera.GetViewMatrix();
                tureShader->setMat4("projection", projection);
                tureShader->setMat4("view", view);
                // render the loaded model
                model = identity;
                model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
                model = glm::scale(model, glm::vec3(1.f, 1.f, 1.f));	// it's a bit too big for our scene, so scale it down
                tureShader->setMat4("model", model);
                tureShader->setMat4("normal_mat", transpose(inverse(model)));
            }
            
            {
                glActiveTexture(GL_TEXTURE12);
                tureShader->setInt("light_PBR.shadowMap", 12);
                glBindTexture(GL_TEXTURE_2D, depthMap);

                glActiveTexture(GL_TEXTURE0 + ALBEDO);
                tureShader->setInt("material.texture_diffuse1", ALBEDO);
                glBindTexture(GL_TEXTURE_2D, texture_albedo.id);

                glActiveTexture(GL_TEXTURE0 + METALLIC);
                tureShader->setInt("material.texture_specular1", METALLIC);
                glBindTexture(GL_TEXTURE_2D, texture_metallic.id);

                glActiveTexture(GL_TEXTURE0 + ROUGHNESS);
                tureShader->setInt("material.texture_roughness1", ROUGHNESS);
                glBindTexture(GL_TEXTURE_2D, texture_roughness.id);

                glActiveTexture(GL_TEXTURE0 + NORMAL);
                tureShader->setInt("material.texture_normal1", NORMAL);
                glBindTexture(GL_TEXTURE_2D, texture_normal.id);

                glActiveTexture(GL_TEXTURE0 + AO);
                tureShader->setInt("material.texture_AO1", AO);
                glBindTexture(GL_TEXTURE_2D, texture_AO.id);
            }

            if (render_mode == MESH)
            {
                ourModel->Draw(*tureShader, GL_LINES);
            }
            else
            {
                ourModel->Draw(*tureShader);

            }
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
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
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

void get_depth_buffer(unsigned int& FBO, unsigned int& depthMap)
{

    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

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
    glBindTexture(GL_TEXTURE_2D, 0);
}