#include "Score.h"
#include <algorithm>
#include <filesystem>
#include <sstream>
#include <iomanip>

void Score::Initialize(DirectXCommon* dxCommon, float score) {
	dxCommon_ = dxCommon;
	score_ = score;

	goTitle_ = false;
	goResult_ = false;

	screenType_ = ScreenType::SCORE;

	nextScene_ = NextScene::TITLE;

	floatTimeCount_ = 0.0f;

	// JSONファイルからランキングデータを読み込み、現在のスコアを含めてソート
	LoadRankingData();
	SortRankingScores();

	ramuneParticle_->Initialize(dxCommon_);
	ramuneParticle_->LoadJson("RamuneResult1");

	ramuneParticle2_->Initialize(dxCommon_);
	ramuneParticle2_->LoadJson("RamuneResult2");

	sRankParticle_->Initialize(dxCommon_);
	sRankParticle_->LoadJson("SRank");

	oneParticle_->Initialize(dxCommon_);
	oneParticle_->LoadJson("1stkirakira");

	twoParticle_->Initialize(dxCommon_);
	twoParticle_->LoadJson("2ndkirakira");

	threeParticle_->Initialize(dxCommon_);
	threeParticle_->LoadJson("3rdkirakira");

	kazeParticle_->Initialize(dxCommon_);
	kazeParticle_->LoadJson("resultKaze");
	kazeParticle_->Stop();

	if (rank_== Rank::S) {
		sRankParticle_->Play();
	} else {
		sRankParticle_->Stop();
	}

	for (int i = 0; i < textEasingTimer_.size(); i++) {
		textEasingTimer_[i].Reset();
	}

	for (int i = 0; i < scoreEasingTimer_.size(); i++) {
		scoreEasingTimer_[i].Reset();
	}

	for (int i = 0; i < rankingInTimer_.size(); i++) {
		rankingInTimer_[i].Reset();
	}

	rankAndPlayerEasingTimer_.Reset();

	scoreOutTimer_.Reset();

	InitTextModel();
	InitScoreModel();
	InitRankModel();
	InitPlayerModel();
	InitRanking();
	InitSprite();

	backGround_->Initialize(dxCommon_, "plane.obj");
	backGround_->SetTexture("resources/image/0.png");
	backGround_->SetUpdateFrustumCulling(false);
	backGround_->SetColorVector3(backGroundStartColor_);
	backGroundTransform_.scale = { 14.62f,8.5f,1.0f };
	backGroundTransform_.rotate = { 0.0f,0.0f,0.0f };
	backGroundTransform_.translate = { 0.0f,2.0f,14.85f };
	backGround_->SetTransform(backGroundTransform_);
}

