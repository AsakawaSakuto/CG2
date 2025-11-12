#define NOMINMAX
#include "Object3d.h"
#include "WinApp.h"
#include <algorithm>
#include <cmath>

#include<cassert>
#pragma comment(lib,"d3d12.lib")
using namespace Microsoft::WRL;

// 修正: PSOManagerをインクルード（相対パス修正）
#include "../../System/PSOManager/PSOManager.h"

// 共有キャッシュの定義
std::unordered_map<std::string, std::shared_ptr<Model::GeometryCache>> Model::s_geometryCache_;

//"resources/uvChecker.png"

void Model::Initialize(DirectXCommon* dxCommon,  const std::string& modelPath) {
	dxCommon_ = dxCommon;
	device_ = dxCommon_->GetDevice();
	commandList_ = dxCommon_->GetCommandList();

	modelPath_ = "resources/model/" + modelPath;

	// まずキャッシュを探す
	auto it = s_geometryCache_.find(modelPath_);
	if (it == s_geometryCache_.end()) {
		// 未ロードなので新規ロードしてキャッシュ
		auto cache = std::make_shared<GeometryCache>();

		cache->modelData = LoadObject3dFile(modelPath_);

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
		cache->vertexResource = CreateBufferResource(device_.Get(), sizeof(Object3dVertexData) * cache->modelData.vertices.size());
		cache->vertexBufferView.BufferLocation = cache->vertexResource->GetGPUVirtualAddress(); // ここのエラーは.mltファイルのTexturePathが間違えてる可能性が高い
		cache->vertexBufferView.SizeInBytes = UINT(sizeof(Object3dVertexData) * cache->modelData.vertices.size());
		cache->vertexBufferView.StrideInBytes = sizeof(Object3dVertexData);
		// 頂点データ書き込み（一時マップ）
		Object3dVertexData* tmp = nullptr;
		cache->vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&tmp));
		std::memcpy(tmp, cache->modelData.vertices.data(), sizeof(Object3dVertexData) * cache->modelData.vertices.size());
		cache->vertexResource->Unmap(0, nullptr);

		// キャッシュ登録
		s_geometryCache_.emplace(modelPath_, cache);
		it = s_geometryCache_.find(modelPath_);
	}
	// else の場合：キャッシュヒット = モデル読み込みスキップ成功

	// キャッシュからインスタンスへ設定
	const auto& cache = it->second;
	modelData_ = cache->modelData;
	textureName_ = cache->textureName;
	textureIndex_ = cache->textureIndex;
	vertexResource_ = cache->vertexResource; // 共有
	vertexBufferView_ = cache->vertexBufferView; // 共有
	boundingRadius_ = cache->boundingRadius;

	transform_ = { {1.0f,1.0f,1.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f} };

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
			return; // 描画をスキップ
		}
	}

	cameraData_->worldPosition = camera_.GetTranslate(); // カメラの位置を渡す

	// 行列の内容を更新
	worldMatrix = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	Matrix4x4 cameraMatrix = MakeAffineMatrix(camera_.GetScale(), camera_.GetRotate(), camera_.GetTranslate());
	Matrix4x4 viewMatrix = InverseMatrix(cameraMatrix);
	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(camera_.GetFovY(), static_cast<float>(WinApp::kClientWidth_) / static_cast<float>(WinApp::kClientHeight_), camera_.GetNearClip(), camera_.GetFarClip());
	Matrix4x4 worldViewProjectionMatrix = MultiplyMatrix(worldMatrix, MultiplyMatrix(viewMatrix, projectionMatrix));

	// 追加処理：法線変換行列を計算
	Matrix4x4 worldInverseMatrix = InverseMatrix(worldMatrix);

	// 書き込み
	//transformationData_->WVP = MultiplyMatrix(modelData_.rootNode.localMatrix, MultiplyMatrix(worldMatrix,worldViewProjectionMatrix));
	transformationData_->WVP = MultiplyMatrix(modelData_.rootNode.localMatrix, worldViewProjectionMatrix);
	transformationData_->World = MultiplyMatrix(modelData_.rootNode.localMatrix, worldMatrix);
	transformationData_->WorldInverseTranspose = worldInverseMatrix;

	directionalLightData_->direction = direction_;

	if (transform_.scale.x <= 0.0f) {
		transform_.scale.x = 0.0f;
	}
	if (transform_.scale.y <= 0.0f) {
		transform_.scale.y = 0.0f;
	}
	if (transform_.scale.z <= 0.0f) {
		transform_.scale.z = 0.0f;
	}

	Matrix4x4 scale = MakeIdentityMatrix();
	scale.m[0][0] = uvScale_.x;
	scale.m[1][1] = uvScale_.y;

	Matrix4x4 rot = MakeIdentityMatrix();
	rot.m[0][0] = cos(uvRotate_);
	rot.m[0][1] = -sin(uvRotate_);
	rot.m[1][0] = sin(uvRotate_);
	rot.m[1][1] = cos(uvRotate_);

	Matrix4x4 trans = MakeIdentityMatrix();
	trans.m[3][0] = uvTranslate_.x;
	trans.m[3][1] = uvTranslate_.y;

	// 最終変換行列
	materialData_->uvTransform = scale * rot * trans;
}

