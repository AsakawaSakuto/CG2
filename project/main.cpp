#pragma once
#include <Windows.h>
#include "application/SceneManager/SceneManager.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    SceneManager sceneManager;
    return sceneManager.Run();
}