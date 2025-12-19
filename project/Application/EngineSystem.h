#pragma once
#include <filesystem>
#include <memory>

#include "SceneManager/IScene.h"

// Include
#include "Core/WinApp/WinApp.h"
#include "Input/KeyConfig.h"
#include "Camera/Camera.h"
#include "Audio/AudioManager.h"
#include "Camera/DebugCamera.h"
#include "Utility/ConvertString/ConvertString.h"
#include "Core/DirectXCommon/DirectXCommon.h"
#include "Core/DirectXCommon/D3DResourceLeakChecker.h"
#include "Core/TextureManager/TextureManager.h"

// Object
#include "2d/Sprite/Sprite.h"
#include "2d/BitmapFont/BitmapFont.h"
#include "3d/Model/Model.h"
#include "3d/Particle/Particles.h"
#include "3d/Particle/Struct/EmitterState.h"
#include "3d/Line/Line.h"

// Math
#include "Math/Type/Vector2.h"
#include "Math/Type/Vector3.h"
#include "Math/Type/Vector4.h"
#include "Math/Type/Matrix4x4.h"
#include "Utility/Collision/Type/Sphere.h"
#include "Utility/Collision/Type/AABB.h"

// Utility
#include "Utility/Random/Random.h"
#include "Utility/Easing/Easing.h"
#include "Utility/GameTimer/GameTimer.h"
#include "Utility/Collision/Collision.h"

using std::unique_ptr;
using std::make_unique;