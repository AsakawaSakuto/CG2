#include <Windows.h>  // 00_01
#include <string>     // 00_01

#include <cstdint>    // 00_03

#include <filesystem> // 00_04 EX ファイルやディレクトリに関する操作を行うライブラリ
#include <fstream>    // 00_04 EX ファイルに書いたり読んだりするライブラリ
#include <chrono>     // 00_04 EX 時間を扱うライブラリ

#include <d3d12.h>               // 00_05
#include <dxgi1_6.h>             // 00_05
#include <cassert>               // 00_05
#pragma comment(lib,"d3d12.lib") // 00_05
#pragma comment(lib,"dxgi.lib")  // 00_05

#include <dxgidebug.h>            // 01_03
#pragma comment(lib,"dxguid.lib") // 01_03
#include <dxcapi.h>                   // 02_00
#pragma comment(lib,"dxcompiler.lib") // 02_00

#include "externals/imgui/imgui.h"            // 02_03
#include "externals/imgui/imgui_impl_dx12.h"  // 02_03
#include "externals/imgui/imgui_impl_win32.h" // 02_03

#include <fstream> // 06_02
#include <sstream> // 06_02

#include <wrl.h> // 06_03

#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

#include <xaudio2.h>
#pragma comment(lib, "xaudio2.lib")

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#include "externals/DirectXTex/DirectXTex.h"

#include"Camera.h"
#include"DebugCamera.h"
#include"Input.h"
#include"WinApp.h"
#include"ConvertString.h"
#include"DirectXCommon.h"
#include"D3DResourceLeakChecker.h"
#include"Sprite.h"
#include"SpriteData.h"
#include"TextureManager.h"

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "VertexData.h"
#include "Matrix4x4.h"
#include "Transform.h"
#include "Material.h"
#include "TransformationMatrix.h"

struct DirectionalLight {
	Vector4 color;
	Vector3 direction;
	float intensity;
	float padding[3];
};

struct MaterialData {
	std::string textureFilePath;
};

struct ModelData {
	std::vector<VertexData> vertices;
	MaterialData material;
};

// 07-00 xAudio関係
struct ChunkHeader {
	char id[4]; // チャンク毎のID
	int32_t size;	// チャンクサイズ
};

struct RiffHeader {
	ChunkHeader chunk; // RIFF
	char type[4]; // WAVE
};

struct FormatChunk {
	ChunkHeader chunk; // fmt
	WAVEFORMATEX fmt; // WAVEフォーマット
};

struct SoundData {
	WAVEFORMATEX wfex; // WAVEフォーマット
	BYTE* pBuffer = nullptr; // 音声データ
	uint32_t bufferSize = 0; // バッファサイズ
	std::string name; // ファイル名
};

SoundData SoundLoadWave(const char* filePath) {

#pragma region ファイルオープン
	//ファイル入力ストリームのインスタンス
	std::ifstream file;
	//wavファイルをバイナリモードで開く
	file.open(filePath, std::ios_base::binary);
	//ファイルオープン失敗を検出する
	assert(file.is_open());
#pragma endregion

#pragma region wavデータ読み込み
	//RIFFヘッダーの読み込み
	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));
	//RIFFヘッダーのチェック
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0) {
		assert(0);
	}
	//タイプがWAVEかチェック
	if (strncmp(riff.type, "WAVE", 4) != 0) {
		assert(0);
	}
	//フォーマットチャンクの読み込み
	FormatChunk format = {};
	//チャンクヘッダーの確認
	file.read((char*)&format, sizeof(ChunkHeader));
	if (strncmp(format.chunk.id, "fmt ", 4) != 0) {
		assert(0);
	}
	//チャンク本体の読み込み
	assert(format.chunk.size <= sizeof(format.fmt));
	file.read((char*)&format.fmt, format.chunk.size);

	//Dataチャンクの読み込み
	ChunkHeader data;
	file.read((char*)&data, sizeof(data));
	//JUNKチャンクを検出した場合
	if (strncmp(data.id, "JUNK ", 4) == 0) {
		//読み取り位置をJUNKチャンクの終わりまで進める
		file.seekg(data.size, std::ios_base::cur);
		//再読み込み
		file.read((char*)&data, sizeof(data));
	}
	if (strncmp(data.id, "data ", 4) != 0) {
		assert(0);
	}
	//Dataチャンクのデータ部（波形データ）の読み込み
	char* pBuffer = new char[data.size];
	file.read(pBuffer, data.size);

	//Waveファイルを閉じる
	file.close();

#pragma endregion

#pragma region 読み込んだ音声データをreturn
	//returnする音声データ
	SoundData soundData = {};

	soundData.wfex = format.fmt;
	soundData.pBuffer = reinterpret_cast<BYTE*>(pBuffer);
	soundData.bufferSize = data.size;

	return soundData;
#pragma endregion
}

void SoundUnload(SoundData* soundData)
{
	// バッファのメモリを解放
	delete[] soundData->pBuffer;

	soundData->pBuffer = 0;
	soundData->bufferSize = 0;
	soundData->wfex = {};
}

void SoundPlayWave(IXAudio2* xAudio2, const SoundData& soundData) {
	HRESULT result;
	//波形フォーマットを元にSourceVoiceの生成
	IXAudio2SourceVoice* pSourceVoice = nullptr;
	result = xAudio2->CreateSourceVoice(&pSourceVoice, &soundData.wfex);
	assert(SUCCEEDED(result));
	//再生する波形データの設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundData.pBuffer;
	buf.AudioBytes = soundData.bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;
	//波形データの再生
	result = pSourceVoice->SubmitSourceBuffer(&buf);
	result = pSourceVoice->Start();
}

#pragma endregion

#pragma region ウィンドウプロシャージャ 00_03

// ウィンドウプロシャージャ
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
		return true;
	}
	// メッセージに応じて固有の処理を行う
	switch (msg) {
		// ウィンドウが破壊された
	case WM_DESTROY:
		// OSに対してアプリの終了を伝える
		PostQuitMessage(0);
		return 0;
	}
	// 標準のメッセージ処理を行う
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

#pragma region 球体メッシュを作成する関数 05_00

