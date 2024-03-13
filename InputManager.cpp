#include "InputManager.h"

#include <iostream>
#include "imgui.h"

InputManager* InputManager::m_Singleton = nullptr;

InputManager::InputManager() :
    lastX(0.0f),
    lastY(0.0f),
    m_FirstMouse(true),
    m_XOffset(0.0f),
    m_YOffset(0.0f)
{
    m_Window = nullptr;
    m_Camera = nullptr;
}


InputManager* InputManager::GetInstance()
{
    if (m_Singleton == nullptr) {
        m_Singleton = new InputManager();
    }
    return m_Singleton;
}

void InputManager::ProcessCamera(float xOff, float yOff)
{
    m_Camera->ProcessMouseInput(xOff, yOff);
}



void InputManager::SetWindow(GLFWwindow*& window)
{
    m_Window = window;
}

void InputManager::mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
    InputManager* im = InputManager::GetInstance();

    if (im->GetFirstMouse())
    {
        im->SetLastX(xpos);
        im->SetLastY(ypos);
        im->SetFirstMouse(false);
    }

    float xOffset = xpos - im->GetLastX();
    float yOffset = im->GetLastY() - ypos; // reversed since y-coordinates go from bottom to top

    im->SetLastX(xpos);
    im->SetLastY(ypos);

    im->ProcessCamera(xOffset, yOffset);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    bool down;
    if (action == GLFW_PRESS)
        down = true;
    else
        down = false;

    ImGuiIO& io = ImGui::GetIO();
    //io.AddMouseButtonEvent(button, down);
}

void InputManager::Start(Camera* camera)
{
    m_Camera = camera;

    if (m_Window == nullptr)
    {
        std::cout << "InputManager: window in null!" << std::endl;
    }
    //glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    //glfwSetCursorPosCallback(m_Window, mouse_callback);
    glfwSetMouseButtonCallback(m_Window, mouse_button_callback);
}