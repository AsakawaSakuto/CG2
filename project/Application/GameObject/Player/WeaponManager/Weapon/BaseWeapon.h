#pragma once
#include "Math/Type/Vector3.h"

class BaseWeapon
{
public:
	void SetDamage(float damage) { damage_ = damage; }
	float GetDamage() const { return damage_; }
	void SetBounceCount(int count) { bounceCount_ = count; }
	int GetBounceCount() const { return bounceCount_; }
	void SetPenetrationCount(int count) { penetrationCount_ = count; }
	int GetPenetrationCount() const { return penetrationCount_; }
	void SetDirectionToEnemy(const Vector3& direction) { directionToEnemy_ = direction.Normalized(); }
	void SetScaleRate(float rate) { scaleRate_ = rate; }
	void SetSpeedRate(float rate) { speedRate_ = rate; }
protected:
	float damage_ = 10.0f;
	int bounceCount_ = 0;
	int penetrationCount_ = 0;
	float scaleRate_ = 1.0f;
	float speedRate_ = 1.0f;
	Vector3 directionToEnemy_ = {};
};