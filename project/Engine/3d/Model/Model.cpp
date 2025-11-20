#define NOMINMAX
#include "Model.h"
#include "WinApp.h"
#include <algorithm>
#include <cmath>

#include<cassert>
#pragma comment(lib,"d3d12.lib")
using namespace Microsoft::WRL;

// 修正: PSOManagerをインクルード（相対パス修正）
#include "../../System/PSOManager/PSOManager.h"
#include "../Utility/GameTimer/DeltaTime.h"
#include "../Model/Animation/Function/AnimationFunction.h"
#include "../Engine/System/HeapManager/DescriptorAllocator.h"

// 共有キャッシュの定義
std::unordered_map<std::string, std::shared_ptr<Model::GeometryCache>> Model::s_geometryCache_;

//"resources/uvChecker.png"

Model::~Model() {
	// 他のマップされたリソースもアンマップ
	if (materialResource_ && materialData_) {
		materialResource_->Unmap(0, nullptr);
		materialData_ = nullptr;
	}
}

void Model::Initialize(DirectXCommon* dxCommon,  const std::string& modelPath) {
	dxCommon_ = dxCommon;
	device_ = dxCommon_->GetDevice();
	commandList_ = dxCommon_->GetCommandList();

	modelPath_ = "resources/model/" + modelPath;

	// 拡張子を取得
	std::string extension = "";
	size_t dotPos = modelPath.rfind('.');
	if (dotPos != std::string::npos) {
		extension = modelPath.substr(dotPos);
		// 小文字に変換
		std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
	}

	// まずキャッシュを探す
	auto it = s_geometryCache_.find(modelPath_);
	if (it == s_geometryCache_.end()) {
		// 未ロードなので新規ロードしてキャッシュ
		auto cache = std::make_shared<GeometryCache>();

		// 拡張子によって処理を分岐
		if (extension == ".obj") {

			// .obj形式の処理
			cache->modelData = LoadObject3dFile(modelPath_);
			cache->animationData = {}; // アニメーションデータを初期化

		} else if (extension == ".gltf" || extension == ".glb") {

			// .gltf/.glb形式の処理
			cache->modelData = LoadObject3dFile(modelPath_);
			cache->animationData = LoadAnimationFile(modelPath);

		} else {
			Logger::Log("Unsupported model format: %s\n");
		}

		// アニメーションタイプを判定
		// スキンクラスターデータがある → BONE（ボーン付きスキニング）
		if (!cache->modelData.skinClusterData.empty()) {
			animationType_ = AnimationType::SKINNING;
			useAnimationTimer_ = true;
			
			cache->skeletonData = CreateSkeleton(cache->modelData.rootNode);

			// Model専用のSRVアロケータから空きインデックスを取得
			cache->skinClusterIndex = dxCommon_->GetModelAlloc().Allocate();

			cache->skinClusterData = CreateSkinCluster(
				device_,
				cache->skeletonData,
				cache->modelData,
				dxCommon_->GetSRV(),
				dxCommon_->GetDescriptorSizeSRV(),
				cache->skinClusterIndex // 動的に割り当てられたインデックスを使用
			);

			char debugBuffer[256];
			sprintf_s(debugBuffer, "AnimationType set to BONE for model: %s\n", modelPath_.c_str());
			OutputDebugStringA(debugBuffer);
			sprintf_s(debugBuffer, "SkinClusterData count: %zu\n", cache->modelData.skinClusterData.size());
			OutputDebugStringA(debugBuffer);
		}
		// アニメーションデータがある → NORMAL（キーフレームアニメーション）
		else if (!cache->animationData.nodeAnimations.empty()) {
			animationType_ = AnimationType::NORMAL;
			useAnimationTimer_ = true;
			
			char debugBuffer[256];
			sprintf_s(debugBuffer, "AnimationType set to NORMAL for model: %s\n", modelPath_.c_str());
			OutputDebugStringA(debugBuffer);
			sprintf_s(debugBuffer, "NodeAnimations count: %zu\n", cache->animationData.nodeAnimations.size());
			OutputDebugStringA(debugBuffer);
		}
		// 両方ない → NONE（アニメーションなし）
		else {
			animationType_ = AnimationType::NONE;
			useAnimationTimer_ = false;
			
			char debugBuffer[256];
			sprintf_s(debugBuffer, "AnimationType set to NONE for model: %s\n", modelPath_.c_str());
			OutputDebugStringA(debugBuffer);
		}

		// バウンディング半径を自動計算
		float maxDistanceSquared = 0.0f;
		for (const auto& vertex : cache->modelData.vertices) {
			float distanceSquared =
				vertex.position.x * vertex.position.x +
				vertex.position.y * vertex.position.y +
				vertex.position.z * vertex.position.z;
			maxDistanceSquared = std::max(maxDistanceSquared, distanceSquared);
		}
		cache->boundingRadius = std::sqrt(maxDistanceSquared);
		if (cache->boundingRadius < 0.1f) {
			cache->boundingRadius = 1.0f;
		}

		cache->textureName = cache->modelData.material.textureFilePath;
		// .objの参照しているテクスチャファイル読み込み（初回のみ）
		TextureManager::GetInstance()->LoadTexture(cache->textureName);
		// 読み込んだテクスチャの番号を取得
		cache->textureIndex = TextureManager::GetInstance()->GetTextureIndexByFilePath(cache->textureName);

		// 頂点リソースをつくる（共有）
  		cache->indexResource = CreateBufferResource(device_.Get(), sizeof(uint32_t) * cache->modelData.indeces.size());
		cache->indexBufferView.BufferLocation = cache->indexResource->GetGPUVirtualAddress(); // ここのエラーは.mltファイルのTexturePathが間違えてる可能性が高い
		cache->indexBufferView.SizeInBytes = UINT(sizeof(uint32_t) * cache->modelData.indeces.size());
		cache->indexBufferView.Format = DXGI_FORMAT_R32_UINT;
		// 頂点データ書き込み（一時マップ）
 		uint32_t* tmp = nullptr;
 		cache->indexResource->Map(0, nullptr, reinterpret_cast<void**>(&tmp));
 		std::memcpy(tmp, cache->modelData.indeces.data(), sizeof(uint32_t) * cache->modelData.indeces.size());
 		cache->indexResource->Unmap(0, nullptr);

		// 頂點リソースをつくる（共有）
 		cache->vertexResource = CreateBufferResource(device_.Get(), sizeof(ModelVertexData) * cache->modelData.vertices.size());
		cache->vertexBufferView.BufferLocation = cache->vertexResource->GetGPUVirtualAddress();
		cache->vertexBufferView.SizeInBytes = UINT(sizeof(ModelVertexData) * cache->modelData.vertices.size());
		cache->vertexBufferView.StrideInBytes = sizeof(ModelVertexData);
		// 頂点データ書き込み（一時マップ）
 		ModelVertexData* vertexTmp = nullptr;
 		cache->vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexTmp));
 		std::memcpy(vertexTmp, cache->modelData.vertices.data(), sizeof(ModelVertexData) * cache->modelData.vertices.size());
		cache->vertexResource->Unmap(0, nullptr);

		// キャッシュ登録
		s_geometryCache_.emplace(modelPath_, cache);
		it = s_geometryCache_.find(modelPath_);
	} else {
		// キャッシュヒット
		std::string message = "Model cache hit: " + modelPath_ + "\n";
		OutputDebugStringA(message.c_str());
		
		// キャッシュヒット時もアニメーションタイプを判定
		const auto& cache = it->second;
		if (!cache->modelData.skinClusterData.empty()) {
			animationType_ = AnimationType::SKINNING;
			useAnimationTimer_ = true;
			
			char debugBuffer[256];
			sprintf_s(debugBuffer, "AnimationType set to BONE (cache hit) for model: %s\n", modelPath_.c_str());
			OutputDebugStringA(debugBuffer);
		} else if (!cache->animationData.nodeAnimations.empty()) {
			animationType_ = AnimationType::NORMAL;
			useAnimationTimer_ = true;
			
			char debugBuffer[256];
			sprintf_s(debugBuffer, "AnimationType set to NORMAL (cache hit) for model: %s\n", modelPath_.c_str());
			OutputDebugStringA(debugBuffer);
		} else {
			animationType_ = AnimationType::NONE;
			useAnimationTimer_ = false;
			
			char debugBuffer[256];
			sprintf_s(debugBuffer, "AnimationType set to NONE (cache hit) for model: %s\n", modelPath_.c_str());
			OutputDebugStringA(debugBuffer);
		}
	}

	// キャッシュからインスタンスへ設定
	const auto& cache = it->second;
	modelData_ = cache->modelData;
	animationData_ = cache->animationData;
	textureName_ = cache->textureName;
	textureIndex_ = cache->textureIndex;
	indexResource_ = cache->indexResource;
	indexBufferView_ = cache->indexBufferView;
	vertexBufferView_ = cache->vertexBufferView;
	vertexResource_ = cache->vertexResource;
	boundingRadius_ = cache->boundingRadius;
	skeleton_ = cache->skeletonData;
	skinCluster_ = cache->skinClusterData;

	transform_ = { {1.0f,1.0f,1.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f} };
	uvTransform_ = { {1.0f,1.0f}, 0.0f, {0.0f,0.0f} };
	direction_ = { 1.0f,-1.0f,1.0f };

	// 以降はインスタンス専用のリソースのみ作成
	CreateMaterialResource();
	CreateTransformationResource();
	CreateDirectionalLightResource();
	CreateCameraResource();
	CreatePointLightResource();
	CreateSpotLightResource();
}

