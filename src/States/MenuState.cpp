#include "../Application.hpp"
#include "MenuState.hpp"

#include <SFML/Window/Event.hpp>

using States::MenuState;

MenuState::MenuState()
{
}

void MenuState::init()
{
}
void MenuState::event(const sf::Event& aEv)
{
}
void MenuState::update(float aDt)
{
}
void MenuState::draw(sf::RenderTarget& aRt, sf::RenderStates aStates) const
{
    auto& view = aRt.getView();

    // aRt.clear(sf::Color::White);
}

