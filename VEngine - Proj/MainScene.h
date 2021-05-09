#pragma once
#include "Scene.h"

class MainScene final : public Scene
{
public:
	MainScene();
	~MainScene();

	MainScene(MainScene& other) = delete;
	MainScene(MainScene&& other) = delete;
	MainScene operator=(MainScene& other) = delete;
	MainScene& operator=(MainScene&& other) = delete;
};