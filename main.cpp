#include <GL/glew.h>
#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <iostream>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include "mesh.h"
#include "meshGL.h"
#include "shader.h"
#include "Texture.h"

int main() {

    GLFWwindow* window;

    // Print OpenGL version
    int major, minor, revision;
    glfwGetVersion(&major, &minor, &revision);
    printf("OpenGL Version: %d.%d.%d\n", major, minor, revision);

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(960, 540, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1); //vsync

    if (glewInit() != GLEW_OK)
        std::cout << "glew ERROR!!!" << std::endl;

    std::cout << glGetString(GL_VERSION) << std::endl;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // it defines how opengl blend alpha pixels
    glEnable(GL_DEPTH_TEST);

    // imgui
    const char* glsl_version = "#version 130";
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
#ifdef __EMSCRIPTEN__
    ImGui_ImplGlfw_InstallEmscriptenCanvasResizeCallback("#canvas");
#endif
    ImGui_ImplOpenGL3_Init(glsl_version);

    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -25.0));
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), 960.0f / 540.0f, 0.1f, 500.0f);

    Mesh mesh;
    mesh.importOBJ("res/models/backpack/backpack.obj");
    Shader shader("res/shaders/basic.hlsl");
    shader.Bind();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0, 0.0, 0.0));
    shader.SetUniformMat4f("u_Model", model);
    shader.SetUniformMat4f("u_View", view);
    shader.SetUniformMat4f("u_Proj", proj);

    Texture texture("res/models/backpack/diffuse.jpg");
    shader.SetUniform1i("u_Texture", 0); // slot of the texture

    float textureColorMode = 0.5;
    float textureGridMode = 0.5;
    float interpolation = 0;
    
    MeshGl meshGl;
    meshGl = mesh.bake();

    // ********************* Renderer Loop ********************* //
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.SetUniform1f("u_TextureColorMode", textureColorMode);
        shader.SetUniform1f("u_TextureGridMode", textureGridMode);
        texture.Bind();

        Mesh meshInterpolated = mesh.interpolate(interpolation);
        meshGl.updateGeometry(meshInterpolated);
        meshGl.draw(shader);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Test");
        ImGui::SliderFloat("Texture Color", &textureColorMode, 0, 1.0f);
        ImGui::SliderFloat("Texture Grid", &textureGridMode, 0, 1.0f);
        ImGui::SliderFloat("Interpolation", &interpolation, 0.0f, 1.0f);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
        
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
	return 0;
}