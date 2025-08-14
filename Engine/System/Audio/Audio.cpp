#include"Audio.h"

void AudioX::Initialize(const std::string& fileName) {
	hr_ = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(hr_));

	hr_ = xAudio2_->CreateMasteringVoice(&masterVoice_);
	assert(SUCCEEDED(hr_));

	soundData_ = SoundLoadWave(fileName);
}

void AudioX::PlayAudio() {
	SoundPlayWave(xAudio2_.Get(), soundData_);
}

void AudioX::Reset() {
	xAudio2_.Reset();
	SoundUnload(&soundData_);
}