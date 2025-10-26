#pragma once
#include <array>
#include <fstream>
#include <sstream>
#include <string>
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
	bool GoResult() { return goResult_; }
	
	// シーンを抜ける際に呼び出すメソッド
	void SaveRankingData();

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
	
	// JSON関連のメソッド
	void LoadRankingData();
	void SortRankingScores();

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

	enum class Rank {
		S,
		A,
		B,
		C,
	};

	float floatTimeCount_ = 0.0f;
	float floatAmplitude_ = -0.3f; // 上下の振幅
	float floatSpeed_ = 2.0f;      // 浮遊速度

private:
	int nowScore_ = 0;

	int score1st_ = 0;
	int score2nd_ = 0;
	int score3rd_ = 0;
	
	// JSONファイルのパス
	const std::string rankingJsonPath_ = "resources/engineResources/test.json";

	float rankingStartX_[6] = { 17.24f,16.34f,15.35f,14.46f,13.5f,11.5f };
	float rankingEndX_[6] = { 7.24f,6.34f,5.35f,4.46f,3.5f,1.5f };
	float nowStartX_[6] = { 17.24f,16.34f,15.35f,14.46f,13.5f,11.59f };
	float nowEndX_[6] = { 7.24f,6.34f,5.35f,4.46f,3.5f,1.59f };

	std::array<unique_ptr<Model>, 6> score1stModel_;
	std::array<Transform, 6> score1stTransform_;
	std::array<unique_ptr<Model>, 6> score2ndModel_;
	std::array<Transform, 6> score2ndTransform_;
	std::array<unique_ptr<Model>, 6> score3rdModel_;
	std::array<Transform, 6> score3rdTransform_;
	std::array<unique_ptr<Model>, 6> nowScoreModel_;
	std::array<Transform, 6> nowScoreTransform_;

	ScreenType screenType_ = ScreenType::SCORE;
	NextScene nextScene_ = NextScene::TITLE;
	Rank rank_ = Rank::S;

	unique_ptr<Sprite> pushAsusumu_ = make_unique<Sprite>();
	unique_ptr<Sprite> titleUI_ = make_unique<Sprite>();
	unique_ptr<Sprite> retryUI_ = make_unique<Sprite>();
	unique_ptr<Sprite> cursolUI_ = make_unique<Sprite>();

	float cursolStartY_ = 500.0f;
	float cursolEndY_ = 600.0f;

	DirectXCommon* dxCommon_ = nullptr;
	std::array<unique_ptr<Model>,8> textModel_;
	std::array<unique_ptr<Model>, 5> scoreModel_;
	std::array<unique_ptr<Model>, 2> rankModel_;
	std::array<Transform, 8> textTransform_;
	std::array<Transform, 5> scoreTransform_;
	std::array<Transform, 2> rankTransform_;

	unique_ptr<Model> playerModel_;
	unique_ptr<Model> machineModel_;
	unique_ptr<Model> player2Model_;
	unique_ptr<Model> player2ArmModel_;
	unique_ptr<Model> machine2Model_;

	Transform playerTransform_;
	Transform machineTransform_;
	Transform player2Transform_;
	Transform player2ArmTransform_;
	Transform machine2Transform_;

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
	std::array<GameTimer, 6> rankingInTimer_;
	GameTimer rankAndPlayerEasingTimer_;
	GameTimer scoreOutTimer_;
	GameTimer cursolMoveTimer_;

	Vector4 rankColor_[4] = {
	{0.255f, 0.839f, 0.153f, 1.0f}, // 緑
	{0.173f, 0.365f, 0.835f, 1.0f}, // 青
	{0.820f, 0.137f, 0.137f, 1.0f}, // 赤
	{0.898f, 0.898f, 0.161f, 1.0f}  // 黄
	};
	
	Vector4 rankingColor_[4] = {
	{ 1.0f, 0.949f, 0.161f, 1.0f },   // 1st
	{ 0.647f, 0.647f, 0.647f, 1.0f }, // 2nd
    { 0.961f, 0.427f, 0.235f, 1.0f }, // 3rd
	{ 1.0f, 1.0f, 1.0f, 1.0f }        // Now
	};

	unique_ptr<Particles> ramuneParticle_ = make_unique<Particles>();
	unique_ptr<Particles> ramuneParticle2_ = make_unique<Particles>();
	unique_ptr<Particles> sRankParticle_ = make_unique<Particles>();
};