#pragma once

#include "Siv3D.hpp"
#include "GVars.h"
#include <memory>

#include "Player.h"

class EnemyBase;

enum class State
{
	Idle,
	Chasing,
	Attacking,
	Dead
};

class EnemyBase
{
public:
	virtual ~EnemyBase() = default;
	EnemyBase();
	void Spawn();
	void spawn_test()
	{
		m_pos = Vec3{ 50,0,50 };

		
		m_normalCollision <<
			OrientedBox{ Arg::bottomCenter(m_pos),Vec3{2,4,2}};
		m_criticalCollision <<
			OrientedBox{ Arg::bottomCenter(m_pos + m_headMargin),Vec3{1,1,1} };
	}
	virtual void Update();
	void Draw();
	inline static Array<std::unique_ptr<EnemyBase>> temp_enemies{};
	const Array<OrientedBox>& GetNormalCollision() { return m_normalCollision; }
	const Array<OrientedBox>& GetCriticalCollision() { return m_criticalCollision; }
	void SetState(State state) { m_state = state; }
	State GetState() const { return m_state; }
	const Vec3& GetPos() const { return m_pos; }
	void ApplyDamage(int damage);
protected:
	void MoveStraight();
	virtual void Attack();
	double m_speed = 5.0;
	Vec3 m_angle{ 0,0,0 };
	Vec3 m_pos{ 0,0,0 };
	Stopwatch m_attackTimer{ StartImmediately::Yes };

	Array<OrientedBox> m_normalCollision;
	Array<OrientedBox> m_criticalCollision;
	double m_health = 100;
	double m_maxHealth = 100;
	double m_atk = 10;
	Vec3 m_headMargin{ 0,2,0 };
	State m_state = State::Idle;
	Color m_color = Palette::White;

};

class Enemy : public EnemyBase
{
public:
	Enemy() = default;
};

class StraightMoveEnemy : public EnemyBase {
public:
	void Update() override;
	void Attack() override;
};

class SlitheringEnemy : public EnemyBase {
public:
	SlitheringEnemy()
		: EnemyBase()
	{
		m_speed = 5.0;
	}
	void Update() override;
	void Attack() override;
	private:
	double amplitude = 10.0;
	double frequency = 1.0;
};

class DistanceAttackEnemy : public EnemyBase {
public:
	DistanceAttackEnemy(): EnemyBase()
	{
		m_speed = 3.0;
		m_color = Linear::Palette::Yellowgreen;
	}
	void Update() override;
private:
	void launchProjectile();
};

class FlyingObject : public EnemyBase {
public:
	FlyingObject() = default;
	FlyingObject(Vec3 pos)
	{
		m_pos = pos;
		m_pos.y += 5;
		m_maxHealth = 50;
		m_health = 50;
		m_normalCollision.clear();
		m_criticalCollision.clear();
		m_normalCollision <<
			OrientedBox{ m_pos,Vec3{2,2,2} };
		m_color = Linear::Palette::Forestgreen;
		m_speed = 12.0;
	}
	void Update() override;
	void Attack() override;
private:
	Vec3 m_targetVec{ 0,0,0 };
	std::once_flag m_flag;
};


class ChargingEnemy : public EnemyBase {
public:
	ChargingEnemy() : EnemyBase()
	{
		m_speed = 16.0;
		m_atk = 20;
		m_normalCollision.clear();
		m_criticalCollision.clear();
		m_headMargin = Vec3{ 0,0,-2 };

		m_normalCollision <<
			OrientedBox{ m_pos, Vec3{2,2,4} };
		m_criticalCollision <<
			OrientedBox{ m_pos+ m_headMargin, Vec3{2,2,2} };
	}
	void Update() override;
	void Attack() override;
private:
	Stopwatch m_chargeTimer{ StartImmediately::Yes };

	Vec3 m_targetVec{ 0,0,0 };
	std::once_flag m_flag;
};


namespace Global
{
	inline Array<std::shared_ptr<EnemyBase>> enemies;
}
