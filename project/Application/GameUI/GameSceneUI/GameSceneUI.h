#pragma once
#include "GameUI/BaseUI.h"
#include "EngineSystem.h"
#include "GameObject/Player/WeaponManager/WeaponStatus.h"

enum class PauseType {
	Back,
	ReStart,
	GoTitle,
};

enum class ResultType {
	GoTitle,
	Restart,
};

class GameSceneUI : public BaseUI {
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void DrawImGui() override;

	void SetNowMoney(int money) { moneyValue_ = money; }
	void SetExpGauge(float currentExp, float maxExp) { currentExpValue_ = currentExp; maxExpValue_ = maxExp; }
	void SetHpGauge(float currentHp, float maxHp) { currentHpValue_ = currentHp; maxHpValue_ = maxHp; }
	void SetNowLv(int lv) { nowLv_ = lv; }
	void SetKillEnemyCount(int count) { killEnemyValue_ = count; }
	void SetChestCost(int cost) { chestCostFont_->SetNumber(cost); }
	void SetPauseType(PauseType type) { pauseType_ = type; }
	void SetIsPaused(bool isPaused) { isPaused_ = isPaused; }
	void SetResultType(ResultType type) { resultType_ = type; }
	
	/// <summary>
	/// ハードモードを設定
	/// </summary>
	void SetHardMode(bool isHardMode) { isHardMode_ = isHardMode; }

	/// <summary>
	/// 武器アイコンを更新する
	/// </summary>
	/// <param name="slotIndex">武器スロット番号（0-3）</param>
	/// <param name="weaponName">武器の種類</param>
	void UpdateWeaponIcon(int slotIndex, WeaponName weaponName);

	void SetPlayTime(float time) { playTime_ = time; }

	/// <summary>
	/// ミニマップ上のプレイヤー位置を設定（ワールド座標から自動変換）
	/// </summary>
	/// <param name="worldPosition">プレイヤーのワールド座標</param>
	void SetPlayerMapPosition(const Vector3& worldPosition) {
		playerWorldPosition_ = worldPosition;
		UpdateMapPlayerPosition();
	}

	/// <summary>
	/// ミニマップ上のプレイヤー位置と向きを設定（ワールド座標から自動変換）
	/// </summary>
	/// <param name="worldPosition">プレイヤーのワールド座標</param>
	/// <param name="rotationY">プレイヤーのY軸回転（ラジアン）</param>
	void SetPlayerMapPositionAndRotation(const Vector3& worldPosition, float rotationY) {
		playerWorldPosition_ = worldPosition;
		playerRotationY_ = rotationY;
		UpdateMapPlayerPosition();
	}

	/// <summary>
	/// ミニマップ上のChestアイコンを更新
	/// </summary>
	/// <param name="chestPositions">Chestのワールド座標リスト</param>
	void UpdateChestIcons(const std::vector<Vector3>& chestPositions);

	/// <summary>
	/// ミニマップ上のJarアイコンを更新
	/// </summary>
	/// <param name="expJarPositions">ExpJarのワールド座標リスト</param>
	/// <param name="moneyJarPositions">MoneyJarのワールド座標リスト</param>
	void UpdateJarIcons(const std::vector<Vector3>& expJarPositions, 
	                    const std::vector<Vector3>& moneyJarPositions);

	/// <summary>
	/// ミニマップ上の全てのオブジェクトアイコンを更新（初期化用）
	/// </summary>
	/// <param name="chestPositions">Chestのワールド座標リスト</param>
	/// <param name="expJarPositions">ExpJarのワールド座標リスト</param>
	/// <param name="moneyJarPositions">MoneyJarのワールド座標リスト</param>
	void InitializeMapObjectIcons(const std::vector<Vector3>& chestPositions,
	                               const std::vector<Vector3>& expJarPositions, 
	                               const std::vector<Vector3>& moneyJarPositions);

	void SetWeaponLv(int slotIndex, int lv) {
		if (slotIndex == 0) {
			weaponLvFont1_->SetNumber(lv);
		}
		if (slotIndex == 1) {
			weaponLvFont2_->SetNumber(lv);
		}
		if (slotIndex == 2) {
			weaponLvFont3_->SetNumber(lv);
		}
		if (slotIndex == 3) {
			weaponLvFont4_->SetNumber(lv);
		}
	}
	
	void SetWeaponEquipped(int slotIndex, bool equipped) {
		if (slotIndex >= 0 && slotIndex < 4) {
			isWeaponEquipped_[slotIndex] = equipped;
		}
	}

