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
protected:
	float damage_ = 10.0f;
	int bounceCount_ = 0;
	int penetrationCount_ = 0;
	Vector3 directionToEnemy_ = {};
};