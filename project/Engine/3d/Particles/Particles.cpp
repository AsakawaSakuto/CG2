#include "Particles.h"
#include "../Model/Model.h"
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

	DescriptorAllocator& alloc = dxCommon_->GetParticleAlloc();

	// 既に初期化済みの場合は、リソース作成をスキップ
	if (isInitialized_) {
		// Emitterの値だけリセット
		ResetEmitterToDefault();
		return;
	}

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
	ResetEmitterToDefault();

	CreateEmitterResource();
	CreateParticleResource();
	CreatePerViewResource();
	CreatePerFrameResource();
	
	isInitialized_ = true;
}

// 新しいヘルパー関数：Emitterをデフォルト値にリセット
void Particles::ResetEmitterToDefault() {
	emitter_.translate = { 0.0f, 0.0f, 0.0f };
	emitter_.radius = 1.0f;
	emitter_.useEmitter = 1;
	emitter_.emit = 0;
	emitter_.count = 1;
	emitter_.kMaxParticle = kMaxParticles_;
	emitter_.frequency = 0.1f;
	emitter_.frequencyTime = 0.0f;
	emitter_.startScale = { 1.0f, 1.0f };
	emitter_.endScale = { 0.0f, 0.0f };
	emitter_.scaleFade = 0;
	emitter_.scaleRandom = 0;
	emitter_.minScale = { 0.0f, 0.0f, 0.0f };
	emitter_.maxScale = { 1.0f, 1.0f, 1.0f };
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
	emitter_.lifeTime = 5.0f;
	emitter_.lifeTimeRandom = 0;
	emitter_.minLifeTime = 1.0f;
	emitter_.maxLifeTime = 5.0f;
	emitter_.isMove = 1;
	emitter_.velocityRandom = 1;
	emitter_.scaleFade = 1;
	emitter_.alphaFade = 1;
	emitter_.colorRandom = 1;
	emitter_.shapeType = static_cast<uint32_t>(EmitterShapeType::POINT);
	emitter_.texturePath = "";
	emitter_.blendMode = kBlendModeAdd;
}