void Model::Draw(Camera& useCamera) {
	camera_ = useCamera;

	// フラスタムカリングのチェック
	if (useDrawFrustumCulling_) {
		Vector3 worldPosition = GetWorldPosition();
		// スケールを考慮したバウンディング半径を計算
		float maxScale = std::max(transform_.scale.x, std::max(transform_.scale.y, transform_.scale.z));
		float adjustedRadius = boundingRadius_ * maxScale;
		
		// カメラのフラスタム内にない場合は描画をスキップ
		if (!useCamera.IsInFrustum(worldPosition, adjustedRadius)) {
			return; // 描画をスキップ
		}
	}

	// --- 描画処理 ---
	
	// PSOManagerからRootSignatureとPSOを取得
	auto& psoManager = PSOManager::GetInstance();
	auto rootSignature = psoManager.GetRootSignature("Object3D");
	
	// RootSignatureを設定
	commandList_->SetGraphicsRootSignature(rootSignature.Get());

	// 半透明フラグでPSO切替（深度書き込みOFFのNormalブレンド）
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pso;
	if (transparent_) {
		pso = psoManager.GetPSO(PSOType::Model_Alpha_Normal);
	} else {
		// 描画モードに応じてPSOを取得・設定
		pso = drawMode_ ? 
			psoManager.GetPSO(PSOType::Model_Solid_Normal) : 
			psoManager.GetPSO(PSOType::Model_Wireframe_Normal);
	}
	commandList_->SetPipelineState(pso.Get());

	// プリミティブトポロジーを設定
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	commandList_->IASetVertexBuffers(0, 1, &vertexBufferView_);
	commandList_->IASetIndexBuffer(nullptr); // インデックス使っていないので設定しない
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// RootParams 設定（そのままでOK）
	commandList_->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(1, transformationResource_->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetSrvHandleGPU(textureIndex_));
	commandList_->SetGraphicsRootConstantBufferView(4, cameraResource_->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(5, pointLightResource_->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(6, spotLightResource_->GetGPUVirtualAddress());

	// 頂点描画（DrawIndexedではなくDrawInstanced）
	commandList_->DrawInstanced(
		static_cast<UINT>(modelData_.vertices.size()), // 頂点数ぶん描画
		1, 0, 0);
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

	ImGui::Text("Transform");
	ImGui::DragFloat3("translate", &transform_.translate.x, 0.01f);
	ImGui::DragFloat3("rotate", &transform_.rotate.x, 0.01f);
	ImGui::DragFloat3("scale", &transform_.scale.x, 0.01f);

	if (ImGui::Button("tReset")) {
		transform_.translate = { 0.0f, 0.0f, 0.0f };
		transform_.rotate = { 0.0f, 0.0f, 0.0f };
		transform_.scale = { 1.0f, 1.0f, 1.0f };
	}

	ImGui::Separator();

	ImGui::Text("MaterialEdit");
	ImGui::DragFloat2("uvTranslate", &uvTranslate_.x, 0.01f);
	ImGui::DragFloat2("uvScale", &uvScale_.x, 0.01f);
	ImGui::DragFloat("uvRotate", &uvRotate_, 0.01f);
	ImGui::ColorEdit4("Color", &materialData_->color.x);
	ImGui::Checkbox("DrawMode", &drawMode_);
	ImGui::Checkbox("Transparent", &transparent_); // 透明描画フラグ
	
	if (ImGui::Button("mReset")) {
		uvTranslate_ = { 0.0f,0.0f };
		uvScale_ = { 1.0f,1.0f };
		uvRotate_ = 0.0f;
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

void Model::CreateVertexResource() {
	// 旧実装は共有キャッシュ導入により未使用（後方互換のため残すが呼ばれない想定）
	// 頂点リソースをつくる
	vertexResource_ = CreateBufferResource(device_.Get(), sizeof(Object3dVertexData) * modelData_.vertices.size());
	// リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点のサイズ
	vertexBufferView_.SizeInBytes = UINT(sizeof(Object3dVertexData) * modelData_.vertices.size()); 
	// 1頂点あたりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(Object3dVertexData);
	// 頂点リソースにデータを書き込む、書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	// 頂点データをリソースにコピー
	std::memcpy(vertexData_, modelData_.vertices.data(), sizeof(Object3dVertexData) * modelData_.vertices.size()); 
}

void Model::CreateMaterialResource() {
	// MaterialResource
	materialResource_ = CreateBufferResource(device_.Get(), sizeof(Object3dMaterial));
	// 書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	// 今回は赤を書き込んでみる（position に赤、texcoord は使わないなら 0.0）
	materialData_->color = { 1.0f, 1.0f, 1.0f, 1.0f }; // 白 (RGBA)
	materialData_->enableLighting = true;
	materialData_->uvTransform = MakeIdentityMatrix();
	materialData_->shininess = 5000.0f;
}

void Model::CreateTransformationResource() {
	// WVP用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	transformationResource_ = CreateBufferResource(device_.Get(), sizeof(Object3dTransformationMatrix));
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