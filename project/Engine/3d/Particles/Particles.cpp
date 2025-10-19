#include "Particles.h"
#include "Object3d.h"
#include <cassert>
#pragma comment(lib,"d3d12.lib")
using namespace Microsoft::WRL;

// 修正: PSOManagerをインクルード（相対パス修正）
#include "../../System/PSOManager/PSOManager.h"

void Particles::Initialize(DirectXCommon* dxCommon, const uint32_t maxParticle, const std::string& TextureName) {
	// DX共通クラスからデバイス・コマンドリストを取得
	dxCommon_ = dxCommon;
	device_ = dxCommon_->GetDevice();
	commandList_ = dxCommon_->GetCommandList();

	ParticleDescriptorAllocator& alloc = dxCommon_->GetParticleAlloc();

	idxSrvParticles_ =      alloc.Allocate();
	idxUavParticles_ =      alloc.Allocate();
	idxUavFreeListIndex_ =  alloc.Allocate();
	idxUavFreeList_ =       alloc.Allocate();

	// ブレンドモードを加算合成に初期化
	blendMode_ = kBlendModeAdd;

	// テクスチャ名を保持しておく
	textureName_ = "resources/image/particle/" + TextureName + ".png";

	// テクスチャマネージャー初期化とテクスチャの読み込み
	TextureManager::GetInstance()->LoadTexture(textureName_);
	textureIndex_ = TextureManager::GetInstance()->GetTextureIndexByFilePath(textureName_);

	// リソースの作成
	CreateVertexResource();   // 頂点バッファ
	CreateIndexResource();    // インデックスバッファ
	CreateMaterialResource(); // マテリアル

	kMaxParticles_ = 512 * maxParticle;
	uint32_t num = kMaxParticles_ / 512;
	kDispatchCount = num;

	// Emitterのデフォルト値
	emitter_.translate = { 0.0f, 0.0f, 0.0f };
	emitter_.radius = 1.0f;
	emitter_.useEmitter = 1;
	emitter_.emit = 0;
	emitter_.count = 1;
	emitter_.kMaxParticle = kMaxParticles_;
	emitter_.frequency = 1.0f;
	emitter_.frequencyTime = 0.0f;
	emitter_.startScale = { 1.0f, 1.0f };
	emitter_.endScale = { 0.0f, 0.0f };
	emitter_.scaleFade = 0;
	emitter_.scaleRandom = 0;
	emitter_.minScale = { 0.5f, 0.5f, 0.5f };
	emitter_.maxScale = { 1.5f, 1.5f, 1.5f };
	emitter_.rotateMove = 0;
	emitter_.startRotateVelocity = 0.0f;
	emitter_.endRotateVelocity = 0.0f;
	emitter_.rotateVelocityRandom = 0;
	emitter_.minRotateVelocity = -1.0f;
	emitter_.maxRotateVelocity = 1.0f;
	emitter_.alphaFade = 0;
	emitter_.colorFade = 0;
	emitter_.startColor = { 1.0f, 1.0f, 1.0f };
	emitter_.endColor = { 1.0f, 0.0f, 0.0f };
	emitter_.colorRandom = 0;
	emitter_.minColor = { 0.0f, 0.0f, 0.0f };
	emitter_.maxColor = { 1.0f, 1.0f, 1.0f };
	emitter_.isMove = 0;
	emitter_.startVelocity = { 0.0f, 0.0f, 0.0f };
	emitter_.endVelocity = { 0.0f, 0.0f, 0.0f };
	emitter_.velocityRandom = 0;
	emitter_.minVelocity = { -1.0f, -1.0f, -1.0f };
	emitter_.maxVelocity = { 1.0f, 1.0f, 1.0f };
	emitter_.lifeTime = 2.0f;
	emitter_.lifeTimeRandom = 0;
	emitter_.minLifeTime = 1.0f;
	emitter_.maxLifeTime = 5.0f;
	
	// Initialize new emitter shape fields
	emitter_.shapeType = static_cast<uint32_t>(EmitterShapeType::SPHERE_VOLUME);
	emitter_.size = { 1.0f, 1.0f, 1.0f };  // Default box size
	emitter_.lineStart = { -1.0f, 0.0f, 0.0f };  // Default line start
	emitter_.lineLength = 2.0f;  // Default line length
	emitter_.ringInnerRadius = 0.5f;  // Default ring inner radius
	emitter_.ringOuterRadius = 1.0f;  // Default ring outer radius

	// Initialize cone and hemisphere fields
	emitter_.coneAngle = 45.0f;  // Default cone angle in degrees
	emitter_.coneHeight = 2.0f;  // Default cone height
	emitter_.coneDirection = { 0.0f, 1.0f, 0.0f };  // Default direction (up)
	emitter_.hemisphereAngle = 180.0f;  // Default hemisphere angle (full hemisphere)

	// Initialize angle-based plane and ring fields
	emitter_.planeNormal = { 0.0f, 1.0f, 0.0f };  // Default plane normal (up)
	emitter_.planeWidth = 2.0f;   // Default plane width
	emitter_.planeHeight = 2.0f;  // Default plane height
	emitter_.ringAngle = 0.0f;    // Default ring rotation angle
	emitter_.ringNormal = { 0.0f, 1.0f, 0.0f };  // Default ring normal (up)

	// Initialize texture path
	emitter_.texturePath = TextureName;

	CreateEmitterResource();
	CreateParticleResource();
	CreatePerViewResource();
	CreatePerFrameResource();
}

