#include "EnemyAIComponent.h"

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>

namespace LD55
{
	void EnemyAIComponent::Reflect(AZ::ReflectContext *context)
	{
        if (AZ::SerializeContext* serializeContext = azrtti_cast<AZ::SerializeContext*>(context)) {
            serializeContext->Class<EnemyAIComponent, AZ::Component>()
                ->Version(1)
                ;

            if (AZ::EditContext* editContext = serializeContext->GetEditContext()) {
                editContext->Class<EnemyAIComponent>("EnemyAIComponent", "Implements enemy AI for LD55")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::Category, "ComponentCategory")
                    ->Attribute(AZ::Edit::Attributes::Icon, "Icons/Components/Component_Placeholder.svg")
                    ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("Game"))
				;
            }
        }
	}
    void EnemyAIComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided) {
        provided.push_back(AZ_CRC_CE("EnemyAIComponentService"));
    }

    void EnemyAIComponent::GetIncompatibleServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& incompatible) {
        //--
    }

    void EnemyAIComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required) {
        required.push_back(AZ_CRC_CE("EnemyAIPlayerLocatorComponentService"));
        required.push_back(AZ_CRC_CE("CombatStatusComponentService"));
    }

    void EnemyAIComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent) {
        //--
    }

	void EnemyAIComponent::Init()
	{
		//--
	}

	void EnemyAIComponent::Activate()
	{
		EnemyAIPlayerLocatorNotificationBus::Handler::BusConnect(GetEntityId());
	}

	void EnemyAIComponent::Deactivate()
	{
		EnemyAIPlayerLocatorNotificationBus::Handler::BusDisconnect(GetEntityId());
	}

	void EnemyAIComponent::OnFoundPlayer([[maybe_unused]] const AZ::EntityId &player, [[maybe_unused]] const AZ::Vector3 &fromPos, [[maybe_unused]] const AZ::Vector3 &playerPos)
	{
		AZ_TracePrintf("EnemyAI", "OnFoundPlayer!!");
	}

	void EnemyAIComponent::OnLostPlayer([[maybe_unused]] const AZ::EntityId &player, [[maybe_unused]] const AZ::Vector3 &fromPos, [[maybe_unused]] const AZ::Vector3 &lastPlayerPos)
	{
		AZ_TracePrintf("EnemyAI", "OnLostPlayer!!");
	}


}