	void ResultTimerStart() {
		if (!resultTimer_.IsActive() && !isResultDraw_) {
			resultTimer_.Start(1.0f, false);
			isResultDraw_ = true;
		}
	}
	
	void LastTimerStart() {
		if (!lastTimer_.IsActive() && !isLastDraw_) {
			lastTimer_.Start(1.0f, false);
			isLastDraw_ = true;
		}
	}

	/// <summary>
	/// リザルト画面の武器データを更新
	/// </summary>
	/// <param name="equippedWeapons">装備している武器の配列（最大4つ）</param>
	/// <param name="weaponKillCounts">各武器のキルカウント配列</param>
	/// <param name="totalKills">総キルカウント</param>
	void UpdateResultData(const std::vector<WeaponName>& equippedWeapons, 
	                      const std::vector<int>& weaponKillCounts, 
	                      int totalKills) {
		// 総キルカウントを設定
		resultKillEnemyFont_->SetNumber(totalKills);
		
		// 各スロットの武器アイコンとキルカウントを更新
		for (int i = 0; i < 4; ++i) {
			if (i < static_cast<int>(equippedWeapons.size()) && 
			    equippedWeapons[i] != WeaponName::None) {
				// 武器が装備されている
				resultWeaponEquipped_[i] = true;
				
				// 武器アイコンを設定
				std::string iconPath = GetWeaponIconPath(equippedWeapons[i]);
				switch (i) {
				case 0:
					resultWeaponIcon1_->SetTexture(iconPath);
					resultKillEnemyWeaponFont1_->SetNumber(weaponKillCounts[i]);
					break;
				case 1:
					resultWeaponIcon2_->SetTexture(iconPath);
					resultKillEnemyWeaponFont2_->SetNumber(weaponKillCounts[i]);
					break;
				case 2:
					resultWeaponIcon3_->SetTexture(iconPath);
					resultKillEnemyWeaponFont3_->SetNumber(weaponKillCounts[i]);
					break;
				case 3:
					resultWeaponIcon4_->SetTexture(iconPath);
					resultKillEnemyWeaponFont4_->SetNumber(weaponKillCounts[i]);
					break;
				}
			} else {
				// 武器が装備されていない
				resultWeaponEquipped_[i] = false;
				
				// Noneアイコンを設定
				switch (i) {
				case 0:
					resultWeaponIcon1_->SetTexture("icon/none.png");
					break;
				case 1:
					resultWeaponIcon2_->SetTexture("icon/none.png");
					break;
				case 2:
					resultWeaponIcon3_->SetTexture("icon/none.png");
					break;
				case 3:
					resultWeaponIcon4_->SetTexture("icon/none.png");
					break;
				}
			}
		}
	}

private:

	// minMap
	std::unique_ptr<Sprite> minMap_;

	std::unique_ptr<Sprite> mapPlayer_;
	Vector3 playerWorldPosition_ = { 0.0f, 0.0f, 0.0f };
	float playerRotationY_ = 0.0f;

	// ChestとJarのアイコン用のSprite（複数個表示するため、動的に生成）
	std::vector<std::unique_ptr<Sprite>> mapChestIcons_;
	std::vector<std::unique_ptr<Sprite>> mapExpJarIcons_;
	std::vector<std::unique_ptr<Sprite>> mapMoneyJarIcons_;

	std::string chestIconPath_ = "UI/game/minMap/chest.png";
	std::string expJarIconPath_ = "UI/game/minMap/expJar.png";
	std::string moneyJarIconPath_ = "UI/game/minMap/moneyJar.png";

	// スクリーン座標のミニマップ範囲
	float mapScreenTop_ =    100.0f; // Z+
	float mapScreenBottom_ = 600.0f; // Z-
	float mapScreenLeft_ =   20.0f;  // X+
	float mapScreenRight_ =  520.0f; // X-

	float mapWorldTop_ =   217.5f; // Z+
	float mapWorldBottom_ = -7.5f; // Z-
	float mapWorldLeft_ =  217.5f; // X+
	float mapWorldRight_ =  -7.5f; // X-

