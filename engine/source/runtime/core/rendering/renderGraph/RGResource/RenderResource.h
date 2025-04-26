#pragma once
#include <iostream>
#include <utility>
#include <vector>
/**
 *
 * */


namespace Hybird{
    enum class RGResourceType {
        Texture,
        Buffer,
        Texture2D,
        Texture3D,
        TextureCube,
        Sampler,
        StructureBuffer
    };
    class RenderResource {
    public:
        RenderResource(std::string name,RGResourceType type): m_name(std::move(name)),m_type(type),m_version(0),m_imported(false){}
        const std::string& GetName() const {return m_name;}
        uint32_t GetVersion() const { return m_version; }
        void SetVersion(uint32_t version) { m_version = version; }
        bool IsImported() const { return m_imported; }
        void SetImported(bool imported) { m_imported = imported; }
        void AddProducer(uint32_t passIndex) { m_producers.push_back(passIndex); }
        void AddConsumer(uint32_t passIndex) { m_consumers.push_back(passIndex); }
        const std::vector<uint32_t>& GetProducers() const { return m_producers; }
        const std::vector<uint32_t>& GetConsumers() const { return m_consumers; }
        void* GetHandle() const { return m_handle; }
        void SetHandle(void* handle) { m_handle = handle; }
    private:
        std::string m_name;
        RGResourceType m_type;
        uint32_t m_version;
        bool m_imported;
        std::vector<uint32_t> m_producers;
        std::vector<uint32_t> m_consumers;
        void* m_handle = nullptr;
    };

    template<typename HandleType>
    class RenderResourceT: public RenderResource{
    public:
        RenderResourceT(std::string name,RGResourceType type): RenderResource(name,type){}

        HandleType* GetTypeHandle() const {return static_cast<HandleType*>(GetHandle());}
        void SetTypedHandle(HandleType* handle) { SetHandle(static_cast<void*>(handle));}
    };


}



