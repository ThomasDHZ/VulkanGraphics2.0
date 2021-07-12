#include "Game.h"

Game::Game()
{
	engine = Engine(1280, 720, "VulkanEngine");
}

Game::~Game()
{
}

void Game::MainLoop()
{
	engine.MainLoop();
}