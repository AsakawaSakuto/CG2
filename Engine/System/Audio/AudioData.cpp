// AudioData.cpp
// // WAV直読み ＋ Media FoundationでMP3→PCM16デコード。
// // 生成したPCMはstd::vectorで所有。リンクにはmfplat/mfreadwrite/mfuuidが必要。

#include "AudioData.h"
#include <fstream>
#include <algorithm>
#include <cassert>
#include <wrl/client.h>
#include <windows.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")

using Microsoft::WRL::ComPtr;

// // 4CCヘッダ
struct ChunkHeader {
    char id[4];
    int32_t size;
};

// // RIFF/WAVE
struct RiffHeader {
    ChunkHeader chunk; // //"RIFF"
    char type[4];      // //"WAVE"
};

// // fmt チャンク
struct FormatChunk {
    ChunkHeader chunk; // //"fmt "
    WAVEFORMATEX fmt;
};

// // 小文字化
static std::string ToLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
        [](unsigned char c) { return (char)std::tolower(c); });
    return s;
}

// // UTF-8→UTF-16
static std::wstring ToWide(const std::string& s) {
    if (s.empty()) { return L""; }
    int len = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), nullptr, 0);
    std::wstring w(len, 0);
    MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), &w[0], len);
    return w;
}

// // Media Foundation 一度だけ起動
static void MFShutdownAtExit() { MFShutdown(); }
static void EnsureMFStarted() {
    static std::once_flag once;
    std::call_once(once, []() {
        HRESULT hr = MFStartup(MF_VERSION, MFSTARTUP_FULL);
        assert(SUCCEEDED(hr) && "MFStartup failed. On 'N' edition Windows, install Media Feature Pack.");
        std::atexit(MFShutdownAtExit);
        });
}

// // WAV ロード（基本PCM前提）
SoundData SoundLoadWave(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    assert(file.is_open() && "WAV file open failed");

    RiffHeader riff{};
    file.read(reinterpret_cast<char*>(&riff), sizeof(riff));
    assert(strncmp(riff.chunk.id, "RIFF", 4) == 0);
    assert(strncmp(riff.type, "WAVE", 4) == 0);

    FormatChunk format{};
    file.read(reinterpret_cast<char*>(&format.chunk), sizeof(ChunkHeader));
    assert(strncmp(format.chunk.id, "fmt ", 4) == 0);
    assert(format.chunk.size <= (int)sizeof(format.fmt));
    file.read(reinterpret_cast<char*>(&format.fmt), format.chunk.size);

    // // dataチャンクまでスキップ（JUNK等に対応）
    ChunkHeader data{};
    for (;;) {
        file.read(reinterpret_cast<char*>(&data), sizeof(data));
        assert(file && "WAV: unexpected EOF while seeking data chunk");
        if (strncmp(data.id, "data", 4) == 0) break;
        file.seekg(data.size, std::ios_base::cur);
    }

    SoundData sd{};
    sd.wfex = format.fmt;
    sd.name = filePath;
    sd.pcm.resize(data.size);
    file.read(reinterpret_cast<char*>(sd.pcm.data()), data.size);

    return sd;
}

// // MP3→PCM16 デコード
static SoundData SoundLoadMp3(const std::string& filePath) {
    EnsureMFStarted();

    ComPtr<IMFSourceReader> reader;
    HRESULT hr = MFCreateSourceReaderFromURL(ToWide(filePath).c_str(), nullptr, &reader);
    assert(SUCCEEDED(hr) && "MFCreateSourceReaderFromURL failed");

    // // 出力をPCM16に固定
    ComPtr<IMFMediaType> outType;
    hr = MFCreateMediaType(&outType);
    assert(SUCCEEDED(hr));
    hr = outType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
    assert(SUCCEEDED(hr));
    hr = outType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
    assert(SUCCEEDED(hr));
    hr = outType->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, 16);
    assert(SUCCEEDED(hr));
    hr = reader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, nullptr, outType.Get());
    assert(SUCCEEDED(hr));

    // // 実際のPCMフォーマット取得
    ComPtr<IMFMediaType> curType;
    hr = reader->GetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, &curType);
    assert(SUCCEEDED(hr));

    WAVEFORMATEX* pwfx = nullptr;
    UINT32 cb = 0;
    hr = MFCreateWaveFormatExFromMFMediaType(curType.Get(), &pwfx, &cb);
    assert(SUCCEEDED(hr));
    WAVEFORMATEX wf{};
    wf.wFormatTag = WAVE_FORMAT_PCM;
    wf.nChannels = pwfx->nChannels;
    wf.nSamplesPerSec = pwfx->nSamplesPerSec;
    wf.wBitsPerSample = 16;
    wf.nBlockAlign = (wf.nChannels * wf.wBitsPerSample) / 8;
    wf.nAvgBytesPerSec = wf.nBlockAlign * wf.nSamplesPerSec;
    CoTaskMemFree(pwfx);

    // // 全サンプル読み切り
    std::vector<BYTE> all;
    for (;;) {
        DWORD flags = 0;
        ComPtr<IMFSample> sample;
        hr = reader->ReadSample(MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, nullptr, &flags, nullptr, &sample);
        assert(SUCCEEDED(hr));

        if (flags & MF_SOURCE_READERF_ENDOFSTREAM) break;
        if (!sample) continue;

        ComPtr<IMFMediaBuffer> buf;
        hr = sample->ConvertToContiguousBuffer(&buf);
        assert(SUCCEEDED(hr));

        BYTE* p = nullptr; DWORD maxLen = 0, curLen = 0;
        hr = buf->Lock(&p, &maxLen, &curLen);
        assert(SUCCEEDED(hr));

        size_t old = all.size();
        all.resize(old + curLen);
        memcpy(all.data() + old, p, curLen);

        hr = buf->Unlock();
        assert(SUCCEEDED(hr));
    }

    // // 終端のブロック境界丸め（安全のため）
    if (!all.empty()) {
        all.resize(all.size() - (all.size() % wf.nBlockAlign));
    }

    SoundData sd{};
    sd.wfex = wf;
    sd.pcm = std::move(all);
    sd.name = filePath;
    return sd;
}

// // ディスパッチ
SoundData SoundLoadAudio(const std::string& filePath) {
    std::string low = ToLower(filePath);
    if (low.rfind(".wav") != std::string::npos) {
        return SoundLoadWave(filePath);
    }
    else if (low.rfind(".mp3") != std::string::npos) {
        return SoundLoadMp3(filePath);
    }
    else {
        assert(!"Unsupported audio format. Use .wav or .mp3");
        return {};
    }
}

// // 互換維持：非推奨の即時再生関数（DestroyVoiceしないので乱発禁止）
void SoundPlayWave(IXAudio2* xAudio2, const SoundData& sd) {
    IXAudio2SourceVoice* v = nullptr;
    HRESULT hr = xAudio2->CreateSourceVoice(&v, &sd.wfex);
    assert(SUCCEEDED(hr));
    XAUDIO2_BUFFER buf{};
    buf.pAudioData = sd.pcm.data();
    buf.AudioBytes = static_cast<UINT32>(sd.pcm.size() - (sd.pcm.size() % sd.wfex.nBlockAlign));
    buf.Flags = XAUDIO2_END_OF_STREAM;
    hr = v->SubmitSourceBuffer(&buf);
    assert(SUCCEEDED(hr));
    hr = v->Start();
    assert(SUCCEEDED(hr));
    // // 注意：DestroyVoice未実施。使い捨て用途のみ。
}
