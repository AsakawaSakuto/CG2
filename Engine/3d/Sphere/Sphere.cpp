#include"Sphere.h"

void Sphere::Initialize(SphereData* sphereData, const std::string& fileName) {

	sphereData_ = sphereData;

	device_ = sphereData_->GetDxCommon()->GetDevice();
	commandList_ = sphereData_->GetDxCommon()->GetCommandList();
	textureIndex_ = TextureManager::GetInstance()->GetTextureIndexByFilePath(fileName);

	rootSignature_ = sphereData_->GetRootsignature();

	graphicsPipelineState_ = sphereData_->GetPipelineState();

	transform_.scale = { 1.f,1.f,1.f };
	transform_.rotate = { 0.f,0.f,0.f };
	transform_.translate = { 0.f,0.f,0.f };

	direction_ = { 1.0f,-1.0f,1.0f };

	CreateVertexResource();
	CreateIndexResource();
	CreateMaterialResource();
	CreateTransformationResource();
	CreateDirectionalLightResource();

	cameraResource_ = CreateBufferResource(device_.Get(), sizeof(CameraForGPU));
	assert(cameraResource_ != nullptr);
	HRESULT hr = cameraResource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraData_));
	assert(SUCCEEDED(hr));

	pointLightResource_ = CreateBufferResource(device_.Get(), sizeof(PointLight));
	pointLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&pointLightData_));
	pointLightData_->color = { 1.0f,1.0f,1.0f,1.0f };
	pointLightData_->position = { 0.0f,5.0f,0.0f };
	pointLightData_->intensity = 1.0f;
	pointLightData_->radius = 20.0f;
	pointLightData_->decay = 2.0f;
	pointLightData_->useLight = false;

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

void Sphere::Update(Camera& useCamera) {

	cameraData_->worldPosition = useCamera.GetTranslate();

	directionalLightData_->direction = direction_;

	// 行列の内容を更新して三角形を動かす
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	Matrix4x4 cameraMatrix = MakeAffineMatrix(useCamera.GetScale(), useCamera.GetRotate(), useCamera.GetTranslate());
	Matrix4x4 viewMatrix = InverseMatrix(cameraMatrix);
	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, static_cast<float>(WinApp::kClientWidth_) / static_cast<float>(WinApp::kClientHeight_), 0.1f, 100.0f);
	Matrix4x4 worldViewProjectionMatrix = MultiplyMatrix(worldMatrix, MultiplyMatrix(viewMatrix, projectionMatrix));

	// 追加処理：法線変換行列を計算
	Matrix4x4 worldInverseMatrix = InverseMatrix(worldMatrix);

	// シーン上で三角形を描画
	transformationData_->WVP = worldViewProjectionMatrix;
	transformationData_->World = worldMatrix;
	transformationData_->WorldInverseTranspose = worldInverseMatrix;

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

void Sphere::Draw() {

	// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	commandList_->SetGraphicsRootSignature(rootSignature_.Get());
	// PSOを設定
	commandList_->SetPipelineState(graphicsPipelineState_.Get());
	// プリミティブトポロジーを設定
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	commandList_->IASetVertexBuffers(0, 1, &vertexBufferView_);
	commandList_->IASetIndexBuffer(&indexBufferView_);
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// RootParams 設定（そのままでOK）
	commandList_->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(1, transformationResource_->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetSrvHandleGPU(textureIndex_));
	commandList_->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(4, cameraResource_->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(5, pointLightResource_->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(6, spotLightResource_->GetGPUVirtualAddress());

	// インデックス描画
	commandList_->DrawIndexedInstanced(
		static_cast<UINT>(indices_.size()),
		1, 0, 0, 0);
}

void Sphere::DrawImGui(const char* objectName) {

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

	if (ImGui::Button("mReset")) {
		uvTranslate_ = { 0.0f,0.0f };
		uvScale_ = { 1.0f,1.0f };
		uvRotate_ = 0.0f;
		materialData_->color = { 1.0f,1.0f,1.0f,1.0f };
	}

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

}

void Sphere::CreateVertexResource() {
	CreateIndexedSphereMesh(vertices_, indices_, subdivision_);

	vertexResource_ = CreateBufferResource(device_.Get(), sizeof(VertexData) * vertices_.size());

	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * vertices_.size());
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	memcpy(vertexData_, vertices_.data(), sizeof(VertexData) * vertices_.size());
	vertexResource_->Unmap(0, nullptr);
}

void Sphere::CreateIndexResource() {
	indexResource_ = CreateBufferResource(device_.Get(), sizeof(uint32_t) * indices_.size());

	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	indexBufferView_.SizeInBytes = UINT(sizeof(uint32_t) * indices_.size());
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));
	memcpy(indexData_, indices_.data(), sizeof(uint32_t) * indices_.size());
	indexResource_->Unmap(0, nullptr);
}

