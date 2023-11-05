#include "Player.h"
#include <Siv3D/Keyboard.hpp>

#include "Siv3D/Cursor.hpp"
#include "Siv3D/Graphics3D.hpp"
#include "Siv3D/Math.hpp"
#include "Siv3D/Mouse.hpp"
#include "Enemy.h"
#include "Item.h"

inline Vec3 GetDirection(double angle)
{
	const Vec2 dir = Circular{ 1.0, angle };
	return{ dir.x, 0.0, -dir.y };
}

inline Vec3 GetFocusPosition(const Vec3& m_pos, double angle)
{
	return (m_pos + GetDirection(angle));
}

inline Vec3 GetDirection3D(double angle, double angle2)
{
	const Vec2 dir = Circular{ 1.0, angle };
	const Vec2 dir2 = Circular{ 1.0, -angle2 };
	return{ dir.x * dir2.y, dir2.x, -dir.y * dir2.y };
}

inline Vec3 GetFocusPosition3D(const Vec3& m_pos, double angle, double angle2)
{
	return (m_pos + GetDirection3D(angle, angle2));
}

void Player::Fire()
{
	if (m_fireTimer.ms() > m_fireRate)
	{
		Ray ray = m_camera.screenToRay(Scene::Center());
		for (auto& enemy : Global::enemies)
		{
			for (const auto collision : enemy->GetNormalCollision())
			{
				auto enemydist = m_pos.distanceFrom(enemy->GetPos());
				if (ray.intersectsAt(collision))
				{
					double damage = CalcDamage(enemydist);
					if(RandomBool(m_luckyHitChance))
					{
						damage *= m_luckyMultiplier;
					}
					enemy->ApplyDamage(damage);
					AudioAsset(U"player_hit_marker").playOneShot(0.3);

					const Mat4x4& viewMat = m_camera.getView();
					const Mat4x4& projMat = m_camera.getProj();

					DirectX::XMFLOAT3 enemypos = DirectX::XMFLOAT3{};

					if (RandomBool(0.5))
					{
						enemypos.x = enemy->GetPos().x - 1;
					}
					else
					{
						enemypos.x = enemy->GetPos().x + 1;
					}
					enemypos.y = enemy->GetPos().y + 2;
					enemypos.z = enemy->GetPos().z;
					Vec2 enemyHead =
						Global::WorldToScreen(enemypos, viewMat.value, projMat.value, Scene::Size().x, Scene::Size().y);
					m_damageQueue << DamageDescriptor{  damage, enemyHead};
					for (auto item : m_items)
					{
						if (item not_eq nullptr)
						{
							item->OnHitAfter();
						}
						else
						{
							//do nothing
						}
					}
				}
			}
			for (const auto collision : enemy->GetCriticalCollision())
			{
				if (ray.intersectsAt(collision))
				{
					auto enemydist = m_pos.distanceFrom(enemy->GetPos());
					auto damage = CalcDamage(enemydist);
					damage *= m_precisionMultiplier;


					if(RandomBool(m_luckyHitChance))
					{
						damage *= m_luckyMultiplier;
					}
					enemy->ApplyDamage(damage);
					AudioAsset(U"player_hit_marker").playOneShot(0.3);
					const Mat4x4& viewMat = m_camera.getView();
					const Mat4x4& projMat = m_camera.getProj();

					DirectX::XMFLOAT3 enemypos = DirectX::XMFLOAT3{};
					enemypos.x = enemy->GetPos().x;
					enemypos.y = enemy->GetPos().y + 2;
					enemypos.z = enemy->GetPos().z;
					Vec2 enemyHead =
						Global::WorldToScreen(enemypos, viewMat.value, projMat.value, Scene::Size().x, Scene::Size().y);
					m_damageQueue << DamageDescriptor{  damage, enemyHead, Palette::Purple};
					for (auto item : m_items)
					{
						if (item not_eq nullptr)
						{
							item->OnHitAfter();
						}
						else
						{
							//do nothing
						}
					}
				}
			}
		}
		AudioAsset(U"player_fire").playOneShot(0.5);
		m_fireTimer.restart();
	}
}

Player::Player()
{
	m_camera = BasicCamera3D{ Global::windowSize, 100_deg, m_pos,GetFocusPosition3D(m_pos,m_angle.x,m_angle.y) };
}