void Particles::Update() {

	// ---- カメラ関連の行列計算（PreView構造体に書き込む） ----
	Matrix4x4 cameraMatrix = MakeAffineMatrix(camera_.GetScale(), camera_.GetRotate(), camera_.GetTranslate());
	Matrix4x4 backToFrontMatrix = MakeRotateYMatrix(std::numbers::pi_v<float>);
	Matrix4x4 billboardMatrix = MultiplyMatrix(backToFrontMatrix, cameraMatrix);

	Matrix4x4 viewMatrix = InverseMatrix(cameraMatrix);
	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(camera_.GetFovY(), static_cast<float>(WinApp::kClientWidth_) / static_cast<float>(WinApp::kClientHeight_), camera_.GetNearClip(), camera_.GetFarClip());

	// PreViewリソースに書き込み
	PerView tempPreView;
	tempPreView.viewProjection = MultiplyMatrix(viewMatrix, projectionMatrix);
	tempPreView.billboardMatrix = billboardMatrix;

	void* mappedPtr = nullptr;
	perViewResource_->Map(0, nullptr, &mappedPtr);
	memcpy(mappedPtr, &tempPreView, sizeof(PerView));
	perViewResource_->Unmap(0, nullptr);

	UpdatePerFrame();

	UpdateEmitter();

	UpdateParticle();
}

void Particles::Draw(Camera& useCamera) {
	camera_ = useCamera;

	// PSOManagerからRootSignatureを取得
	auto& psoManager = PSOManager::GetInstance();
	auto rootSignature = psoManager.GetRootSignature("Particle");

	// ルートシグネチャの設定（パイプラインステートだけでは設定されないため明示的に指定が必要）
	commandList_->SetGraphicsRootSignature(rootSignature.Get());

	// 現在のブレンドモードに応じて、PSOManagerから適切なPSOを取得
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pso;
	switch (blendMode_) {
	case kBlendModeNone:
		pso = psoManager.GetPSO(PSOType::Particle_None);
		break;
	case kBlendModeNormal:
		pso = psoManager.GetPSO(PSOType::Particle_Normal);
		break;
	case kBlendModeAdd:
		pso = psoManager.GetPSO(PSOType::Particle_Add);
		break;
	case kBlendModeSubtract:
		pso = psoManager.GetPSO(PSOType::Particle_Subtract);
		break;
	case kBlendModeMultily:
		pso = psoManager.GetPSO(PSOType::Particle_Multiply);
		break;
	case kBlendModeScreen:
		pso = psoManager.GetPSO(PSOType::Particle_Screen);
		break;
	default:
		pso = psoManager.GetPSO(PSOType::Particle_Add);
		break;
	}

	commandList_->SetPipelineState(pso.Get());

	// 三角形リストで描画することを指定（パーティクルは四角形だが、インデックスで三角形×2として構成）
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 頂点バッファ（VBV）とインデックスバッファ（IBV）の設定
	commandList_->IASetVertexBuffers(0, 1, &vertexBufferView_);
	commandList_->IASetIndexBuffer(&indexBufferView_);

	// Material
	commandList_->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());

	// カメラ・ビルボード行列（PreViewなど）
	commandList_->SetGraphicsRootConstantBufferView(1, perViewResource_->GetGPUVirtualAddress()); // ←ここ重要

	// パーティクルインスタンシングSRV（StructuredBufferなど）
	commandList_->SetGraphicsRootDescriptorTable(2, dxCommon_->GetSrvGPUHandle(idxSrvParticles_)); // ←今まで1番だったやつ

	// テクスチャSRV
	commandList_->SetGraphicsRootDescriptorTable(3, TextureManager::GetInstance()->GetSrvHandleGPU(textureIndex_));

	// DrawIndexedInstanced(インデックス数, インスタンス数, 開始インデックス, ベース頂点, 開始インスタンス)
	commandList_->DrawIndexedInstanced(6, kMaxParticles_, 0, 0, 0);
}