void Sphere::CreateTransformationResource() {
	// MaterialResource
	materialResource_ = CreateBufferResource(device_.Get(), sizeof(Material));
	// 書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	// 今回は赤を書き込んでみる（position に赤、texcoord は使わないなら 0.0）
	materialData_->color = { 1.0f, 1.0f, 1.0f, 1.0f }; // 白 (RGBA)
	materialData_->enableLighting = true;
	materialData_->uvTransform = MakeIdentityMatrix();
	materialData_->shininess = 30.0f;
	materialResource_->Unmap(0, nullptr);
}

void Sphere::CreateMaterialResource() {
	//
	transformationResource_ = CreateBufferResource(device_.Get(), sizeof(TransformationMatrix));
	//
	transformationResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformationData_));
	//
	transformationData_->WVP = MakeIdentityMatrix();
	transformationData_->World = MakeIdentityMatrix();
}

void Sphere::CreateDirectionalLightResource() {
	directionalLightResource_ = CreateBufferResource(device_.Get(), sizeof(DirectionalLight));
	//
	directionalLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData_));
	// 初期化（資料に基づく）
	directionalLightData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };      // 白い光
	directionalLightData_->direction = { 0.0f, -1.0f, 0.0f };       // 真上から真下
	directionalLightData_->intensity = 1.0f;                        // 光の強さ
	directionalLightData_->useLight = true;
	directionalLightData_->useHalfLambert = true;
}

// 球体メッシュをインデックス付きで作成する関数
void Sphere::CreateIndexedSphereMesh(std::vector<VertexData>& vertices, std::vector<uint32_t>& indices, int subdivision) {
	const float pi = 3.1415926535f;
	const float lonEvery = 2.0f * pi / subdivision;
	const float latEvery = pi / subdivision;

	vertices.clear();
	indices.clear();

	auto Normalize = [](const Vector3& v) -> Vector3 {
		float len = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
		if (len > 0.0f) return { v.x / len, v.y / len, v.z / len };
		else return { 0.0f, 1.0f, 0.0f };
		};

	for (int lat = 0; lat <= subdivision; ++lat) {
		float theta = -pi / 2.0f + latEvery * lat;
		float v = 1.0f - float(lat) / subdivision;

		for (int lon = 0; lon <= subdivision; ++lon) {
			float phi = lonEvery * lon;
			float u = float(lon) / subdivision;

			Vector3 pos = {
				cosf(theta) * cosf(phi),
				sinf(theta),
				cosf(theta) * sinf(phi)
			};
			Vector3 normal = Normalize(pos);
			vertices.push_back({ { pos.x, pos.y, pos.z, 1.0f }, { u, v }, normal });
		}
	}

	for (int lat = 0; lat < subdivision; ++lat) {
		for (int lon = 0; lon < subdivision; ++lon) {
			int i0 = lat * (subdivision + 1) + lon;
			int i1 = i0 + 1;
			int i2 = i0 + (subdivision + 1);
			int i3 = i2 + 1;

			indices.push_back(i0);
			indices.push_back(i2);
			indices.push_back(i1);

			indices.push_back(i1);
			indices.push_back(i2);
			indices.push_back(i3);
		}
	}
}