#include "GameController.h"

GameController::GameController()
{
    const char* mappings = "SDL_GamepadBind 030000004c050000c405000000010000,PS4 Controller,platform:Windows,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:a3,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:a4,rightx:a2,righty:a5,start:b9,x:b0,y:b3,";
    //glfwUpdateGamepadMappings(mappings);
    if (glfwJoystickIsGamepad(GLFW_JOYSTICK_1))
    {
        const char* name = glfwGetJoystickName(GLFW_JOYSTICK_1);
    }

    int present = glfwJoystickPresent(GLFW_JOYSTICK_1);
}

GameController::~GameController()
{
}

void GameController::Update()
{
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;


    if (glfwGetGamepadState(GLFW_JOYSTICK_1, &state))
    {
        if (state.buttons[GLFW_GAMEPAD_BUTTON_X])
        {
            std::cout << "X Button " << state.buttons[GLFW_GAMEPAD_BUTTON_X] << std::endl;
        }

        if (state.buttons[GLFW_GAMEPAD_BUTTON_CIRCLE])
        {
            std::cout << "O Button " << state.buttons[GLFW_GAMEPAD_BUTTON_CIRCLE] << std::endl;
        }

        if (state.buttons[GLFW_GAMEPAD_BUTTON_SQUARE])
        {
            std::cout << "GLFW_GAMEPAD_BUTTON_TRIANGLE Button " << state.buttons[GLFW_GAMEPAD_BUTTON_TRIANGLE] << std::endl;
        }

        /* if (state.axes[GLFW_GAMEPAD_AXIS_LEFT_X])
         {
             std::cout << "GLFW_GAMEPAD_AXIS_LEFT_X Button " << state.axes[GLFW_GAMEPAD_AXIS_LEFT_X] << std::endl;
         }

         if (state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X])
         {
             std::cout << "GLFW_GAMEPAD_AXIS_RIGHT_X Button " << state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X] << std::endl;
         }

         if (state.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER])
         {
             std::cout << "GLFW_GAMEPAD_AXIS_LEFT_TRIGGER Button " << state.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER] << std::endl;
         }

         if (state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER])
         {
             std::cout << "GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER Button " << state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER] << std::endl;
         }*/

         /*  if (state.axes[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER])
           {
               std::cout << "GLFW_GAMEPAD_BUTTON_LEFT_BUMPER Button " << state.axes[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER] << std::endl;
           }

           if (state.axes[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER])
           {
               std::cout << "GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER Button " << state.axes[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER] << std::endl;
           }*/
    }
}
