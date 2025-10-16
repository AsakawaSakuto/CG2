#include"IScene.h"

int IScene::sceneNo = RESULT;

int IScene::GetSceneNo() { return sceneNo; }

void IScene::DrawSceneName() {
	ImGui::SetNextWindowPos(ImVec2(0, 0));     // 左上に固定
	ImGui::SetNextWindowSize(ImVec2(100, 50)); // 任意のサイズ
	ImGui::SetNextWindowBgAlpha(0.5f);         // 背景透明度（0.0f〜1.0f）
	ImGui::Begin("SceneName", nullptr,
		ImGuiWindowFlags_NoMove |            // 移動不可
		ImGuiWindowFlags_NoResize |          // リサイズ不可
		ImGuiWindowFlags_NoCollapse |        // 折り畳み不可
		ImGuiWindowFlags_NoTitleBar          // タイトルバー非表示（必要なら外す）
	);
	// ここにUIを書いていく
	switch (sceneNo) {
	case TEST:
		ImGui::Text("TestScene");
		break;
	case TITLE:
		ImGui::Text("TitleScene");
		break;
	case GAME:
		ImGui::Text("GameScene");
		break;
	case RESULT:
		ImGui::Text("ResultScene");
		break;
	default:
		break;
	}
	ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
	ImGui::End();
}