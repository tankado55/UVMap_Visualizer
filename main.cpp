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
#include "Camera.h"
#include "InputManager.h"
#include "directionalLight.h"

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
#endif````
    ImGui_ImplOpenGL3_Init(glsl_version);

    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -25.0));
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), 960.0f / 540.0f, 0.1f, 500.0f);
    DirectionalLight dirLight(
        glm::vec3(-0.2f, -1.0f, -0.3f),
        glm::vec3(0.05f, 0.05f, 0.05f),
        glm::vec3(0.4f, 0.4f, 0.4f),
        glm::vec3(0.5f, 0.5f, 0.5f)
    );

    Mesh mesh;
    //mesh.buildCylinder();
    //mesh.buildPlane();
    //mesh.importOBJ("res/models/cylinder/cylinder.obj");
    //mesh.exportOBJ("res/models/plane/plane.obj");
    mesh.importOBJ("res/models/_Wheel_195_50R13x10_OBJ/wheel.obj");
    Camera camera;
    Shader shader("res/shaders/basic.hlsl");
    shader.Bind();
    MeshGl meshGl;
    meshGl = mesh.bake();
    float scalingFactor = 1.0 / mesh.boundingSphere.radius * 2.0f;
    meshGl.model = glm::scale(meshGl.model, glm::vec3(scalingFactor, scalingFactor, scalingFactor));

    Mesh plane;
    plane.importOBJ("res/models/plane/plane.obj");
    MeshGl planeGl;
    planeGl = plane.bake();
    planeGl.model = glm::scale(planeGl.model, glm::vec3(2.0, 1.0, 2.0));
    planeGl.model = glm::translate(planeGl.model, glm::vec3(0.0, -5.0, 0.0));

    dirLight.setUniform(shader); // TODO: refactor
    shader.SetUniform1f("material.shininess", 32.0f);
    shader.SetUniformVec3f("u_ViewPos", camera.GetPos());
    shader.SetUniformMat4f("u_View", view);
    shader.SetUniformMat4f("u_Proj", proj);

    Texture texture("res/models/_Wheel_195_50R13x10_OBJ/diffuse.png");
    Texture floorTexture("res/models/plane/Prototype_Grid_Gray_08-512x512.png");
    shader.SetUniform1i("u_Texture", 0); // slot of the texture

    float textureColorMode = 0.5;
    float textureGridMode = 0.5;
    float interpolation = 0.0;
    float interpolationSpeed = 1.0;
    

    float deltaTime = 0.0f;
    float lastFrame = 0.0f; // Time of last frame

    InputManager::GetInstance()->SetWindow(window);
    InputManager::GetInstance()->Start(&camera);

    // ********************* Renderer Loop ********************* //
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        camera.ProcessKeyboardInput(deltaTime, window);
        view = camera.GetView();

        shader.Bind();
        shader.SetUniformMat4f("u_View", view);
        shader.SetUniform1f("u_TextureColorMode", textureColorMode);
        shader.SetUniform1f("u_TextureGridMode", textureGridMode);
        shader.SetUniformVec3f("u_ViewPos", camera.GetPos());

        texture.Bind();
        Mesh meshInterpolated = mesh.interpolate(interpolation);
        meshGl.updateGeometry(meshInterpolated);
        shader.SetUniformMat4f("u_Model", meshGl.model);
        shader.SetUniformMat3f("u_NormalMatrix", glm::mat3(transpose(inverse(meshGl.model))));
        meshGl.draw(shader);

        // Draw plane
        floorTexture.Bind();
        shader.SetUniformMat4f("u_Model", planeGl.model);
        shader.SetUniformMat3f("u_NormalMatrix", glm::mat3(transpose(inverse(planeGl.model))));
        planeGl.draw(shader);

        float speed = interpolationSpeed * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
            interpolation -= speed;
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
            interpolation += speed;

        interpolation = std::max(0.0f, std::min(interpolation, 1.0f));

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