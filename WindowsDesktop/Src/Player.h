#pragma once
#include <Siv3D/Vector2D.hpp>
#include <Siv3D/Vector3D.hpp>
#include <Siv3D/Circular.hpp>

#include "Siv3D/BasicCamera3D.hpp"
#include "Siv3D/OrientedBox.hpp"
#include "Siv3D/Stopwatch.hpp"
#include "Siv3D/TextureAsset.hpp"
//#include "GVars.h"

struct Item;

inline double rpmToMilliseconds(int64 rpm)
{
	return (1.0 / rpm) * 60.0 * 1000.0;
}

class Player
{
public:
	static Player& GetInstance()
	{
		static Player instance;
		return instance;
	}
	void Input();
	void Update();
	void Draw();
	void UIDraw();
	void Init();
	Vec3 GetPos() const { return m_pos; }
	Vec2 GetAngle() const { return m_angle; }
	double GetSpeed() const { return m_speed; }
	int GetItemCount() const { return m_items.size(); }
	const BasicCamera3D& GetCamera() { return m_camera; }
	Stopwatch& GetRecordDPSTimer() { return m_recordDPSTimer; }
	OrientedBox& GetCollision() { return m_collision; }
	double GetHealth() const { return m_health; }
	double GetMaxHealth() const { return m_maxHealth; }

	void Fire();
	double CalcDamage(double dist);
	void ApplyDamaged(double damage);

	//Item用のstatus accessors
	//HACK:Commandパターンなどを適用する
	double& fireRate() { return m_fireRate; }
	double& RegenRate() { return m_regenerateRate; }
	double& Speed() { return m_speed; }
	double& Health() { return m_health; }
	double& MaxHealth() { return m_maxHealth; }
	double& Atk() { return m_atk; }
	double& LuckyHitChance() { return m_luckyHitChance; }
	double& LuckyMultiplier() { return m_luckyMultiplier; }
	double& PrecisionMultiplier() { return m_precisionMultiplier; }
	Array<Item*>& GetItems() { return m_items; }

	void DrawDamageQueue()
	{
		for (auto& damageDesc : m_damageQueue)
		{
			FontAsset(U"damageFont")(U"{:.1f}"_fmt(damageDesc.damage))
			.drawAt(damageDesc.pos - Vec2{0, damageDesc.timer.ms() * 0.2},damageDesc.color);
		}
	}
	double m_sensitivity = 30_deg;
private:
	struct DamageDescriptor
	{
		double damage;
		Vec2 pos;
		ColorF color = Palette::White;
		Stopwatch timer{ StartImmediately::Yes };
	};
	Array<DamageDescriptor> m_damageQueue{};

	Player();
	Player(const Player&);
	Player& operator=(const Player&) = delete;

	Vec2 m_angle{ 180_deg,0 };
	Vec3 m_pos{ 0,5,-16 };
	BasicCamera3D m_camera;
	double m_fireRate = rpmToMilliseconds(120);
	Stopwatch m_fireTimer{ StartImmediately::Yes };
	double m_regenerateRate = 1;
	Stopwatch m_regenerateTimer{ StartImmediately::Yes };
	Stopwatch m_recordDPSTimer{ StartImmediately::Yes };

	double m_speed = 10.0;
	double m_health = 100.0;
	double m_maxHealth = 100.0;
	double m_atk = 10.0;
	double m_luckyHitChance = 1.0;
	double m_luckyMultiplier = 3.0;
	double m_precisionMultiplier = 2.0;
	OrientedBox m_collision{ m_pos,Vec3{2,5,2} };

	Array<Item*> m_items;
};
