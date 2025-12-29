#pragma once
#include <filesystem>
#include <memory>

#include "SceneManager/IScene.h"

// Include
#include "Core/WinApp/WinApp.h"
#include "Input/MyInput.h"
#include "Camera/Camera.h"
#include "Audio/MyAudio.h"
#include "Camera/DebugCamera.h"
#include "Utility/ConvertString/ConvertString.h"
#include "Core/DirectXCommon/DirectXCommon.h"
#include "Core/DirectXCommon/D3DResourceLeakChecker.h"
#include "Core/TextureManager/TextureManager.h"

// Object
#include "2d/Sprite/Sprite.h"
#include "2d/BitmapFont/BitmapFont.h"
#include "2d/Gauge/Gauge.h"

#include "3d/Model/Model.h"
#include "3d/Model/SkiningModel.h"
#include "3d/Particle/Particles.h"
#include "3d/Particle/Struct/EmitterState.h"
#include "3d/Line/MyDebugLine.h"

// Math
#include "Math/Type/Vector2.h"
#include "Math/Type/Vector3.h"
#include "Math/Type/Vector4.h"
#include "Math/Type/Matrix4x4.h"

// Utility
#include "Utility/Random/Random.h"
#include "Utility/Easing/Easing.h"
#include "Utility/GameTimer/GameTimer.h"
#include "Utility/Collision/Collision.h"
#include "Utility/Collision/Type/Sphere.h"
#include "Utility/Collision/Type/AABB.h"

using std::unique_ptr;
using std::make_unique;