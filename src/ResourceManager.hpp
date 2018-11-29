#pragma once

#include "Logging.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

class ResourceManager
{
public:
    struct ResourceDefinition
    {
        std::string Name;
        std::string Path;
        int ExtraData;
        const std::type_info& Type;
    };

    ResourceManager();
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager(ResourceManager&&) = delete;
    ~ResourceManager();

    ResourceManager& operator=(const ResourceManager&) = delete;
    ResourceManager& operator=(ResourceManager&&) = delete;

    template<typename T>
    std::shared_ptr<T> load(const std::string& aName) {
        if (m_registeredResources.count(aName) == 0)
        {
            LOG(WARN) << "ResMgr: Tried to load unregistered resource " << aName << std::endl;
            return std::shared_ptr<T>();
        }

        const auto& def = m_registeredResources.at(aName);

        if (def.Type != typeid(T))
        {
            LOG(WARN) << "ResMgr: Resource " << aName << " is not of type " << def.Type.name() << std::endl;
            return std::shared_ptr<T>();
        }

        if (m_loadedResources.count(aName) > 0)
        {
            auto& ptr = m_loadedResources.at(aName);
            std::shared_ptr<void*> shared;
            if (!ptr.expired())
                shared = ptr.lock();

            if (shared)
            {
                LOG(DEBUG) << "ResMgr: Returning already loaded resource " << aName << std::endl;
                auto p = reinterpret_cast<typename std::shared_ptr<T>::element_type*>(shared.get());
                return std::shared_ptr<T>(shared, p);
            }
        }

        std::shared_ptr<T> loaded(loadFile<T>(def));

        auto voidp = reinterpret_cast<void**>(loaded.get());
        std::shared_ptr<void*> shared(loaded, voidp);
        m_loadedResources[aName] = shared;

        return loaded;
    }

    template<typename T>
    void registerType(const std::string& aName, const std::string& aPath, int aExtraData = 0) {
        registerType(aName, { aName, aPath, aExtraData, typeid(T) });
    }

    bool registered(const std::string& aName) const;

private:
    void registerType(const std::string& aName, ResourceDefinition&& aResource);

    template<typename T>
    std::shared_ptr<T> loadFile(const ResourceDefinition& aFile);

    std::unordered_map<std::string, ResourceDefinition> m_registeredResources;
    std::unordered_map<std::string, std::weak_ptr<void*>> m_loadedResources;
};
