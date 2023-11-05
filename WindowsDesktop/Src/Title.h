#pragma once

using App = SceneManager<String>;

class Title : public App::Scene
{
public:
	Title(const InitData& init) : IScene(init)
	{
			while (ShowCursor(TRUE) < 0){}
	}
	void update() override
	{
		Rect{ Scene::Size() }.draw(m_bgColor);
		TextureAsset{ U"title" }.draw(Arg::leftCenter(Vec2{0,Scene::Center().y}));
		if (SimpleGUI::ButtonAt(U"Start", Vec2{Scene::Size().x * 0.75,Scene::Size().y * 0.33}))
		{
			changeScene(U"Game");
		}
		if (SimpleGUI::ButtonAt(U"Exit", Vec2{Scene::Size().x * 0.75,Scene::Size().y * 0.66}))
		{
			System::Exit();
		}

	}
private:
	Color m_bgColor = Color{ 212, 206, 182 };
};
