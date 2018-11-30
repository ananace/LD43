#pragma once

#include "../IState.hpp"

#include <SFML/Graphics/Texture.hpp>

namespace States
{

class IntroState : public IState
{
public:
    IntroState();

    void init() override;
    void event(const sf::Event& aEv) override;
    void update(float aDt) override;
    void draw(sf::RenderTarget& aRt, sf::RenderStates aStates) const override;

private:
    float m_introTimer;
    std::shared_ptr<sf::Texture> m_introTexture;
};

}