double Player::CalcDamage(double dist)
{
	for (auto item : m_items)
	{
		if (item not_eq nullptr)
		{
			item->OnHitBefore(dist);
		}
		else
		{
			//do nothing
		}
	}
	return m_atk;
}

void Player::ApplyDamaged(double damage)
{
	for (auto item : m_items)
	{
		if (item not_eq nullptr)
		{
			damage = item->OnDamaged(damage);
		}
		else
		{
			//do nothing
		}
	}
	m_health -= damage;
	AudioAsset(U"player_damaged").playOneShot(0.5);
	if (m_health <= 0)
	{
		Global::isGameOver = true;
	}
}

void Player::Input()
{
	auto oldPos = m_pos;
	if (KeyA.pressed())
	{
		m_pos += (GetDirection(m_angle.x + 90_deg) * m_speed * Global::deltaTime);
	}
	if (KeyD.pressed())
	{
		m_pos += (GetDirection(m_angle.x - 90_deg) * m_speed * Global::deltaTime);
	}

	if (KeyW.pressed())
	{
		m_pos += (-GetDirection(m_angle.x) * m_speed * Global::deltaTime);
	}
	if (KeyS.pressed())
	{
		m_pos += (GetDirection(m_angle.x) * m_speed * Global::deltaTime);
	}

	if (Cursor::ScreenDelta().x != 0)
	{
		m_angle.x += (Cursor::ScreenDelta().x * Global::deltaTime * m_sensitivity);
	}

	if (Cursor::ScreenDelta().y != 0)
	{
		m_angle.y += (Cursor::ScreenDelta().y * Global::deltaTime * m_sensitivity);
		m_angle.y = Clamp(m_angle.y, -89.9_deg, 89.9_deg);
	}

	Cursor::SetPos(Scene::Center());

	if (MouseL.down() or MouseL.pressed())
	{
		Fire();
	}
	
	for (auto& wall : Global::walls)
	{
		auto isintersect = wall.intersects(m_collision);
		if (isintersect)
		{
			m_pos.x = oldPos.x * 0.99;
			m_pos.z = oldPos.z * 0.99;
		}
	}

	// 位置・注目点情報を更新
	m_camera.setView(m_pos, GetFocusPosition(m_pos, m_angle.x));
	m_camera.setView(m_pos, GetFocusPosition3D(m_pos, m_angle.x, m_angle.y));
	Graphics3D::SetCameraTransform(m_camera);
}

void Player::Update()
{
	Ray ray{ m_pos,Vec3{0,-1,0} };
	m_health += m_regenerateRate * Global::deltaTime;
	m_health = Clamp(m_health, 0.0, m_maxHealth);

	m_collision.setPos(m_pos);
	m_pos = m_camera.getEyePosition();
	if (!ray.intersects(Global::gameFloor))
	{
		m_pos.y -= 30 * Global::deltaTime;
	}
	if(m_pos.y < -300)
	{
		m_health = 0;
		Global::isGameOver = true;
	}
	std::_Erase_remove_if(m_damageQueue, [](auto damageDesc)
	{
		return damageDesc.timer.ms() > 1000;
	});
}

void Player::Init()
{
    m_fireTimer.restart();
    m_damageQueue.clear();
	m_angle = { 180_deg,0 };
	m_pos = { 0,5,-16 };
	m_fireRate = rpmToMilliseconds(120);
	m_fireTimer.restart();	   m_regenerateRate = 1;
	 m_regenerateTimer.restart();
	 m_recordDPSTimer.restart() ;

	 m_speed = 10.0;
	 m_health = 100.0;
	 m_maxHealth = 100.0;
	 m_atk = 10.0;
	 m_luckyHitChance = 1.0;
	 m_luckyMultiplier = 3.0;
	 m_precisionMultiplier = 2.0;
	m_collision = { m_pos,Vec3{2,5,2} };
    m_camera = BasicCamera3D{ Global::windowSize, 100_deg, m_pos,GetFocusPosition3D(m_pos,m_angle.x,m_angle.y) };

	m_items.clear();

	m_camera.setView(m_pos, GetFocusPosition(m_pos, m_angle.x));
	m_camera.setView(m_pos, GetFocusPosition3D(m_pos, m_angle.x, m_angle.y));
	Graphics3D::SetCameraTransform(m_camera);

}
