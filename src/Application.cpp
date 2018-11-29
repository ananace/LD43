#include "Application.hpp"
#include "IState.hpp"

#include <SFML/Window/Event.hpp>

#include <algorithm>
#include <chrono>

Application::Application()
{
}

Application::~Application()
{
}

void Application::setState(std::unique_ptr<IState>&& aState)
{
    aState->m_app = this;
    aState->init();
    m_curState = std::move(aState);
}

sf::RenderWindow& Application::getRenderWindow()
{
    return m_window;
}
const sf::RenderWindow& Application::getRenderWindow() const
{
    return m_window;
}

void Application::init(int aArgc, char** aArgv)
{
}

void Application::run()
{
    auto curFrame = std::chrono::high_resolution_clock::now(),
         lastFrame = curFrame;
    auto dt = curFrame - lastFrame;

    sf::Event ev{};
    while (m_window.isOpen())
    {
        lastFrame = curFrame;
        curFrame = std::chrono::high_resolution_clock::now();
        dt = curFrame - lastFrame;

        while (m_window.pollEvent(ev))
        {
            if (m_curState)
                m_curState->event(ev);

            if (ev.type == sf::Event::Closed)
                m_window.close();
        }

        if (m_curState)
            m_curState->update(std::chrono::duration_cast<std::chrono::duration<float>>(dt).count());

        m_window.clear();

        if (m_curState)
            m_curState->draw(m_window, {});

        m_window.display();
    }
}