void Model::Update() {

	if (useUpdateFrustumCulling_) {
		Vector3 worldPosition = GetWorldPosition();
		// スケールを考慮したバウンディング半径を計算
		float maxScale = std::max(transform_.scale.x, std::max(transform_.scale.y, transform_.scale.z));
		float adjustedRadius = boundingRadius_ * maxScale;

		// カメラのフラスタム内にない場合は描画をスキップ
		if (!camera_.IsInFrustum(worldPosition, adjustedRadius)) {
			isInFrustum_ = true;
			return; // 更新をスキップ
		} else {
			isInFrustum_ = false;
		}
	}

	transform_.scale.x = std::clamp(transform_.scale.x, 0.0f, 100.0f);
	transform_.scale.y = std::clamp(transform_.scale.y, 0.0f, 100.0f);
	transform_.scale.z = std::clamp(transform_.scale.z, 0.0f, 100.0f);

	cameraData_->worldPosition = camera_.GetTranslate(); // カメラの位置を渡す

	// 行列の内容を更新
	worldMatrix = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	Matrix4x4 cameraMatrix = MakeAffineMatrix(camera_.GetScale(), camera_.GetRotate(), camera_.GetTranslate());
	Matrix4x4 viewMatrix = InverseMatrix(cameraMatrix);
	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(camera_.GetFovY(), static_cast<float>(WinApp::kClientWidth_) / static_cast<float>(WinApp::kClientHeight_), camera_.GetNearClip(), camera_.GetFarClip());
	Matrix4x4 worldViewProjectionMatrix = MultiplyMatrix(worldMatrix, MultiplyMatrix(viewMatrix, projectionMatrix));

	// 追加処理：法線変換行列を計算
	Matrix4x4 worldInverseMatrix = InverseMatrix(worldMatrix);

	if (useAnimationTimer_) {
		animationTime_ += deltaTime_;
		animationTime_ = fmod(animationTime_, animationData_.duration);
	}

	switch (animationType_)
	{
	case AnimationType::NONE: {

		transformationData_->WVP = MultiplyMatrix(modelData_.rootNode.localMatrix, worldViewProjectionMatrix);
		transformationData_->World = MultiplyMatrix(modelData_.rootNode.localMatrix, worldMatrix);

		break;
	}
	case AnimationType::NORMAL: {

		NodeAnimation& rootNodeAnimation = animationData_.nodeAnimations[modelData_.rootNode.name];
		Vector3 translate = CalculateValue(rootNodeAnimation.translate.keyframes, animationTime_);
		Quaternion rotate = CalculateValue(rootNodeAnimation.rotate.keyframes, animationTime_);
		Vector3 scale = CalculateValue(rootNodeAnimation.scale.keyframes, animationTime_);
		Matrix4x4 localMatrix = MakeAffineAnimationMatrix(scale, rotate, translate);

		modelData_.rootNode.localMatrix = localMatrix;

		transformationData_->WVP = MultiplyMatrix(modelData_.rootNode.localMatrix, worldViewProjectionMatrix);
		transformationData_->World = MultiplyMatrix(modelData_.rootNode.localMatrix, worldMatrix);

		break;
	}
	case AnimationType::SKINNING: {

		ApplyAnimation(skeleton_, animationData_, animationTime_);
		UpdateAnimation(skeleton_);
		UpdateCluster(skinCluster_, skeleton_);

		transformationData_->WVP = worldViewProjectionMatrix;
		transformationData_->World = worldMatrix;

		break;
	}
	}

	transformationData_->WorldInverseTranspose = worldInverseMatrix;

	directionalLightData_->direction = direction_;

	// UV変換行列の計算
	Matrix4x4 scale = MakeIdentityMatrix();
	scale.m[0][0] = uvTransform_.scale.x;
	scale.m[1][1] = uvTransform_.scale.y;

	Matrix4x4 rot = MakeIdentityMatrix();
	rot.m[0][0] = cos(uvTransform_.rotate);
	rot.m[0][1] = -sin(uvTransform_.rotate);
	rot.m[1][0] = sin(uvTransform_.rotate);
	rot.m[1][1] = cos(uvTransform_.rotate);

	Matrix4x4 trans = MakeIdentityMatrix();
	trans.m[3][0] = uvTransform_.translate.x;
	trans.m[3][1] = uvTransform_.translate.y;

	// 最終変換行列
	materialData_->uvTransformMatrix = scale * rot * trans;
}

