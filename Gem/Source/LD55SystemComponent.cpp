
#include <AzCore/Serialization/SerializeContext.h>

#include "LD55SystemComponent.h"

#include "EnemyAIPlayerLocatorComponent.h"

#include <LD55/LD55TypeIds.h>

#include <AtomLyIntegration/AtomViewportDisplayInfo/AtomViewportInfoDisplayBus.h>

namespace LD55
{
    AZ_COMPONENT_IMPL(LD55SystemComponent, "LD55SystemComponent",
        LD55SystemComponentTypeId);

    void LD55SystemComponent::Reflect(AZ::ReflectContext* context)
    {
    	ReflectAILocatorStrategies(context);
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<LD55SystemComponent, AZ::Component>()
                ->Version(0)
                ;
        }
    }

    void LD55SystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("LD55Service"));
    }

    void LD55SystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC_CE("LD55Service"));
    }

    void LD55SystemComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
    }

    void LD55SystemComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
    }

    LD55SystemComponent::LD55SystemComponent()
    {
        if (LD55Interface::Get() == nullptr)
        {
            LD55Interface::Register(this);
        }
    }

    LD55SystemComponent::~LD55SystemComponent()
    {
        if (LD55Interface::Get() == this)
        {
            LD55Interface::Unregister(this);
        }
    }

    void LD55SystemComponent::Init()
    {
    }

    void LD55SystemComponent::Activate()
    {
        LD55RequestBus::Handler::BusConnect();

        //disable atom debug display
        AZ::AtomBridge::AtomViewportInfoDisplayRequestBus::Broadcast(
            &AZ::AtomBridge::AtomViewportInfoDisplayRequestBus::Events::SetDisplayState,
            AZ::AtomBridge::ViewportInfoDisplayState::NoInfo
        );
    }

    void LD55SystemComponent::Deactivate()
    {
        LD55RequestBus::Handler::BusDisconnect();
    }
}