// 球体メッシュを作成する関数
void CreateSphereMesh(std::vector<VertexData>& vertices, int subdivision) {
	const float pi = 3.1415926535f;
	const float lonEvery = 2.0f * pi / subdivision;
	const float latEvery = pi / subdivision;

	vertices.clear();
	vertices.resize(subdivision * subdivision * 6);

	auto Normalize = [](const Vector3& v) -> Vector3 {
		float len = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
		if (len > 0.0f) return { v.x / len, v.y / len, v.z / len };
		else return { 0.0f, 1.0f, 0.0f }; // fallback（ゼロベクトル回避）
		};

	for (int latIndex = 0; latIndex < subdivision; ++latIndex) {
		float lat = -pi / 2.0f + latEvery * latIndex;
		for (int lonIndex = 0; lonIndex < subdivision; ++lonIndex) {
			float lon = lonEvery * lonIndex;
			uint32_t start = (latIndex * subdivision + lonIndex) * 6;

			// 頂点座標
			Vector3 a = { cosf(lat) * cosf(lon), sinf(lat), cosf(lat) * sinf(lon) };
			Vector3 b = { cosf(lat) * cosf(lon + lonEvery), sinf(lat), cosf(lat) * sinf(lon + lonEvery) };
			Vector3 c = { cosf(lat + latEvery) * cosf(lon), sinf(lat + latEvery), cosf(lat + latEvery) * sinf(lon) };
			Vector3 d = { cosf(lat + latEvery) * cosf(lon + lonEvery), sinf(lat + latEvery), cosf(lat + latEvery) * sinf(lon + lonEvery) };

			// UV座標
			float u0 = float(lonIndex) / subdivision;
			float v0 = 1.0f - float(latIndex) / subdivision;
			float u1 = float(lonIndex + 1) / subdivision;
			float v1 = 1.0f - float(latIndex + 1) / subdivision;

			// 法線
			Vector3 na = Normalize(a);
			Vector3 nb = Normalize(b);
			Vector3 nc = Normalize(c);
			Vector3 nd = Normalize(d);

			// 頂点データ代入（paddingも構造体に含まれている前提）
			vertices[start + 0] = { {a.x, a.y, a.z, 1.0f}, {u0, v0}, na };
			vertices[start + 1] = { {c.x, c.y, c.z, 1.0f}, {u0, v1}, nc };
			vertices[start + 2] = { {b.x, b.y, b.z, 1.0f}, {u1, v0}, nb };

			vertices[start + 3] = { {c.x, c.y, c.z, 1.0f}, {u0, v1}, nc };
			vertices[start + 4] = { {d.x, d.y, d.z, 1.0f}, {u1, v1}, nd };
			vertices[start + 5] = { {b.x, b.y, b.z, 1.0f}, {u1, v0}, nb };
		}
	}
}

// 球体メッシュをインデックス付きで作成する関数
void CreateIndexedSphereMesh(std::vector<VertexData>& vertices, std::vector<uint32_t>& indices, int subdivision) {
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


#pragma endregion

#pragma region DiscriptorHandleを取得する関数 05_01

//D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index)
//{
//	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
//	handleCPU.ptr += (descriptorSize * index);
//	return handleCPU;
//}
//
//D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index)
//{
//	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
//	handleGPU.ptr += (descriptorSize * index);
//	return handleGPU;
//}

#pragma endregion

#pragma region LoadMaterial関数 06_02

MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename)
{
	// 1.中で必要となる変数の宣言
	MaterialData materialData;  // 構築するMaterialData

	// 2.ファイルを開く
	std::string line;                                   // ファイルから読んだ1行を格納するもの
	std::ifstream file(directoryPath + "/" + filename); // ファイルを開く
	assert(file.is_open());                             // とりあえず開けなかったら止める

	// 3.実際にファイルを読み、MaterialDataを構築していく
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		// identifierに応じた処理
		if (identifier == "map_Kd") {
			std::string textureFilename;
			s >> textureFilename;
			// 連結してファイルパスにする
			materialData.textureFilePath = directoryPath + "/" + textureFilename;
		}
	}
	// 4.MaterialData を返す
	return materialData;
}

#pragma endregion

#pragma region Objファイルを読み込む関数 06_02

ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename) {

	ModelData modelData;
	std::vector<Vector4> positions;
	std::vector<Vector3> normals;
	std::vector<Vector2> texcoords;
	std::string line;

	std::ifstream file(directoryPath + "/" + filename);
	assert(file.is_open());

	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		if (identifier == "v") {
			Vector4 position;
			s >> position.x >> position.y >> position.z;
			position.w = 1.0f;
			position.x *= -1.0f; // X軸反転
			positions.push_back(position);
		}
		else if (identifier == "vt") {
			Vector2 texcoord;
			s >> texcoord.x >> texcoord.y;
			texcoords.push_back(texcoord);
		}
		else if (identifier == "vn") {
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;
			normal.x *= -1.0f; // 法線のX軸反転
			normals.push_back(normal);
		}
		else if (identifier == "f") {
			VertexData triangle[3];
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
				std::string vertexDefinition;
				s >> vertexDefinition;
				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3];
				for (int32_t element = 0; element < 3; ++element) {
					std::string index;
					std::getline(v, index, '/');
					elementIndices[element] = std::stoi(index);
				}
				Vector4 position = positions[elementIndices[0] - 1];
				Vector2 texcoord = texcoords[elementIndices[1] - 1];
				texcoord.y = 1.0f - texcoord.y; // Y軸反転（UV原点の違いを補正）
				Vector3 normal = normals[elementIndices[2] - 1];
				triangle[faceVertex] = { position, texcoord, normal };
			}
			// 頂点の順番を逆に登録して回り順を反転
			modelData.vertices.push_back(triangle[2]);
			modelData.vertices.push_back(triangle[1]);
			modelData.vertices.push_back(triangle[0]);
		}
		else if (identifier == "mtllib") {
			// materialTemplateLibraryファイルの名前を取得する
			std::string materialFilename;
			s >> materialFilename;
			// 基本的にobjファイルと同一階層にmtlは存在させるので、ディレクトリ名とファイル名を渡す
			modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
		}
	}
	return modelData;
}

#pragma endregion

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

#pragma region main関数の先頭でCOMを初期化 03_00

	CoInitializeEx(0, COINIT_MULTITHREADED);

#pragma endregion

	WinApp* winApp = new WinApp();
	winApp->Initialize(L"CG2_Window");
	winApp->EnableResize(true);

	DirectXCommon* dxCommon = new DirectXCommon();
	dxCommon->Initialize(winApp);

	D3ResourceLeakChecker d3ResourceLeakCheker;

	TextureManager::GetInstance()->Initialize(dxCommon);

	SpriteData* spriteData = new SpriteData();
	spriteData->Initialize(dxCommon);

	Sprite* sprite = new Sprite();
	sprite->Initialize(spriteData, "resources/uvChecker.png");

	Sprite* sprite2 = new Sprite();
	sprite2->Initialize(spriteData, "resources/monsterBall.png");

	HRESULT hr;
	Microsoft::WRL::ComPtr<ID3D12Device> device = dxCommon->GetDevice();
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue = dxCommon->GetCommandQueue();
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator = dxCommon->GetCommandAllocator();
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList = dxCommon->GetCommandList();
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain = dxCommon->GetSwapChain();
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = dxCommon->GetSwapChainDesc();
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap = dxCommon->GetDSV();
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap = dxCommon->GetSRV();
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap = dxCommon->GetRTV();
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle[2];
	rtvHandle[0] = dxCommon->GetRtvHandle0();
	rtvHandle[1] = dxCommon->GetRtvHandle1();
	D3D12_VIEWPORT viewport = dxCommon->GetViewport();
	D3D12_RECT scissorRect = dxCommon->GetScissor();

	Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils = dxCommon->GetDxcUtils();
	Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler = dxCommon->GetDxcCompiler();
	Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler = dxCommon->GetDxcHandler();

	D3D12_RESOURCE_BARRIER barrier = dxCommon->GerBarrier();
