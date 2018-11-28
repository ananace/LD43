#pragma once

class Application
{
public:
    Application();
    Application(const Application&) = delete;
    Application(Application&&) = default;
    ~Application();

    Application& operator=(const Application&) = delete;
    Application& operator=(Application&&) = default;

    void init(int argc, char** argv);
    void run();

private:

};