void Model::Draw(Camera& useCamera, const Transform& transform) {

	// フラスタムカリングのチェック
	if (useDrawFrustumCulling_) {
		Vector3 worldPosition = GetWorldPosition();
		float maxScale = std::max(transform_.scale.x, std::max(transform_.scale.y, transform_.scale.z));
		float adjustedRadius = boundingRadius_ * maxScale;

		if (!useCamera.IsInFrustum(worldPosition, adjustedRadius)) {
			isInFrustum_ = true;
			return;
		} else {
			isInFrustum_ = false;
		}
	}

	camera_ = useCamera;
	transform_ = transform;

	auto& psoManager = PSOManager::GetInstance();

	// アニメーション使用の有無でRootSignatureとPSOを切り替え
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pso;
	
	switch (animationType_)
	{
	case Model::AnimationType::NONE: {

		rootSignature = psoManager.GetRootSignature("Object3D");

		if (useTransparent_) {
			pso = psoManager.GetPSO(PSOType::Model_Alpha_Normal);
		} else {
			pso = useWireFrame ?
				psoManager.GetPSO(PSOType::Model_Wireframe_Normal) :
				psoManager.GetPSO(PSOType::Model_Solid_Normal);
		}

		break;
	}
	case Model::AnimationType::NORMAL: {

		rootSignature = psoManager.GetRootSignature("Object3D");

		if (useTransparent_) {
			pso = psoManager.GetPSO(PSOType::Model_Alpha_Normal);
		} else {
			pso = useWireFrame ?
				psoManager.GetPSO(PSOType::Model_Wireframe_Normal) :
				psoManager.GetPSO(PSOType::Model_Solid_Normal);
		}

		break;
	}
	case Model::AnimationType::SKINNING: {

		rootSignature = psoManager.GetRootSignature("Skinning");

		if (useTransparent_) {
			pso = psoManager.GetPSO(PSOType::SkinningModel_Alpha_Normal);
		} else {
			pso = useWireFrame ?
				psoManager.GetPSO(PSOType::SkinningModel_Wireframe_Normal) :
				psoManager.GetPSO(PSOType::SkinningModel_Solid_Normal);
		}

		break;
	}
	}

	commandList_->SetGraphicsRootSignature(rootSignature.Get());
	commandList_->SetPipelineState(pso.Get());

	if (animationType_ == AnimationType::SKINNING) {
		// Skinningモデルは2つのVBVを使用
		D3D12_VERTEX_BUFFER_VIEW vbvs[2] = {
			vertexBufferView_,
			skinCluster_.influenceBufferView
		};
		commandList_->IASetVertexBuffers(0, 2, vbvs);
	} else {
		// 通常モデルは1つのVBVのみ
		commandList_->IASetVertexBuffers(0, 1, &vertexBufferView_);
	}

	commandList_->IASetIndexBuffer(&indexBufferView_);
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 共通パラメータの設定
	commandList_->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(1, transformationResource_->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetSrvHandleGPU(textureIndex_));
	commandList_->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(4, cameraResource_->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(5, pointLightResource_->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(6, spotLightResource_->GetGPUVirtualAddress());

	// Skinningモデルの場合、MatrixPaletteを設定
	if (animationType_ == AnimationType::SKINNING) {
		commandList_->SetGraphicsRootDescriptorTable(7, skinCluster_.paletteSrvHandle.second);
	}

	// 描画
	commandList_->DrawIndexedInstanced(
		static_cast<UINT>(modelData_.indeces.size()), 1, 0, 0, 0);
}

void Model::SetTexture(const std::string& textureName) {
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

Vector3 Model::GetWorldPosition() {
	Vector3 worldPos = {};
	worldPos.x = worldMatrix.m[3][0];
	worldPos.y = worldMatrix.m[3][1];
	worldPos.z = worldMatrix.m[3][2];
	return worldPos;
}

void Model::DrawImGui(const char* objectName) {

#ifdef USE_IMGUI

	ImGui::Begin(objectName);

	ImGui::Text("MaterialEdit");
	ImGui::DragFloat2("uvTranslate", &uvTransform_.translate.x, 0.01f);
	ImGui::DragFloat2("uvScale", &uvTransform_.scale.x, 0.01f);
	ImGui::DragFloat("uvRotate", &uvTransform_.rotate, 0.01f);
	ImGui::ColorEdit4("Color", &materialData_->color.x);
	ImGui::Checkbox("DrawMode", &useWireFrame);
	ImGui::Checkbox("Transparent", &useTransparent_); // 透明描画フラグ
	
	if (ImGui::Button("mReset")) {
		uvTransform_.translate = { 0.0f,0.0f };
		uvTransform_.scale = { 1.0f,1.0f };
		uvTransform_.rotate = 0.0f;
		materialData_->color = { 1.0f,1.0f,1.0f,1.0f };
	}

	ImGui::Separator();

	ImGui::Text("Culling");
	ImGui::Checkbox("Enable Frustum Culling", &useDrawFrustumCulling_);
	ImGui::DragFloat("Bounding Radius", &boundingRadius_, 0.1f, 0.1f, 100.0f);
	
	ImGui::Separator();

	ImGui::Text("LightEdit");

	uint32_t useLight = materialData_->enableLighting;
	bool temp = (useLight != 0);
	if (ImGui::Checkbox("UseLight", &temp)) {
		materialData_->enableLighting = temp ? 1 : 0;
	}

	if (materialData_->enableLighting != 0) {
		ImGui::Separator();

		uint32_t useLight1 = directionalLightData_->useLight;
		bool temp1 = (useLight1 != 0);
		if (ImGui::Checkbox("UseDirectionLight", &temp1)) {
			directionalLightData_->useLight = temp1 ? 1 : 0;
		}

		if (directionalLightData_->useLight != 0) {

			uint32_t useHalfLambert = directionalLightData_->useHalfLambert;
			bool temp = (useHalfLambert != 0);
			if (ImGui::Checkbox("UseHalfLambert", &temp)) {
				directionalLightData_->useHalfLambert = temp ? 1 : 0;
			}

			ImGui::DragFloat3("d.Direction", &direction_.x, 0.01f, -1.0f, 1.0f);
			direction_ = direction_.Normalize();
			ImGui::DragFloat("d.Intensity", &directionalLightData_->intensity, 0.01f, 0.0f, 5.0f);
			ImGui::DragFloat("d.Shininess", &materialData_->shininess, 0.01f, 0.0f, 100.0f);
			ImGui::ColorEdit4("d.Color", &directionalLightData_->color.x);

			if (ImGui::Button("dReset")) {
				direction_ = { 1.0f,-1.0f,1.0f };
				materialData_->shininess = 30.0f;
				directionalLightData_->intensity = 1.0f;
				directionalLightData_->color = { 1.0f,1.0f,1.0f,1.0f };
			}
		}

		ImGui::Separator();

		uint32_t useLight2 = pointLightData_->useLight;
		bool temp2 = (useLight2 != 0);
		if (ImGui::Checkbox("UsePointLight", &temp2)) {
			pointLightData_->useLight = temp2 ? 1 : 0;
		}

		if (pointLightData_->useLight != 0) {
			ImGui::DragFloat3("p.Pos", &pointLightData_->position.x, 0.01f);
			ImGui::DragFloat("p.Intensity", &pointLightData_->intensity, 0.01f, 0.0f, 5.0f);
			ImGui::DragFloat("p.Radius", &pointLightData_->radius, 0.01f);
			ImGui::DragFloat("p.Decay", &pointLightData_->decay, 0.01f);
			ImGui::ColorEdit4("p.Color", &pointLightData_->color.x);

			if (ImGui::Button("pReset")) {
				pointLightData_->color = { 1.0f,1.0f,1.0f,1.0f };
				pointLightData_->position = { 0.0f,5.0f,0.0f };
				pointLightData_->intensity = 1.0f;
				pointLightData_->radius = 20.0f;
				pointLightData_->decay = 2.0f;
			}
		}

		ImGui::Separator();

		uint32_t useLight3 = spotLightData_->useLight;
		bool temp3 = (useLight3 != 0);
		if (ImGui::Checkbox("UseSpotLight", &temp3)) {
			spotLightData_->useLight = temp3 ? 1 : 0;
		}

		if (spotLightData_->useLight != 0) {
			ImGui::DragFloat3("s.Pos", &spotLightData_->position.x, 0.01f);
			ImGui::DragFloat("s.Intensity", &spotLightData_->intensity, 0.01f, 0.0f, 10.0f);
			ImGui::DragFloat("s.Distance", &spotLightData_->distance, 0.01f);
			spotLightData_->direction = spotLightData_->direction.Normalize();
			ImGui::DragFloat3("s.Direction", &spotLightData_->direction.x, 0.01f);
			ImGui::DragFloat("s.Decay", &spotLightData_->decay, 0.01f);
			float angleDeg = 60.0f;            // cosAngle用の角度（UI用、一時変数）
			float falloffStartDeg = 30.0f;     // cosFalloffStart用の角度
			// 現在のcos値から度に変換してUIに表示（必要なら）
			angleDeg = std::acos(spotLightData_->cosAngle) * 180.0f / std::numbers::pi_v<float>;
			falloffStartDeg = std::acos(spotLightData_->cosFalloffStart) * 180.0f / std::numbers::pi_v<float>;
			// ImGuiスライダー（例：0〜90度まで）
			ImGui::SliderFloat("Spot Angle (deg)", &angleDeg, 1.0f, 90.0f);
			ImGui::SliderFloat("Falloff Start (deg)", &falloffStartDeg, 0.0f, angleDeg - 0.01f); // 必ず angle より小さく
			// 入力された角度からcos値に変換して反映
			spotLightData_->cosAngle = std::cos(angleDeg * std::numbers::pi_v<float> / 180.0f);
			spotLightData_->cosFalloffStart = std::cos(falloffStartDeg * std::numbers::pi_v<float> / 180.0f);
			ImGui::ColorEdit4("s.Color", &spotLightData_->color.x);

			if (ImGui::Button("sReset")) {
				spotLightData_->color = { 1.0f,1.0f,1.0f,1.0f };
				spotLightData_->position = { 2.0f,1.25f,0.0f };
				spotLightData_->distance = 7.0f;
				spotLightData_->direction = { -1.0f,-1.0f,0.0f };
				spotLightData_->intensity = 4.0f;
				spotLightData_->decay = 2.0f;
				spotLightData_->cosAngle = std::cos(std::numbers::pi_v<float> / 6.0f);
				spotLightData_->cosFalloffStart = std::cos(std::numbers::pi_v<float> / 3.0f);
			}
		}
	}

	ImGui::End();

#endif
}

void Model::CreateMaterialResource() {
	// MaterialResource
	materialResource_ = CreateBufferResource(device_.Get(), sizeof(ModelMaterial));
	// 書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	// 今回は赤を書き込んでみる（position に赤、texcoord は使わないなら 0.0）
	materialData_->color = { 1.0f, 1.0f, 1.0f, 1.0f }; // 白 (RGBA)
 	materialData_->enableLighting = true;
 	materialData_->uvTransformMatrix = MakeIdentityMatrix();
 	materialData_->shininess = 100.0f;
}

void Model::CreateTransformationResource() {
	// WVP用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	transformationResource_ = CreateBufferResource(device_.Get(), sizeof(ModelTransformationMatrix));
	// 書き込むためのアドレスを取得
	transformationResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformationData_));
	// 単位行列を書き込む
	transformationData_->WVP = MakeIdentityMatrix();
	transformationData_->World = MakeIdentityMatrix();
	transformationData_->WorldInverseTranspose;
}

void Model::CreateDirectionalLightResource() {
	// ライトのリソースを作る
	directionalLightResource_ = CreateBufferResource(device_.Get(), sizeof(DirectionalLight));
	// リソースをマップしてデータを書き込む
	directionalLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData_));
	// 初期化（資料に基づく）
	directionalLightData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };      // 白い光
	directionalLightData_->direction = { 0.0f, -1.0f, 0.0f };       // 真上から真下
	directionalLightData_->intensity = 1.0f;                        // 光の強さ
	directionalLightData_->useLight = true;
	directionalLightData_->useHalfLambert = true;
}