#pragma region ディレクトリを掘る 00_04 EX

	// ログのディレクトリを用意
	std::filesystem::create_directory("logs");

#pragma endregion



#pragma region DescriptorHeapを生成する 01_00 関数化 RTVとSRV用のヒープを作成 02_03

	//// RTV用のヒープでディスクリプタの数は2 RTVはShader内で触るものではないのでShaderVisibleはfalse
	//ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap = CreateDescriptorHeap(device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);

	//// SRVのヒープでディスクリプタの数は128 SRVはShader内で触るものなのでShaderVisibleはftrue
	//ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap = CreateDescriptorHeap(device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);

#pragma endregion

#pragma region DescriptorSizeの取得

	// DescriptorsSizeを取得しておく
	const uint32_t descriptorSizeSRV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	const uint32_t descriptorSizeRTV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	const uint32_t descriptorSizeDSV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

#pragma endregion

#pragma region SwapChainからResourceを引っ張てくる 01_00

	// SwapChainからResourceを取得
	ComPtr<ID3D12Resource> swapChainResources[2] = { nullptr };
	hr = swapChain->GetBuffer(0, IID_PPV_ARGS(&swapChainResources[0]));
	// うまく取得できなければ起動できない
	assert(SUCCEEDED(hr));
	hr = swapChain->GetBuffer(1, IID_PPV_ARGS(&swapChainResources[1]));
	// うまく取得できなければ起動できない
	assert(SUCCEEDED(hr));

#pragma endregion

#pragma region RTVを作る 01_00

	//// RTVの設定
	//D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	//rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 出力結果をsRGBに変換して書き込む
	//rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D; // 2dテクスチャとして書き込む
	//// ディスクリプタの先頭を取得する
	//D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle = GetCPUDescriptorHandle(rtvDescriptorHeap.Get(), descriptorSizeRTV, 0);
	//// RTVを2つ作るのでディスクリプタを2つ用意
	//D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];
	//// まず1つ目を作る。1つ目は最初のところに作る。作る場所をこちらで指定してあげる必要がある
	//rtvHandles[0] = rtvStartHandle;
	//device->CreateRenderTargetView(swapChainResources[0].Get(), &rtvDesc, rtvHandles[0]);
	//// 2つ目のディスクリプタハンドルを得る（自力で）
	//rtvHandles[1].ptr = rtvHandles[0].ptr + descriptorSizeSRV;
	//// 2つ目を作る
	//device->CreateRenderTargetView(swapChainResources[1].Get(), &rtvDesc, rtvHandles[1]);

#pragma endregion

#pragma region FenceとEventを生成する *メインループの開始前に作る 01_02

	// 初期値0でFenceを作成する
	ComPtr<ID3D12Fence> fence = nullptr;
	uint64_t fenceValue = 0;
	hr = device->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
	assert(SUCCEEDED(hr));

	// FenceのSignal後のためのイベントを作成する
	HANDLE fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent != nullptr);

#pragma endregion

#pragma region DXCの初期化 02_00

	//// dxcCompilerを初期化
	//ComPtr<IDxcUtils> dxcUtils = nullptr;
	//ComPtr<IDxcCompiler3> dxcCompiler = nullptr;
	//hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
	//assert(SUCCEEDED(hr));
	//hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
	//assert(SUCCEEDED(hr));

	//// 現時点でincludeはしないが、includeに対応するための設定を行っておく
	//ComPtr<IDxcIncludeHandler> includeHandler = nullptr;
	//hr = dxcUtils->CreateDefaultIncludeHandler(&includeHandler);
	//assert(SUCCEEDED(hr));

#pragma endregion

	// 07-01 Input
	Input* input = new Input();
	input->Initialize(winApp);

	// 07-00 xAudio関係

	Microsoft::WRL::ComPtr<IXAudio2> xAudio2;
	IXAudio2MasteringVoice* masterVoice = nullptr;

	hr = XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(hr));

	hr = xAudio2->CreateMasteringVoice(&masterVoice);
	assert(SUCCEEDED(hr));

	SoundData soundData1 = SoundLoadWave("Resources/fanfare.wav");

	/*SoundPlayWave(xAudio2.Get(), soundData1);*/

#pragma region RootSignatureを生成する 02_00

//	// RootSignature作成
//	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
//	descriptionRootSignature.Flags =
//		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
//
//	// DiscriptorRange 03_00
//
//	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
//	descriptorRange[0].BaseShaderRegister = 0; // 0から始まる
//	descriptorRange[0].NumDescriptors = 1; // 数は1つ
//	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // SRVを使う
//	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // Offsetを自動計算
//
//	// Samplerの設定
//
//	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
//	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR; // バイリニアフィルタ
//	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP; // 0-1の範囲外をリピート
//	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
//	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
//	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER; // 比較しない
//	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX; // あったたのMipmapを使う
//	staticSamplers[0].ShaderRegister = 0; // レジスタ番号を使う
//	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
//	descriptionRootSignature.pStaticSamplers = staticSamplers;
//	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);
//
//
//#pragma region RootParameterを作成 02_01 内容を1つ増やして追加 02_02 書き換える 03_00
//
//	// RootParameter作成。複数設定できるので配列。今回は単1つだけなので長さ1の配列
//	D3D12_ROOT_PARAMETER rootParameters[4] = {};
//
//	// RootParam[0] → b0: Material（PS）
//	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
//	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
//	rootParameters[0].Descriptor.ShaderRegister = 0;
//
//	// RootParam[1] → b1: WVP（VS）
//	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
//	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
//	rootParameters[1].Descriptor.ShaderRegister = 1;
//
//	// RootParam[2] → t0: テクスチャ（PS）
//	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
//	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
//	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;
//	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);
//
//	// RootParam[3] → b2: DirectionalLight（PS）
//	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
//	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
//	rootParameters[3].Descriptor.ShaderRegister = 2;
//
//	// レジスタ番号0をバインド
//	descriptionRootSignature.pParameters = rootParameters;              // ルートパラメータ配列へのポインタ
//	descriptionRootSignature.NumParameters = _countof(rootParameters);  // 配列の長さ

	// UVTransform用の変数
	Transform uvTransformSprite{
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f},
	};

	// マテリアル用のリソースを作る。今回は VertexData 1つ分のサイズを用意する
	ComPtr<ID3D12Resource> materialResource = CreateBufferResource(device.Get(), sizeof(Material));

	// マテリアルにデータを書き込む
	Material* materialData = nullptr;
	// 書き込むためのアドレスを取得
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));

	// 今回は赤を書き込んでみる（position に赤、texcoord は使わないなら 0.0）
	materialData->color = { 1.0f, 1.0f, 1.0f, 1.0f }; // 赤 (RGBA)
	materialData->enableLighting = true;
	materialData->uvTransform = MakeIdentityMatrix();

	//// マテリアル用のリソースを作る。今回は VertexData 1つ分のサイズを用意する
	//ComPtr<ID3D12Resource> materialResourceSprite = CreateBufferResource(device.Get(), sizeof(Material));

	//// マテリアルにデータを書き込む
	//Material* materialDataSprite = nullptr;
	//// 書き込むためのアドレスを取得
	//materialResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&materialDataSprite));

	//// 今回は赤を書き込んでみる（position に赤、texcoord は使わないなら 0.0）
	//materialDataSprite->color = { 1.0f, 1.0f, 1.0f, 1.0f }; // 赤 (RGBA)
	//materialDataSprite->enableLighting = false;
	//materialDataSprite->uvTransform = MakeIdentityMatrix();