void Score::Update() {

	floatTimeCount_ += deltaTime_;
	float floatOffset = sinf(floatTimeCount_ * floatSpeed_) * floatAmplitude_;

	playerTransform_.translate.y = 1.01f + floatOffset;
	machineTransform_.translate.y = 0.67f + floatOffset;
	player2Transform_.translate.y = 3.5f + floatOffset;
	player2ArmTransform_.translate.y = 3.78f + floatOffset;
	machine2Transform_.translate.y = 2.39f + floatOffset;

	switch (screenType_)
	{
	case Score::ScreenType::SCORE:

		ScoreIn();
		pushAsusumu_->SetColor({ 1.0f,1.0f,1.0f,rankAndPlayerEasingTimer_.GetProgress() });

		break;

	case Score::ScreenType::RANKING:

		ScoreOut();
		pushAsusumu_->SetColor({ 1.0f,1.0f,1.0f,rankingInTimer_[0].GetReverseProgress()});
		titleUI_->SetColor({ 1.0f,1.0f,1.0f,rankingInTimer_[0].GetProgress() });
		retryUI_->SetColor({ 1.0f,1.0f,1.0f,rankingInTimer_[0].GetProgress() });
		cursolUI_->SetColor({ 1.0f,1.0f,1.0f,rankingInTimer_[0].GetProgress() });

		for (int i = 0; i < 6; i++) {
			score1stTransform_[i].translate.x = Easing::Lerp(
				rankingStartX_[i],
				rankingEndX_[i],
				rankingInTimer_[i].GetProgress(),
				Easing::Type::EaseInOutBack
			);

			score2ndTransform_[i].translate.x = Easing::Lerp(
				rankingStartX_[i],
				rankingEndX_[i],
				rankingInTimer_[i].GetProgress(),
				Easing::Type::EaseInOutBack
			);

			score3rdTransform_[i].translate.x = Easing::Lerp(
				rankingStartX_[i],
				rankingEndX_[i],
				rankingInTimer_[i].GetProgress(),
				Easing::Type::EaseInOutBack
			);

			nowScoreTransform_[i].translate.x = Easing::Lerp(
				nowStartX_[i],
				nowEndX_[i],
				rankingInTimer_[i].GetProgress(),
				Easing::Type::EaseInOutBack
			);
		}

		player2Transform_.translate.x = Easing::Lerp(
			-14.58f,
			-4.58f,
			rankingInTimer_[5].GetProgress(),
			Easing::Type::EaseInOutBack
		);

		player2ArmTransform_.translate.x = Easing::Lerp(
			-14.46f,
			-4.46f,
			rankingInTimer_[5].GetProgress(),
			Easing::Type::EaseInOutBack
		);

		machine2Transform_.translate.x = Easing::Lerp(
			-13.8f,
			-3.8f,
			rankingInTimer_[5].GetProgress(),
			Easing::Type::EaseInOutBack
		);

		for (int i = 0; i < score1stModel_.size(); i++) {
			score1stModel_[i]->SetTransform(score1stTransform_[i]);
		}

		for (int i = 0; i < score2ndModel_.size(); i++) {
			score2ndModel_[i]->SetTransform(score2ndTransform_[i]);
		}

		for (int i = 0; i < score3rdModel_.size(); i++) {
			score3rdModel_[i]->SetTransform(score3rdTransform_[i]);
		}

		switch (nextScene_)
		{
		case Score::NextScene::TITLE:

			if (rankingInTimer_[0].IsFinished() && input_->TriggerKey(DIK_SPACE) || gamePad_->TriggerButton(GamePad::A)) {
				goTitle_ = true;
			}

			if (rankingInTimer_[0].IsFinished() && !cursolMoveTimer_.IsActive() && (input_->TriggerKey(DIK_S) || input_->TriggerKey(DIK_DOWN) || gamePad_->TriggerButton(GamePad::DOWN_BOTTON))) {
				nextScene_ = NextScene::RESULT;
				cursolMoveTimer_.Start(0.25f);
				cursolStartY_ = 500.0f;
				cursolEndY_ = 600.0f;
			}

			if (cursolMoveTimer_.IsActive()) {
				cursolUI_->SetPosition({ 180.0f,Easing::Lerp(
					cursolStartY_,
					cursolEndY_,
					cursolMoveTimer_.GetProgress(),
					Easing::Type::EaseInOutQuint
				) });
			} else {
				cursolUI_->SetPosition({ 180.0f,500.0f });
			}

			break;
		case Score::NextScene::RESULT:

			if (rankingInTimer_[0].IsFinished() && input_->TriggerKey(DIK_SPACE) || gamePad_->TriggerButton(GamePad::A)) {
				goResult_ = true;
			}

			if (rankingInTimer_[0].IsFinished() && !cursolMoveTimer_.IsActive() && (input_->TriggerKey(DIK_W) || input_->TriggerKey(DIK_UP) || gamePad_->TriggerButton(GamePad::UP_BOTTON))) {
				nextScene_ = NextScene::TITLE;
				cursolMoveTimer_.Start(0.25f);
				cursolStartY_ = 600.0f;
				cursolEndY_ = 500.0f;
			}

			if (cursolMoveTimer_.IsActive()) {
				cursolUI_->SetPosition({ 180.0f,Easing::Lerp(
					cursolStartY_,
					cursolEndY_,
					cursolMoveTimer_.GetProgress(),
					Easing::Type::EaseInOutQuint
				) });
			} else {
				cursolUI_->SetPosition({ 180.0f,600.0f });
			}

			break;
		}

		break;
	}

	for (int i = 0; i < textModel_.size(); ++i) {
		textModel_[i]->Update();
	}

	for (int i = 0; i < scoreModel_.size(); ++i) {
		scoreModel_[i]->Update();
	}
	
	for (int i = 0; i < rankModel_.size(); ++i) {
		rankModel_[i]->Update();
	}

	for (int i = 0; i < score1stModel_.size(); i++) {
		score1stModel_[i]->SetColor(rankingColor_[0]);
		score1stModel_[i]->Update();
	}

	for (int i = 0; i < score2ndModel_.size(); i++) {
		score2ndModel_[i]->SetColor(rankingColor_[1]);
		score2ndModel_[i]->Update();
	}

	for (int i = 0; i < score3rdModel_.size(); i++) {
		score3rdModel_[i]->SetColor(rankingColor_[2]);
		score3rdModel_[i]->Update();
	}

	for (int i = 0; i < rankingInTimer_.size(); i++) {
		rankingInTimer_[i].Update();
	}

	for (int i = 0; i < nowScoreModel_.size(); i++) {
		nowScoreModel_[i]->SetTransform(nowScoreTransform_[i]);
		nowScoreModel_[i]->SetColor(rankingColor_[3]);
		nowScoreModel_[i]->Update();
	}

	playerModel_->Update();
	machineModel_->Update();

	player2Model_->SetTransform(player2Transform_);
	player2ArmModel_->SetTransform(player2ArmTransform_);
	machine2Model_->SetTransform(machine2Transform_);
	player2Model_->Update();
	player2ArmModel_->Update();
	machine2Model_->Update();

	pushAsusumu_->Update();
	titleUI_->Update();
	retryUI_->Update();
	cursolUI_->Update();

	ramuneParticle_->SetEmitterPosition(playerTransform_.translate);
	ramuneParticle_->SetOffSet({ 1.4f,-1.18f,0.0f });
	ramuneParticle_->Update();

	ramuneParticle2_->SetEmitterPosition(player2Transform_.translate);
	ramuneParticle2_->SetOffSet({ -0.8f,-1.1f,0.0f });
	ramuneParticle2_->Update();

	sRankParticle_->SetEmitterPosition(rankTransform_[0].translate);
	sRankParticle_->SetOffSet({ 0.2f,0.1f,-1.0f });
	sRankParticle_->Update();

	kazeParticle_->Update();

	oneParticle_->SetEmitterPosition(score1stTransform_[5].translate);
	oneParticle_->SetOffSet({ 0.0f,0.0f,-1.0f });
	oneParticle_->Update();

	twoParticle_->SetEmitterPosition(score2ndTransform_[5].translate);
	twoParticle_->SetOffSet({ 0.0f,0.0f,-1.0f });
	twoParticle_->Update();

	threeParticle_->SetEmitterPosition(score3rdTransform_[5].translate);
	threeParticle_->SetOffSet({ 0.0f,0.0f,-1.0f });
	threeParticle_->Update();

	cursolMoveTimer_.Update();

	if (rankingInTimer_[5].IsActive()) {
		backGround_->SetColorVector3({
		Easing::LerpVector3(
			backGroundStartColor_,
			backGroundEndColor_,
			rankingInTimer_[5].GetProgress(),
			Easing::Type::Linear
		) }
		);
	}
	backGround_->Update();
}

