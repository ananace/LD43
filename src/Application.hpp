#pragma once

#include <SFML/Graphics/RenderWindow.hpp>

#include <memory>

class IState;

class Application
{
public:
    Application();
    Application(const Application&) = delete;
    Application(Application&&) = delete;
    ~Application();

    Application& operator=(const Application&) = delete;
    Application& operator=(Application&&) = delete;

    void setState(std::unique_ptr<IState>&& aState);

    sf::RenderWindow& getRenderWindow();
    const sf::RenderWindow& getRenderWindow() const;

    void init(int aArgc, char** aArgv);
    void run();

private:
    sf::RenderWindow m_window;
    std::unique_ptr<IState> m_curState;
};