#pragma endregion

	//// シリアライズしてバイナリにする
	//ComPtr<ID3DBlob> signatureBlob = nullptr;
	//ComPtr<ID3DBlob> errorBlob = nullptr;
	//hr = D3D12SerializeRootSignature(&descriptionRootSignature,
	//	D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	//if (FAILED(hr)) {
	//	Logger::Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
	//	assert(false);
	//}
	//// バイナリを元に生成
	//ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	//hr = device->CreateRootSignature(0,
	//	signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(),
	//	IID_PPV_ARGS(&rootSignature));
	//assert(SUCCEEDED(hr));

#pragma endregion

#pragma region InputLayoutの設定を行う 02_00 InputLayoutの拡張 03_00 ライティングで拡張 05_03

	// InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};

	// POSITION
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].InputSlot = 0;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[0].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	inputElementDescs[0].InstanceDataStepRate = 0;

	// TEXCOORD
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].InputSlot = 0;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	inputElementDescs[1].InstanceDataStepRate = 0;

	// NORMAL ← ここが大事！
	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].InputSlot = 0;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[2].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	inputElementDescs[2].InstanceDataStepRate = 0;

	// Layout desc
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);


#pragma endregion

#pragma region BlendStateの設定を行う 02_00

	// BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	// すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

#pragma endregion

#pragma region RasiterzerStateの設定 02_00

	// RasiterzerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// 裏面（時計回り）を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	// 三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

#pragma endregion