void Score::Draw(Camera camera) {

	backGround_->Draw(camera);

	switch (screenType_)
	{
	case Score::ScreenType::SCORE:

		break;
	case Score::ScreenType::RANKING:

		for (int i = 0; i < score1stModel_.size(); i++) {
			score1stModel_[i]->Draw(camera);
		}

		for (int i = 0; i < score2ndModel_.size(); i++) {
			score2ndModel_[i]->Draw(camera);
		}

		for (int i = 0; i < score3rdModel_.size(); i++) {
			score3rdModel_[i]->Draw(camera);
		}

		for (int i = 0; i < nowScoreModel_.size(); i++) {
			nowScoreModel_[i]->Draw(camera);
		}

		player2Model_->Draw(camera);
		player2ArmModel_->Draw(camera);
		machine2Model_->Draw(camera);

		break;
	}

	for (int i = 0; i < textModel_.size(); ++i) {
		textModel_[i]->Draw(camera);
	}

	for (int i = 0; i < scoreModel_.size(); ++i) {
		scoreModel_[i]->Draw(camera);
	}

	for (int i = 0; i < rankModel_.size(); ++i) {
		rankModel_[i]->Draw(camera);
	}

	playerModel_->Draw(camera);
	machineModel_->Draw(camera);

	ramuneParticle_->Draw(camera);
	ramuneParticle2_->Draw(camera);
	sRankParticle_->Draw(camera);
	kazeParticle_->Draw(camera);
	oneParticle_->Draw(camera);
	twoParticle_->Draw(camera);
	threeParticle_->Draw(camera);

	pushAsusumu_->Draw();
	titleUI_->Draw();
	retryUI_->Draw();
	cursolUI_->Draw();
}

void Score::DrawImGui() {

	ImGui::Begin("GB_ColorFade");

	ImGui::ColorEdit3("start", &backGroundStartColor_.x);
	ImGui::ColorEdit3("end", &backGroundEndColor_.x);

	ImGui::End();

	kazeParticle_->DrawImGui("starParticle");
	
	//oneParticle_->DrawImGui("oneParticle");
	//twoParticle_->DrawImGui("wtoParticle");
	//threeParticle_->DrawImGui("threeParticle");

	//backGround_->DrawImGui("back");

	//ramuneParticle_->DrawImGui("ramuneParticle");
	//ramuneParticle2_->DrawImGui("ramuneParticle2");

	//sRankParticle_->DrawImGui("sRankParticle");

	//pushAsusumu_->DrawImGui("pushAsusumu");

	//ImGui::Begin("text");

	//ImGui::DragFloat3("t0", &textTransform_[0].translate.x,0.01f);
	//ImGui::DragFloat3("t1", &textTransform_[1].translate.x,0.01f);
	//ImGui::DragFloat3("t2", &textTransform_[2].translate.x,0.01f);
	//ImGui::DragFloat3("t3", &textTransform_[3].translate.x,0.01f);
	//ImGui::DragFloat3("t4", &textTransform_[4].translate.x,0.01f);
	//ImGui::DragFloat3("t5", &textTransform_[5].translate.x,0.01f);
	//ImGui::DragFloat3("t6", &textTransform_[6].translate.x,0.01f);
	//ImGui::DragFloat3("t7", &textTransform_[7].translate.x,0.01f);

	//ImGui::Separator();
	//ImGui::Text("Score Digits");
	//ImGui::DragFloat3("s0", &scoreTransform_[0].translate.x,0.01f);
	//ImGui::DragFloat3("s1", &scoreTransform_[1].translate.x,0.01f);
	//ImGui::DragFloat3("s2", &scoreTransform_[2].translate.x,0.01f);
	//ImGui::DragFloat3("s3", &scoreTransform_[3].translate.x,0.01f);
	//ImGui::DragFloat3("s4", &scoreTransform_[4].translate.x,0.01f);

	//ImGui::Separator();
	//ImGui::Text("rank Digits");
	//ImGui::DragFloat3("r0", &rankTransform_[0].translate.x, 0.01f);
	//ImGui::DragFloat3("r1", &rankTransform_[1].translate.x, 0.01f);
	//ImGui::DragFloat3("rr0", &rankTransform_[0].rotate.x, 0.01f);
	//ImGui::DragFloat3("rr1", &rankTransform_[1].rotate.x, 0.01f);
	//ImGui::DragFloat3("rs0", &rankTransform_[0].scale.x, 0.01f);
	//ImGui::DragFloat3("rs1", &rankTransform_[1].scale.x, 0.01f);//

	//ImGui::End();

	//ImGui::Begin("playerTranslate");
	//
	//ImGui::DragFloat3("Pt", &playerTransform_.translate.x, 0.01f);
	//ImGui::DragFloat3("Pr", &playerTransform_.rotate.x, 0.01f);
	//ImGui::DragFloat3("Ps", &playerTransform_.scale.x, 0.01f);
	//ImGui::DragFloat3("Mt", &machineTransform_.translate.x, 0.01f);
	//ImGui::DragFloat3("Mr", &machineTransform_.rotate.x, 0.01f);
	//ImGui::DragFloat3("Ms", &machineTransform_.scale.x, 0.01f);
	//
	//ImGui::End();

	//titleUI_->DrawImGui("title");
	//retryUI_->DrawImGui("retry");
	//cursolUI_->DrawImGui("curcol");

	/*ImGui::Begin("p2");

	ImGui::DragFloat3("Pt", &player2Transform_.translate.x, 0.01f);
	ImGui::DragFloat3("Pr", &player2Transform_.rotate.x, 0.01f);
	ImGui::DragFloat3("Ps", &player2Transform_.scale.x, 0.01f);
	ImGui::DragFloat3("PtA", &player2ArmTransform_.translate.x, 0.01f);
	ImGui::DragFloat3("PrA", &player2ArmTransform_.rotate.x, 0.01f);
	ImGui::DragFloat3("PsA", &player2ArmTransform_.scale.x, 0.01f);
	ImGui::DragFloat3("Mt", &machine2Transform_.translate.x, 0.01f);
	ImGui::DragFloat3("Mr", &machine2Transform_.rotate.x, 0.01f);
	ImGui::DragFloat3("Ms", &machine2Transform_.scale.x, 0.01f);

	ImGui::End();*/

	/*ImGui::Begin("1st");

	ImGui::DragFloat3("0", &score1stTransform_[0].translate.x,0.01f);
	ImGui::DragFloat3("1", &score1stTransform_[1].translate.x,0.01f);
	ImGui::DragFloat3("2", &score1stTransform_[2].translate.x,0.01f);
	ImGui::DragFloat3("3", &score1stTransform_[3].translate.x,0.01f);
	ImGui::DragFloat3("4", &score1stTransform_[4].translate.x,0.01f);
	ImGui::DragFloat3("5", &score1stTransform_[5].translate.x,0.01f);

	ImGui::End();

	ImGui::Begin("2nd");

	ImGui::DragFloat3("0", &score2ndTransform_[0].translate.x, 0.01f);
	ImGui::DragFloat3("1", &score2ndTransform_[1].translate.x, 0.01f);
	ImGui::DragFloat3("2", &score2ndTransform_[2].translate.x, 0.01f);
	ImGui::DragFloat3("3", &score2ndTransform_[3].translate.x, 0.01f);
	ImGui::DragFloat3("4", &score2ndTransform_[4].translate.x, 0.01f);
	ImGui::DragFloat3("5", &score2ndTransform_[5].translate.x, 0.01f);

	ImGui::End();

	ImGui::Begin("3rd");

	ImGui::DragFloat3("0", &score3rdTransform_[0].translate.x, 0.01f);
	ImGui::DragFloat3("1", &score3rdTransform_[1].translate.x, 0.01f);
	ImGui::DragFloat3("2", &score3rdTransform_[2].translate.x, 0.01f);
	ImGui::DragFloat3("3", &score3rdTransform_[3].translate.x, 0.01f);
	ImGui::DragFloat3("4", &score3rdTransform_[4].translate.x, 0.01f);
	ImGui::DragFloat3("5", &score3rdTransform_[5].translate.x, 0.01f);

	ImGui::End();*/
}

