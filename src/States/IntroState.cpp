#include "../Application.hpp"
#include "IntroState.hpp"
#include "MenuState.hpp"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Window/Event.hpp>

using States::IntroState;

IntroState::IntroState()
    : m_introTimer(0)
{
}

void IntroState::init()
{
    auto& resman = getApp().getResourceManager();

    // TODO: Probably should do resource registration some other place
    if (!resman.registered("sprite/intro"))
        resman.registerType<sf::Texture>("sprite/intro", "res/intro.png");

    m_introTexture = resman.load<sf::Texture>("sprite/intro");
}
void IntroState::event(const sf::Event& aEv)
{
    if (aEv.type == aEv.KeyPressed && aEv.key.code == sf::Keyboard::Escape)
        m_introTimer = 9001;
}
void IntroState::update(float aDt)
{
    m_introTimer += aDt;

    if (m_introTimer >= 10)
        getApp().setState(std::make_unique<MenuState>());
}
void IntroState::draw(sf::RenderTarget& aRt, sf::RenderStates aStates) const
{
    // Rotate and zoom out to the target view
    aStates.transform
        .rotate(10 - m_introTimer)
        .scale(0.5f, 0.5f);

    aRt.clear(sf::Color::White);

    sf::Sprite introImage;
    // TODO: Better fallback system?
    if (m_introTexture)
        introImage.setTexture(*m_introTexture);
    aRt.draw(introImage, aStates);
}
