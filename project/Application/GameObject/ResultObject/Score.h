#pragma once
#include <array>
#include "Application/EngineSystem.h"

class Score
{
public:
	void Initialize(DirectXCommon* dxCommon, float score);

	void Update();

	void Draw(Camera camera);

	void DrawImGui();

	void SetInput(Input* input,GamePad* gamePad) {
		input_ = input;
		gamePad_ = gamePad;
	}

	bool GoTitle() { return goTitle_; }

private:
	bool goTitle_ = false;
	bool goResult_ = false;

	Input* input_ = nullptr;
	GamePad* gamePad_ = nullptr;

	void InitTextModel();
	void InitScoreModel();
	void InitRankModel();
	void InitPlayerModel();
	void InitSprite();
	void InitRanking();

	void ScoreIn();
	void ScoreOut();

	enum class ScreenType {
		SCORE,
		RANKING,
	};

	enum class NextScene {
		TITLE,
		RESULT,
	};

private:
	int nowScore_ = 0;
	int score1st_ = 65432;
	int score2nd_ = 54321;
	int score3rd_ = 43210;

	float rankingStartX_[6] = { 17.24f,16.34f,15.35f,14.46f,13.5f,11.5f };
	float rankingEndX_[6] = { 7.24f,6.34f,5.35f,4.46f,3.5f,1.5f };
	float nowStartX_[6] = { 17.24f,16.34f,15.35f,14.46f,13.5f,11.59f };
	float nowEndX_[6] = { 7.24f,6.34f,5.35f,4.46f,3.5f,1.59f };

	std::array<GameTimer, 6> rankingInTimer_;

	std::array<unique_ptr<Model>, 6> score1stModel_;
	std::array<Transform, 6> score1stTransform_;
	std::array<unique_ptr<Model>, 6> score2ndModel_;
	std::array<Transform, 6> score2ndTransform_;
	std::array<unique_ptr<Model>, 6> score3rdModel_;
	std::array<Transform, 6> score3rdTransform_;
	std::array<unique_ptr<Model>, 6> nowScoreModel_;
	std::array<Transform, 6> nowScoreTransform_;

	ScreenType screenType_ = ScreenType::SCORE;

	unique_ptr<Sprite> pushAsusumu_ = make_unique<Sprite>();
	unique_ptr<Sprite> titleUI_ = make_unique<Sprite>();
	unique_ptr<Sprite> retryUI_ = make_unique<Sprite>();
	unique_ptr<Sprite> cursolUI_ = make_unique<Sprite>();

	DirectXCommon* dxCommon_ = nullptr;
	std::array<unique_ptr<Model>,8> textModel_;
	std::array<unique_ptr<Model>, 5> scoreModel_;
	std::array<unique_ptr<Model>, 2> rankModel_;
	unique_ptr<Model> playerModel_;
	unique_ptr<Model> machineModel_;
	std::array<Transform, 8> textTransform_;
	std::array<Transform, 5> scoreTransform_;
	std::array<Transform, 2> rankTransform_;
	Transform playerTransform_;
	Transform machineTransform_;

	float score_ = 0.0f;
	float scoreStartY_ = 14.75f;
	float scoreEndY_ = 4.75f;
	float textStartY_ = 10.0f;
	float textEndY_ = 4.5f;

	float rankStartX_ = -15.65f;
	float rankEndX_ =  -5.65f;
	float rankTextStartX_ = -13.18f;
	float rankTextEndX_ = -3.18f;
	float playerStartX_ = 14.4f;
	float playerEndX_ = 4.4f;
	float machineStartX_ = 13.85f;
	float machineEndX_ = 3.85f;

	std::array<GameTimer, 8> textEasingTimer_;
	std::array<GameTimer, 5> scoreEasingTimer_;
	GameTimer rankAndPlayerEasingTimer_;
	GameTimer scoreOutTimer_;
};