#include"IScene.h"

SCENE IScene::GetSceneNo() { return nowSceneName_; }

void IScene::Quit() {
	PostQuitMessage(0);
}

void IScene::DrawSceneName() {
#ifdef  USE_IMGUI
	ImGui::SetNextWindowPos(ImVec2(0, 0));     // 左上に固定
	ImGui::SetNextWindowSize(ImVec2(150, 75)); // 任意のサイズ
	ImGui::SetNextWindowBgAlpha(0.5f);         // 背景透明度（0.0f〜1.0f）
	ImGui::Begin("SceneName", nullptr,
		ImGuiWindowFlags_NoMove |            // 移動不可
		ImGuiWindowFlags_NoResize |          // リサイズ不可
		ImGuiWindowFlags_NoCollapse |        // 折り畳み不可
		ImGuiWindowFlags_NoTitleBar          // タイトルバー非表示（必要なら外す）
	);
	// ここにUIを書いていく
	switch (nowSceneName_)
	{
	case SCENE::TEST:
		ImGui::Text("TEST SCENE");
		break;
	case SCENE::TITLE:
		ImGui::Text("TITLE SCENE");
		break;
	case SCENE::GAME:
		ImGui::Text("GAME SCENE");
		break;
	case SCENE::RESULT:
		ImGui::Text("RESULT SCENE");
		break;
	default:
		break;
	}
	ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

	if (ImGui::TreeNode("シーンの切り替え")) {

		if (ImGui::Button("TEST")) { ChangeScene(SCENE::TEST); }
		if (ImGui::Button("TITLE")) { ChangeScene(SCENE::TITLE); }
		if (ImGui::Button("GAME")) { ChangeScene(SCENE::GAME); }
		if (ImGui::Button("RESULT")) { ChangeScene(SCENE::RESULT); }
		if (ImGui::Button("QUIT")) { Quit(); }

		ImGui::TreePop();
	}

	ImGui::End();
#endif //  USE_IMGUI
}