void Model::CreateCameraResource() {
	cameraResource_ = CreateBufferResource(device_.Get(), sizeof(CameraForGPU));
	assert(cameraResource_ != nullptr);
	HRESULT hr = cameraResource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraData_));
	assert(SUCCEEDED(hr));
}

void Model::CreatePointLightResource() {
	pointLightResource_ = CreateBufferResource(device_.Get(), sizeof(PointLight));
	pointLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&pointLightData_));
	pointLightData_->color = { 1.0f,1.0f,1.0f,1.0f };
	pointLightData_->position = { 0.0f,5.0f,0.0f };
	pointLightData_->intensity = 1.0f;
	pointLightData_->radius = 20.0f;
	pointLightData_->decay = 2.0f;
	pointLightData_->useLight = false;
}

void Model::CreateSpotLightResource() {
	spotLightResource_ = CreateBufferResource(device_.Get(), sizeof(SpotLight));
	spotLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&spotLightData_));
	spotLightData_->color = { 1.0f,1.0f,1.0f,1.0f };
	spotLightData_->position = { 2.0f,1.25f,0.0f };
	spotLightData_->distance = 7.0f;
	spotLightData_->direction = { -1.0f,-1.0f,0.0f };
	spotLightData_->intensity = 4.0f;
	spotLightData_->decay = 2.0f;
	spotLightData_->cosAngle = std::cos(std::numbers::pi_v<float> / 6.0f);
	spotLightData_->cosFalloffStart = std::cos(std::numbers::pi_v<float> / 3.0f);
	spotLightData_->useLight = false;
}
