#pragma once

#include "Camera.h"

class InputManager
{
protected:
    InputManager();

    static InputManager* m_Singleton;

public:
    InputManager(InputManager& other) = delete; // Singletons should not be cloneable.
    void operator=(const InputManager&) = delete; // Singletons should not be assignable.
    static InputManager* GetInstance();

    float GetFirstMouse() const { return m_FirstMouse; }
    void SetFirstMouse(bool status) { m_FirstMouse = status; }
    float GetLastX() const { return lastX; };
    void SetLastX(float xpos) { lastX = xpos; }
    float GetLastY() const { return lastY; };
    void SetLastY(float ypos) { lastY = ypos; }
    float GetMouseOffsetX() { return m_XOffset; }
    float GetMouseOffsetY() { return m_YOffset; }
    void SetMouseOffsetX(float offset) { m_XOffset = offset; }
    void SetMouseOffsetY(float offset) { m_YOffset = offset; }

    void Start(Camera* camera);
    void SetWindow(GLFWwindow*& window);
    static void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
    void ProcessCamera(float xOff, float yOff);

private:
    // mouse
    float lastX;
    float lastY;
    bool m_FirstMouse;
    float m_XOffset;
    float m_YOffset;

    GLFWwindow* m_Window;
    Camera* m_Camera;
};

