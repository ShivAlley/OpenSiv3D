#pragma once
#include "Enemy.h"
#include "GVars.h"
#include "Player.h"
#include "Siv3D/Mesh.hpp"
#include "Siv3D/MeshData.hpp"

struct Item
{
	Item() = default;
	Item(Vec3 pos)
		:m_pos{ pos }
	{
	}
	virtual ~Item() {}
	virtual void Draw()
	{
		if (m_isPickedUp)
		{
			return;
		}
		m_billBoard
			.draw(Player::GetInstance().GetCamera()
							.billboard(m_pos + Vec3{0, -2, 0}), textureInstance);
	}
	void drawUI(Vec2 pos)
	{
		textureInstance.drawAt(pos);
	}
	const Texture& GetTexture() { return textureInstance; }
	virtual void DrawDescription(Vec2 pos)
	{
		Rect base;
		if (pos.x > Scene::Width() * 0.5)
		{
			base = FontAsset{ U"UIFont" }(m_description).draw(Arg::topRight(pos)).asRect();
		}
		else
		{
			base = FontAsset{ U"UIFont" }(m_description).draw(Arg::topLeft(pos)).asRect();
		}
		base.draw(Palette::Darkblue);
		if (pos.x > Scene::Width() * 0.5)
		{
			FontAsset{ U"UIFont" }(m_description).draw(Arg::topRight(pos));
		}
		else
		{
			FontAsset{ U"UIFont" }(m_description).draw(Arg::topLeft(pos));
		}
	}

	void Tick()
	{
		if (m_isPickedUp)
		{
			return;
		}
		Vec3 vecToPlayer = Player::GetInstance().GetPos() - m_pos;
		if (vecToPlayer.lengthSq() > 1.0)
		{
			vecToPlayer.normalize();
			Vec3 moveVec = vecToPlayer * Global::deltaTime * 10.0;
			m_pos += moveVec;
		}
		else
		{
			OnPickUp();
		}
	}
	virtual void Update() {}
	virtual void OnPickUp()
	{
		m_isPickedUp = true;
		Player::GetInstance().GetItems() << this;
	}
	virtual void OnHitBefore(double enemydist) {}
	virtual void OnHitAfter() {}
	virtual double OnDamaged(double damage) { return damage; }
	//virtual void Draw() = 0;
	Vec3 m_pos{ 0,0,0 };
	Mesh m_billBoard{ MeshData::Billboard() };
	Texture textureInstance;
	bool m_isPickedUp = false;
	bool m_isApplied = false;
	uint64 m_appliedValue = 0;
	String m_description = U"";
};

//tier 0 status up
struct AttackUp : public Item
{
	AttackUp(Vec3 pos) :Item(pos)
	{
		textureInstance = TextureAsset{ U"attack_up" };
		m_description = U"攻撃力を1.2倍する";
	}
	~AttackUp()override {}
	void Update()override {}
	void OnPickUp()override
	{
		Item::OnPickUp();
		Player::GetInstance().Atk() *= 1.2;
	}
};

struct SpeedUp : public Item
{
	SpeedUp(Vec3 pos) :Item(pos)
	{
		textureInstance = TextureAsset{ U"speed_up" };
		m_description = U"移動速度を1.1倍する";
	}
	~SpeedUp()override {}
	void Update()override {}

	void OnPickUp()override
	{
		Item::OnPickUp();
		Player::GetInstance().Speed() *= 1.1;
	}
};

struct FireRateUp : public Item
{
public:
	FireRateUp(Vec3 pos) :Item(pos)
	{
		textureInstance = TextureAsset{ U"firerate_up" };
		m_description = U"攻撃速度を1.1倍する";
	}
	~FireRateUp()override {}
	void OnPickUp()override
	{
		Item::OnPickUp();
		Player::GetInstance().fireRate() *= 0.9;
	}
};
struct HealthUp : public Item
{
	HealthUp(Vec3 pos) :Item(pos)
	{
		textureInstance = TextureAsset{ U"health_up" };
		m_description = U"最大体力を1.1倍する";
	}
	~HealthUp()override {}
	void OnPickUp()override
	{
		Item::OnPickUp();
		Player::GetInstance().MaxHealth() *= 1.1;
		Player::GetInstance().Health() *= 1.1;
	}
};