void Particles::Update() {

	// バッファリセットが必要な場合、コンピュートシェーダーで即座にリセット
	if (needsBufferReset_) {
		// コマンドリスト上で直接初期化シェーダーを実行（GPU同期なし）
		D3D12_RESOURCE_BARRIER toUAV = CD3DX12_RESOURCE_BARRIER::Transition(
			particleBufferResource_.Get(),
			D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS
		);
		commandList_->ResourceBarrier(1, &toUAV);

		commandList_->SetComputeRootSignature(csRootSignature_.Get());
		commandList_->SetPipelineState(csInitializePipelineState_.Get());
		commandList_->SetComputeRootUnorderedAccessView(0, particleBufferResource_->GetGPUVirtualAddress());
		commandList_->SetComputeRootUnorderedAccessView(1, freeListIndexResource_->GetGPUVirtualAddress());
		commandList_->SetComputeRootUnorderedAccessView(2, freeListResource_->GetGPUVirtualAddress());
		commandList_->SetComputeRootConstantBufferView(3, emitterResource_->GetGPUVirtualAddress());
		commandList_->Dispatch(kDispatchCount, 1, 1);

		D3D12_RESOURCE_BARRIER toSRV = CD3DX12_RESOURCE_BARRIER::Transition(
			particleBufferResource_.Get(),
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
			D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE
		);
		commandList_->ResourceBarrier(1, &toSRV);

		needsBufferReset_ = false;
	}

	// ---- カメラ関連の行列計算（PreView構造体に書き込む） ----
	Matrix4x4 cameraMatrix = MakeAffineMatrix(camera_.GetScale(), camera_.GetRotate(), camera_.GetTranslate());
	Matrix4x4 backToFrontMatrix = MakeRotateYMatrix(std::numbers::pi_v<float>);
	Matrix4x4 billboardMatrix = cameraMatrix;

	//Matrix4x4 billboardMatrix = MultiplyMatrix(backToFrontMatrix, cameraMatrix);

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

#ifdef USE_IMGUI

	ImGui::Begin(objectName);

	// Play/Stop control buttons
	if (ImGui::Button("再生 (ループ)")) {
		Play(true);
	}

	ImGui::SameLine();

	if (ImGui::Button("再生 (1回)")) {
		Play(false);
	}

	ImGui::SameLine();

	if (ImGui::Button("停止")) {
		Stop();
	}

	// 現在の再生状態を表示
	ImGui::Text("状態: %s", isPlaying_ ? "再生中" : "停止中");

	ImGui::Separator();

	// BlendMode選択コンボボックスを追加
	const char* blendModeNames[] = {
		"None (なし)",
		"Normal (通常)", 
		"Add (加算)",
		"Subtract (減算)",
		"Multily (乗算)",
		"Screen (スクリーン)"
	};
	int currentBlendMode = static_cast<int>(emitter_.blendMode);
	if (ImGui::Combo("ブレンドモード", &currentBlendMode, blendModeNames, IM_ARRAYSIZE(blendModeNames))) {
		emitter_.blendMode = static_cast<BlendMode>(currentBlendMode);
		blendMode_ = emitter_.blendMode; // 内部のblendMode_も更新
	}

	ImGui::Separator();

	if (EmitterStateLoader::InputText("JSONファイル名", loadToSaveName_)) {
		// 入力が変更されたらここに来る
		printf("Generate Name Changed to: %s\n", loadToSaveName_.c_str());
	}

	if (ImGui::Button("JSONから読み込み")) {
		jsonFilePath_ = "resources/Data/Particle/" + (loadToSaveName_ + ".json");
		emitter_ = EmitterStateLoader::Load(jsonFilePath_);
		
		// JSONから読み込んだBlendModeを内部変数にも反映
		blendMode_ = emitter_.blendMode;

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

	if (ImGui::Button("JSONに保存")) {
		jsonFilePath_ = "resources/Data/Particle/" + (loadToSaveName_ + ".json");
		EmitterStateLoader::Save(jsonFilePath_, emitter_);
	}

	ImGui::SameLine();

	if (ImGui::Button("JSON生成")) {
		EmitterStateLoader::SaveToCurrentDir(emitter_, loadToSaveName_);
	}

	ImGui::Separator();

	if (EmitterStateLoader::InputText("テクスチャ名", emitter_.texturePath)) {
		// 入力が変更されたらここに来る
		printf("Texture Name Changed to: %s\n", emitter_.texturePath.c_str());
	}

	if (ImGui::Button("テクスチャ読み込み")) {
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

	ImGui::DragFloat3("位置", &emitter_.translate.x, 0.01f);

	ImGui::DragFloat3("オフセット", &offset_.x, 0.01f);
	
	// Emitter Shape Selection
	const char* shapeNames[] = { 
		"点", 
		"線", 
		"球 (内部)", 
		"球 (表面)", 
		"箱 (内部)", 
		"箱 (表面)", 
		"リング (XZ平面)", 
		"リング (XY平面)", 
		"リング (YZ平面)",
		"円錐 (内部)",
		"円錐 (表面)",
		"半球 (内部)",
		"半球 (表面)",
		"平面 (角度範囲)",
		"平面 (エッジ)",
		"リング (角度範囲)",
		"リング (角度エッジ)"
	};
	int currentShape = static_cast<int>(emitter_.shapeType);
	if (ImGui::Combo("エミッター形状", &currentShape, shapeNames, IM_ARRAYSIZE(shapeNames))) {
		emitter_.shapeType = static_cast<uint32_t>(currentShape);
	}
	
	// Shape-specific parameters
	switch (static_cast<EmitterShapeType>(emitter_.shapeType))
	{
		case EmitterShapeType::POINT:
		{
			// Point emitter has no additional parameters
			ImGui::Text("点エミッター - 正確な位置で生成");
			break;
		}
		
		case EmitterShapeType::LINE:
		{
			ImGui::DragFloat3("線の開始位置", &emitter_.lineStart.x, 0.01f);
			ImGui::DragFloat3("線の方向", &emitter_.size.x, 0.01f);
			ImGui::DragFloat("線の長さ", &emitter_.lineLength, 0.01f, 0.0f, 100.0f);
			break;
		}
		
		case EmitterShapeType::SPHERE_VOLUME:
		case EmitterShapeType::SPHERE_SURFACE:
		{
			ImGui::DragFloat("半径", &emitter_.radius, 0.01f, 0.0f, 1000.0f);
			if (static_cast<EmitterShapeType>(emitter_.shapeType) == EmitterShapeType::SPHERE_SURFACE) {
				ImGui::Text("表面のみ - 球の表面で生成");
			} else {
				ImGui::Text("内部 - 球の内部で生成");
			}
			break;
		}
		
		case EmitterShapeType::BOX_VOLUME:
		{
			ImGui::DragFloat3("箱のサイズ", &emitter_.size.x, 0.01f, 0.0f, 100.0f);
			ImGui::Text("内部 - 箱の内部で生成");
			break;
		}
		
		case EmitterShapeType::BOX_SURFACE:
		{
			ImGui::DragFloat3("箱のサイズ", &emitter_.size.x, 0.01f, 0.0f, 100.0f);
			ImGui::Text("表面のみ - 箱の表面で生成");
			break;
		}
		
		case EmitterShapeType::RING_XZ:
		{
			ImGui::DragFloat("内側半径", &emitter_.ringInnerRadius, 0.01f, 0.0f, 100.0f);
			ImGui::DragFloat("外側半径", &emitter_.ringOuterRadius, 0.01f, 0.0f, 100.0f);
			ImGui::Text("XZ平面のリング (水平)");
			// Ensure inner radius is not larger than outer radius
			if (emitter_.ringInnerRadius > emitter_.ringOuterRadius) {
				emitter_.ringInnerRadius = emitter_.ringOuterRadius;
			}
			break;
		}
		
		case EmitterShapeType::RING_XY:
		{
			ImGui::DragFloat("内側半径", &emitter_.ringInnerRadius, 0.01f, 0.0f, 100.0f);
			ImGui::DragFloat("外側半径", &emitter_.ringOuterRadius, 0.01f, 0.0f, 100.0f);
			ImGui::Text("XY平面のリング (垂直・前向き)");
			// Ensure inner radius is not larger than outer radius
			if (emitter_.ringInnerRadius > emitter_.ringOuterRadius) {
				emitter_.ringInnerRadius = emitter_.ringOuterRadius;
			}
			break;
		}
		
		case EmitterShapeType::RING_YZ:
		{
			ImGui::DragFloat("内側半径", &emitter_.ringInnerRadius, 0.01f, 0.0f, 100.0f);
			ImGui::DragFloat("外側半径", &emitter_.ringOuterRadius, 0.01f, 0.0f, 100.0f);
			ImGui::Text("YZ平面のリング (垂直・右向き)");
			// Ensure inner radius is not larger than outer radius
			if (emitter_.ringInnerRadius > emitter_.ringOuterRadius) {
				emitter_.ringInnerRadius = emitter_.ringOuterRadius;
			}
			break;
		}
		
		case EmitterShapeType::CONE_VOLUME:
		{
			ImGui::DragFloat("円錐角度", &emitter_.coneAngle, 1.0f, 0.0f, 180.0f);
			ImGui::DragFloat("円錐高さ", &emitter_.coneHeight, 0.01f, 0.0f, 100.0f);
			ImGui::DragFloat3("円錐方向", &emitter_.coneDirection.x, 0.01f);
			ImGui::Text("内部 - 円錐の内部で生成");
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
			ImGui::DragFloat("円錐角度", &emitter_.coneAngle, 1.0f, 0.0f, 180.0f);
			ImGui::DragFloat("円錐高さ", &emitter_.coneHeight, 0.01f, 0.0f, 100.0f);
			ImGui::DragFloat3("円錐方向", &emitter_.coneDirection.x, 0.01f);
			ImGui::Text("表面のみ - 円錐の表面で生成");
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
			ImGui::DragFloat("半径", &emitter_.radius, 0.01f, 0.0f, 1000.0f);
			ImGui::DragFloat("半球角度", &emitter_.hemisphereAngle, 1.0f, 0.0f, 180.0f);
			ImGui::DragFloat3("方向", &emitter_.coneDirection.x, 0.01f);
			ImGui::Text("内部 - 半球の内部で生成");
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
			ImGui::DragFloat("半径", &emitter_.radius, 0.01f, 0.0f, 1000.0f);
			ImGui::DragFloat("半球角度", &emitter_.hemisphereAngle, 1.0f, 0.0f, 180.0f);
			ImGui::DragFloat3("方向", &emitter_.coneDirection.x, 0.01f);
			ImGui::Text("表面のみ - 半球の表面で生成");
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
			ImGui::DragFloat("平面幅", &emitter_.planeWidth, 0.01f, 0.01f, 100.0f);
			ImGui::DragFloat("平面高さ", &emitter_.planeHeight, 0.01f, 0.01f, 100.0f);
			ImGui::DragFloat3("平面法線", &emitter_.planeNormal.x, 0.01f);
			ImGui::Text("範囲 - 平面上で生成");
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
			ImGui::DragFloat("平面幅", &emitter_.planeWidth, 0.01f, 0.01f, 100.0f);
			ImGui::DragFloat("平面高さ", &emitter_.planeHeight, 0.01f, 0.01f, 100.0f);
			ImGui::DragFloat3("平面法線", &emitter_.planeNormal.x, 0.01f);
			ImGui::Text("エッジのみ - 平面のエッジで生成");
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
			ImGui::DragFloat("内側半径", &emitter_.ringInnerRadius, 0.01f, 0.0f, 100.0f);
			ImGui::DragFloat("外側半径", &emitter_.ringOuterRadius, 0.01f, 0.0f, 100.0f);
			ImGui::DragFloat3("リング法線", &emitter_.ringNormal.x, 0.01f);
			ImGui::DragFloat("リング角度", &emitter_.ringAngle, 1.0f, 0.0f, 360.0f);
			ImGui::Text("範囲 - リング範囲内で生成");
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
			ImGui::DragFloat("内側半径", &emitter_.ringInnerRadius, 0.01f, 0.0f, 100.0f);
			ImGui::DragFloat("外側半径", &emitter_.ringOuterRadius, 0.01f, 0.0f, 100.0f);
			ImGui::DragFloat3("リング法線", &emitter_.ringNormal.x, 0.01f);
			ImGui::DragFloat("リング角度", &emitter_.ringAngle, 1.0f, 0.0f, 360.0f);
			ImGui::Text("エッジのみ - リングの円周上で生成");
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

	ImGui::Checkbox("エミッター使用", reinterpret_cast<bool*>(&emitter_.useEmitter));
	ImGui::Checkbox("放出", reinterpret_cast<bool*>(&emitter_.emit));
	ImGui::DragInt("放出数", reinterpret_cast<int*>(&emitter_.count), 1, 0, kMaxParticles_);

	ImGui::Separator();

	ImGui::DragFloat("放出頻度", &emitter_.frequency, 0.01f, 0.0f, 10.0f);
	ImGui::DragFloat("頻度タイマー", &emitter_.frequencyTime, 0.01f, 0.0f, 10.0f);

	ImGui::Separator();

	ImGui::Checkbox("移動有効", reinterpret_cast<bool*>(&emitter_.isMove));
	ImGui::DragFloat3("通常速度", &emitter_.normalVelocity.x, 0.01f);
	ImGui::Checkbox("速度ランダム", reinterpret_cast<bool*>(&emitter_.velocityRandom));
	if (emitter_.velocityRandom) {
		ImGui::DragFloat3("最小速度", &emitter_.minVelocity.x, 0.01f);
		ImGui::DragFloat3("最大速度", &emitter_.maxVelocity.x, 0.01f);
	}

	ImGui::Separator();

	ImGui::Checkbox("スケールフェード", reinterpret_cast<bool*>(&emitter_.scaleFade));
	ImGui::DragFloat2("開始スケール", &emitter_.startScale.x, 0.01f, 0.0f, 10.0f);
	ImGui::DragFloat2("終了スケール", &emitter_.endScale.x, 0.01f, 0.0f, 10.0f);

	ImGui::Checkbox("スケールランダム", reinterpret_cast<bool*>(&emitter_.scaleRandom));
	if (emitter_.scaleRandom) {
		ImGui::DragFloat3("最小スケール", &emitter_.minScale.x, 0.01f, 0.0f, 10.0f);
		ImGui::DragFloat3("最大スケール", &emitter_.maxScale.x, 0.01f, 0.0f, 10.0f);
	}

	ImGui::Separator();

	ImGui::Checkbox("回転移動", reinterpret_cast<bool*>(&emitter_.rotateMove));
	ImGui::DragFloat("右回転速度", &emitter_.startRotateVelocity, 0.01f);
	ImGui::DragFloat("左回転速度", &emitter_.endRotateVelocity, 0.01f);

	ImGui::Checkbox("回転速度ランダム", reinterpret_cast<bool*>(&emitter_.rotateVelocityRandom));
	if (emitter_.rotateVelocityRandom) {
		ImGui::DragFloat("最小回転速度", &emitter_.minRotateVelocity, 0.01f);
		ImGui::DragFloat("最大回転速度", &emitter_.maxRotateVelocity, 0.01f);
	}

	ImGui::Separator();

	ImGui::Checkbox("アルファフェード有効", reinterpret_cast<bool*>(&emitter_.alphaFade));
	ImGui::Checkbox("カラーフェード有効", reinterpret_cast<bool*>(&emitter_.colorFade));

	ImGui::ColorEdit3("開始色", &emitter_.startColor.x);
	ImGui::ColorEdit3("終了色", &emitter_.endColor.x);

	ImGui::Checkbox("カラーランダム", reinterpret_cast<bool*>(&emitter_.colorRandom));
	if (emitter_.colorRandom) {
		ImGui::ColorEdit3("最小色", &emitter_.minColor.x);
		ImGui::ColorEdit3("最大色", &emitter_.maxColor.x);
	}

	ImGui::Separator();

	ImGui::DragFloat("寿命", &emitter_.lifeTime, 0.01f, 0.0f, 100.0f);
	ImGui::Checkbox("寿命ランダム", reinterpret_cast<bool*>(&emitter_.lifeTimeRandom));
	if (emitter_.lifeTimeRandom) {
		ImGui::DragFloat("最小寿命", &emitter_.minLifeTime, 0.01f, 0.0f, 100.0f);
		ImGui::DragFloat("最大寿命", &emitter_.maxLifeTime, 0.01f, 0.0f, 100.0f);
	}

	ImGui::Separator();

	ImGui::Checkbox("重力使用", reinterpret_cast<bool*>(&emitter_.useGravity));
	ImGui::DragFloat("重力", &emitter_.gravityY, 0.01f, 0.0f, 100.0f);
	ImGui::DragFloat("加速度", &emitter_.accelerationY, 0.01f, 0.0f, 100.0f);

	ImGui::Separator();

	// デバッグ情報表示
	ImGui::Text("デバッグ情報");
	ImGui::Text("最大パーティクル数: %u", kMaxParticles_);
	ImGui::Text("ディスパッチ数: %u", kDispatchCount);

	// パーティクル強制リセット
	if (ImGui::Button("全パーティクル強制リセット")) {
		// 初期化シェーダーを再実行してパーティクルをリセット
		ResetAllParticles();
	}

	ImGui::Separator();
	ImGui::Spacing();

	ImGui::End();

#endif
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
	CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(EmitterStateGPU)); // ✅ サイズ変更

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
	
	// BlendModeをuint32_tに変換してGPU構造体に格納
	emitter_.blendModeValue = static_cast<uint32_t>(emitter_.blendMode);

	// EmitterStateをマップしてGPU互換部分のみをコピー
	EmitterStateGPU* mappedEmitter = nullptr;
	emitterResource_->Map(0, nullptr, reinterpret_cast<void**>(&mappedEmitter));
	
	// PODデータ（EmitterStateGPU）のみをコピー
	memcpy(mappedEmitter, static_cast<EmitterStateGPU*>(&emitter_), sizeof(EmitterStateGPU));
	
	// Unmapを追加（リソースリークを防ぐ）
	emitterResource_->Unmap(0, nullptr);
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

Particles::~Particles() {
	// 確保したSRV/UAVインデックスを返却
	if (dxCommon_) {
		DescriptorAllocator& alloc = dxCommon_->GetParticleAlloc();
		alloc.Free(idxSrvParticles_);
		alloc.Free(idxUavParticles_);
		alloc.Free(idxUavFreeListIndex_);
		alloc.Free(idxUavFreeList_);
	}
}

void Particles::LoadJson(const std::string& filePath) {
	// 既に同じファイルを読み込んでいる場合はスキップ
	std::string fullPath = "resources/Data/Particle/" + (filePath + ".json");
	if (jsonFilePath_ == fullPath && isJsonLoaded_) {
		return;
	}
	
	jsonFilePath_ = fullPath;
	
	// 完全なコピーで読み込み
	EmitterState loadedState = EmitterStateLoader::Load(jsonFilePath_);
	emitter_ = loadedState;  // 構造体全体をコピー
	
	loadToSaveName_ = filePath;
	
	// JSONから読み込んだBlendModeを内部変数にも反映
	blendMode_ = emitter_.blendMode;
	
	// texturePath をローカル変数に完全コピー
	if (!emitter_.texturePath.empty()) {
		// 安全なコピーを作成
		std::string texturePathCopy = emitter_.texturePath;
		std::string newTextureName = "resources/image/particle/" + texturePathCopy + ".png";
		
		if (textureName_ != newTextureName) {
			// 先にメンバ変数に代入
			textureName_ = newTextureName;
			
			// メンバ変数を使用（安全）
			TextureManager::GetInstance()->LoadTexture(textureName_);
			textureIndex_ = TextureManager::GetInstance()->GetTextureIndexByFilePath(textureName_);
		}
	}
	
	isJsonLoaded_ = true;
}