#pragma region Shaderをコンパイルする 02_00

	// Shaderをコンパイルする
	ComPtr<IDxcBlob> vertexShaderBlob = dxCommon->CompileShader(L"resources/shaders/Object3D.VS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob != nullptr);

	ComPtr<IDxcBlob> pixelShaderBlob = dxCommon->CompileShader(L"resources/shaders/Object3D.PS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob != nullptr);

#pragma endregion

#pragma region DepthStencilView(DSV) 03_01

	//// DepthStencilTextureをウィンドウのサイズで作成 03_01
	//ComPtr<ID3D12Resource> depthStencilResource = CreateDepthStencilTextureResource(device.Get(), 1280, 720);

	//// DSV用のヒープでディスクリプタの数は1、DSVはShader内で触るものではないので、ShaderVisibleはfalse 03_01
	//ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap = CreateDescriptorHeap(device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);

	//// DSVの設定 Heap上にDSVを構築する 03_01
	//D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	//dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // Format。基本的にResourceに合わせる
	//dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D; // 2Dテクスチャ

	//// DSVHeapの配列にDSVを作る
	//device->CreateDepthStencilView(depthStencilResource.Get(), &dsvDesc, dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

#pragma endregion

#pragma region PSOを生成する 02_00 DepthStencilStateの設定を行う 03_01

	//// グラフィックスパイプラインステートの設定
	//D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc = {};
	//graphicsPipelineStateDesc.pRootSignature = rootSignature.Get(); // RootSignature
	//graphicsPipelineStateDesc.InputLayout = inputLayoutDesc; // InputLayout
	//graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize() }; // VertexShader
	//graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize() }; // PixelShader
	//graphicsPipelineStateDesc.BlendState = blendDesc; // BlendState
	//graphicsPipelineStateDesc.RasterizerState = rasterizerDesc; // RasterizerState
	//// 書き込み先のRTVの情報
	//graphicsPipelineStateDesc.NumRenderTargets = 1;
	//graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//// 利用するプリミティブ（幾何）タイプ。三角形
	//graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//// どのように画面に色を打ち込むかの設定（気にしなくて良い）
	//graphicsPipelineStateDesc.SampleDesc.Count = 1;
	//graphicsPipelineStateDesc.SampleDesc.Quality = 0;
	//graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	//// DepthStencilStateの設
	//D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	//// Depthの機能を有効化する
	//depthStencilDesc.DepthEnable = true;
	//// 書き込みします
	//depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	//// 比較関数LessEqual、つまり、深ければ描画される
	//depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	//// DepthStencilStateの設定 03_01
	//graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	//graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	//// 実際に生成
	//ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;
	//hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState));
	//assert(SUCCEEDED(hr));

#pragma endregion

#pragma region Sphereのリソースを作成 05_00

	// Sphere用の頂点バッファリソースを生成
	std::vector<VertexData> sphereVertices;
	std::vector<uint32_t> sphereIndices; // Index
	const int sphereSubdivision = 32; // 分割数（細かさ）

	CreateIndexedSphereMesh(sphereVertices, sphereIndices, sphereSubdivision);

	// 球体 ModelDataを使うようにする 06_02

	// モデルを読み込む
	ModelData modelData = LoadObjFile("resources", "plane.obj");
	// 頂点リソースをつくる
	ComPtr<ID3D12Resource> vertexResource = CreateBufferResource(device.Get(), sizeof(VertexData) * modelData.vertices.size());

	// モデルを読み込む
	ModelData modelData2 = LoadObjFile("resources", "plane.obj");
	// 頂点リソースをつくる
	ComPtr<ID3D12Resource> vertexResource2 = CreateBufferResource(device.Get(), sizeof(VertexData) * modelData2.vertices.size());

	// 頂点リソース
	ComPtr<ID3D12Resource> sphereVertexResource = CreateBufferResource(device.Get(), sizeof(VertexData) * UINT(sphereVertices.size()));
	VertexData* sphereVertexData = nullptr;
	sphereVertexResource->Map(0, nullptr, reinterpret_cast<void**>(&sphereVertexData));
	memcpy(sphereVertexData, sphereVertices.data(), sizeof(VertexData) * sphereVertices.size());
	sphereVertexResource->Unmap(0, nullptr);

	// VBV作成(VertexBufferView)
	D3D12_VERTEX_BUFFER_VIEW sphereVertexBufferView{};
	sphereVertexBufferView.BufferLocation = sphereVertexResource->GetGPUVirtualAddress();
	sphereVertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * sphereVertices.size());
	sphereVertexBufferView.StrideInBytes = sizeof(VertexData);

	//// 2Dスプライト
	//ComPtr<ID3D12Resource> vertexResourceSprite = CreateBufferResource(device.Get(), sizeof(VertexData) * 6);

	// インデックスリソース
	ComPtr<ID3D12Resource> sphereIndexResource = CreateBufferResource(device.Get(), sizeof(uint32_t) * sphereIndices.size());
	uint32_t* indexData = nullptr;
	sphereIndexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
	memcpy(indexData, sphereIndices.data(), sizeof(uint32_t) * sphereIndices.size());
	sphereIndexResource->Unmap(0, nullptr);

	// IBV作成(IndexBufferView)
	D3D12_INDEX_BUFFER_VIEW sphereIBV{};
	sphereIBV.BufferLocation = sphereIndexResource->GetGPUVirtualAddress();
	sphereIBV.SizeInBytes = UINT(sizeof(uint32_t) * sphereIndices.size());
	sphereIBV.Format = DXGI_FORMAT_R32_UINT;

#pragma endregion

#pragma region VertexBufferViewを作成する VertexResourceにデータを書き込む 02_00 VertexBufferViewSpriteを作成する VertexResourceSpriteにデータを書き込む 04_00 ライティング 05_03

	// ModelDataを使う頂点バッファビューを作成する 06_02
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress(); // リソースの先頭のアドレスから使う
	vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size()); // 使用するリソースのサイズは頂点のサイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData); // 1頂点あたりのサイズ

	// ModelDataを使う頂点バッファビューを作成する 06_02
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView2{};
	vertexBufferView2.BufferLocation = vertexResource2->GetGPUVirtualAddress(); // リソースの先頭のアドレスから使う
	vertexBufferView2.SizeInBytes = UINT(sizeof(VertexData) * modelData2.vertices.size()); // 使用するリソースのサイズは頂点のサイズ
	vertexBufferView2.StrideInBytes = sizeof(VertexData); // 1頂点あたりのサイズ

	// 頂点リソースにデータを書き込む
	VertexData* vertexData = nullptr;
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData)); // 書き込むためのアドレスを取得
	std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size()); // 頂点データをリソースにコピー

	// 頂点リソースにデータを書き込む
	VertexData* vertexData2 = nullptr;
	vertexResource2->Map(0, nullptr, reinterpret_cast<void**>(&vertexData2)); // 書き込むためのアドレスを取得
	std::memcpy(vertexData2, modelData2.vertices.data(), sizeof(VertexData)* modelData2.vertices.size()); // 頂点データをリソースにコピー

	//// 頂点バッファビューを作成する
	//D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	//// リソースの先頭のアドレスから使う
	//vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	//// 使用するリソースのサイズは頂点3つ分のサイズ
	//vertexBufferView.SizeInBytes = sizeof(VertexData) * 6;
	//// 頂点あたりのサイズ
	//vertexBufferView.StrideInBytes = sizeof(VertexData);

	//// 頂点リソースにデータを書き込む
	//VertexData* vertexData = nullptr;
	//// 書き込むためのアドレスを取得
	//vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	//// 左下
	//vertexData[0].position = { -0.5f, -0.5f, 0.0f, 1.0f };
	//vertexData[0].texcoord = { 0.0f,1.0f };
	//vertexData[0].normal = { 0.0f,0.0f,-1.0f };
	//// 上
	//vertexData[1].position = { 0.0f, 0.5f, 0.0f, 1.0f };
	//vertexData[1].texcoord = { 0.5f,0.0f };
	//vertexData[1].normal = { 0.0f,0.0f,-1.0f };
	//// 右下
	//vertexData[2].position = { 0.5f, -0.5f, 0.0f, 1.0f };
	//vertexData[2].texcoord = { 1.0f,1.0f };
	//vertexData[2].normal = { 0.0f,0.0f,-1.0f };

	//// 左下2
	//vertexData[3].position = { -0.5f, -0.5f, 0.5f, 1.0f };
	//vertexData[3].texcoord = { 0.0f,1.0f };
	//vertexData[3].normal = { 0.0f,0.0f,-1.0f };
	//// 上2
	//vertexData[4].position = { 0.0f, 0.0f, 0.0f, 1.0f };
	//vertexData[4].texcoord = { 0.5f,0.0f };
	//vertexData[4].normal = { 0.0f,0.0f,-1.0f };
	//// 右下2
	//vertexData[5].position = { 0.5f, -0.5f, -0.5f, 1.0f };
	//vertexData[5].texcoord = { 1.0f,1.0f };
	//vertexData[5].normal = { 0.0f,0.0f,-1.0f };

	//for (int i = 0; i < 6; i++)
	//{
	//	Vector3 pos = { vertexData[i].position.x, vertexData[i].position.y, vertexData[i].position.z };
	//	vertexData[i].normal = pos.Normalize();
	//}

	/*----------
	   Sprite
	----------*/

	//// 頂点バッファビューを作成する
	//D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite{};
	//// リソースの先頭のアドレスから使う
	//vertexBufferViewSprite.BufferLocation = vertexResourceSprite->GetGPUVirtualAddress();
	//// 使用するリソースのサイズは頂点3つ分のサイズ
	//vertexBufferViewSprite.SizeInBytes = sizeof(VertexData) * 6;
	//// 頂点あたりのサイズ
	//vertexBufferViewSprite.StrideInBytes = sizeof(VertexData);

	//// 頂点リソースにデータを書き込む
	//VertexData* vertexDataSprite = nullptr;
	//// 書き込むためのアドレスを取得
	//vertexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataSprite));
	//// 1枚目の三角形
	//// index描画に切り替え 06_00
	//vertexDataSprite[0].position = { 0.0f, 180.0f, 0.0f, 1.0f };
	//vertexDataSprite[0].texcoord = { 0.0f, 1.0f };
	//vertexDataSprite[0].normal = { 0.0f,0.0f,-1.0f };

	//vertexDataSprite[1].position = { 0.0f, 0.0f, 0.0f, 1.0f };
	//vertexDataSprite[1].texcoord = { 0.0f, 0.0f };
	//vertexDataSprite[1].normal = { 0.0f,0.0f,-1.0f };

	//vertexDataSprite[2].position = { 320.0f, 180.0f, 0.0f, 1.0f };
	//vertexDataSprite[2].texcoord = { 1.0f, 1.0f };
	//vertexDataSprite[2].normal = { 0.0f,0.0f,-1.0f };

	//vertexDataSprite[3].position = { 320.0f, 0.0f, 0.0f, 1.0f };
	//vertexDataSprite[3].texcoord = { 1.0f, 0.0f };
	//vertexDataSprite[3].normal = { 0.0f,0.0f,-1.0f };

	///*-----インデックスを使った描画-----*/

	//ComPtr<ID3D12Resource> indexResourceSprite = CreateBufferResource(device.Get(), sizeof(uint32_t) * 6);

	//D3D12_INDEX_BUFFER_VIEW indexBufferViewSprite{};
	//// リソースの先頭のアドレスから使う
	//indexBufferViewSprite.BufferLocation = indexResourceSprite->GetGPUVirtualAddress();
	//// 使用するリソースのサイズはインデックス6つ分のサイズ
	//indexBufferViewSprite.SizeInBytes = sizeof(uint32_t) * 6;
	//// インデックスはuint32_tとする
	//indexBufferViewSprite.Format = DXGI_FORMAT_R32_UINT;
	//// インデックスリソースにデータを書き込む
	//uint32_t* indexDataSprite = nullptr;
	//indexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&indexDataSprite));
	//indexDataSprite[0] = 0;    indexDataSprite[1] = 1;    indexDataSprite[2] = 2;
	//indexDataSprite[3] = 1;    indexDataSprite[4] = 3;    indexDataSprite[5] = 2;