void Particles::DrawImGui(const char* objectName) {

	ImGui::Begin(objectName);

	// Play/Stop control buttons
	if (ImGui::Button("Play (Loop)")) {
		Play(true);
	}

	ImGui::SameLine();

	if (ImGui::Button("Play (Once)")) {
		Play(false);
	}

	ImGui::SameLine();

	if (ImGui::Button("Stop")) {
		Stop();
	}

	// 現在の再生状態を表示
	ImGui::Text("Status: %s", isPlaying_ ? "Playing" : "Stopped");

	ImGui::Separator();

	if (EmitterStateLoader::InputText("File Name", loadToSaveName_)) {
		// 入力が変更されたらここに来る
		printf("Generate Name Changed to: %s\n", loadToSaveName_.c_str());
	}

	if (ImGui::Button("Load to Json")) {
		jsonFilePath_ = "resources/Data/Particle/" + (loadToSaveName_ + ".json");
		emitter_ = EmitterStateLoader::Load(jsonFilePath_);

		// JSONから読み込んだテクスチャパスを適用
		if (!emitter_.texturePath.empty()) {
			std::string newTextureName = "resources/image/particle/" + emitter_.texturePath + ".png";
			if (textureName_ != newTextureName) {
				textureName_ = newTextureName;
				// テクスチャファイル読み込み
				TextureManager::GetInstance()->LoadTexture(textureName_);
				// 読み込んだテクスチャの番号を取得
				textureIndex_ = TextureManager::GetInstance()->GetTextureIndexByFilePath(textureName_);
			}
		}
	}

	ImGui::SameLine();

	if (ImGui::Button("Save to Json")) {
		jsonFilePath_ = "resources/Data/Particle/" + (loadToSaveName_ + ".json");
		EmitterStateLoader::Save(jsonFilePath_, emitter_);
	}

	ImGui::SameLine();

	if (ImGui::Button("Generate to Json")) {
		EmitterStateLoader::SaveToCurrentDir(emitter_, loadToSaveName_);
	}

	ImGui::Separator();

	if (EmitterStateLoader::InputText("texture Name", emitter_.texturePath)) {
		// 入力が変更されたらここに来る
		printf("Texture Name Changed to: %s\n", emitter_.texturePath.c_str());
	}

	if (ImGui::Button("Load Texture")) {
		// すでに同じテクスチャなら処理をスキップ
		std::string newTextureName = "resources/image/particle/" + emitter_.texturePath + ".png";
		if (textureName_ == newTextureName) {
			return;
		}
		textureName_ = newTextureName;
		// .objの参照しているテクスチャファイル読み込み
		TextureManager::GetInstance()->LoadTexture(textureName_);
		// 読み込んだテクスチャの番号を取得
		textureIndex_ = TextureManager::GetInstance()->GetTextureIndexByFilePath(textureName_);
	}

	ImGui::Separator();

	ImGui::DragFloat3("Translate", &emitter_.translate.x, 0.01f);
	
	// Emitter Shape Selection
	const char* shapeNames[] = { 
		"Point", 
		"Line", 
		"Sphere (Volume)", 
		"Sphere (Surface)", 
		"Box (Volume)", 
		"Box (Surface)", 
		"Ring (XZ Plane)", 
		"Ring (XY Plane)", 
		"Ring (YZ Plane)",
		"Cone (Volume)",
		"Cone (Surface)",
		"Hemisphere (Volume)",
		"Hemisphere (Surface)",
		"Plane (Angle Volume)",
		"Plane (Angle Edges)",
		"Ring (Angle Volume)",
		"Ring (Angle Edge)"
	};
	int currentShape = static_cast<int>(emitter_.shapeType);
	if (ImGui::Combo("Emitter Shape", &currentShape, shapeNames, IM_ARRAYSIZE(shapeNames))) {
		emitter_.shapeType = static_cast<uint32_t>(currentShape);
	}
	
	// Shape-specific parameters
	switch (static_cast<EmitterShapeType>(emitter_.shapeType))
	{
		case EmitterShapeType::POINT:
		{
			// Point emitter has no additional parameters
			ImGui::Text("Point emitter - particles spawn at exact position");
			break;
		}
		
		case EmitterShapeType::LINE:
		{
			ImGui::DragFloat3("Line Start", &emitter_.lineStart.x, 0.01f);
			ImGui::DragFloat3("Line Direction", &emitter_.size.x, 0.01f);
			ImGui::DragFloat("Line Length", &emitter_.lineLength, 0.01f, 0.0f, 100.0f);
			break;
		}
		
		case EmitterShapeType::SPHERE_VOLUME:
		case EmitterShapeType::SPHERE_SURFACE:
		{
			ImGui::DragFloat("Radius", &emitter_.radius, 0.01f, 0.0f, 1000.0f);
			if (static_cast<EmitterShapeType>(emitter_.shapeType) == EmitterShapeType::SPHERE_SURFACE) {
				ImGui::Text("Surface only - particles spawn on sphere surface");
			} else {
				ImGui::Text("Volume - particles spawn inside sphere");
			}
			break;
		}
		
		case EmitterShapeType::BOX_VOLUME:
		{
			ImGui::DragFloat3("Box Size", &emitter_.size.x, 0.01f, 0.0f, 100.0f);
			ImGui::Text("Volume - particles spawn inside box");
			break;
		}
		
		case EmitterShapeType::BOX_SURFACE:
		{
			ImGui::DragFloat3("Box Size", &emitter_.size.x, 0.01f, 0.0f, 100.0f);
			ImGui::Text("Surface only - particles spawn on box faces");
			break;
		}
		
		case EmitterShapeType::RING_XZ:
		{
			ImGui::DragFloat("Inner Radius", &emitter_.ringInnerRadius, 0.01f, 0.0f, 100.0f);
			ImGui::DragFloat("Outer Radius", &emitter_.ringOuterRadius, 0.01f, 0.0f, 100.0f);
			ImGui::Text("Ring in XZ plane (horizontal)");
			// Ensure inner radius is not larger than outer radius
			if (emitter_.ringInnerRadius > emitter_.ringOuterRadius) {
				emitter_.ringInnerRadius = emitter_.ringOuterRadius;
			}
			break;
		}
		
		case EmitterShapeType::RING_XY:
		{
			ImGui::DragFloat("Inner Radius", &emitter_.ringInnerRadius, 0.01f, 0.0f, 100.0f);
			ImGui::DragFloat("Outer Radius", &emitter_.ringOuterRadius, 0.01f, 0.0f, 100.0f);
			ImGui::Text("Ring in XY plane (vertical facing forward)");
			// Ensure inner radius is not larger than outer radius
			if (emitter_.ringInnerRadius > emitter_.ringOuterRadius) {
				emitter_.ringInnerRadius = emitter_.ringOuterRadius;
			}
			break;
		}
		
		case EmitterShapeType::RING_YZ:
		{
			ImGui::DragFloat("Inner Radius", &emitter_.ringInnerRadius, 0.01f, 0.0f, 100.0f);
			ImGui::DragFloat("Outer Radius", &emitter_.ringOuterRadius, 0.01f, 0.0f, 100.0f);
			ImGui::Text("Ring in YZ plane (vertical facing right)");
			// Ensure inner radius is not larger than outer radius
			if (emitter_.ringInnerRadius > emitter_.ringOuterRadius) {
				emitter_.ringInnerRadius = emitter_.ringOuterRadius;
			}
			break;
		}
		
		case EmitterShapeType::CONE_VOLUME:
		{
			ImGui::DragFloat("Cone Angle", &emitter_.coneAngle, 1.0f, 0.0f, 180.0f);
			ImGui::DragFloat("Cone Height", &emitter_.coneHeight, 0.01f, 0.0f, 100.0f);
			ImGui::DragFloat3("Cone Direction", &emitter_.coneDirection.x, 0.01f);
			ImGui::Text("Volume - particles spawn inside cone");
			// Normalize direction vector
			float dirLength = sqrt(emitter_.coneDirection.x * emitter_.coneDirection.x + 
			                      emitter_.coneDirection.y * emitter_.coneDirection.y + 
			                      emitter_.coneDirection.z * emitter_.coneDirection.z);
			if (dirLength > 0.001f) {
				emitter_.coneDirection.x /= dirLength;
				emitter_.coneDirection.y /= dirLength;
				emitter_.coneDirection.z /= dirLength;
			}
			break;
		}
		
		case EmitterShapeType::CONE_SURFACE:
		{
			ImGui::DragFloat("Cone Angle", &emitter_.coneAngle, 1.0f, 0.0f, 180.0f);
			ImGui::DragFloat("Cone Height", &emitter_.coneHeight, 0.01f, 0.0f, 100.0f);
			ImGui::DragFloat3("Cone Direction", &emitter_.coneDirection.x, 0.01f);
			ImGui::Text("Surface only - particles spawn on cone surface");
			// Normalize direction vector
			float dirLength = sqrt(emitter_.coneDirection.x * emitter_.coneDirection.x + 
			                       emitter_.coneDirection.y * emitter_.coneDirection.y + 
			                       emitter_.coneDirection.z * emitter_.coneDirection.z);
			if (dirLength > 0.001f) {
				emitter_.coneDirection.x /= dirLength;
				emitter_.coneDirection.y /= dirLength;
				emitter_.coneDirection.z /= dirLength;
			}
			break;
		}
		
		case EmitterShapeType::HEMISPHERE_VOLUME:
		{
			ImGui::DragFloat("Radius", &emitter_.radius, 0.01f, 0.0f, 1000.0f);
			ImGui::DragFloat("Hemisphere Angle", &emitter_.hemisphereAngle, 1.0f, 0.0f, 180.0f);
			ImGui::DragFloat3("Direction", &emitter_.coneDirection.x, 0.01f);
			ImGui::Text("Volume - particles spawn inside hemisphere");
			// Normalize direction vector
			float dirLength = sqrt(emitter_.coneDirection.x * emitter_.coneDirection.x + 
			                       emitter_.coneDirection.y * emitter_.coneDirection.y + 
			                       emitter_.coneDirection.z * emitter_.coneDirection.z);
			if (dirLength > 0.001f) {
				emitter_.coneDirection.x /= dirLength;
				emitter_.coneDirection.y /= dirLength;
				emitter_.coneDirection.z /= dirLength;
			}
			break;
		}
		
		case EmitterShapeType::HEMISPHERE_SURFACE:
		{
			ImGui::DragFloat("Radius", &emitter_.radius, 0.01f, 0.0f, 1000.0f);
			ImGui::DragFloat("Hemisphere Angle", &emitter_.hemisphereAngle, 1.0f, 0.0f, 180.0f);
			ImGui::DragFloat3("Direction", &emitter_.coneDirection.x, 0.01f);
			ImGui::Text("Surface only - particles spawn on hemisphere surface");
			// Normalize direction vector
			float dirLength = sqrt(emitter_.coneDirection.x * emitter_.coneDirection.x + 
			                       emitter_.coneDirection.y * emitter_.coneDirection.y + 
			                       emitter_.coneDirection.z * emitter_.coneDirection.z);
			if (dirLength > 0.001f) {
				emitter_.coneDirection.x /= dirLength;
				emitter_.coneDirection.y /= dirLength;
				emitter_.coneDirection.z /= dirLength;
			}
			break;
		}
		
		case EmitterShapeType::PLANE_ANGLE:
		{
			ImGui::DragFloat("Plane Width", &emitter_.planeWidth, 0.01f, 0.01f, 100.0f);
			ImGui::DragFloat("Plane Height", &emitter_.planeHeight, 0.01f, 0.01f, 100.0f);
			ImGui::DragFloat3("Plane Normal", &emitter_.planeNormal.x, 0.01f);
			ImGui::Text("Volume - particles spawn on plane surface");
			// Normalize normal vector
			float normalLength = sqrt(emitter_.planeNormal.x * emitter_.planeNormal.x + 
			                         emitter_.planeNormal.y * emitter_.planeNormal.y + 
			                         emitter_.planeNormal.z * emitter_.planeNormal.z);
			if (normalLength > 0.001f) {
				emitter_.planeNormal.x /= normalLength;
				emitter_.planeNormal.y /= normalLength;
				emitter_.planeNormal.z /= normalLength;
			}
			break;
		}
		
		case EmitterShapeType::PLANE_ANGLE_EDGE:
		{
			ImGui::DragFloat("Plane Width", &emitter_.planeWidth, 0.01f, 0.01f, 100.0f);
			ImGui::DragFloat("Plane Height", &emitter_.planeHeight, 0.01f, 0.01f, 100.0f);
			ImGui::DragFloat3("Plane Normal", &emitter_.planeNormal.x, 0.01f);
			ImGui::Text("Edges only - particles spawn on plane edges");
			// Normalize normal vector
			float normalLength = sqrt(emitter_.planeNormal.x * emitter_.planeNormal.x + 
			                         emitter_.planeNormal.y * emitter_.planeNormal.y + 
			                         emitter_.planeNormal.z * emitter_.planeNormal.z);
			if (normalLength > 0.001f) {
				emitter_.planeNormal.x /= normalLength;
				emitter_.planeNormal.y /= normalLength;
				emitter_.planeNormal.z /= normalLength;
			}
			break;
		}
		
		case EmitterShapeType::RING_ANGLE:
		{
			ImGui::DragFloat("Inner Radius", &emitter_.ringInnerRadius, 0.01f, 0.0f, 100.0f);
			ImGui::DragFloat("Outer Radius", &emitter_.ringOuterRadius, 0.01f, 0.0f, 100.0f);
			ImGui::DragFloat3("Ring Normal", &emitter_.ringNormal.x, 0.01f);
			ImGui::DragFloat("Ring Angle", &emitter_.ringAngle, 1.0f, 0.0f, 360.0f);
			ImGui::Text("Volume - particles spawn in ring area");
			// Ensure inner radius is not larger than outer radius
			if (emitter_.ringInnerRadius > emitter_.ringOuterRadius) {
				emitter_.ringInnerRadius = emitter_.ringOuterRadius;
			}
			// Normalize normal vector
			float normalLength = sqrt(emitter_.ringNormal.x * emitter_.ringNormal.x + 
			                         emitter_.ringNormal.y * emitter_.ringNormal.y + 
			                         emitter_.ringNormal.z * emitter_.ringNormal.z);
			if (normalLength > 0.001f) {
				emitter_.ringNormal.x /= normalLength;
				emitter_.ringNormal.y /= normalLength;
				emitter_.ringNormal.z /= normalLength;
			}
			break;
		}
		
		case EmitterShapeType::RING_ANGLE_EDGE:
		{
			ImGui::DragFloat("Inner Radius", &emitter_.ringInnerRadius, 0.01f, 0.0f, 100.0f);
			ImGui::DragFloat("Outer Radius", &emitter_.ringOuterRadius, 0.01f, 0.0f, 100.0f);
			ImGui::DragFloat3("Ring Normal", &emitter_.ringNormal.x, 0.01f);
			ImGui::DragFloat("Ring Angle", &emitter_.ringAngle, 1.0f, 0.0f, 360.0f);
			ImGui::Text("Edge only - particles spawn on ring circumference");
			// Ensure inner radius is not larger than outer radius
			if (emitter_.ringInnerRadius > emitter_.ringOuterRadius) {
				emitter_.ringInnerRadius = emitter_.ringOuterRadius;
			}
			// Normalize normal vector
			float normalLength = sqrt(emitter_.ringNormal.x * emitter_.ringNormal.x + 
			                         emitter_.ringNormal.y * emitter_.ringNormal.y + 
			                         emitter_.ringNormal.z * emitter_.ringNormal.z);
			if (normalLength > 0.001f) {
				emitter_.ringNormal.x /= normalLength;
				emitter_.ringNormal.y /= normalLength;
				emitter_.ringNormal.z /= normalLength;
			}
			break;
		}
	}

	ImGui::Separator();

	ImGui::Checkbox("UseEmitter", reinterpret_cast<bool*>(&emitter_.useEmitter));
	ImGui::Checkbox("Emit", reinterpret_cast<bool*>(&emitter_.emit));
	ImGui::DragInt("Count", reinterpret_cast<int*>(&emitter_.count), 1, 0, kMaxParticles_);

	ImGui::Separator();

	ImGui::DragFloat("Frequency", &emitter_.frequency, 0.01f, 0.0f, 10.0f);
	ImGui::DragFloat("FrequencyTime", &emitter_.frequencyTime, 0.01f, 0.0f, 10.0f);  // frequencyTimerをfrequencyTimeに修正

	ImGui::Separator();

	ImGui::Checkbox("EnableMove", reinterpret_cast<bool*>(&emitter_.isMove));
	ImGui::DragFloat3("normalVelocity", &emitter_.normalVelocity.x, 0.01f);
	ImGui::Checkbox("VelocityRandom", reinterpret_cast<bool*>(&emitter_.velocityRandom));
	if (emitter_.velocityRandom) {
		ImGui::DragFloat3("MinVelocity", &emitter_.minVelocity.x, 0.01f);
		ImGui::DragFloat3("MaxVelocity", &emitter_.maxVelocity.x, 0.01f);
	}

	ImGui::Separator();

	ImGui::Checkbox("ScaleFade", reinterpret_cast<bool*>(&emitter_.scaleFade));
	ImGui::DragFloat2("StartScale", &emitter_.startScale.x, 0.01f, 0.0f, 10.0f);
	ImGui::DragFloat2("EndScale", &emitter_.endScale.x, 0.01f, 0.0f, 10.0f);

	ImGui::Checkbox("ScaleRandom", reinterpret_cast<bool*>(&emitter_.scaleRandom));
	if (emitter_.scaleRandom) {
		ImGui::DragFloat3("MinScale", &emitter_.minScale.x, 0.01f, 0.0f, 10.0f);
		ImGui::DragFloat3("MaxScale", &emitter_.maxScale.x, 0.01f, 0.0f, 10.0f);
	}

	ImGui::Separator();

	ImGui::Checkbox("RotateMove", reinterpret_cast<bool*>(&emitter_.rotateMove));
	ImGui::DragFloat("RightRotateVelocity", &emitter_.startRotateVelocity, 0.01f);
	ImGui::DragFloat("LeftRotateVelocity", &emitter_.endRotateVelocity, 0.01f);

	ImGui::Checkbox("RotateVelocityRandom", reinterpret_cast<bool*>(&emitter_.rotateVelocityRandom));
	if (emitter_.rotateVelocityRandom) {
		ImGui::DragFloat("MinRotateVelocity", &emitter_.minRotateVelocity, 0.01f);
		ImGui::DragFloat("MaxRotateVelocity", &emitter_.maxRotateVelocity, 0.01f);
	}

	ImGui::Separator();

	ImGui::Checkbox("EnableAlphaFade", reinterpret_cast<bool*>(&emitter_.alphaFade));
	ImGui::Checkbox("EnableColorFade", reinterpret_cast<bool*>(&emitter_.colorFade));

	ImGui::ColorEdit3("StartColor", &emitter_.startColor.x);
	ImGui::ColorEdit3("EndColor", &emitter_.endColor.x);

	ImGui::Checkbox("ColorRandom", reinterpret_cast<bool*>(&emitter_.colorRandom));
	if (emitter_.colorRandom) {
		ImGui::ColorEdit3("MinColor", &emitter_.minColor.x);
		ImGui::ColorEdit3("MaxColor", &emitter_.maxColor.x);
	}

	ImGui::Separator();

	ImGui::DragFloat("LifeTime", &emitter_.lifeTime, 0.01f, 0.0f, 100.0f);
	ImGui::Checkbox("LifeTimeRandom", reinterpret_cast<bool*>(&emitter_.lifeTimeRandom));
	if (emitter_.lifeTimeRandom) {
		ImGui::DragFloat("MinLifeTime", &emitter_.minLifeTime, 0.01f, 0.0f, 100.0f);
		ImGui::DragFloat("MaxLifeTime", &emitter_.maxLifeTime, 0.01f, 0.0f, 100.0f);
	}

	ImGui::Separator();

	ImGui::Checkbox("useGravity", reinterpret_cast<bool*>(&emitter_.useGravity));
	ImGui::DragFloat("gravity", &emitter_.gravityY, 0.01f, 0.0f, 100.0f);
	ImGui::DragFloat("acceleration", &emitter_.accelerationY, 0.01f, 0.0f, 100.0f);

	ImGui::Separator();

	// デバッグ情報表示
	ImGui::Text("DEBUG INFO");
	ImGui::Text("Max Particles: %u", kMaxParticles_);
	ImGui::Text("Dispatch Count: %u", kDispatchCount);

	// パーティクル強制リセット
	if (ImGui::Button("Force Reset All Particles")) {
		// 初期化シェーダーを再実行してパーティクルをリセット
		ResetAllParticles();
	}

	ImGui::Separator();
	ImGui::Spacing();

	ImGui::End();
}

