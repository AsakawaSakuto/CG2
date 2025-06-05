#pragma once
#include <fstream>
#include <cstdint>
#include <cassert>
#include <xaudio2.h>
#pragma comment(lib, "xaudio2.lib")

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include"ConvertString.h"

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

SoundData SoundLoadWave(const std::string& filePath);

void SoundUnload(SoundData* soundData);

void SoundPlayWave(IXAudio2* xAudio2, const SoundData& soundData);