void Score::InitTextModel() {
	for (int i = 0; i < textModel_.size(); ++i) {
		textModel_[i] = make_unique<Model>();
	}
	textModel_[0]->Initialize(dxCommon_, "resultLogo/a.obj");
	textModel_[1]->Initialize(dxCommon_, "resultLogo/tu.obj");
	textModel_[2]->Initialize(dxCommon_, "resultLogo/me.obj");
	textModel_[3]->Initialize(dxCommon_, "resultLogo/ta.obj");
	textModel_[4]->Initialize(dxCommon_, "resultLogo/o.obj");
	textModel_[5]->Initialize(dxCommon_, "resultLogo/ka.obj");
	textModel_[6]->Initialize(dxCommon_, "resultLogo/si.obj");
	textModel_[7]->Initialize(dxCommon_, "resultLogo/ha.obj");

	for (int i = 0; i < 4; i++) {
		textModel_[i]->SetColor({0.706f, 1.000f, 0.471f, 1.000f});
	}

	for (int i = 4; i < 8; i++) {
		textModel_[i]->SetColor({ 1.000f, 0.482f, 0.953f, 1.000f });
	}

	for (int i = 0; i < textModel_.size(); ++i) {
		textModel_[i]->SetTexture("resources/image/0.png");
		textModel_[i]->SetUpdateFrustumCulling(false);
	}

	for (int i = 0; i < textTransform_.size(); i++) {
		textTransform_[i].scale = { 0.5f,0.5f,0.5f };
		textTransform_[i].rotate = { 0.0f,0.0f,0.0f };
	}
	textTransform_[0].translate = { -6.97f,4.5f,0.0f };
	textTransform_[1].translate = { -6.36f,4.5f,0.0f };
	textTransform_[2].translate = { -5.72f,4.5f,0.0f };
	textTransform_[3].translate = { -5.1f,4.5f,0.0f };
	textTransform_[4].translate = { -4.5f,4.5f,0.0f };
	textTransform_[5].translate = { -3.91f,4.5f,0.0f };
	textTransform_[6].translate = { -3.22f,4.5f,0.0f };
	textTransform_[7].translate = { -2.72f,4.5f,0.0f };

	textEasingTimer_[0].Start(1.2f, false);
	textEasingTimer_[1].Start(1.4f, false);
	textEasingTimer_[2].Start(1.6f, false);
	textEasingTimer_[3].Start(1.8f, false);
	textEasingTimer_[4].Start(2.0f, false);
	textEasingTimer_[5].Start(2.2f, false);
	textEasingTimer_[6].Start(2.4f, false);
	textEasingTimer_[7].Start(2.6f, false);
}

