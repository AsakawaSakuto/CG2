#pragma once
#include "EngineSystem.h"
#include "EnemyStatus.h"
#include <functional>

class BaseEnemy
{
public:
	void Damage(int damage) { 
		if (!invicibilityTimer_.IsActive()) {
			invicibilityTimer_.Start(0.5f, false);
			status_.hp -= damage;
			
			int r = MyRand::Int(1, 5);
			if (r == 1) {
				MyAudio::PlaySE(SE_List::EnemyDamage1);
			} else if (r == 2) {
				MyAudio::PlaySE(SE_List::EnemyDamage2);
			} else if (r == 3) {
				MyAudio::PlaySE(SE_List::EnemyDamage3);
			} else if (r == 4) {
				MyAudio::PlaySE(SE_List::EnemyDamage4);
			} else if (r == 5) {
				MyAudio::PlaySE(SE_List::EnemyDamage5);
			}

			// HPが0以下になったら死亡コールバックを呼ぶ
			if (status_.hp <= 0 && onDeathCallback_) {
				onDeathCallback_();
			}
		}
	}

	int GetPower() { return status_.power; }
	
	// HPを取得
	int GetHP() const { return status_.hp; }

	void SetHP(int hp) { status_.hp = hp; }
	
	// 死亡時のコールバックを設定
	void SetOnDeathCallback(std::function<void()> callback) {
		onDeathCallback_ = callback;
	}
	
protected:
	EnemyStatus status_;
	GameTimer invicibilityTimer_;
	std::function<void()> onDeathCallback_;
};