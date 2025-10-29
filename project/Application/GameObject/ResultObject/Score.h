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
	
	bool GetNext() { return pushNext_; }

	// シーンを抜ける際に呼び出すメソッド
	void SaveRankingData();

	int NextSceneNum() { return sceneNum_; }

	// 追加：ランキング変更検知用メソッド
	bool IsNewRecord() const { return isNewRecord_; }
	int GetRankingPosition() const { return rankingPosition_; } // 1,2,3,4(圏外)を返す

	void ResetAudio();
private:
	bool goTitle_ = false;
	bool goResult_ = false;
	bool pushNext_ = false;

	// 追加：ランキング関連フラグ
	bool isNewRecord_ = false;  // ランキングに入ったかどうか
	int rankingPosition_ = 4;   // ランキング内での位置 (1=1st, 2=2nd, 3=3rd, 4=圏外)

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
	int sceneNum_ = 0;

	int nowScore_ = 0;

	int score1st_ = 50000;  // デフォルト値を明示的に設定
	int score2nd_ = 40000;  // デフォルト値を明示的に設定
	int score3rd_ = 30000;  // デフォルト値を明示的に設定
	
	// JSONファイルのパスを修正
	const std::string rankingJsonPath_ = "resources/engineresources/test.json";

	float rankingStartX_[6] = { 17.24f,16.34f,15.35f,14.46f,13.5f,11.5f };
	float rankingEndX_[6] = { 6.5f,5.5f,4.5f,3.5f,2.5f,0.7f };
	float nowStartX_[6] = { 17.24f,16.34f,15.35f,14.46f,13.5f,11.59f };
	float nowEndX_[6] = { 6.5f,5.5f,4.5f,3.5f,2.5f,0.7f };

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

	unique_ptr<Sprite> msgUI_ = make_unique<Sprite>();

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
	float scoreEndY_ = 5.0f;
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

	bool isDrumroll_ = false;

	std::array<GameTimer, 8> textEasingTimer_;
	std::array<GameTimer, 5> scoreEasingTimer_;
	std::array<GameTimer, 6> rankingInTimer_;
	GameTimer rankAndPlayerEasingTimer_;
	GameTimer scoreOutTimer_;
	GameTimer cursolMoveTimer_;

	GameTimer maskTimer_;

	//　テキストのバウンスアニメーション用タイマー
	GameTimer textBounceStartTimer_;
	std::array<GameTimer, 8> textBounceTimer_;
	std::array<float, 8> textBaseY_; // 各テキストの基準Y座標を保存

	// スコアのバウンスアニメーション用タイマー
	GameTimer scoreBounceStartTimer_;
	std::array<GameTimer, 5> scoreBounceTimer_;
	std::array<float, 5> scoreBaseY_; // 各スコアの基準Y座標を保存

	// nowScoreランキング画面でのバウンスアニメーション用タイマー
	GameTimer nowScoreBounceStartTimer_;
	std::array<GameTimer, 5> nowScoreBounceTimer_;
	std::array<float, 5> nowScoreBaseY_; // 各nowScoreの基準Y座標を保存

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
	unique_ptr<Particles> kazeParticle_ = make_unique<Particles>();
	unique_ptr<Particles> oneParticle_ = make_unique<Particles>();
	unique_ptr<Particles> twoParticle_ = make_unique<Particles>();
	unique_ptr<Particles> threeParticle_ = make_unique<Particles>();

	unique_ptr<Particles> fallCandyParticle_ = make_unique<Particles>();
	unique_ptr<Particles> shotCandyParticle_ = make_unique<Particles>();
	unique_ptr<Particles> shotCandy2Particle_ = make_unique<Particles>();

	bool isCandyShot_ = false;

	unique_ptr<Model> backGround_ = make_unique<Model>();
	Transform backGroundTransform_;
	Vector3 backGroundStartColor_ = { 0.0f,0.0f,0.0f };
	Vector3 backGroundEndColor_ = { 0.0353f, 0.0353f, 0.0627f };
	Vector3 backGroundColor_ = {};

	//---------------------------------------------------------------------//

	unique_ptr<AudioX> completeSE_ = make_unique<AudioX>();
	unique_ptr<AudioX> drumrollSE_ = make_unique<AudioX>();
	unique_ptr<AudioX> moveCursolSE_ = make_unique<AudioX>();
	unique_ptr<AudioX> decideSE_ = make_unique<AudioX>();
	unique_ptr<AudioX> resultSceneBGM_ = make_unique<AudioX>();

	float completeSE_BaseVolume_ = 0.5f;
	float drumrollSE_BaseVolume_ = 0.5f;
	float moveCursolSE_BaseVolume_ = 0.5f;
	float decideSE_BaseVolume_ = 0.5f;
	float resultSceneBGM_BaseVolume_ = 0.5f;

	void UpdateAudio();
};