void Score::InitScoreModel() {
	int scoreInt = static_cast<int>(score_);
	nowScore_ = scoreInt;
	std::array<int, 5> digits = { 0, 0, 0, 0, 0 };

	for (int i = 0; i < 5; ++i) {
		digits[i] = scoreInt % 10;
		scoreInt /= 10;
	}

	for (int i = 0; i < scoreModel_.size(); ++i) {
		scoreModel_[i] = make_unique<Model>();
		std::string modelPath = "number/" + std::to_string(digits[i]) + ".obj";
		scoreModel_[i]->Initialize(dxCommon_, modelPath);
		scoreModel_[i]->SetTexture("resources/image/0.png");
		scoreModel_[i]->SetUpdateFrustumCulling(false);
	}

	for (int i = 0; i < scoreTransform_.size(); i++) {
		scoreTransform_[i].scale = { 1.5f,1.5f,0.5f };
		scoreTransform_[i].rotate = { 0.0f,0.0f,0.0f };
	}
	scoreTransform_[0].translate = { 4.15f,14.75f,0.0f };
	scoreTransform_[1].translate = { 2.71f,14.75f,0.0f };
	scoreTransform_[2].translate = { 1.3f,14.75f,0.0f };
	scoreTransform_[3].translate = { -0.1f,14.75f,0.0f };
	scoreTransform_[4].translate = { -1.5f,14.75f,0.0f };

	//------------------------------------------------------------//

	for (int i = 0; i < 5; ++i) {
		nowScoreModel_[i] = make_unique<Model>();
		std::string modelPath = "number/" + std::to_string(digits[i]) + ".obj";
		nowScoreModel_[i]->Initialize(dxCommon_, modelPath);
		nowScoreModel_[i]->SetTexture("resources/image/0.png");
		nowScoreModel_[i]->SetUpdateFrustumCulling(false);
	}

	nowScoreModel_[5] = make_unique<Model>();
	nowScoreModel_[5]->Initialize(dxCommon_, "player/head/head.obj");
	nowScoreModel_[5]->SetUpdateFrustumCulling(false);

	for (int i = 0; i < 5; i++) {
		nowScoreTransform_[i].scale = { 1.0f,1.0f,0.5f };
		nowScoreTransform_[i].rotate = { 0.0f,0.0f,0.0f };
	}
	nowScoreTransform_[5].scale = { 5.0f,5.0f,2.0f };
	nowScoreTransform_[5].rotate = { 0.0f,3.16f,0.0f };

	nowScoreTransform_[0].translate = { 17.24f,-0.5f,0.0f };
	nowScoreTransform_[1].translate = { 16.34f,-0.5f,0.0f };
	nowScoreTransform_[2].translate = { 15.35f,-0.5f,0.0f };
	nowScoreTransform_[3].translate = { 14.46f,-0.5f,0.0f };
	nowScoreTransform_[4].translate = { 13.5f, -0.5f,0.0f };
	nowScoreTransform_[5].translate = { 11.5f, -0.5f,0.0f };
}

void Score::InitRankModel() {
	rankModel_[0] = make_unique<Model>();
	rankModel_[1] = make_unique<Model>();

	switch (rank_)
	{
	case Score::Rank::S:
		rankModel_[0]->Initialize(dxCommon_, "rank/s.obj");
		rankModel_[0]->SetColor(rankColor_[3]);
		break;
	case Score::Rank::A:
		rankModel_[0]->Initialize(dxCommon_, "rank/a.obj");
		rankModel_[0]->SetColor(rankColor_[2]);
		break;
	case Score::Rank::B:
		rankModel_[0]->Initialize(dxCommon_, "rank/b.obj");
		rankModel_[0]->SetColor(rankColor_[1]);
		break;
	case Score::Rank::C:
		rankModel_[0]->Initialize(dxCommon_, "rank/c.obj");
		rankModel_[0]->SetColor(rankColor_[0]);
		break;
	}
	
	rankModel_[1]->Initialize(dxCommon_, "rank/rank.obj");

	rankModel_[0]->SetTexture("resources/image/0.png");
	rankModel_[1]->SetTexture("resources/image/0.png");

	rankModel_[0]->SetUpdateFrustumCulling(false);
	rankModel_[1]->SetUpdateFrustumCulling(false);

	rankTransform_[0].scale = { 2.5f,3.0f,0.5f };
	rankTransform_[1].scale = { 1.0f,1.0f,0.5f };
	rankTransform_[0].rotate = { 0.0f,-0.55f,0.0f };
	rankTransform_[1].rotate = { 0.0f,-0.55f,0.0f };
	rankTransform_[0].translate = { -15.65f,0.3f,0.0f };
	rankTransform_[1].translate = { -13.18f,-0.74f,1.8f };
}