void Particles::SetTexture(const std::string& textureName) {
	// すでに同じテクスチャなら処理をスキップ
	if (textureName_ == textureName) {
		return;
	}
	textureName_ = textureName;
	// .objの参照しているテクスチャファイル読み込み
	TextureManager::GetInstance()->LoadTexture(textureName_);
	// 読み込んだテクスチャの番号を取得
	textureIndex_ = TextureManager::GetInstance()->GetTextureIndexByFilePath(textureName_);
}

void Particles::CreateParticleResource() {

	// DefaultHeap上にパーティクル配列を作成（UAV/SRV兼用）
	CD3DX12_HEAP_PROPERTIES heapProp(D3D12_HEAP_TYPE_DEFAULT);
	CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(
		sizeof(ParticleDataCS) * kMaxParticles_,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS // UAV必須
	);

	device_->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&bufferDesc,
		D3D12_RESOURCE_STATE_COMMON, // 初期はCOMMONでOK
		nullptr,
		IID_PPV_ARGS(&particleBufferResource_)
	);

	// UAV: Unordered Access View
	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.NumElements = kMaxParticles_;
	uavDesc.Buffer.StructureByteStride = sizeof(ParticleDataCS);

	device_->CreateUnorderedAccessView(
		particleBufferResource_.Get(),
		nullptr, // CounterResource: nullでOK
		&uavDesc,
		dxCommon_->GetSrvCPUHandle(idxUavParticles_) // UAVヒープ上の任意のスロット
	);

	// SRV: Shader Resource View
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = kMaxParticles_;
	srvDesc.Buffer.StructureByteStride = sizeof(ParticleDataCS);
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

	device_->CreateShaderResourceView(
		particleBufferResource_.Get(),
		&srvDesc,
		dxCommon_->GetSrvCPUHandle(idxSrvParticles_) // SRVヒープ上の任意のスロット
	);

	//----------------------------------------------------------------//

	// freeListIndex u1
    D3D12_RESOURCE_DESC counterDesc2 = CD3DX12_RESOURCE_DESC::Buffer(sizeof(int32_t), D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

	device_->CreateCommittedResource(
		&heapProp, D3D12_HEAP_FLAG_NONE, &counterDesc2,
		D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&freeListIndexResource_));

	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc2 = {};
	uavDesc2.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc2.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	uavDesc2.Buffer.FirstElement = 0;
	uavDesc2.Buffer.NumElements = 1;
	uavDesc2.Buffer.StructureByteStride = sizeof(int32_t);

	device_->CreateUnorderedAccessView(
		freeListIndexResource_.Get(), nullptr, &uavDesc2,
		dxCommon_->GetSrvCPUHandle(idxUavFreeListIndex_));

	// freeList u2
	D3D12_RESOURCE_DESC counterDesc3 = CD3DX12_RESOURCE_DESC::Buffer(sizeof(uint32_t) * kMaxParticles_, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

	device_->CreateCommittedResource(
		&heapProp, D3D12_HEAP_FLAG_NONE, &counterDesc3,
		D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&freeListResource_));

	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc3 = {};
	uavDesc3.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc3.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	uavDesc3.Buffer.FirstElement = 0;
	uavDesc3.Buffer.NumElements = kMaxParticles_;
	uavDesc3.Buffer.StructureByteStride = sizeof(uint32_t);

	device_->CreateUnorderedAccessView(
		freeListResource_.Get(), nullptr, &uavDesc3,
		dxCommon_->GetSrvCPUHandle(idxUavFreeList_));

	//----------------------------------------------------------------//

	// 1. RootSignature作成
	D3D12_ROOT_PARAMETER csRootParams[5] = {};
	csRootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_UAV; // u0: Particleバッファ
	csRootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	csRootParams[0].Descriptor.ShaderRegister = 0;

	csRootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_UAV; // u1: ListIndex
	csRootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	csRootParams[1].Descriptor.ShaderRegister = 1;

	csRootParams[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_UAV; // u2: List
	csRootParams[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	csRootParams[2].Descriptor.ShaderRegister = 2;

	csRootParams[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; // b5: Emitter
	csRootParams[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	csRootParams[3].Descriptor.ShaderRegister = 5;

	csRootParams[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; // b6: PerFrame
	csRootParams[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	csRootParams[4].Descriptor.ShaderRegister = 6;

	D3D12_ROOT_SIGNATURE_DESC csRootSigDesc = {};
	csRootSigDesc.pParameters = csRootParams;
	csRootSigDesc.NumParameters = _countof(csRootParams);
	csRootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;

	ComPtr<ID3DBlob> csSignature, csError;
	D3D12SerializeRootSignature(&csRootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &csSignature, &csError);

	device_->CreateRootSignature(0, csSignature->GetBufferPointer(), csSignature->GetBufferSize(), IID_PPV_ARGS(&csRootSignature_));

	// --- 初期化用CS（ParticlesInitialize.CS.hlsl） ---
	ComPtr<IDxcBlob> csInitBlob = dxCommon_->CompileShader(L"resources/shaders/Particles/InitializeParticle.CS.hlsl", L"cs_6_0");
	D3D12_COMPUTE_PIPELINE_STATE_DESC csInitDesc = {};
	csInitDesc.pRootSignature = csRootSignature_.Get();
	csInitDesc.CS = { csInitBlob->GetBufferPointer(), csInitBlob->GetBufferSize() };
	device_->CreateComputePipelineState(&csInitDesc, IID_PPV_ARGS(&csInitializePipelineState_));

	// --- エミッタ用CS（EmitterParticle.CS.hlsl） ---
	ComPtr<IDxcBlob> csEmitterBlob = dxCommon_->CompileShader(L"resources/shaders/Particles/EmitterParticle.CS.hlsl", L"cs_6_0");
	D3D12_COMPUTE_PIPELINE_STATE_DESC csEmitterDesc = {};
	csEmitterDesc.pRootSignature = csRootSignature_.Get();
	csEmitterDesc.CS = { csEmitterBlob->GetBufferPointer(), csEmitterBlob->GetBufferSize() };
	device_->CreateComputePipelineState(&csEmitterDesc, IID_PPV_ARGS(&csEmitterPipelineState_));

	// --- 動作用CS（EmitterParticle.CS.hlsl） ---
	ComPtr<IDxcBlob> csUpdateBlob = dxCommon_->CompileShader(L"resources/shaders/Particles/UpdateParticle.CS.hlsl", L"cs_6_0");
	D3D12_COMPUTE_PIPELINE_STATE_DESC csUpdateDesc = {};
	csUpdateDesc.pRootSignature = csRootSignature_.Get();
	csUpdateDesc.CS = { csUpdateBlob->GetBufferPointer(), csUpdateBlob->GetBufferSize() };
	device_->CreateComputePipelineState(&csUpdateDesc, IID_PPV_ARGS(&csUpdatePipelineState_));

	// 初期化実行
	ExecuteInitialization();
}

void Particles::ExecuteInitialization() {
	// 1. コマンドアロケータ/リスト作成（DIRECTでOK）
	ComPtr<ID3D12CommandAllocator> alloc;
	device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&alloc));
	ComPtr<ID3D12GraphicsCommandList> list;
	device_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, alloc.Get(), csInitializePipelineState_.Get(), IID_PPV_ARGS(&list));

	// 2. 必要ならResourceBarrier（初期状態COMMON→UNORDERED_ACCESS）
	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		particleBufferResource_.Get(),
		D3D12_RESOURCE_STATE_COMMON,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS
	);
	list->ResourceBarrier(1, &barrier);

	// 3. RootSignature/PSO/UAVセット
	list->SetComputeRootSignature(csRootSignature_.Get());
	list->SetPipelineState(csInitializePipelineState_.Get());
	list->SetComputeRootUnorderedAccessView(0, particleBufferResource_->GetGPUVirtualAddress());
	list->SetComputeRootUnorderedAccessView(1, freeListIndexResource_->GetGPUVirtualAddress());
	list->SetComputeRootUnorderedAccessView(2, freeListResource_->GetGPUVirtualAddress());
	list->SetComputeRootConstantBufferView(3, emitterResource_->GetGPUVirtualAddress());

	// 4. Dispatch
	list->Dispatch(kDispatchCount, 1, 1);

	// 5. BarrierでSRVに戻す（必要なら）
	auto barrier2 = CD3DX12_RESOURCE_BARRIER::Transition(
		particleBufferResource_.Get(),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE
	);
	list->ResourceBarrier(1, &barrier2);

	// 6. コマンド実行＆同期
	list->Close();
	ID3D12CommandList* lists[] = { list.Get() };
	dxCommon_->GetCommandQueue()->ExecuteCommandLists(_countof(lists), lists);
	// 必要に応じてWaitForGPU()でGPU待ち
	dxCommon_->WaitForGPU();
}

void Particles::ResetAllParticles() {
	OutputDebugStringA("Force resetting all particles...\n");
	ExecuteInitialization();
	OutputDebugStringA("Particle reset completed.\n");
}

void Particles::UpdateParticle() {
	// 1. 必要な場合だけ「UAV」に遷移
	D3D12_RESOURCE_BARRIER toUAV = CD3DX12_RESOURCE_BARRIER::Transition(
		particleBufferResource_.Get(),
		D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, // ←前回の状態に応じて
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS
	);
	commandList_->ResourceBarrier(1, &toUAV);

	// Emitterの更新、Particleの生成
	commandList_->SetComputeRootSignature(csRootSignature_.Get());
	commandList_->SetPipelineState(csEmitterPipelineState_.Get()); // CSをセット
	commandList_->SetComputeRootUnorderedAccessView(0, particleBufferResource_->GetGPUVirtualAddress()); // u0
	commandList_->SetComputeRootUnorderedAccessView(1, freeListIndexResource_->GetGPUVirtualAddress());  // u1
	commandList_->SetComputeRootUnorderedAccessView(2, freeListResource_->GetGPUVirtualAddress());       // u2
	commandList_->SetComputeRootConstantBufferView(3, emitterResource_->GetGPUVirtualAddress());         // b5
	commandList_->SetComputeRootConstantBufferView(4, perFrameResource_->GetGPUVirtualAddress());        // b6
	// Emitterの処理を実行
	commandList_->Dispatch(1, 1, 1);

	D3D12_RESOURCE_BARRIER bs[] = {
    CD3DX12_RESOURCE_BARRIER::UAV(particleBufferResource_.Get()),
    CD3DX12_RESOURCE_BARRIER::UAV(freeListIndexResource_.Get()),
    CD3DX12_RESOURCE_BARRIER::UAV(freeListResource_.Get()),
	};
	commandList_->ResourceBarrier(_countof(bs), bs);

	// Particleの更新
	commandList_->SetComputeRootSignature(csRootSignature_.Get());
	commandList_->SetPipelineState(csUpdatePipelineState_.Get()); // CSをセット
	commandList_->SetComputeRootUnorderedAccessView(0, particleBufferResource_->GetGPUVirtualAddress()); // u0
	commandList_->SetComputeRootUnorderedAccessView(1, freeListIndexResource_->GetGPUVirtualAddress());  // u1
	commandList_->SetComputeRootUnorderedAccessView(2, freeListResource_->GetGPUVirtualAddress());       // u2
	commandList_->SetComputeRootConstantBufferView(3, emitterResource_->GetGPUVirtualAddress());         // b5
	commandList_->SetComputeRootConstantBufferView(4, perFrameResource_->GetGPUVirtualAddress());        // b6
	// Particleの更新を「kDispatchCount」の数分実行
	commandList_->Dispatch(kDispatchCount, 1, 1);

	// 4. SRV状態へバリア
	D3D12_RESOURCE_BARRIER toSRV = CD3DX12_RESOURCE_BARRIER::Transition(
		particleBufferResource_.Get(),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_GENERIC_READ
	);
	commandList_->ResourceBarrier(1, &toSRV);
}

void Particles::CreateEmitterResource() {

	CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(EmitterState));

	device_->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&emitterResource_)
	);
}

