#include "Application.hpp"
#include "IState.hpp"

#include "States/IntroState.hpp"

#include <SFML/Window/Event.hpp>

#include <SFML/Graphics/Font.hpp>
#ifndef NDEBUG
#include <SFML/Graphics/Text.hpp>
#endif

#if defined(SFML_SYSTEM_LINUX)
#include <fontconfig/fontconfig.h>
#elif defined(SFML_SYSTEM_WINDOWS)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <algorithm>
#include <chrono>

namespace
{

sf::VideoMode s_videoMode { 800, 600 };
int s_windowFlags = sf::Style::Default;

}

bool findSystemFont(std::string* fontPath, const std::string& name, const std::string& style = "Regular");

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

ResourceManager& Application::getResourceManager()
{
    return m_resourceManager;
}
const ResourceManager& Application::getResourceManager() const
{
    return m_resourceManager;
}

void Application::init(int aArgc, char** aArgv)
{
#ifndef NDEBUG
    Logging::SetLevel(Logging::Debug);
#endif

    for  (int i = 1; i < aArgc; ++i)
    {
        std::string arg = aArgv[i];
        if (arg == "-v")
            Logging::SetLevel(Logging::Debug);
        else if (arg == "-f")
        {
            LOG(INFO) << "Running in borderless fullscreen" << std::endl;
            s_videoMode = sf::VideoMode::getDesktopMode();
            s_windowFlags = sf::Style::None;
        }
        else if (arg == "-F")
        {
            LOG(INFO) << "Running in fullscreen" << std::endl;
            s_videoMode = sf::VideoMode::getFullscreenModes().front();
            s_windowFlags = sf::Style::Fullscreen;
        }
    }

    // Find default fonts
    // TODO: Move this to a more reasonable place, instead of in the middle of the application initialization
    static std::array<std::tuple<std::string, std::string>, 10> s_defaultFontList {{
        std::make_tuple("DejaVu Sans", "Book"),
        std::make_tuple("Bitstream Vera Sans", "Roman"),
        std::make_tuple("Liberation Sans", "Regular"),
        std::make_tuple("Arial", "Regular"),
        std::make_tuple("Helvetica", "Regular"),
        std::make_tuple("Verdana", "Regular"),
        std::make_tuple("Tahoma", "Regular"),
        std::make_tuple("*Libertine*", "Regular"),
        std::make_tuple("*Sans*", "Regular"),
        std::make_tuple("*Sans*", "Book")
    }};
    static std::array<std::tuple<std::string, std::string>, 8> s_monoFontList {{
        std::make_tuple("DejaVu Sans Mono", "Book"),
        std::make_tuple("Bitstream Vera Sans Mono", "Roman"),
        std::make_tuple("Liberation Mono", "Regular"),
        std::make_tuple("Inconsolata", "Regular"),
        std::make_tuple("Consolas", "Regular"),
        std::make_tuple("*Mono*", "Regular"),
        std::make_tuple("*Mono*", "Book"),
        std::make_tuple("*", "Mono")
    }};

    LOG(INFO) << "Finding default fonts..." << std::endl;
    for (auto& it : s_defaultFontList) {
        LOG(DEBUG) << "Checking for font matching " << std::get<0>(it) << " " << std::get<1>(it) << "..." << std::endl;
        std::string fontPath;
        if (findSystemFont(&fontPath, std::get<0>(it), std::get<1>(it)))
        {
            m_resourceManager.registerType<sf::Font>("font/default", fontPath);
            break;
        }
    }

    for (auto& it : s_monoFontList) {
        LOG(DEBUG) << "Checking for font matching " << std::get<0>(it) << " " << std::get<1>(it) << "..." << std::endl;
        std::string fontPath;
        if (findSystemFont(&fontPath, std::get<0>(it), std::get<1>(it)))
        {
            m_resourceManager.registerType<sf::Font>("font/mono", fontPath);
            break;
        }
    }

    if (!m_resourceManager.registered("font/default") || !m_resourceManager.registered("font/mono"))
    {
        LOG(ERROR) << "Didn't manage to find a default font, probably going to explode now. Sorry." << std::endl;
        return;
    }

    setState(std::make_unique<States::IntroState>());
}

void Application::run()
{
    m_window.create(s_videoMode, "LD43", s_windowFlags);
    m_window.setVerticalSyncEnabled(true);

    auto curFrame = std::chrono::high_resolution_clock::now(),
         lastFrame = curFrame;
    auto dt = curFrame - lastFrame;

#ifndef NDEBUG
    auto monoFont = m_resourceManager.load<sf::Font>("font/mono");
    sf::Text fpsDisplay("0 FPS (0 ms)", *monoFont, 12);
    auto lastUpdate = curFrame;
#endif

    sf::View defaultView = m_window.getDefaultView();
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

            switch (ev.type)
            {
            case sf::Event::Resized:
                defaultView.setSize(sf::Vector2f(m_window.getSize()));
                defaultView.setCenter(sf::Vector2f(m_window.getSize()) / 2.f);
                break;

            case sf::Event::Closed:
                m_window.close();
                break;

            default: break;
            }
        }

        if (m_curState)
            m_curState->update(std::chrono::duration_cast<std::chrono::duration<float>>(dt).count());

        m_window.clear();

        if (m_curState)
            m_curState->draw(m_window, {});

        // Start every frame with the default view
        m_window.setView(defaultView);

