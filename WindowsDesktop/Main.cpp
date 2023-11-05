#include <Siv3D/Windows/Windows.hpp>
#include "Siv3D.hpp"
#include "Src/Game.h"
#include "Src/GVars.h"

#include "Src/Player.h"
#include "Src/Title.h"

void RegisterResource();

void Main()
{
	System::SetTerminationTriggers(UserAction::CloseButtonClicked);
	Cursor::RequestStyle(CursorStyle::Hidden);
	while (ShowCursor(FALSE) >= 0)
		Window::Resize(Global::windowSize);
	Window::Resize({ 1920,1080 });
	const ColorF backgroundColor = ColorF{ 0.4, 0.6, 0.8 }.removeSRGBCurve();
	RegisterResource();
	const MSRenderTexture renderTexture{ Scene::Size(), TextureFormat::R8G8B8A8_Unorm_SRGB, HasDepth::Yes };

	FontAsset::Register(U"TitleFont", 60, Typeface::Heavy);
	FontAsset::Register(U"UIFont", 30, Typeface::Regular);
	FontAsset::Register(U"damageFont", 20, Typeface::Light);
	App manager;
	manager.add<Title>(U"Title");
	manager.add<Game>(U"Game");
	while (System::Update())
	{
		if (not manager.update())
		{
			break;
		}
	}
}

void RegisterResource()
{
	Array<bool> b;
	b << TextureAsset::Register(U"attack_up", U"raw_resource/attack_up.png", TextureDesc::MippedSRGB);
	b << TextureAsset::Register(U"firerate_up", U"raw_resource/firerate_up.png", TextureDesc::MippedSRGB);
	b << TextureAsset::Register(U"focus_crystal", U"raw_resource/focus_crystal.png", TextureDesc::MippedSRGB);
	b << TextureAsset::Register(U"fullfine", U"raw_resource/fullfine.png", TextureDesc::MippedSRGB);
	b << TextureAsset::Register(U"health_up", U"raw_resource/health_up.png", TextureDesc::MippedSRGB);
	b << TextureAsset::Register(U"long_shot", U"raw_resource/long_shot.png", TextureDesc::MippedSRGB);
	b << TextureAsset::Register(U"luck_up", U"raw_resource/luck_up.png", TextureDesc::MippedSRGB);
	b << TextureAsset::Register(U"lucky_multi_up", U"raw_resource/lucky_multi_up.png", TextureDesc::MippedSRGB);
	b << TextureAsset::Register(U"plate", U"raw_resource/plate.png", TextureDesc::MippedSRGB);
	b << TextureAsset::Register(U"precision_multi_up", U"raw_resource/precision_multi_up.png", TextureDesc::MippedSRGB);
	b << TextureAsset::Register(U"regen_up", U"raw_resource/regen_up.png", TextureDesc::MippedSRGB);
	b << TextureAsset::Register(U"rockstanding", U"raw_resource/rockstanding.png", TextureDesc::MippedSRGB);
	b << TextureAsset::Register(U"speed_up", U"raw_resource/speed_up.png", TextureDesc::MippedSRGB);
	b << TextureAsset::Register(U"vamp", U"raw_resource/vamp.png", TextureDesc::MippedSRGB);
	b << TextureAsset::Register(U"void_shield", U"raw_resource/void_shield.png", TextureDesc::MippedSRGB);
	b << TextureAsset::Register(U"wire", U"raw_resource/wire.png", TextureDesc::MippedSRGB);
	b << TextureAsset::Register(U"title", U"raw_resource/title.png", TextureDesc::MippedSRGB);
	b << AudioAsset::Register(U"player_fire", Resource(U"embed_rc/player_fire.mp3"));
	b << AudioAsset::Register(U"player_hit_marker", Resource(U"embed_rc/hit_marker.mp3"));
	b << AudioAsset::Register(U"player_damaged", Resource(U"embed_rc/player_damaged.mp3"));
	auto result = b.all();
	assert(result);
}
