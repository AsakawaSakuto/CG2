#include "3d/Ring/RingRenderer.h"

#include "Core/ServiceLocator/ServiceLocator.h"
#include "Core/CreateResource/CreateResource.h"
#include "Core/PSOManager/PSOManager.h"
#include "Core/TextureManager/TextureManager.h"
#include "Math/MatrixFunction/MatrixFunction.h"

#ifdef USE_IMGUI
#include "imgui.h"
#endif

RingRenderer::~RingRenderer() {
	if (vertexResource_   && vertexData_)    { vertexResource_->Unmap(0, nullptr);   vertexData_    = nullptr; }
	if (indexResource_    && indexData_)     { indexResource_->Unmap(0, nullptr);    indexData_     = nullptr; }
	if (materialResource_ && materialData_)  { materialResource_->Unmap(0, nullptr); materialData_  = nullptr; }
	if (transformResource_ && transformData_){ transformResource_->Unmap(0, nullptr);transformData_ = nullptr; }
}

void RingRenderer::Initialize(const std::string& texturePath,
							   uint32_t divideCount,
							   float outerRadius,
							   float innerRadius) {
	DirectXCommon* dxCommon = ServiceLocator::GetDXCommon();
	device_      = dxCommon->GetDevice();
	commandList_ = dxCommon->GetCommandList();

	// ジオメトリ生成
	ring_.Create(divideCount, outerRadius, innerRadius);

	// テクスチャ読み込み
	texturePath_ = texturePath;
	TextureManager::GetInstance()->LoadTexture(texturePath_);
	textureIndex_ = TextureManager::GetInstance()->GetTextureIndexByFilePath(texturePath_);

	// GPUリソース生成
	CreateVertexBuffer();
	CreateIndexBuffer();
	CreateMaterialBuffer();
	CreateTransformBuffer();
}

void RingRenderer::Draw(Camera& camera) {
	// WVP行列を更新
	Matrix4x4 world = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	transformData_->WVP = MultiplyMatrix(world, camera.GetViewProjectionMatrix());

	// UVスクロール行列を更新（U方向のみ）
	Matrix4x4 uvMatrix        = MakeIdentityMatrix();
	uvMatrix.m[3][0]          = uvOffsetU_; // U方向オフセット
	materialData_->uvTransformMatrix = uvMatrix;

	// PSO / RootSignature は Sprite 用を流用
	// → AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP のため
	//   リング中心部の白いアーティファクトが自動的に防止される
	auto& psoManager  = PSOManager::GetInstance();
	auto rootSignature = psoManager.GetRootSignature("Sprite");
	auto pso           = psoManager.GetPSO(PSOType::Sprite_Normal);

	commandList_->SetGraphicsRootSignature(rootSignature.Get());
	commandList_->SetPipelineState(pso.Get());
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList_->IASetVertexBuffers(0, 1, &vertexBufferView_);
	commandList_->IASetIndexBuffer(&indexBufferView_);

	// b0: Material, b1: Transform WVP, t0: Texture
	commandList_->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(1, transformResource_->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetSrvHandleGPU(textureIndex_));

	const uint32_t indexCount = static_cast<uint32_t>(ring_.GetIndices().size());
	commandList_->DrawIndexedInstanced(indexCount, 1, 0, 0, 0);
}

void RingRenderer::DrawImGui(const char* label) {
#ifdef USE_IMGUI
	if (ImGui::TreeNode(label)) {
		ImGui::DragFloat3("Translate", &transform_.translate.x, 0.01f);
		ImGui::DragFloat3("Rotate",    &transform_.rotate.x,    0.01f);
		ImGui::DragFloat3("Scale",     &transform_.scale.x,     0.01f);
		ImGui::ColorEdit4("Color",     &materialData_->color.x);
		ImGui::DragFloat("UV OffsetU", &uvOffsetU_,             0.01f);
		ImGui::TreePop();
	}
#endif
}

void RingRenderer::SetColor(const Vector4& color) {
	if (materialData_) { materialData_->color = color; }
}

void RingRenderer::SetUvScrollX(float offsetU) {
	uvOffsetU_ = offsetU;
}

// -----------------------------------------------------------------------
// プライベート：GPUリソース生成
// -----------------------------------------------------------------------

void RingRenderer::CreateVertexBuffer() {
	const auto& verts = ring_.GetVertices();
	const size_t bufSize = sizeof(SpriteVertexData) * verts.size();

	vertexResource_ = CreateBufferResource(device_.Get(), bufSize);
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes    = static_cast<UINT>(bufSize);
	vertexBufferView_.StrideInBytes  = sizeof(SpriteVertexData);
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

	// Ring::Vertex(Vector3, Vector2) → SpriteVertexData(Vector4, Vector2) に変換してコピー
	for (size_t i = 0; i < verts.size(); ++i) {
		vertexData_[i].position = { verts[i].pos.x, verts[i].pos.y, verts[i].pos.z, 1.0f };
		vertexData_[i].texcoord = verts[i].uv;
	}
}

void RingRenderer::CreateIndexBuffer() {
	const auto& indices = ring_.GetIndices();
	const size_t bufSize = sizeof(uint32_t) * indices.size();

	indexResource_ = CreateBufferResource(device_.Get(), bufSize);
	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	indexBufferView_.SizeInBytes    = static_cast<UINT>(bufSize);
	indexBufferView_.Format         = DXGI_FORMAT_R32_UINT;
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));

	memcpy(indexData_, indices.data(), bufSize);
}

void RingRenderer::CreateMaterialBuffer() {
	materialResource_ = CreateBufferResource(device_.Get(), sizeof(SpriteMaterial));
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	materialData_->color              = { 1.0f, 1.0f, 1.0f, 1.0f };
	materialData_->uvTransformMatrix  = MakeIdentityMatrix();
}

void RingRenderer::CreateTransformBuffer() {
	transformResource_ = CreateBufferResource(device_.Get(), sizeof(SpriteTransformationMatrix));
	transformResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformData_));
	transformData_->WVP = MakeIdentityMatrix();
}
