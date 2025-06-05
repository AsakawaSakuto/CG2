#include"AudioData.h"

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#include <wrl.h>
#include <d3d12.h>
#include <dxcapi.h>

class Audio
{
public:
	
	void Initialize(const std::string& fileName);

	void PlayAudio();

	void Reset();

private:
	HRESULT hr_;
	Microsoft::WRL::ComPtr<IXAudio2> xAudio2_;
	IXAudio2MasteringVoice* masterVoice_ = nullptr;
	SoundData soundData_;
};