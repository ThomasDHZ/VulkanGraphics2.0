#include "ObjectViewCamera.h"

ObjectViewCamera::ObjectViewCamera()
{
    Position = glm::vec3(0.0f);
    Up = glm::vec3(0.0f, 1.0f, 0.0f);
    Front = glm::vec3(0.0f, 0.0f, -1.0f);
    cameraType = CameraType::ObjectView_Camera;

    MovementSpeed = 2.5f;
    MouseSensitivity = 0.1f;
    WorldUp = Up;

    Zoom = 45.0f;
    Yaw = -90.0f;
    Pitch = 0.0f;
}

ObjectViewCamera::ObjectViewCamera(std::string Name, glm::vec2 ScreenSize, glm::vec3 position) : Camera(Name)
{
    Position = position;
    Up = glm::vec3(0.0f, 1.0f, 0.0f);
    Front = glm::vec3(0.0f, 0.0f, -1.0f);
    cameraType = CameraType::ObjectView_Camera;

    MovementSpeed = 2.5f;
    MouseSensitivity = 0.1f;
    WorldUp = Up;

    Zoom = 45.0f;
    Yaw = -90.0f;
    Pitch = 0.0f;
}

ObjectViewCamera::ObjectViewCamera(std::string Name, glm::vec2 ScreenSize, glm::vec3 position, float pitch, float yaw) : Camera(Name)
{
    Position = position;
    Up = glm::vec3(0.0f, 1.0f, 0.0f);
    Front = glm::vec3(0.0f, 0.0f, -1.0f);
    cameraType = CameraType::ObjectView_Camera;

    MovementSpeed = 2.5f;
    MouseSensitivity = 0.1f;
    WorldUp = Up;

    Zoom = 45.0f;
    Yaw = pitch;
    Pitch = yaw;
}

ObjectViewCamera::~ObjectViewCamera()
{
}

void ObjectViewCamera::Update(std::shared_ptr<VulkanEngine> engine)
{
}

void ObjectViewCamera::Update(glm::vec3 position)
{
    Position = position;
    ProjectionMatrix = glm::perspective((float)(3.14159265358979323846f / 2.0), 1.0f, 0.1f, 100.0f);
    ProjectionMatrix[1][1] *= -1;

    ViewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-Position.x, -Position.y, -Position.z));
    ViewMatrix = glm::rotate(ViewMatrix, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    ViewMatrix = glm::rotate(ViewMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    ViewMatrix = glm::rotate(ViewMatrix, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
}

void ObjectViewCamera::Update(glm::vec3 position, glm::vec3 direction)
{

    ProjectionMatrix = glm::perspective((float)(60.0f), EnginePtr::GetEnginePtr()->SwapChain.GetSwapChainResolution().width / (float)EnginePtr::GetEnginePtr()->SwapChain.GetSwapChainResolution().height, 0.1f, 10000.0f);

    glm::vec3 LightDelta = glm::normalize(direction - OldLightPos);
    Up = glm::normalize(Up);
    glm::vec3 rotate = glm::cross(direction, Up);
    glm::mat4 rotationMatrix = glm::mat3(1.0f);
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(LightDelta.x), glm::vec3(1.0f, 0.0f, 0.0f));
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(LightDelta.y), glm::vec3(0.0f, 1.0f, 0.0f));
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(LightDelta.z), glm::vec3(0.0f, 0.0f, 1.0f));
    glm::vec3 ViewDirection = rotationMatrix * glm::vec4(direction, 1.0f);



    ViewMatrix = glm::lookAt(position, position + Front, Up);

    OldLightPos = position;
}

void ObjectViewCamera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch)
{
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw += xoffset;
    Pitch += yoffset;

    if (constrainPitch)
    {
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
    }
}
