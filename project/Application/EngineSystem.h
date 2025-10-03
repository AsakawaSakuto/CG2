#pragma once
#include "Application/SceneManager/IScene.h"
#include "AppContext.h"

// Include
#include "WinApp.h"
#include "Input.h"
#include "Camera.h"
#include "Audio.h"
#include "DebugCamera.h"
#include "ConvertString.h"
#include "DirectXCommon.h"
#include "D3DResourceLeakChecker.h"
#include "TextureManager.h"
#include "GamePad.h"
#include <filesystem>
#include <memory>

// Object
#include "Sprite.h"
#include "Object3d.h"
#include "Sphere.h"
#include "SphereData.h"
#include "Triangle.h"
#include "Particles.h"
#include "EmitterRange.h"
#include "EmitterSpfere.h"

// Math
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"
#include "Engine/System/Utility/Math/Sphere.h"
#include "Engine/System/Utility/Math/AABB.h"

// Utility
#include "Engine/system/Utility/Random/Random.h"
#include "Engine/system/Utility/Easing/Easing.h"
#include "Engine/system/Utility/GameTimer/GameTimer.h"