void Particles::UpdateEmitter() {
	// このemitterSphereをCBufferとしてGPUへ転送
	if (emitter_.useEmitter) {
		emitter_.frequencyTime += kDeltaTime_;  // frequencyTimerをfrequencyTimeに修正
	}

	if (emitter_.frequency <= emitter_.frequencyTime) {
		emitter_.frequencyTime = 0.0f;
		emitter_.emit = true;
	} else {
		emitter_.emit = false;
	}

	emitter_.kMaxParticle = kMaxParticles_;
	emitter_.translate = emitter_.translate + offset_;

	// Unmapは不要。UploadHeapの場合、毎フレームマップしっぱなしでOK
	EmitterState* mappedEmitter = nullptr;
	emitterResource_->Map(0, nullptr, reinterpret_cast<void**>(&mappedEmitter));
	// ここで値をコピーまたは書き換え
	*mappedEmitter = emitter_; // 構造体ごとコピー
}

void Particles::CreatePerFrameResource() {
	CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(PerFrame));
	device_->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&perFrameResource_)
	);
}

void Particles::UpdatePerFrame() {
	frameIndex++;
	totalTime_ += kDeltaTime_;
	// フレームごとに
	PerFrame* mapped = nullptr;
	perFrameResource_->Map(0, nullptr, reinterpret_cast<void**>(&mapped));
	mapped->time = totalTime_;      // 例えば経過時間など
	mapped->deltaTime = kDeltaTime_;  // フレームごとの経過時間
	mapped->index = frameIndex;
	mapped->pad1 = 0.0f;            // パディングを初期化
}