	GameTimer resultTimer_;
	std::unique_ptr<Sprite> resultBg_;
	std::unique_ptr<Sprite> resultWeaponIcon1_;
	std::unique_ptr<Sprite> resultWeaponIcon2_;
	std::unique_ptr<Sprite> resultWeaponIcon3_;
	std::unique_ptr<Sprite> resultWeaponIcon4_;
	std::unique_ptr<Sprite> resultRestart_;
	std::unique_ptr<Sprite> resultQuit_;
	std::unique_ptr<Sprite> resultEnemyIcon_;
	std::unique_ptr<BitmapFont> resultKillEnemyFont_;
	std::unique_ptr<BitmapFont> resultKillEnemyWeaponFont1_;
	std::unique_ptr<BitmapFont> resultKillEnemyWeaponFont2_;
	std::unique_ptr<BitmapFont> resultKillEnemyWeaponFont3_;
	std::unique_ptr<BitmapFont> resultKillEnemyWeaponFont4_;
	bool isResultDraw_ = false;
	Vector2 resultMin_ = { 0.15f,0.15f };
	Vector2 resultMax_ = { 0.25f,0.25f };
	ResultType resultType_ = ResultType::GoTitle;
	
	// 各スロットに武器が装備されているかどうか（リザルト画面用）
	bool resultWeaponEquipped_[4] = { false, false, false, false };
	
	// ハードモードフラグ
	bool isHardMode_ = false;

	PauseType pauseType_ = PauseType::Back;
	bool isPaused_ = false;
	std::unique_ptr<Sprite> pauseBg_;
	std::unique_ptr<Sprite> back_;
	std::unique_ptr<Sprite> restart_;
	std::unique_ptr<Sprite> goTitle_;
	Vector2 pauseMin_ = { 0.35f,0.35f };
	Vector2 pauseMax_ = { 0.4f,0.4f };

	std::unique_ptr<BitmapFont> enemyFont_;
	std::unique_ptr<Sprite> enemy_;
	int killEnemyValue_ = 0;


	std::unique_ptr<BitmapFont> moneyFont_;
	std::unique_ptr<Sprite> money_;
	int moneyValue_ = 0;

	std::unique_ptr<BitmapFont> lvFont_;
	std::unique_ptr<Sprite> lv_;
	int nowLv_ = 1;

	std::unique_ptr<Gauge> expGauge_;
	float currentExpValue_ = 0.0f;
	float maxExpValue_ = 100.0f;

	std::unique_ptr<Gauge> hpGauge_;
	std::unique_ptr<BitmapFont> currentHpFont_;
	std::unique_ptr<BitmapFont> maxHpFont_;
	std::unique_ptr<Sprite> hpSlash_;
	float currentHpValue_ = 0.0f;
	float maxHpValue_ = 100.0f;

	std::unique_ptr<Sprite> weaponIcon1_;
	std::unique_ptr<Sprite> weaponIcon2_;
	std::unique_ptr<Sprite> weaponIcon3_;
	std::unique_ptr<Sprite> weaponIcon4_;

	std::unique_ptr<BitmapFont> playTimeFont_;
	float playTime_ = 0.0f;

	std::unique_ptr<Sprite> chestIcon_;
	unique_ptr<BitmapFont> chestCostFont_;

	std::unique_ptr<Sprite> weaponLv1_;
	std::unique_ptr<Sprite> weaponLv2_;
	std::unique_ptr<Sprite> weaponLv3_;
	std::unique_ptr<Sprite> weaponLv4_;
	unique_ptr<BitmapFont> weaponLvFont1_;
	unique_ptr<BitmapFont> weaponLvFont2_;
	unique_ptr<BitmapFont> weaponLvFont3_;
	unique_ptr<BitmapFont> weaponLvFont4_;

	std::unique_ptr<Sprite> startText_;
	GameTimer startTimer_;
	Vector2 startTextMin_ = { -500.0f,360.0f };
	Vector2 startTextMax_ = { 1780.0f,360.0f };

	std::unique_ptr<Sprite> text_;
	std::unique_ptr<Sprite> text2_;
	bool isTextDraw_ = true;

	std::unique_ptr<Sprite> last_;
	GameTimer lastTimer_;
	bool isLastDraw_ = false;

	// 各スロットに武器が装備されているかどうか
	bool isWeaponEquipped_[4] = { false, false, false, false };

	/// <summary>
	/// ワールド座標をミニマップスクリーン座標に変換
	/// </summary>
	void UpdateMapPlayerPosition();

	/// <summary>
	/// ワールド座標をミニマップスクリーン座標に変換（汎用）
	/// </summary>
	Vector2 WorldToMapScreen(const Vector3& worldPos) const;

	/// <summary>
	/// 武器名からアイコンのテクスチャパスを取得
	/// </summary>
	std::string GetWeaponIconPath(WeaponName weaponName) const;
};