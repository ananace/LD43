#pragma once

#include <SFML/Graphics/Drawable.hpp>

#include <memory>

namespace sf { class Event; }

class Application;

class IState : public sf::Drawable
{
public:
    IState();

    virtual void init() = 0;
    virtual void event(const sf::Event& aEv) = 0;
    virtual void update(float aDt) = 0;
    virtual void draw(sf::RenderTarget& aRt, sf::RenderStates aStates) const override = 0;

protected:
    Application& getApp() { return *m_app; }
    const Application& getApp() const { return *m_app; }

private:
    Application* m_app;

    friend class Application;
};