struct LuckUp : public Item
{
	LuckUp(Vec3 pos) :Item(pos)
	{
		textureInstance = TextureAsset{ U"luck_up" };
		m_description = U"クリティカルヒットの確率を1.1倍する";
	}
	~LuckUp()override {}
	void OnPickUp()override
	{
		Item::OnPickUp();
		Player::GetInstance().LuckyHitChance() *= 1.1;
	}
};

struct RegenUp : public Item
{
	RegenUp(Vec3 pos) :Item(pos)
	{
		textureInstance = TextureAsset{ U"regen_up" };
		m_description = U"体力回復速度を1.1倍する";
	}
	~RegenUp()override {}
	void OnPickUp()override
	{
		Item::OnPickUp();
		Player::GetInstance().RegenRate() *= 1.1;
	}
};
struct PrecisionMultiUp : public Item
{
	PrecisionMultiUp(Vec3 pos) :Item(pos)
	{
		textureInstance = TextureAsset{ U"precision_multi_up" };
		m_description = U"クリティカルヒットの威力を1.1倍する";
	}
	~PrecisionMultiUp()override {}
	void OnPickUp()override
	{
		Item::OnPickUp();
		Player::GetInstance().PrecisionMultiplier() *= 1.1;
	}
};

struct LuckyMultiUp : public Item
{
	LuckyMultiUp(Vec3 pos) :Item(pos)
	{
		textureInstance = TextureAsset{ U"lucky_multi_up" };
		m_description = U"ラッキーヒットの威力を1.1倍する";
	}
	~LuckyMultiUp()override {}
	void OnPickUp()override
	{
		Item::OnPickUp();
		Player::GetInstance().LuckyMultiplier() *= 1.1;
	}
};
//tier 1 trigger effect
struct FullFine : public Item
{
	FullFine(Vec3 pos) :Item(pos)
	{
		textureInstance = TextureAsset{ U"fullfine" };
		m_description = U"体力が満タンの時、攻撃力を1.5倍にする";
	}
	~FullFine()override {}
	void OnHitBefore(double dist) override
	{
		if (Player::GetInstance().Health() == Player::GetInstance().MaxHealth())
		{
			auto before = Player::GetInstance().Atk();
			Player::GetInstance().Atk() *= 1.5;
			auto after = Player::GetInstance().Atk();
			m_appliedValue = after - before;
			m_isApplied = true;
		}
	}
	void OnHitAfter() override
	{
		if (m_isApplied)
		{
			Player::GetInstance().Atk() -= m_appliedValue;
			m_appliedValue = 0;
			m_isApplied = false;
		}
	}
};

struct RockStanding : public Item
{
	RockStanding(Vec3 pos) :Item(pos)
	{
		textureInstance = TextureAsset{ U"rockstanding" };
		m_description = U"移動していない時、攻撃力を1.5倍にする";
	}
	~RockStanding()override {}
	void OnHitBefore(double dist) override
	{
		if (not KeyW.pressed()
			and not KeyA.pressed()
			and not KeyS.pressed()
			and not KeyD.pressed())
		{
			auto before = Player::GetInstance().Atk();
			Player::GetInstance().Atk() *= 1.5;
			auto after = Player::GetInstance().Atk();
			m_appliedValue = after - before;
			m_isApplied = true;
		}
	}
	void OnHitAfter() override
	{
		if (m_isApplied)
		{
			Player::GetInstance().Atk() -= m_appliedValue;
			m_appliedValue = 0;
			m_isApplied = false;
		}
	}
};

struct FocusStone :public Item
{
	FocusStone(Vec3 pos) :Item(pos)
	{
		textureInstance = TextureAsset{ U"focus_crystal" };
		m_description = U"敵との距離が10以下の時、攻撃力を1.5倍にする";
	}
	~FocusStone()override {}
	void OnHitBefore(double dist) override
	{
		if (dist < 10.0)
		{
			auto before = Player::GetInstance().Atk();
			Player::GetInstance().Atk() *= 1.5;
			auto after = Player::GetInstance().Atk();
			m_appliedValue = after - before;
			m_isApplied = true;
		}
	}
	void OnHitAfter() override
	{
		if (m_isApplied)
		{
			Player::GetInstance().Atk() -= m_appliedValue;
			m_appliedValue = 0;
			m_isApplied = false;
		}
	}
};