void Particles::CreatePerViewResource() {
	perViewResource_ = CreateBufferResource(device_.Get(), sizeof(PerView));

	// 書き込むためのアドレスを取得
	perViewResource_->Map(0, nullptr, reinterpret_cast<void**>(&perViewData_));

	// 今回は赤を書き込んでみる（position に赤、texcoord は使わないなら 0.0）
	perViewData_->viewProjection = {};
	perViewData_->billboardMatrix = {};

	perViewResource_->Unmap(0, nullptr);
}

void Particles::CreateVertexResource() {
	// 頂点リソース
	vertexResource_ = CreateBufferResource(device_.Get(), sizeof(ParticleVertexData) * 4);
	// リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	// 使用するリソースのサイズ
	vertexBufferView_.SizeInBytes = sizeof(ParticleVertexData) * 4;
	// 1頂点あたりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(ParticleVertexData);
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

	vertexData_[0].position = { -0.5f,-0.5f,0.0f,1.0f };
	vertexData_[1].position = { -0.5f,0.5f,0.0f,1.0f };
	vertexData_[2].position = { 0.5f,-0.5f,0.0f,1.0f };
	vertexData_[3].position = { 0.5f,0.5f,0.0f,1.0f };

	vertexData_[0].texcoord = { 0.f,1.0f };
	vertexData_[1].texcoord = { 0.0f,0.0f };
	vertexData_[2].texcoord = { 1.0f,1.f };
	vertexData_[3].texcoord = { 1.0f,0.0f };
}

void Particles::CreateIndexResource() {
	// IndexResource
	indexResource_ = CreateBufferResource(device_.Get(), sizeof(uint32_t) * 6);
	// リソースの先頭のアドレスから使う
	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	// 使用するリソースのサイズ
	indexBufferView_.SizeInBytes = sizeof(uint32_t) * 6;
	// format
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));
	// 表面（三角形2枚）
	indexData_[0] = 0; indexData_[1] = 1; indexData_[2] = 2;
	indexData_[3] = 1; indexData_[4] = 3; indexData_[5] = 2;
}

void Particles::CreateMaterialResource() {
	// MaterialResource
	materialResource_ = CreateBufferResource(device_.Get(), sizeof(ParticleMaterial));
	// 書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	// 今回は赤を書き込んでみる（position に赤、texcoord は使わないなら 0.0）
	materialData_->color = { 1.0f, 1.0f, 1.0f, 1.0f }; // 白 (RGBA)
	materialData_->uvTransform = MakeIdentityMatrix();

	materialResource_->Unmap(0, nullptr);
}