#pragma endregion

#pragma region ビューポートとシザー 02_00


#pragma endregion

#pragma region TransformationMatrix用のResourceを作る 02_02 TransformationMatrixSprite用のResourceを作る 04_00

	// WVP用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	ComPtr<ID3D12Resource> wvpResource = CreateBufferResource(device.Get(), sizeof(TransformationMatrix));
	// データを転送
	TransformationMatrix* wvpData = nullptr;
	// 書き込むためのアドレスを取得
	wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	// 単位行列を書き込む
	wvpData->WVP = MakeIdentityMatrix();
	wvpData->World = MakeIdentityMatrix();

	// WVP用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	ComPtr<ID3D12Resource> wvpResource2 = CreateBufferResource(device.Get(), sizeof(TransformationMatrix));
	// データを転送
	TransformationMatrix* wvpData2 = nullptr;
	// 書き込むためのアドレスを取得
	wvpResource2->Map(0, nullptr, reinterpret_cast<void**>(&wvpData2));
	// 単位行列を書き込む
	wvpData2->WVP = MakeIdentityMatrix();
	wvpData2->World = MakeIdentityMatrix();
	/*Matrix4x4* transformationMatrixData = nullptr;*/

	/*----------
	   Sprite
	----------*/

	//// WVP用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	//ComPtr<ID3D12Resource> transformationMatrixResourceSprite = CreateBufferResource(device.Get(), sizeof(TransformationMatrix));
	//// データを転送
	//TransformationMatrix* transformationMatrixDataSprite = nullptr;
	//// 書き込むためのアドレスを取得
	//transformationMatrixResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixDataSprite));
	//// 単位行列を書き込む
	//transformationMatrixDataSprite->WVP = MakeIdentityMatrix();
	//transformationMatrixDataSprite->World = MakeIdentityMatrix();

	// --- DirectionalLight用のリソースを作成する ---
	ComPtr<ID3D12Resource> directionalLightResource = CreateBufferResource(device.Get(), sizeof(DirectionalLight));

	// リソースをマップしてデータを書き込む
	DirectionalLight* directionalLightData = nullptr;
	directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));

	// 初期化（資料に基づく）
	directionalLightData->color = { 1.0f, 1.0f, 1.0f, 1.0f };      // 白い光
	directionalLightData->direction = { 0.0f, -1.0f, 0.0f };       // 真上から真下
	directionalLightData->intensity = 1.0f;                        // 光の強さ

#pragma endregion

#pragma region メインループ開始前にtransform変数を作る 02_02 04_00

	Transform transform{ {1.0f,1.0f,1.0f}, {0.0f,3.1f,0.0f}, {0.0f,0.0f,0.0f} };
	Transform transform2{ {1.0f,1.0f,1.0f}, {0.0f,3.1f,0.0f}, {0.0f,0.0f,0.0f} };
	Transform transformSprite{ {1.0f,1.0f,1.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f} };

	Camera* camera = new Camera();
	DebugCamera* debugCamera = new DebugCamera();
	Camera* useCamera = new Camera();
	bool isDebugCamera = false;
#pragma endregion

#pragma region Imguiの初期化 メインループ前に行う 02_03

	//// ImGuiの初期化。詳細はさして重要ではないので解説は省略する。
	//// こういうもんである
	//IMGUI_CHECKVERSION();
	//ImGui::CreateContext();
	//ImGui::StyleColorsDark();
	//ImGui_ImplWin32_Init(winApp->GetHWND());
	//ImGui_ImplDX12_Init(device.Get(),
	//	swapChainDesc.BufferCount,
	//	dxCommon->GetRtvDesc().Format,
	//	srvDescriptorHeap.Get(),
	//	srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
	//	srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

#pragma endregion

#pragma region Textureを読んで転送する 03_00 2枚目 05_01

	// Textureを読んで転送する
	DirectX::ScratchImage mipImages = LoadTexture("resources/uvChecker.png");
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	ComPtr<ID3D12Resource> textureResource = CreateTextureResource(device.Get(), metadata);
	UploadTextureData(textureResource.Get(), mipImages);

	// 2枚目のTextureを読んで転送する
	DirectX::ScratchImage mipImages2 = LoadTexture(modelData.material.textureFilePath);
	const DirectX::TexMetadata& metadata2 = mipImages2.GetMetadata();
	ComPtr<ID3D12Resource> textureResource2 = CreateTextureResource(device.Get(), metadata2);
	UploadTextureData(textureResource2.Get(), mipImages2);

	bool useMonsterBall = true;

#pragma endregion

#pragma region ShaderResourceViewをつくる 03_00 2枚目のSRV 05_01

	// metaDataを基にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; // 2Dテクスチャ
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

	// SRVを作成するDescriptorHeapの場所を決める
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = dxCommon->GetSrvCPUHandle(0);
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU = dxCommon->GetSrvGPUHandle(0);

	// 先頭はImGuiが使っているのでその次を使う
	textureSrvHandleCPU.ptr += descriptorSizeSRV;
	textureSrvHandleGPU.ptr += descriptorSizeSRV;

	// metaDataを基にSRVの設定2
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc2{};
	srvDesc2.Format = metadata2.format;
	srvDesc2.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc2.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; // 2Dテクスチャ
	srvDesc2.Texture2D.MipLevels = UINT(metadata2.mipLevels);

	// SRVを作成するDescriptorHeapの場所を決める2
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU2 = dxCommon->GetSrvCPUHandle(2);
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU2 = dxCommon->GetSrvGPUHandle(2);

	// 先頭はImGuiが使っているのでその次を使う2
	textureSrvHandleCPU2.ptr += descriptorSizeSRV;
	textureSrvHandleGPU2.ptr += descriptorSizeSRV;

	// SRVの生成
	device->CreateShaderResourceView(textureResource.Get(), &srvDesc, textureSrvHandleCPU);

	// SRVの生成
	device->CreateShaderResourceView(textureResource2.Get(), &srvDesc2, textureSrvHandleCPU2);