struct LongShot :public Item
{
	LongShot(Vec3 pos) :Item(pos)
	{
		textureInstance = TextureAsset{ U"long_shot" };
		m_description = U"敵との距離に応じて攻撃力を上昇させる";
	}
	~LongShot()override {}
	void OnHitBefore(double dist) override
	{
		Player::GetInstance().Atk() += dist;
		distValue = dist;
		m_isApplied = true;
	}
	void OnHitAfter() override
	{
		if (m_isApplied)
		{
			Player::GetInstance().Atk() -= distValue;
			m_isApplied = false;
		}
	}
	double distValue;
};

struct Vamp :public Item
{
	Vamp(Vec3 pos) :Item(pos)
	{
		textureInstance = TextureAsset{ U"vamp" };
		m_description = U"敵に攻撃が当たった時、体力を1回復する";
	}
	~Vamp()override {}
	void OnHitBefore(double dist) override
	{
		Player::GetInstance().Health() += 1;
	}
};

struct Wire :public Item
{
	Wire(Vec3 pos) :Item(pos)
	{
		textureInstance = TextureAsset{ U"wire" };
		m_description = U"敵の攻撃を受けた時、周囲の敵に攻撃力の半分のダメージを与える";
	}
	~Wire()override {}

	double OnDamaged(double damage) override
	{
		for (auto& enemy : Global::enemies)
		{
			auto dist = enemy->GetPos().distanceFrom(Player::GetInstance().GetPos());
			if (dist < 10.0)
			{
				enemy->ApplyDamage(Player::GetInstance().Atk());
			}
		}
		{
			return damage;
		}
	}
};

struct Plate :public Item
{
	Plate(Vec3 pos) :Item(pos)
	{
		textureInstance = TextureAsset{ U"plate" };
		m_description = U"敵の攻撃を受けたとき、ダメージを10減らす";
	}
	~Plate()override {}
	double OnDamaged(double damage) override
	{
		damage -= 10;
		return damage;
	}
};
struct voidShield
	:public Item
{
	voidShield(Vec3 pos) :Item(pos)
	{
		textureInstance = TextureAsset{ U"void_shield" };
		m_description = U"敵の攻撃を受けたとき、10%の確率でダメージを1にする";
	}
	~voidShield()override {}
	double OnDamaged(double damage) override
	{
		if (RandomBool(0.1))
		{
			damage = 1;
		}
		return damage;
	}
};

namespace Global
{
	inline Array<std::shared_ptr<Item>> items;

	inline void RouletteDropItem(Vec3 pos)
	{
		//0-1 tier
		int tier = BiasedRandom(CharaParam::itemWeights);
		switch (tier)
		{
		case 0:
			switch (Random(0, 7))
			{
			case 0:
				items << std::make_shared<AttackUp>(pos);
				break;
			case 1:
				items << std::make_shared<SpeedUp>(pos);
				break;
			case 2:
				items << std::make_shared<FireRateUp>(pos);
				break;
			case 3:
				items << std::make_shared<HealthUp>(pos);
				break;
			case 4:
				items << std::make_shared<LuckUp>(pos);
				break;
			case 5:
				items << std::make_shared<RegenUp>(pos);
				break;
			case 6:
				items << std::make_shared<PrecisionMultiUp>(pos);
				break;
			case 7:
				items << std::make_shared<LuckyMultiUp>(pos);
				break;
			}
			break;
		case 1:
			switch (Random(0, 7))
			{
			case 0:
				items << std::make_shared<FullFine>(pos);
				break;
			case 1:
				items << std::make_shared<RockStanding>(pos);
				break;
			case 2:
				items << std::make_shared<FocusStone>(pos);
				break;
			case 3:
				items << std::make_shared<LongShot>(pos);
				break;
			case 4:
				items << std::make_shared<Vamp>(pos);
				break;
			case 5:
				items << std::make_shared<Wire>(pos);
				break;
			case 6:
				items << std::make_shared<Plate>(pos);
				break;
			case 7:
				items << std::make_shared<voidShield>(pos);
				break;
			}
			break;
		}
	}
}
