#include "TiTleScene.h"
#include "Core/ServiceLocator/ServiceLocator.h"

TitleScene::~TitleScene() {
	CleanupResources();
}

void TitleScene::CleanupResources() {

}

void TitleScene::Initialize() {
	ChangeScene(SCENE::TITLE);

	titleUI_ = make_unique<TitleSceneUI>();
	titleUI_->Initialize();

	player_ = make_unique<SkiningModel>();
	player_->Initialize("player/Animation/Dash.gltf");
	player_->SetTexture();
	playerTransform_.SetAllScale(1.5f);
	playerTransform_.rotate.y = 3.8f;
	playerTransform_.translate = { -0.7f,1.0f,-10.0f };
	player_->SetTransform(playerTransform_);

	block_ = make_unique<Model>();
	block_->Initialize("mapBlock/normalBlock.obj");
	blockTransform_.scale = { 12.0f,1.0f,12.0f };
	block2_ = make_unique<Model>();
	block2_->Initialize("mapBlock/normalBlock.obj");
	block2Transform_.scale = { 12.0f,10.0f,1.0f };
	block2Transform_.translate = { 0.0f,0.0f,12.0f };
	block3_ = make_unique<Model>();
	block3_->Initialize("mapBlock/normalBlock.obj");
	block3Transform_.scale = { 1.0f,11.4f,12.0f };
	block3Transform_.translate = { -12.95f,2.0f,0.0f };

	slope_ = make_unique<Model>();
	slope_->Initialize("mapBlock/slopeBlock.obj");
	slopeTransform_.scale = { 12.26f,6.26f,5.0f };
	slopeTransform_.rotate = { 0.0f,-1.57f,0.0f };
	slopeTransform_.translate = { 12.26f,3.8f,-1.0f };

	tree_ = make_unique<Model>();
	tree_->Initialize("mapObject/tree/tree.obj");
	treeTransform_.scale = { 3.0f,3.0f,3.0f };
	treeTransform_.rotate.y = -0.65f;
	treeTransform_.translate = { -3.0f,1.0f,0.0f };

	auto postEffect = ServiceLocator::GetDXCommon()->GetPostEffectManager();
	postEffect->SetEnabled(false);

	fadeBG_ = make_unique<Sprite>();
	fadeBG_->Initialize("loading.png", { 0.0f,0.0f }, { 1.0f,1.0f });

	// タイトルシーン開始時はフェードアウト（透明にする）
	fadeInTimer_ = GameTimer(0.0f, false);
	fadeOutTimer_ = GameTimer(1.0f, false);
	fadeOutTimer_.Start(1.0f, false);

	bgmVolume_ = static_cast<int>(MyAudio::GetBgmMasterVolume() * 10.0f);
	seVolume_ = static_cast<int>(MyAudio::GetSeMasterVolume() * 10.0f);

	MyAudio::PlayBGM(BGM_List::Title, titleBgmVolume_);
}