#pragma endregion
	// メインループ外で定義（前フレームのキー状態を保存）
	BYTE preKey[256] = {};

	// メインループ 00_03
	MSG msg{};
	// ウィンドウのxボタンが押されるまでループ
	while (msg.message != WM_QUIT) {
		// Windowにメッセージが来てたら最優先で処理させる
		if (winApp->ProcessMessage()) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			break;
		} else { // ゲームの処理

			input->Update();

			// Zキーがトリガー（今回押されていて、前回押されていない）なら再生
			if (input->TriggerKey(DIK_Z)) {
				SoundPlayWave(xAudio2.Get(), soundData1);
			}

			if (input->TriggerKey(DIK_SPACE)) {
				if (isDebugCamera) {
					isDebugCamera = false;
				} else {
					isDebugCamera = true;
				}
			}

			if (isDebugCamera)
			{
				if (debugCamera != nullptr)
				{
					debugCamera->Update();
					useCamera = debugCamera;
				}
			} else {
				if (camera != nullptr)
				{
					camera->Update();
					useCamera = camera;
				}
			}

			if (true)
			{

			}
			else
			{

			}

#pragma region Imguiを使う

			// フレームの先頭でImguiにここからフレームが始まる旨を告げる
			ImGui_ImplDX12_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();
			// 開発用UIの処理、実際に開発用のUIを出す場合はここをゲーム固有の処理に置き換える
			/*ImGui::ShowDemoWindow();*/

			// --- ImGuiカメラコントローラ ---

			ImGui::Begin("Camera Control");

			if (isDebugCamera) {
				ImGui::Text("Debug Camera");
				ImGui::DragFloat3("CameraRotate", &useCamera->GetRotate().x, 0.1f);
				ImGui::DragFloat3("CameraTranslate", &useCamera->GetTranslate().x, 0.1f);
				ImGui::Checkbox("CameraModeChange", &isDebugCamera);
			}
			else {
				ImGui::Text("Normal Camera");
				ImGui::DragFloat3("CameraRotate", &useCamera->GetRotate().x, 0.1f);
				ImGui::DragFloat3("CameraTranslate", &useCamera->GetTranslate().x, 0.1f);
				ImGui::Checkbox("CameraModeChange", &isDebugCamera);
			}

			ImGui::End();

			ImGui::Begin("Model Control");

			ImGui::Text("Model Scale");
			ImGui::DragFloat3("ModelScale", &transform.scale.x, 0.1f);

			ImGui::Text("Model Rotate");
			ImGui::DragFloat3("ModelRotate", &transform.rotate.x, 0.1f);

			ImGui::Text("Model Translate");
			ImGui::DragFloat3("ModelTranslate", &transform.translate.x, 0.1f);

			ImGui::Text("Model TextureChange");
			ImGui::Checkbox("useMonsterBall", &useMonsterBall);

			// カラー（Vector4）の操作（RGBA）
			ImGui::ColorEdit3("Light Color", reinterpret_cast<float*>(&directionalLightData->color));

			// 向き（Vector3）操作（XYZ方向）
			ImGui::SliderFloat3("Direction", reinterpret_cast<float*>(&directionalLightData->direction), -1.0f, 1.0f);

			// 光の強さ（intensity）操作
			ImGui::SliderFloat("Intensity", &directionalLightData->intensity, 0.0f, 10.0f);

			ImGui::DragFloat2("UVTranslate", &uvTransformSprite.translate.x, 0.01f, -10.0f, 10.0f);
			ImGui::DragFloat2("UVScale", &uvTransformSprite.scale.x, 0.01f, -10.0f, 10.0f);
			ImGui::SliderAngle("UVRotate", &uvTransformSprite.rotate.z);

			ImGui::End();

			// Imguiの内部コマンドを生成する
			ImGui::Render();

#pragma endregion

#pragma region 三角形の回転処理 02_02 Sprite用のWorldViewProjectionMatrixを作る 04_00

			/*transform.rotate.y += 0.03f;*/
			// 行列の内容を更新して三角形を動かす
			Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
			Matrix4x4 cameraMatrix = MakeAffineMatrix(useCamera->GetTransform().scale, useCamera->GetTransform().rotate, useCamera->GetTransform().translate);
			Matrix4x4 viewMatrix = InverseMatrix(cameraMatrix);
			Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(1280) / float(720), 0.1f, 100.0f);
			Matrix4x4 worldViewProjectionMatrix = MultiplyMatrix(worldMatrix, MultiplyMatrix(viewMatrix, projectionMatrix));
			// シーン上で三角形を描画
			wvpData->WVP = worldViewProjectionMatrix;
			wvpData->World = worldViewProjectionMatrix;

			Matrix4x4 worldMatrix2 = MakeAffineMatrix(transform2.scale, transform2.rotate, transform2.translate);
			Matrix4x4 cameraMatrix2 = MakeAffineMatrix(useCamera->GetTransform().scale, useCamera->GetTransform().rotate, useCamera->GetTransform().translate);
			Matrix4x4 viewMatrix2 = InverseMatrix(cameraMatrix2);
			Matrix4x4 projectionMatrix2 = MakePerspectiveFovMatrix(0.45f, float(1280) / float(720), 0.1f, 100.0f);
			Matrix4x4 worldViewProjectionMatrix2 = MultiplyMatrix(worldMatrix2, MultiplyMatrix(viewMatrix2, projectionMatrix2));
			// シーン上で三角形を描画
			wvpData2->WVP = worldViewProjectionMatrix2;
			wvpData2->World = worldViewProjectionMatrix2;

			// UVトランスフォーム行列の作成と書き込み
			Matrix4x4 uvTransformMatrix = MakeScaleMatrix(uvTransformSprite.scale);
			uvTransformMatrix = MultiplyMatrix(uvTransformMatrix, MakeRotateZMatrix(uvTransformSprite.rotate.z));
			uvTransformMatrix = MultiplyMatrix(uvTransformMatrix, MakeTranslateMatrix(uvTransformSprite.translate));
			/*materialDataSprite->uvTransform = uvTransformMatrix;*/

			//// Sprite用のWorldViewProjectionMatrixを作る
			//Matrix4x4 worldMatrixSprite = MakeAffineMatrix(transformSprite.scale, transformSprite.rotate, transformSprite.translate);
			//Matrix4x4 viewMatrixSprite = MakeIdentityMatrix();
			//Matrix4x4 projectionMatrixSprite = MakeOrthographicMatrix(0.0f, 0.0f, static_cast<float>(1280), static_cast<float>(720), 0.0f, 100.0f);
			//Matrix4x4 worldViewProjectionMatrixSprite = MultiplyMatrix(worldMatrixSprite, MultiplyMatrix(viewMatrixSprite, projectionMatrixSprite));
			//transformationMatrixDataSprite->WVP = worldViewProjectionMatrixSprite;
			//transformationMatrixDataSprite->World = worldViewProjectionMatrixSprite;

			Vector2 pos = sprite->GetPosition();
			pos += {1.f, 1.f};
			sprite->SetPosition(pos);
			sprite->Update();
			sprite2->Update();