void Score::InitPlayerModel() {
	playerModel_ = make_unique<Model>();
	playerModel_->Initialize(dxCommon_, "resultLogo/player.obj");
	playerModel_->SetUpdateFrustumCulling(false);
	playerTransform_.translate = { -14.4f,1.01f,1.16f };
	playerTransform_.rotate = { 1.35f,3.36f,0.65f };
	playerTransform_.scale = { 3.5f,3.5f,3.5f };

	machineModel_ = make_unique<Model>();
	machineModel_->Initialize(dxCommon_, "resultLogo/machine.obj");
	machineModel_->SetUpdateFrustumCulling(false);
	machineTransform_.translate = { 13.85f,0.67f,-0.06f };
	machineTransform_.rotate = { 1.5f,3.02f,1.11f };
	machineTransform_.scale = { 3.5f,3.5f,3.5f };

	player2Model_ = make_unique<Model>();
	player2Model_->Initialize(dxCommon_, "resultLogo/player2.obj");
	player2Model_->SetUpdateFrustumCulling(false);
	player2Transform_.translate = { -14.58f ,3.5f ,-0.3f };
	player2Transform_.rotate = { 1.58f ,1.95f ,0.03f };
	player2Transform_.scale = { 3.5f,3.5f,3.5f };

	player2ArmModel_ = make_unique<Model>();
	player2ArmModel_->Initialize(dxCommon_, "resultLogo/player2Arm.obj");
	player2ArmModel_->SetUpdateFrustumCulling(false);
	player2ArmTransform_.translate = { -14.46f ,3.74f ,0.1f };
	player2ArmTransform_.rotate = { 2.41f ,3.78f ,-5.06f };
	player2ArmTransform_.scale = { 3.5f,3.5f,3.5f };

	machine2Model_ = make_unique<Model>();
	machine2Model_->Initialize(dxCommon_, "resultLogo/machine2.obj");
	machine2Model_->SetUpdateFrustumCulling(false);
	machine2Transform_.translate = { -13.8f ,2.39f ,0.0f };
	machine2Transform_.rotate = { 0.0f ,3.1f ,0.0f };
	machine2Transform_.scale = { 3.5f,3.5f,3.5f };
}

void Score::ScoreIn() {
	for (int i = 0; i < textModel_.size(); ++i) {
		textTransform_[i].translate.y = Easing::Lerp(
			textStartY_,
			textEndY_,
			textEasingTimer_[i].GetProgress(),
			Easing::Type::EaseInOutBounce
		);
		textEasingTimer_[i].Update();
		textModel_[i]->SetTransform(textTransform_[i]);
	}

	if (textEasingTimer_[7].IsActive()) {
		scoreEasingTimer_[0].Start(1.0f, false);
		scoreEasingTimer_[1].Start(1.5f, false);
		scoreEasingTimer_[2].Start(2.0f, false);
		scoreEasingTimer_[3].Start(2.5f, false);
		scoreEasingTimer_[4].Start(3.0f, false);
	}

	for (int i = 0; i < scoreModel_.size(); ++i) {
		scoreTransform_[i].translate.y = Easing::Lerp(
			scoreStartY_,
			scoreEndY_,
			scoreEasingTimer_[i].GetProgress(),
			Easing::Type::EaseInOutElastic
		);
		scoreEasingTimer_[i].Update();
		scoreModel_[i]->SetTransform(scoreTransform_[i]);
	}

	if (scoreEasingTimer_[4].IsActive()) {
		rankAndPlayerEasingTimer_.Start(1.0f, false);
	}

	rankTransform_[0].translate.x = Easing::Lerp(
		rankStartX_,
		rankEndX_,
		rankAndPlayerEasingTimer_.GetProgress(),
		Easing::Type::EaseInOutBack
	);

	rankTransform_[1].translate.x = Easing::Lerp(
		rankTextStartX_,
		rankTextEndX_,
		rankAndPlayerEasingTimer_.GetProgress(),
		Easing::Type::EaseInOutBack
	);

	for (int i = 0; i < rankModel_.size(); ++i) {
		rankModel_[i]->SetTransform(rankTransform_[i]);
	}

	playerTransform_.translate.x = Easing::Lerp(
		playerStartX_,
		playerEndX_,
		rankAndPlayerEasingTimer_.GetProgress(),
		Easing::Type::EaseInOutBack
	);
	playerModel_->SetTransform(playerTransform_);

	machineTransform_.translate.x = Easing::Lerp(
		machineStartX_,
		machineEndX_,
		rankAndPlayerEasingTimer_.GetProgress(),
		Easing::Type::EaseInOutBack
	);
	machineModel_->SetTransform(machineTransform_);

	rankAndPlayerEasingTimer_.Update();

	if (rankAndPlayerEasingTimer_.IsFinished()) {
		if (input_->TriggerKey(DIK_SPACE) || gamePad_->TriggerButton(GamePad::A)) {
			screenType_ = ScreenType::RANKING;
			scoreOutTimer_.Start(1.0f, false);
			
			kazeParticle_->Play();

			for (int i = 0; i < rankingInTimer_.size(); i++) {
				rankingInTimer_[i].Start(2.2f - i * 0.2f, false);
			}
		}
	}
}

