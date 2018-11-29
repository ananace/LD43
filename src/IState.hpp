#pragma once

#include <SFML/Graphics/Drawable.hpp>

#include <memory>

namespace sf { class Event; }

class Application;
class IState : public sf::Drawable
{
public:
    virtual void init();
    virtual void event(const sf::Event& aEv);
    virtual void update(float aDt);
    virtual void draw(sf::RenderTarget& aRt, sf::RenderStates aStates) const override;

protected:
    Application& getApp() { return *m_app; }
    const Application& getApp() const { return *m_app; }

private:
    Application* m_app;

    friend class Application;
};
