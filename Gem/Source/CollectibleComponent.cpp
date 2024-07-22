#include "CollectibleComponent.h"

#include <AzCore/Component/TransformBus.h>
#include <AzFramework/Physics/RigidBodyBus.h>
#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzFramework/Physics/PhysicsSystem.h>

namespace LD55
{
	void CollectibleComponent::Reflect(AZ::ReflectContext *context)
	{
        if (AZ::SerializeContext* serializeContext = azrtti_cast<AZ::SerializeContext*>(context)) {
            serializeContext->Class<CollectibleComponent, AZ::Component>()
                ->Version(1)
                ->Field("Respawns", &CollectibleComponent::m_respawns)
                ->Field("Respawn Timer", &CollectibleComponent::m_respawnTimer)
                ->Field("Type", &CollectibleComponent::m_type)
                ->Field("Value", &CollectibleComponent::m_value)
                ;

            if (AZ::EditContext* editContext = serializeContext->GetEditContext()) {
                editContext->Class<CollectibleComponent>("CollectibleComponent", "A spawnable projectile")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::Category, "ComponentCategory")
                    ->Attribute(AZ::Edit::Attributes::Icon, "Icons/Components/Component_Placeholder.svg")
                    ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("Game"))
                    ->DataElement(AZ::Edit::UIHandlers::Default, &CollectibleComponent::m_respawns, "Respawns", "Whether or not this collectible respawns")
                    ->DataElement(AZ::Edit::UIHandlers::Default, &CollectibleComponent::m_respawnTimer, "Respawn Timer", "in seconds")
                    ->DataElement(AZ::Edit::UIHandlers::Default, &CollectibleComponent::m_type, "Type", "the collectible type (0: Health | 1: Ammo)")
                    ->DataElement(AZ::Edit::UIHandlers::Default, &CollectibleComponent::m_value, "Value", "health or ammo gained")
				;
            }
        }
	}

    CollectibleComponent::CollectibleComponent()
        : m_onTriggerEnterHandler([this]([[maybe_unused]] AzPhysics::SimulatedBodyHandle bodyHandle,
            const  AzPhysics::TriggerEvent& triggerEvent)
            {
                OnTriggerEnter(triggerEvent);
            })
        , m_onTriggerExitHandler([this]([[maybe_unused]] AzPhysics::SimulatedBodyHandle bodyHandle,
            const  AzPhysics::TriggerEvent& triggerEvent)
            {
                OnTriggerExit(triggerEvent);
            })
	{
		//--
	}

	void CollectibleComponent::Init()
	{
		//--
	}

	void CollectibleComponent::Activate()
	{
		AZ::TickBus::Handler::BusConnect();
		Physics::RigidBodyNotificationBus::Handler::BusConnect(GetEntityId());

		spawnTransform = GetEntity()->GetTransform()->GetWorldTM();
	}

	void CollectibleComponent::OnTriggerEnter(const AzPhysics::TriggerEvent& triggerEvent)
	{
		on_collected();
		CollectibleNotificationBus::Event(
			triggerEvent.m_otherBody->GetEntityId(),
			&CollectibleNotificationBus::Events::OnCollected,
			m_type,
			m_value,
			GetEntity()->GetTransform()->GetWorldTranslation()
		);
	}

	void CollectibleComponent::Deactivate()
	{
		AZ::TickBus::Handler::BusDisconnect();
		Physics::RigidBodyNotificationBus::Handler::BusDisconnect(GetEntityId());
	}

	void CollectibleComponent::OnTriggerExit([[maybe_unused]] const AzPhysics::TriggerEvent& triggerEvent)
	{
		//--
	}

    void CollectibleComponent::OnPhysicsEnabled(const AZ::EntityId& entityId)
    {
        if (auto* physicsSystem = AZ::Interface<AzPhysics::SystemInterface>::Get())
        {
            AZStd::pair<AzPhysics::SceneHandle, AzPhysics::SimulatedBodyHandle> foundBody =
                physicsSystem->FindAttachedBodyHandleFromEntityId(entityId);
            if (foundBody.first != AzPhysics::InvalidSceneHandle)
            {
                AzPhysics::SimulatedBodyEvents::RegisterOnTriggerEnterHandler(foundBody.first, foundBody.second, m_onTriggerEnterHandler);
                AzPhysics::SimulatedBodyEvents::RegisterOnTriggerExitHandler(foundBody.first, foundBody.second, m_onTriggerExitHandler);
            }
        }
    }
    void CollectibleComponent::OnPhysicsDisabled([[maybe_unused]] const AZ::EntityId& entityId)
    {
        m_onTriggerEnterHandler.Disconnect();
        m_onTriggerExitHandler.Disconnect();
    }

	void CollectibleComponent::respawn()
	{
		GetEntity()->GetTransform()->SetWorldTM(spawnTransform);
	}

	void CollectibleComponent::on_collected()
	{
		GetEntity()->GetTransform()->SetWorldTranslation(AZ::Vector3(100,0,-10000)); //collectible hell
	}

	void CollectibleComponent::OnTick([[maybe_unused]] float deltaTime, [[maybe_unused]] AZ::ScriptTimePoint time)
	{
		//--
	}
}