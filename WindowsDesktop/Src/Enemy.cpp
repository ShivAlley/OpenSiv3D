#include "Enemy.h"

#include "Player.h"
#include "GVars.h"
#include "Item.h"

EnemyBase::EnemyBase()
	:m_pos{ Vec3{ RandomClosed(-45, 45), 5, 100 } }
	, m_health{ 100 * Global::escalation }
	, m_maxHealth{ 100 * Global::escalation }
{
	m_normalCollision <<
		OrientedBox{ m_pos, Vec3{ 2,3,2 }};
	m_criticalCollision <<
		OrientedBox{ m_pos + m_headMargin, Vec3{ 1,1,1 } };
}

void EnemyBase::MoveStraight()
{
	Vec3 vecToPlayer = Player::GetInstance().GetPos() - m_pos;
	if (vecToPlayer.lengthSq() > 2.0 * 2.0)
	{
		vecToPlayer.normalize();
		Vec3 moveVec = vecToPlayer * m_speed * Global::deltaTime;
		m_pos += moveVec;
	}
	else
	{
		Attack();
	}
}

void EnemyBase::Attack()
{
	if (m_attackTimer.ms() > 1500)
	{
		Player::GetInstance().ApplyDamaged(m_atk);
		m_attackTimer.restart();

		Vec3 vecToPlayer = Player::GetInstance().GetPos() - m_pos;
		vecToPlayer.normalize();

		Vec3 impulseKnockBackVec = -vecToPlayer * 5.0;
		m_pos += impulseKnockBackVec;
	}
}

void EnemyBase::ApplyDamage(int damage)
{
	if (Player::GetInstance().GetRecordDPSTimer().ms() > 1000)
	{
		Player::GetInstance().GetRecordDPSTimer().restart();
		if (Global::accumlatedDamage > Global::maxDPS)
		{
			Global::maxDPS = Global::accumlatedDamage;
		}
		Global::accumlatedDamage = 0;
	}
	Global::accumlatedDamage += damage;
	m_health -= damage;
	if (m_health <= 0)
	{
		m_state = State::Dead;
		if (RandomBool(Global::CalcDropRate(Player::GetInstance().GetItemCount())))
		{
			Global::RouletteDropItem(m_pos);
		}
	}
}

void EnemyBase::Update()
{
	for (auto& box : m_normalCollision)
	{
		box.setPos(m_pos);
	}
	for (auto& box : m_criticalCollision)
	{
		box.setPos(m_pos + m_headMargin);
	}

	Ray ray{ m_pos,Vec3{0,-1,0} };

	if (!ray.intersects(Global::gameFloor))
	{
		m_pos.y -= 30 * Global::deltaTime;
	}
	if (m_pos.y < -300)
	{
		m_state = State::Dead;
	}
}

void EnemyBase::Draw()
{
	for (const auto& box : m_normalCollision)
	{
		box.drawFrame(m_color);
	}
	for (const auto& box : m_criticalCollision)
	{
		box.draw(Linear::Palette::Red);
	}
}

void StraightMoveEnemy::Update()
{
	EnemyBase::Update();
	MoveStraight();
}

void StraightMoveEnemy::Attack()
{
	EnemyBase::Attack();
}

void SlitheringEnemy::Update()
{
	EnemyBase::Update();

	Vec3 vecToPlayer = Player::GetInstance().GetPos() - m_pos;
	if (vecToPlayer.lengthSq() > 2.0 * 2.0)
	{
		vecToPlayer.normalize();

		static double timeFactor = 0.0;
		timeFactor += Global::deltaTime;

		double slitherOffset = amplitude * sin(frequency * timeFactor);

		Vec3 lateralDir = vecToPlayer.cross(Vec3{ 0, 1, 0 });
		lateralDir.normalize();

		Vec3 movement = (vecToPlayer * m_speed + lateralDir * slitherOffset) * Global::deltaTime;
		m_pos += movement;
	}
	else
	{
		Attack();
	}
}

void SlitheringEnemy::Attack()
{
	EnemyBase::Attack();
}

void DistanceAttackEnemy::Update()
{
	EnemyBase::Update();
	auto dist = Player::GetInstance().GetPos() - m_pos;
	double distance = dist.length();
	if (distance > 50.0)
	{
		MoveStraight();
	}
	else
	{
		if (m_attackTimer.ms() > 5000)
		{
			launchProjectile();
			m_attackTimer.restart();
		}
	}
}

void DistanceAttackEnemy::launchProjectile()
{
	temp_enemies.push_back(std::make_unique<FlyingObject>(
		m_pos
	));
}

void FlyingObject::Update()
{
	EnemyBase::Update();

	std::call_once(m_flag, [&]()
	{
	Vec3 vecToPlayer = Player::GetInstance().GetPos() - m_pos;
	m_targetVec = vecToPlayer.normalized();
	});
	Vec3 moveVec = m_targetVec * m_speed * Global::deltaTime;
	m_pos += moveVec;

	Vec3 vec = Player::GetInstance().GetPos() - m_pos;
	if (vec.lengthSq() < 2.0 * 2.0)
	{
		Attack();
	}

}

void FlyingObject::Attack()
{
	Player::GetInstance().ApplyDamaged(m_atk);
	m_state = State::Dead;
}

void ChargingEnemy::Update()
{
	EnemyBase::Update();
	std::call_once(m_flag, [&]()
	{
	Vec3 vecToPlayer = Player::GetInstance().GetPos() - m_pos;
	m_targetVec = vecToPlayer.normalized();
	});
	Vec3 moveVec = m_targetVec * m_speed * Global::deltaTime;
	m_pos += moveVec;

	Vec3 vec = Player::GetInstance().GetPos() - m_pos;
	if (vec.lengthSq() < 2.0 * 2.0)
	{
		Attack();
	}
}

void ChargingEnemy::Attack()
{
	if (m_attackTimer.ms() > 2000)
	{
		Player::GetInstance().ApplyDamaged(m_atk);
		m_attackTimer.restart();
	}
}

