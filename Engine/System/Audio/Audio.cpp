#include"Audio.h"

void Audio::Initialize(const std::string& fileName) {
	hr_ = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(hr_));

	hr_ = xAudio2_->CreateMasteringVoice(&masterVoice_);
	assert(SUCCEEDED(hr_));

	soundData_ = SoundLoadWave(fileName);
}

void Audio::PlayAudio() {
	SoundPlayWave(xAudio2_.Get(), soundData_);
}

void Audio::Reset() {
	xAudio2_.Reset();
	SoundUnload(&soundData_);
}