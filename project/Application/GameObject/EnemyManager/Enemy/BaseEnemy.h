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

	/// <summary>
	/// ステータスに倍率を適用
	/// プレイヤーレベルに応じた難易度調整に使用
	/// </summary>
	/// <param name="hpMul">HP倍率</param>
	/// <param name="powerMul">攻撃力倍率</param>
	/// <param name="speedMul">移動速度倍率</param>
	void ApplyStatusMultipliers(float hpMul, float powerMul, float speedMul) {
		status_.hp = static_cast<int>(status_.hp * hpMul);
		status_.power = static_cast<int>(status_.power * powerMul);
		status_.moveSpeed = status_.moveSpeed * speedMul;
	}

	/// <summary>
	/// 移動速度を取得
	/// </summary>
	float GetMoveSpeed() const { return status_.moveSpeed; }
	
	/// <summary>
	/// ノックバックを適用
	/// </summary>
	/// <param name="direction">ノックバックの方向（正規化されたベクトル）</param>
	/// <param name="force">ノックバックの強さ（デフォルト: 5.0f）</param>
	void ApplyKnockback(const Vector3& direction, float force = 5.0f) {
		knockbackVelocity_ = direction.Normalized() * force;
		isKnockback_ = true;
	}
	
	/// <summary>
	/// ノックバック中かどうか
	/// </summary>
	bool IsKnockback() const { return isKnockback_; }
	
	/// <summary>
	/// ノックバック速度を取得
	/// </summary>
	Vector3 GetKnockbackVelocity() const { return knockbackVelocity_; }
	
	/// <summary>
	/// ノックバック速度を減衰させる
	/// </summary>
	void UpdateKnockback(float deltaTime) {
		if (isKnockback_) {
			// 減衰係数
			float decayRate = 8.0f;
			knockbackVelocity_ = knockbackVelocity_ * (1.0f - decayRate * deltaTime);
			
			// 速度が十分小さくなったらノックバック終了
			float speed = std::sqrt(knockbackVelocity_.x * knockbackVelocity_.x + 
			                        knockbackVelocity_.z * knockbackVelocity_.z);
			if (speed < 0.1f) {
				isKnockback_ = false;
				knockbackVelocity_ = { 0.0f, 0.0f, 0.0f };
			}
		}
	}
	
protected:
	EnemyStatus status_;
	GameTimer invicibilityTimer_;
	std::function<void()> onDeathCallback_;
	
	// ノックバック関連
	Vector3 knockbackVelocity_ = { 0.0f, 0.0f, 0.0f };
	bool isKnockback_ = false;
};