#pragma endregion

#pragma region コマンドを積み込んで確定させる 01_00

			// これから書き込むバックバッファのインデックスを取得
			//UINT backBufferIndex = swapChain->GetCurrentBackBufferIndex();

			// TransitionBarrierの設定 *backBufferIndexを取得した直後、RenderTargetを設定する前に行う
			//D3D12_RESOURCE_BARRIER barrier{};
			// 今回のバリアはTransition
			//barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			// Noneにしておく
			//barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			// バリアを張る対象のリソース。現在のバックバッファに対して行う
			//barrier.Transition.pResource = swapChainResources[backBufferIndex].Get();
			// 遷移前（現在）のResourceState
			//barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
			// 遷移後のResourceState
			//barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
			// TransitionBarrierを指定
			//commandList->ResourceBarrier(1, &barrier);

			// 描画先のRTVとDSVを指定する 03_01
			//D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dxCommon->GetDsvCPUHandle(0);
			// 描画先のRTVを設定する
			//commandList->OMSetRenderTargets(1, &rtvHandle[backBufferIndex], false, &dsvHandle);

			// 指定した色で画面全体をクリアする
			//float clearColor[] = { 0.1f, 0.25f, 0.5f, 1.0f }; // 青っぽい色、RGBAの順
			//commandList->ClearRenderTargetView(rtvHandle[backBufferIndex], clearColor, 0, nullptr);

			// 描画用のDescriptorHeapの設定 02_03
			//ID3D12DescriptorHeap* descriptorHeaps[] = { srvDescriptorHeap.Get() };
			//commandList->SetDescriptorHeaps(1, descriptorHeaps);

			// 指定した深度で画面全体をクリアする 03_01
			//commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

			//commandList->RSSetViewports(1, &viewport);    // Viewportを設定
			//commandList->RSSetScissorRects(1, &scissorRect);    // Scissorを設定
			
			/*dxCommon->ResetCommand();*/
			dxCommon->PreDraw();

			// RootSignatureを設定。PSOに設定しているけど別途設定が必要
			//commandList->SetGraphicsRootSignature(rootSignature.Get());
			//commandList->SetPipelineState(graphicsPipelineState.Get());    // PSOを設定

			spriteData->SpriteDataSet();


			/*-----三角形描画-----*/

			//commandList->IASetVertexBuffers(0, 1, &vertexBufferViewSprite);    // VBVを設定
			//// 形状を設定。PSOに設定しているものとは独立。同じものを設定すると考えておけば良い
			//commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			//// マテリアルCBufferの場所を設定
			//commandList->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
			//// wvp用のBufferの場所を設定
			//commandList->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());
			//// SRVのDescriptorTableの戦闘を設定、2はrootParameter[2] 03_00
			//commandList->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);

			//commandList->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());
			//// 描画！(DrawCall「ドローコール」)。頂点元で2つのインスタンス。インスタンスについては今後
			//commandList->DrawInstanced(6, 1, 0, 0);

#pragma endregion

#pragma region Sphere描画 05_00

			// --- Sphere描画 ---
			commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
			commandList->IASetIndexBuffer(nullptr); // インデックス使っていないので設定しない
			commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			// RootParams 設定（そのままでOK）
			commandList->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
			commandList->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());
			commandList->SetGraphicsRootDescriptorTable(2, useMonsterBall ? textureSrvHandleGPU2 : textureSrvHandleGPU);
			commandList->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());

			// 頂点描画（DrawIndexedではなくDrawInstanced）
			commandList->DrawInstanced(
				static_cast<UINT>(modelData.vertices.size()), // 頂点数ぶん描画
				1, 0, 0);

			// --- Sphere描画 ---
			commandList->IASetVertexBuffers(0, 1, &vertexBufferView2);
			commandList->IASetIndexBuffer(nullptr); // インデックス使っていないので設定しない
			commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			// RootParams 設定（そのままでOK）
			commandList->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
			commandList->SetGraphicsRootConstantBufferView(1, wvpResource2->GetGPUVirtualAddress());
			commandList->SetGraphicsRootDescriptorTable(2, useMonsterBall ? textureSrvHandleGPU2 : textureSrvHandleGPU);
			commandList->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());

			// 頂点描画（DrawIndexedではなくDrawInstanced）
			commandList->DrawInstanced(
				static_cast<UINT>(modelData2.vertices.size()), // 頂点数ぶん描画
				1, 0, 0);

#pragma endregion

#pragma region Spritenの描画コマンドを積む 3D描画の後Imguiの前 04_00

			//// Spriteの描画。変更が必要なものだけ変更する
			//commandList->IASetVertexBuffers(0, 1, &vertexBufferViewSprite);  // VBVを設定
			//commandList->IASetIndexBuffer(&indexBufferViewSprite);
			//// TransformationMatrixCBufferの場所を設定
			//commandList->SetGraphicsRootConstantBufferView(1, transformationMatrixResourceSprite->GetGPUVirtualAddress());
			//// Spriteを常にuvCheckerにする
			//commandList->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);
			////
			//commandList->SetGraphicsRootConstantBufferView(0, materialResourceSprite->GetGPUVirtualAddress());
			//// 描画！ (DrawCall/ドローコール)
			//commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);

			sprite->Draw();
			sprite2->Draw();

#pragma endregion

#pragma region Imguiを描画する 02_03

			// 諸諸の処理が終わった後にコマンドを積む、GUIは画面の最前面に映すので最後の描画
			// ただしResourceBarrierによってD3D12_RESOURCE_STATE_RENDER_TARGET→D3D12_RESOURCE_STATE_PRESENTへ遷移させる前

			// 実際のcommandListのImGuiの描画コマンドを積む
			ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList.Get());

#pragma endregion

			dxCommon->PostDraw();

#pragma endregion

		}
	} // メインループ外

#pragma region ゲーム終了時にはCOMの終了処理を行っておく

	CoUninitialize();

#pragma endregion


#pragma region Imguiの終了処理

	// ImGuiの終了処理。詳細はさして重要ではないので解説は省略する。
	// こういうもんである。初期化と逆順に行う
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

#pragma endregion

	xAudio2.Reset();
	SoundUnload(&soundData1);

#pragma region 解放処理(リソースチェックの前) 01_03

	CloseHandle(fenceEvent);
	winApp->Finalize();
	TextureManager::GetInstance()->Finalize();
	
#pragma endregion

#pragma region リソースリークチェック(最後の最後に残っているものがないか) *main関数のreturnの直前に行う 01_03



#pragma endregion


	delete camera;
	camera = nullptr;

	delete debugCamera;
	debugCamera = nullptr;

	useCamera = nullptr;

	delete input;
	input = nullptr;

	delete winApp;
	winApp = nullptr;

	delete sprite;
	sprite = nullptr;

	delete spriteData;
	spriteData = nullptr;

	return 0;
}
