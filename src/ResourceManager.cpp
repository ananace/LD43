#include "ResourceManager.hpp"

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Audio/Music.hpp>
#include <SFML/Audio/SoundBuffer.hpp>

ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
}

void ResourceManager::registerType(const std::string& aName, ResourceDefinition&& aResource)
{
    LOG(DEBUG) << "ResMgr: Registeringg " << aResource.Name << " as " << aResource.Path << "." << std::endl; \
    m_registeredResources.emplace(aName, std::move(aResource));
}

bool ResourceManager::registered(const std::string& aName) const
{
    return m_registeredResources.count(aName) > 0;
}

#define DEFAULT_LOAD(T) \
template<> \
std::shared_ptr<T> ResourceManager::loadFile<T>(const ResourceDefinition& aDef) \
{ \
    const std::string& aFile = aDef.Path; \
    LOG(DEBUG) << "ResMgr: Loading " << aDef.Name << " (" #T ") from " << aFile << "..." << std::endl; \
    auto ptr = std::shared_ptr<T>(new T(), [&aDef](auto p) { \
        LOG(DEBUG) << "ResMgr: Unloading " << aDef.Name << std::endl; \
        delete p; \
    }); \
    if (ptr->loadFromFile(aFile)) \
        return ptr; \
    LOG(ERROR) << "ResMgr: Failed to load " << aDef.Name << " (" #T ") from " << aFile << "..." << std::endl; \
    return std::shared_ptr<T>(); \
} \

#define MODIFIED_LOAD(T, LOAD) \
template<> \
std::shared_ptr<T> ResourceManager::loadFile<T>(const ResourceDefinition& aDef) \
{ \
    const std::string& aFile = aDef.Path; \
    LOG(DEBUG) << "ResMgr: Loading " << aDef.Name << " (" #T ") from " << aFile << "..." << std::endl; \
    auto ptr = std::shared_ptr<T>(new T(), [&aDef](auto p) { \
        LOG(DEBUG) << "ResMgr: Unloading " << aDef.Name << std::endl; \
        delete p; \
    }); \
    if (ptr-> LOAD) \
        return ptr; \
    LOG(ERROR) << "ResMgr: Failed to load " << aDef.Name << " (" #T ") from " << aFile << "..." << std::endl; \
    return std::shared_ptr<T>(); \
} \


DEFAULT_LOAD(sf::Font)
DEFAULT_LOAD(sf::Image)
MODIFIED_LOAD(sf::Shader, loadFromFile(aFile, sf::Shader::Type(aDef.ExtraData)));
DEFAULT_LOAD(sf::Texture)
MODIFIED_LOAD(sf::Music, openFromFile(aFile))
DEFAULT_LOAD(sf::SoundBuffer)
