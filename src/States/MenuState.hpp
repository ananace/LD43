#pragma once

#include "../IState.hpp"

namespace States
{

class MenuState : public IState
{
public:
    MenuState();

    void init() override;
    void event(const sf::Event& aEv) override;
    void update(float aDt) override;
    void draw(sf::RenderTarget& aRt, sf::RenderStates aStates) const override;

private:
};

}