#ifndef NDEBUG
        if (curFrame - lastUpdate > std::chrono::seconds(1))
        {
            auto msDt = std::chrono::duration_cast<std::chrono::microseconds>(dt).count();
            int fps = 1000000 / std::max<int>(msDt, 1);
            fpsDisplay.setString(std::to_string(fps) + "FPS (" + std::to_string(msDt / 1000.f) + " ms)");
            lastUpdate = curFrame;
        }

        m_window.draw(fpsDisplay);
#endif

        m_window.display();
    }
}


#if defined(SFML_SYSTEM_LINUX)
typedef char wildchar;
struct FontConfigData_t
{
    FontConfigData_t()
    {
        config = FcInitLoadConfigAndFonts();
    }

    FcConfig* config;
} FontConfigData;
#elif defined(SFML_SYSTEM_WINDOWS)
typedef wchar_t wildchar;
#endif

bool wildcmp(const wildchar* wildcard, const wildchar* string)
{
    while ((*string) && (*wildcard) != '*')
    {
        if ((*wildcard != *string) && (*wildcard != '?'))
            return false;

        ++wildcard;
        ++string;
    }

    const wildchar *cp = nullptr, *mp = nullptr;

    while (*string)
    {
        if (*wildcard == '*')
        {
            if (!*++wildcard)
                return true;

            mp = wildcard;
            cp = string + 1;
        }
        else if ((*wildcard == *string) || (*wildcard == '?'))
        {
            ++wildcard;
            ++string;
        }
        else
        {
            wildcard = mp;
            string = cp++;
        }
    }

    while (*wildcard == '*')
        ++wildcard;

    return !*wildcard;
}

bool findSystemFont(std::string* p_fontPath, const std::string& fontName, const std::string& fontStyle)
{
    auto& fontPath = *p_fontPath;
#if defined(SFML_SYSTEM_LINUX)

    FcPattern* pat = FcPatternCreate();
    FcObjectSet* os = FcObjectSetBuild(FC_FAMILY, FC_STYLE, FC_FILE, (char*)nullptr);
    FcFontSet* fs = FcFontList(FontConfigData.config, pat, os);
    bool found = false;

    for (int i = 0; fs && i < fs->nfont; ++i)
    {
        FcPattern* font = fs->fonts[i];
        FcChar8 *file, *style, *family;
        if (FcPatternGetString(font, FC_FAMILY, 0, &family) == FcResultMatch &&
            FcPatternGetString(font, FC_STYLE, 0, &style) == FcResultMatch &&
            FcPatternGetString(font, FC_FILE, 0, &file) == FcResultMatch)
        {
            // Never use Noto fonts (Badly named for wildcard matching) or Comic Sans (ew)
            if (wildcmp("*Noto*", (char*)family) || wildcmp("*Comic*", (char*)family))
                continue;

            std::string tmp((char*)file);

            std::reverse(tmp.begin(), tmp.end());
            tmp = tmp.substr(0, 4);
            if (tmp != "ftt." && tmp != "fto.")
                continue;

            if (wildcmp(fontName.c_str(), (char*)family) &&
                wildcmp(fontStyle.c_str(), (char*)style))
            {
                fontPath = std::string((char*)file);
                found = true;
                break;
            }
        }
    }

    if (fs) FcFontSetDestroy(fs);
    if (os) FcObjectSetDestroy(os);
    if (pat) FcPatternDestroy(pat);

    return found;
#elif defined(SFML_SYSTEM_WINDOWS)
    static const LPWSTR fontRegistryPath = L"Software\\Microsoft\\Windows NT\\CurrentVersion\\Fonts";
    HKEY hKey;
    LONG result;
    std::wstring wsFaceName(fontName.begin(), fontName.end());

    // Open Windows font registry key
    result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, fontRegistryPath, 0, KEY_READ, &hKey);
    if (result != ERROR_SUCCESS) {
        return false;
    }

    DWORD maxValueNameSize, maxValueDataSize;
    result = RegQueryInfoKey(hKey, 0, 0, 0, 0, 0, 0, 0, &maxValueNameSize, &maxValueDataSize, 0, 0);
    if (result != ERROR_SUCCESS) {
        return false;
    }

    DWORD valueIndex = 0;
    LPWSTR valueName = new WCHAR[maxValueNameSize];
    LPBYTE valueData = new BYTE[maxValueDataSize];
    DWORD valueNameSize, valueDataSize, valueType;
    std::wstring wsFontFile;

    // Look for a matching font name
    do {
        wsFontFile.clear();
        valueDataSize = maxValueDataSize;
        valueNameSize = maxValueNameSize;

        result = RegEnumValue(hKey, valueIndex, valueName, &valueNameSize, 0, &valueType, valueData, &valueDataSize);

        valueIndex++;

        if (result != ERROR_SUCCESS || valueType != REG_SZ) {
            continue;
        }

        std::wstring wsValueName(valueName, valueNameSize);

        // Found a match
        if (wildcmp(wsFaceName.c_str(), wsValueName.c_str())) {
          wsFontFile.assign((LPWSTR)valueData, valueDataSize);
          break;
        }
    }
    while (result != ERROR_NO_MORE_ITEMS);

    delete[] valueName;
    delete[] valueData;

    RegCloseKey(hKey);

    if (wsFontFile.empty()) {
        return false;
    }

    // Build full font file path
    WCHAR winDir[MAX_PATH];
    GetWindowsDirectory(winDir, MAX_PATH);

    std::wstringstream ss;
    ss << winDir << "\\Fonts\\" << wsFontFile;
    wsFontFile = ss.str();

    fontPath = std::string(wsFontFile.begin(), wsFontFile.end());
    return true;

#endif
    return false;
}

IState::IState()
    : m_app(nullptr)
{ }