void Score::ScoreOut() {
	if (!scoreOutTimer_.IsFinished()) {
		for (int i = 0; i < textModel_.size(); ++i) {
			textTransform_[i].translate.y = Easing::Lerp(
				textEndY_,
				textStartY_,
				scoreOutTimer_.GetProgress(),
				Easing::Type::EaseInOutBounce
			);
			textEasingTimer_[i].Update();
			textModel_[i]->SetTransform(textTransform_[i]);
		}

		for (int i = 0; i < scoreModel_.size(); ++i) {
			scoreTransform_[i].translate.y = Easing::Lerp(
				scoreEndY_,
				scoreStartY_,
				scoreOutTimer_.GetProgress(),
				Easing::Type::EaseInOutElastic
			);
			scoreEasingTimer_[i].Update();
			scoreModel_[i]->SetTransform(scoreTransform_[i]);
		}

		rankTransform_[0].translate.x = Easing::Lerp(
			rankEndX_,
			rankStartX_,
			scoreOutTimer_.GetProgress(),
			Easing::Type::EaseInOutBack
		);

		rankTransform_[1].translate.x = Easing::Lerp(
			rankTextEndX_,
			rankTextStartX_,
			scoreOutTimer_.GetProgress(),
			Easing::Type::EaseInOutBack
		);

		for (int i = 0; i < rankModel_.size(); ++i) {
			rankModel_[i]->SetTransform(rankTransform_[i]);
		}

		playerTransform_.translate.x = Easing::Lerp(
			playerEndX_,
			playerStartX_,
			scoreOutTimer_.GetProgress(),
			Easing::Type::EaseInOutBack
		);
		playerModel_->SetTransform(playerTransform_);

		machineTransform_.translate.x = Easing::Lerp(
			machineEndX_,
			machineStartX_,
			scoreOutTimer_.GetProgress(),
			Easing::Type::EaseInOutBack
		);
		machineModel_->SetTransform(machineTransform_);
	}
	scoreOutTimer_.Update();
}

void Score::InitSprite() {
	pushAsusumu_->Initialize(dxCommon_, "resources/image/UI/ContinueAUI.png", { 1155.0f,666.0f }, { 0.3f,0.3f });
	pushAsusumu_->SetColor({ 1.0f,1.0f,1.0f,0.0f });
	titleUI_->Initialize(dxCommon_, "resources/image/UI/BackToTitleUI.png", { 350.0f,500.0f }, { 0.3f,0.3f });
	retryUI_->Initialize(dxCommon_, "resources/image/UI/retryUI.png", { 355.0f,600.0f }, { 0.3f,0.3f });
	cursolUI_->Initialize(dxCommon_, "resources/image/UI/Cursol.png", { 180.0f,500.0f }, { 0.3f,0.3f });
	titleUI_->SetColor({ 1.0f,1.0f,1.0f,0.0f });
	retryUI_->SetColor({ 1.0f,1.0f,1.0f,0.0f });
	cursolUI_->SetColor({ 1.0f,1.0f,1.0f,0.0f });
}

void Score::InitRanking() {
	std::array<int, 5> digits = { 0, 0, 0, 0, 0 };
	int tempScore1st = score1st_; // 一時変数を使用

	for (int i = 0; i < 5; ++i) {
		digits[i] = tempScore1st % 10;
		tempScore1st /= 10;
	}

	for (int i = 0; i < 5; ++i) {
		score1stModel_[i] = make_unique<Model>();
		std::string modelPath = "number/" + std::to_string(digits[i]) + ".obj";
		score1stModel_[i]->Initialize(dxCommon_, modelPath);
		score1stModel_[i]->SetTexture("resources/image/0.png");
		score1stModel_[i]->SetUpdateFrustumCulling(false);
	}

	score1stModel_[5] = make_unique<Model>();
	score1stModel_[5]->Initialize(dxCommon_, "resultLogo/1st.obj");
	score1stModel_[5]->SetTexture("resources/image/0.png");
	score1stModel_[5]->SetUpdateFrustumCulling(false);

	for (int i = 0; i < score1stTransform_.size(); i++) {
		score1stTransform_[i].scale = { 1.0f,1.0f,0.5f };
		score1stTransform_[i].rotate = { 0.0f,0.0f,0.0f };
	}
	score1stTransform_[5].scale = { 1.5f,1.5f,0.5f };

	score1stTransform_[0].translate = { 17.24f,4.85f,0.0f };
	score1stTransform_[1].translate = { 16.34f,4.85f,0.0f };
	score1stTransform_[2].translate = { 15.35f,4.85f,0.0f };
	score1stTransform_[3].translate = { 14.46f,4.85f,0.0f };
	score1stTransform_[4].translate = { 13.5f, 4.85f,0.0f };
	score1stTransform_[5].translate = { 11.5f, 4.85f,0.0f };

	// -------------------------------------------------------------------------//

	std::array<int, 5> digits2 = { 0, 0, 0, 0, 0 };
	int tempScore2nd = score2nd_; // 一時変数を使用

	for (int i = 0; i < 5; ++i) {
		digits2[i] = tempScore2nd % 10;
		tempScore2nd /= 10;
	}

	for (int i = 0; i < 5; ++i) {
		score2ndModel_[i] = make_unique<Model>();
		std::string modelPath = "number/" + std::to_string(digits2[i]) + ".obj";
		score2ndModel_[i]->Initialize(dxCommon_, modelPath);
		score2ndModel_[i]->SetTexture("resources/image/0.png");
		score2ndModel_[i]->SetUpdateFrustumCulling(false);
	}

	score2ndModel_[5] = make_unique<Model>();
	score2ndModel_[5]->Initialize(dxCommon_, "resultLogo/2nd.obj");
	score2ndModel_[5]->SetTexture("resources/image/0.png");
	score2ndModel_[5]->SetUpdateFrustumCulling(false);

	for (int i = 0; i < score2ndTransform_.size(); i++) {
		score2ndTransform_[i].scale = { 1.0f,1.0f,0.5f };
		score2ndTransform_[i].rotate = { 0.0f,0.0f,0.0f };
	}
	score2ndTransform_[5].scale = { 1.5f,1.5f,0.5f };

	score2ndTransform_[0].translate = { 17.24f,3.18f,0.0f };
	score2ndTransform_[1].translate = { 16.34f,3.18f,0.0f };
	score2ndTransform_[2].translate = { 15.35f,3.18f,0.0f };
	score2ndTransform_[3].translate = { 14.46f,3.18f,0.0f };
	score2ndTransform_[4].translate = { 13.5f, 3.18f,0.0f };
	score2ndTransform_[5].translate = { 11.5f, 3.18f,0.0f };

	// -------------------------------------------------------------------------//

	std::array<int, 5> digits3 = { 0, 0, 0, 0, 0 };
	int tempScore3rd = score3rd_; // 一時変数を使用

	for (int i = 0; i < 5; ++i) {
		digits3[i] = tempScore3rd % 10;
		tempScore3rd /= 10;
	}

	for (int i = 0; i < 5; ++i) {
		score3rdModel_[i] = make_unique<Model>();
		std::string modelPath = "number/" + std::to_string(digits3[i]) + ".obj";
		score3rdModel_[i]->Initialize(dxCommon_, modelPath);
		score3rdModel_[i]->SetTexture("resources/image/0.png");
		score3rdModel_[i]->SetUpdateFrustumCulling(false);
	}

	score3rdModel_[5] = make_unique<Model>();
	score3rdModel_[5]->Initialize(dxCommon_, "resultLogo/3rd.obj");
	score3rdModel_[5]->SetTexture("resources/image/0.png");
	score3rdModel_[5]->SetUpdateFrustumCulling(false);

	for (int i = 0; i < score3rdTransform_.size(); i++) {
		score3rdTransform_[i].scale = { 1.0f,1.0f,0.5f };
		score3rdTransform_[i].rotate = { 0.0f,0.0f,0.0f };
	}
	score3rdTransform_[5].scale = { 1.5f,1.5f,0.5f };

	score3rdTransform_[0].translate = { 17.24f,1.55f,0.0f };
	score3rdTransform_[1].translate = { 16.34f,1.55f,0.0f };
	score3rdTransform_[2].translate = { 15.35f,1.55f,0.0f };
	score3rdTransform_[3].translate = { 14.46f,1.55f,0.0f };
	score3rdTransform_[4].translate = { 13.5f, 1.55f,0.0f };
	score3rdTransform_[5].translate = { 11.5f, 1.55f,0.0f };
}

