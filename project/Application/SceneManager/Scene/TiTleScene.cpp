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
	playerTransform_.translate = { 0.0f,1.0f,-10.0f };
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

}

void TitleScene::Update() {
	
	switch (selectState_) {
	case TitleSelectState::PLAY:

		if (MyInput::Trigger(Action::CELECT_DOWN)) {
			selectState_ = TitleSelectState::EDIT;
		}

		if (MyInput::Trigger(Action::CONFIRM)) {
			selectState_ = TitleSelectState::SELECT;
		}

		break;
	case TitleSelectState::EDIT:

		if (MyInput::Trigger(Action::CELECT_UP)) {
			selectState_ = TitleSelectState::PLAY;
		}

		if (MyInput::Trigger(Action::CELECT_DOWN)) {
			selectState_ = TitleSelectState::QUIT;
		}

		if (MyInput::Trigger(Action::CONFIRM)) {
			// 編集モードは未実装
		}

		break;
	case TitleSelectState::QUIT:

		if (MyInput::Trigger(Action::CELECT_UP)) {
			selectState_ = TitleSelectState::EDIT;
		}

		if (MyInput::Trigger(Action::CONFIRM)) {
			Quit();
		}

		break;

	case TitleSelectState::SELECT:

		if (MyInput::Trigger(Action::CONFIRM)) {
			ChangeScene(SCENE::GAME);
		}

		if (MyInput::Trigger(Action::CANCEL)) {
			selectState_ = TitleSelectState::PLAY;
		}

		break;
		default:
			break;
	}


	player_->Update();

	titleUI_->SetSelectState(selectState_);
	titleUI_->Update();
}

void TitleScene::Draw() {

	block_->Draw(camera_, blockTransform_);
	block2_->Draw(camera_, block2Transform_);
	block3_->Draw(camera_, block3Transform_);
	slope_->Draw(camera_, slopeTransform_);
	tree_->Draw(camera_, treeTransform_);
	player_->Draw(camera_, playerTransform_);
	titleUI_->Draw();
}

void TitleScene::DrawImGui() {
	
	player_->DrawImGui("TitleScenePlayerModel");
	//blockTransform_.DrawImGui("TitleSceneBlockTransform");
	//block2Transform_.DrawImGui("TitleSceneBlock2Transform");
	//block3Transform_.DrawImGui("a");
	//playerTransform_.DrawImGui("TitleScenePlayerTransform");
	//slope_->DrawImGui("TitleSceneSlopeModel");
	//tree_->DrawImGui("TitleSceneTreeModel");

	titleUI_->DrawImGui();
}

void TitleScene::PostFrameCleanup() {

}