void TitleScene::Update() {
	
	switch (selectState_) {
	case TitleSelectState::Play:

		if (MyInput::Trigger(Action::SELECT_DOWN)) {
			MyAudio::PlaySE(SE_List::Select);
			selectState_ = TitleSelectState::Edit;
		}
		
		if (MyInput::Trigger(Action::CONFIRM)) {
			MyAudio::PlaySE(SE_List::Confirm);
			selectState_ = TitleSelectState::PlayerSelect;
			playerName_ = PlayerName::PowerMan;
			weaponName_ = WeaponName::FireBall;
		}

		break;
	case TitleSelectState::Edit:

		if (MyInput::Trigger(Action::SELECT_UP)) {
			MyAudio::PlaySE(SE_List::Select);
			selectState_ = TitleSelectState::Play;
		}
		if (MyInput::Trigger(Action::SELECT_DOWN)) {
			MyAudio::PlaySE(SE_List::Select);
			selectState_ = TitleSelectState::Quit;
		}

		if (MyInput::Trigger(Action::CONFIRM)) {
			MyAudio::PlaySE(SE_List::Confirm);
			selectState_ = TitleSelectState::EditSelect;
			editType_ = EditType::Screen;
		}

		break;

	case TitleSelectState::EditSelect:

		if (MyInput::Trigger(Action::CANCEL)) {
			MyAudio::PlaySE(SE_List::Confirm);
			selectState_ = TitleSelectState::Edit;
		}

		switch (editType_)
		{
		case EditType::Screen:

			if (MyInput::Trigger(Action::CONFIRM) || MyInput::Trigger(Action::SELECT_RIGHT) || MyInput::Trigger(Action::SELECT_LEFT)) {
				MyAudio::PlaySE(SE_List::Confirm);
				auto dxCommon = ServiceLocator::GetDXCommon();
				auto winApp = dxCommon->GetWinApp();
				if (winApp->IsFullscreen()) {
					winApp->ExitBorderlessFullscreen();
					isFullScreen_ = false;
				} else {
					winApp->EnterBorderlessFullscreen();
					isFullScreen_ = true;
				}
			}

			if (MyInput::Trigger(Action::SELECT_DOWN)) {
				MyAudio::PlaySE(SE_List::Select);
				editType_ = EditType::BgmVolume;
			}
			
			break;

		case EditType::BgmVolume:

			if (MyInput::Trigger(Action::SELECT_LEFT)) {
				MyAudio::PlaySE(SE_List::Select);
				bgmVolume_--;
			}

			if (MyInput::Trigger(Action::SELECT_RIGHT)) {
				MyAudio::PlaySE(SE_List::Select);
				bgmVolume_++;
			}

			bgmVolume_ = std::clamp(bgmVolume_, 1, 9);
			MyAudio::SetBgmMasterVolume(static_cast<float>(bgmVolume_) / 10.0f);

			if (MyInput::Trigger(Action::SELECT_UP)) {
				MyAudio::PlaySE(SE_List::Select);
				editType_ = EditType::Screen;
			}

			if (MyInput::Trigger(Action::SELECT_DOWN)) {
				MyAudio::PlaySE(SE_List::Select);
				editType_ = EditType::SeVolume;
			}

			break;

		case EditType::SeVolume:

			if (MyInput::Trigger(Action::SELECT_LEFT)) {
				MyAudio::PlaySE(SE_List::Select);
				seVolume_--;
			}

			if (MyInput::Trigger(Action::SELECT_RIGHT)) {
				MyAudio::PlaySE(SE_List::Select);
				seVolume_++;
			}

			seVolume_ = std::clamp(seVolume_, 1, 9);
			MyAudio::SetSeMasterVolume(static_cast<float>(seVolume_) / 10.0f);

			if (MyInput::Trigger(Action::SELECT_UP)) {
				MyAudio::PlaySE(SE_List::Select);
				editType_ = EditType::BgmVolume;
			}

			break;
		}

		break;

	case TitleSelectState::Quit:

		if (MyInput::Trigger(Action::SELECT_UP)) {
			MyAudio::PlaySE(SE_List::Select);
			selectState_ = TitleSelectState::Edit;
		}
		if (MyInput::Trigger(Action::CONFIRM)) {
			MyAudio::PlaySE(SE_List::Confirm);
			Quit();
		}

		break;

	case TitleSelectState::PlayerSelect:

		if (MyInput::Trigger(Action::CONFIRM)) {
			MyAudio::PlaySE(SE_List::Confirm);
			selectState_ = TitleSelectState::WeaponSelect;
		}

		if (MyInput::Trigger(Action::CANCEL)) {
			MyAudio::PlaySE(SE_List::Confirm);
			selectState_ = TitleSelectState::Play;
		}

		switch (playerName_) {
		case PlayerName::PowerMan:

			if (MyInput::Trigger(Action::SELECT_RIGHT)) {
				MyAudio::PlaySE(SE_List::Select);
				playerName_ = PlayerName::TankMan;
			}

			break;
		case PlayerName::TankMan:

			if (MyInput::Trigger(Action::SELECT_RIGHT)) {
				MyAudio::PlaySE(SE_List::Select);
				playerName_ = PlayerName::JumpMan;
			}
			if (MyInput::Trigger(Action::SELECT_LEFT)) {
				MyAudio::PlaySE(SE_List::Select);
				playerName_ = PlayerName::PowerMan;
			}

			break;
		case PlayerName::JumpMan:

			if (MyInput::Trigger(Action::SELECT_RIGHT)) {
				MyAudio::PlaySE(SE_List::Select);
				playerName_ = PlayerName::SpeedMan;
			}
			if (MyInput::Trigger(Action::SELECT_LEFT)) {
				MyAudio::PlaySE(SE_List::Select);
				playerName_ = PlayerName::TankMan;
			}

			break;
		case PlayerName::SpeedMan:

			if (MyInput::Trigger(Action::SELECT_LEFT)) {
				MyAudio::PlaySE(SE_List::Select);
				playerName_ = PlayerName::JumpMan;
			}

			break;
		default:
			break;
		}

		break;

	case TitleSelectState::WeaponSelect:

		if (MyInput::Trigger(Action::CONFIRM)) {
			MyAudio::PlaySE(SE_List::Confirm);
			selectState_ = TitleSelectState::Confirmed;
		}

		if (MyInput::Trigger(Action::CANCEL)) {
			MyAudio::PlaySE(SE_List::Confirm);
			selectState_ = TitleSelectState::PlayerSelect;
		}

		switch (weaponName_) {
		case WeaponName::FireBall:
			if (MyInput::Trigger(Action::SELECT_RIGHT)) {
				MyAudio::PlaySE(SE_List::Select);
				weaponName_ = WeaponName::Laser;
			}
			if (MyInput::Trigger(Action::SELECT_DOWN)) {
				MyAudio::PlaySE(SE_List::Select);
				weaponName_ = WeaponName::Boomerang;
			}
			break;
		case WeaponName::Laser:
			if (MyInput::Trigger(Action::SELECT_RIGHT)) {
				MyAudio::PlaySE(SE_List::Select);
				weaponName_ = WeaponName::Runa;
			}
			if (MyInput::Trigger(Action::SELECT_LEFT)) {
				MyAudio::PlaySE(SE_List::Select);
				weaponName_ = WeaponName::FireBall;
			}
			if (MyInput::Trigger(Action::SELECT_DOWN)) {
				MyAudio::PlaySE(SE_List::Select);
				weaponName_ = WeaponName::Dice;
			}
			break;
		case WeaponName::Runa:
			if (MyInput::Trigger(Action::SELECT_RIGHT)) {
				MyAudio::PlaySE(SE_List::Select);
				weaponName_ = WeaponName::Axe;
			}
			if (MyInput::Trigger(Action::SELECT_LEFT)) {
				MyAudio::PlaySE(SE_List::Select);
				weaponName_ = WeaponName::Laser;
			}
			if (MyInput::Trigger(Action::SELECT_DOWN)) {
				MyAudio::PlaySE(SE_List::Select);
				weaponName_ = WeaponName::Toxic;
			}
			break;
		case WeaponName::Axe:
			if (MyInput::Trigger(Action::SELECT_LEFT)) {
				MyAudio::PlaySE(SE_List::Select);
				weaponName_ = WeaponName::Runa;
			}
			if (MyInput::Trigger(Action::SELECT_DOWN)) {
				MyAudio::PlaySE(SE_List::Select);
				weaponName_ = WeaponName::Area;
			}
			break;
		case WeaponName::Boomerang:
			if (MyInput::Trigger(Action::SELECT_RIGHT)) {
				MyAudio::PlaySE(SE_List::Select);
				weaponName_ = WeaponName::Dice;
			}
			if (MyInput::Trigger(Action::SELECT_UP)) {
				MyAudio::PlaySE(SE_List::Select);
				weaponName_ = WeaponName::FireBall;
			}
			if (MyInput::Trigger(Action::SELECT_DOWN)) {
				MyAudio::PlaySE(SE_List::Select);
				weaponName_ = WeaponName::Gun;
			}
			break;
		case WeaponName::Dice:
			if (MyInput::Trigger(Action::SELECT_RIGHT)) {
				MyAudio::PlaySE(SE_List::Select);
				weaponName_ = WeaponName::Toxic;
			}
			if (MyInput::Trigger(Action::SELECT_LEFT)) {
				MyAudio::PlaySE(SE_List::Select);
				weaponName_ = WeaponName::Boomerang;
			}
			if (MyInput::Trigger(Action::SELECT_UP)) {
				MyAudio::PlaySE(SE_List::Select);
				weaponName_ = WeaponName::Laser;
			}
			break;
		case WeaponName::Toxic:
			if (MyInput::Trigger(Action::SELECT_RIGHT)) {
				MyAudio::PlaySE(SE_List::Select);
				weaponName_ = WeaponName::Area;
			}
			if (MyInput::Trigger(Action::SELECT_LEFT)) {
				MyAudio::PlaySE(SE_List::Select);
				weaponName_ = WeaponName::Dice;
			}
			if (MyInput::Trigger(Action::SELECT_UP)) {
				MyAudio::PlaySE(SE_List::Select);
				weaponName_ = WeaponName::Runa;
			}
			break;
		case WeaponName::Area:
			if (MyInput::Trigger(Action::SELECT_LEFT)) {
				MyAudio::PlaySE(SE_List::Select);
				weaponName_ = WeaponName::Toxic;
			}
			if (MyInput::Trigger(Action::SELECT_UP)) {
				MyAudio::PlaySE(SE_List::Select);
				weaponName_ = WeaponName::Axe;
			}
			break;
		case WeaponName::Gun:
			if (MyInput::Trigger(Action::SELECT_UP)) {
				MyAudio::PlaySE(SE_List::Select);
				weaponName_ = WeaponName::Boomerang;
			}
			break;
		default:
			break;
		}

		break;

	case TitleSelectState::Confirmed:

		if (!fadeInTimer_.IsActive()) {
			if (MyInput::Trigger(Action::CONFIRM)) {
				MyAudio::PlaySE(SE_List::LockIn);
				fadeInTimer_.Start(1.0f, false);
			}

			if (MyInput::Trigger(Action::CANCEL)) {
				MyAudio::PlaySE(SE_List::Confirm);
				selectState_ = TitleSelectState::WeaponSelect;
			}
		}

		if (fadeInTimer_.IsFinished()) {
			// GameSceneに選択したプレイヤーと武器を渡す
			SetSelectedPlayerName(playerName_);
			SetSelectedWeaponName(weaponName_);
			MyAudio::StopBGM(BGM_List::Title);
			ChangeScene(SCENE::GAME);
		}

		break;
		default:
			break;
	}

	player_->SetColor3(playerColors[static_cast<int>(playerName_)]);
	player_->Update();

	titleUI_->SetSelectState(selectState_);
	titleUI_->SetPlayerName(playerName_);
	titleUI_->SetWeaponName(weaponName_);
    titleUI_->SetEditType(editType_);
	titleUI_->SetVolume(bgmVolume_, seVolume_);
	titleUI_->SetIsFullScreen(isFullScreen_);
	titleUI_->SetRanking(12345, 1024, 199);
	titleUI_->Update();

	// フェードイン（GameSceneへ遷移時、徐々に不透明に）
	if (fadeInTimer_.IsActive()) {
		fadeInTimer_.Update();
		fadeBG_->SetColor({ 1.0f, 1.0f, 1.0f, fadeInTimer_.GetProgress() });
		MyAudio::SetBGMVolume(BGM_List::Title, titleBgmVolume_* fadeInTimer_.GetReverseProgress());
	}

	// フェードアウト（TitleScene開始時、徐々に透明に）
	if (fadeOutTimer_.IsActive() && !fadeInTimer_.IsActive()) {
		fadeOutTimer_.Update();
		fadeBG_->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f - fadeOutTimer_.GetProgress() });
		MyAudio::SetBGMVolume(BGM_List::Title, titleBgmVolume_ * fadeInTimer_.GetProgress());
	}

	fadeBG_->Update();
}

void TitleScene::Draw() {

	block_->Draw(camera_, blockTransform_);
	block2_->Draw(camera_, block2Transform_);
	block3_->Draw(camera_, block3Transform_);
	slope_->Draw(camera_, slopeTransform_);
	tree_->Draw(camera_, treeTransform_);
	player_->Draw(camera_, playerTransform_);
	titleUI_->Draw();
	fadeBG_->Draw();
}

void TitleScene::DrawImGui() {
	
	player_->DrawImGui("TitleScenePlayerModel");
	//blockTransform_.DrawImGui("TitleSceneBlockTransform");
	//block2Transform_.DrawImGui("TitleSceneBlock2Transform");
	//block3Transform_.DrawImGui("a");
	//playerTransform_.DrawImGui("TitleScenePlayerTransform");
	//slope_->DrawImGui("TitleSceneSlopeModel");
	//tree_->DrawImGui("TitleSceneTreeModel");
	fadeBG_->DrawImGui("TitleSceneFadeBG");
	titleUI_->DrawImGui();
}

void TitleScene::PostFrameCleanup() {

}