void Score::LoadRankingData() {
	// デフォルトのランキングデータ
	//score1st_ = 30000;
	//score2nd_ = 20000;
	//score3rd_ = 10000;
	
	try {
		std::ifstream file(rankingJsonPath_);
		if (file.is_open()) {
			std::string line;
			while (std::getline(file, line)) {
				// 簡単なJSON解析（score1st, score2nd, score3rdのみ）
				if (line.find("\"score1st\"") != std::string::npos) {
					size_t colonPos = line.find(':');
					if (colonPos != std::string::npos) {
						std::string valueStr = line.substr(colonPos + 1);
						// 数値以外の文字を除去
						valueStr.erase(std::remove_if(valueStr.begin(), valueStr.end(), 
							[](char c) { return !std::isdigit(c) && c != '-'; }), valueStr.end());
						if (!valueStr.empty()) {
							score1st_ = std::stoi(valueStr);
						}
					}
				}
				else if (line.find("\"score2nd\"") != std::string::npos) {
					size_t colonPos = line.find(':');
					if (colonPos != std::string::npos) {
						std::string valueStr = line.substr(colonPos + 1);
						valueStr.erase(std::remove_if(valueStr.begin(), valueStr.end(), 
							[](char c) { return !std::isdigit(c) && c != '-'; }), valueStr.end());
						if (!valueStr.empty()) {
							score2nd_ = std::stoi(valueStr);
						}
					}
				}
				else if (line.find("\"score3rd\"") != std::string::npos) {
					size_t colonPos = line.find(':');
					if (colonPos != std::string::npos) {
						std::string valueStr = line.substr(colonPos + 1);
						valueStr.erase(std::remove_if(valueStr.begin(), valueStr.end(), 
							[](char c) { return !std::isdigit(c) && c != '-'; }), valueStr.end());
						if (!valueStr.empty()) {
							score3rd_ = std::stoi(valueStr);
						}
					}
				}
			}
			file.close();
		}
	} catch (const std::exception&) {
		// ファイルが存在しない場合や読み込みエラーの場合はデフォルト値を使用
	}
}

void Score::SortRankingScores() {
	// 現在のスコアを含めた4つのスコアを配列に格納
	std::array<int, 4> scores = { score1st_, score2nd_, score3rd_, nowScore_ };
	
	// 降順にソート
	std::sort(scores.begin(), scores.end(), std::greater<int>());
	
	// ソート結果を各スコアに代入
	score1st_ = scores[0];
	score2nd_ = scores[1];
	score3rd_ = scores[2];
	// 4位のスコアは保存しない（上位3つのみ保存）
}

void Score::SaveRankingData() {
	try {
		// ディレクトリが存在しない場合は作成
		std::filesystem::path jsonPath(rankingJsonPath_);
		std::filesystem::create_directories(jsonPath.parent_path());
		
		std::ofstream file(rankingJsonPath_);
		if (file.is_open()) {
			file << "{\n";
			file << "    \"score1st\": " << score1st_ << ",\n";
			file << "    \"score2nd\": " << score2nd_ << ",\n";
			file << "    \"score3rd\": " << score3rd_ << "\n";
			file << "}\n";
			file.close();
		}
	} catch (const std::exception&) {
		// エラーログを出力したい場合はここに追加
	}
}