#include "Source/PlayerControlComponent.h"

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/RTTI/BehaviorContext.h>

namespace LD55 {
    void PlayerControlComponent::Reflect(AZ::ReflectContext* context) {
        if (AZ::SerializeContext* serializeContext = azrtti_cast<AZ::SerializeContext*>(context)) {
            serializeContext->Class<PlayerControlComponent, AZ::Component>()
                ->Version(1)
                ->Field("FPS Camera", &PlayerControlComponent::m_fpscamEntityId)
                ->Field("Arms", &PlayerControlComponent::m_armsEntityId)
                ->Field("Muzzle Flash Light", &PlayerControlComponent::m_muzzleFlashEntityId)
                ->Field("Move Friction", &PlayerControlComponent::m_moveFriction)
                ->Field("Walk Speed", &PlayerControlComponent::m_walkSpeed)
                ->Field("Run Speed", &PlayerControlComponent::m_runSpeed)
                ->Field("Jump Strenth", &PlayerControlComponent::m_jumpStrength)
                ->Field("Can Run", &PlayerControlComponent::m_canRun)
                ->Field("Can Jump", &PlayerControlComponent::m_canJump)
                ->Field("Air Control", &PlayerControlComponent::m_airControl)
                ->Field("Muzzle Offset", &PlayerControlComponent::muzzle_offset)
                ->Field("Sound Shoot", &PlayerControlComponent::m_audioShoot)
                ->Field("Sound Reload", &PlayerControlComponent::m_audioReload)
                ->Field("Sound Walk", &PlayerControlComponent::m_audioWalk)
                ->Field("Sound Heal", &PlayerControlComponent::m_audioHeal)
                ->Field("Sound Coin", &PlayerControlComponent::m_audioCoin)
                ->Field("Game UI", &PlayerControlComponent::m_gameUi)
                ;

            if (AZ::EditContext* editContext = serializeContext->GetEditContext()) {
                editContext->Class<PlayerControlComponent>("PlayerControlComponent", "Implements FPS camera features")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::Category, "ComponentCategory")
                    ->Attribute(AZ::Edit::Attributes::Icon, "Icons/Components/Component_Placeholder.svg")
                    ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("Game"))
					->DataElement(AZ::Edit::UIHandlers::Default, &PlayerControlComponent::m_fpscamEntityId, "FPS Camera Entity", "entity used as our camera")
					->DataElement(AZ::Edit::UIHandlers::Default, &PlayerControlComponent::m_armsEntityId, "Arms Entity", "entity with animgraph component for arms")
					->DataElement(AZ::Edit::UIHandlers::Default, &PlayerControlComponent::m_muzzleFlashEntityId, "Muzzle Flash Light", "light to activate when shooting")
					->DataElement(AZ::Edit::UIHandlers::Default, &PlayerControlComponent::m_moveFriction, "Move Friction", "player config")
					->DataElement(AZ::Edit::UIHandlers::Default, &PlayerControlComponent::m_walkSpeed, "Walk Speed", "player config")
					->DataElement(AZ::Edit::UIHandlers::Default, &PlayerControlComponent::m_runSpeed, "Run Speed", "player config")
					->DataElement(AZ::Edit::UIHandlers::Default, &PlayerControlComponent::m_jumpStrength, "Jump Strenth", "player config")
					->DataElement(AZ::Edit::UIHandlers::Default, &PlayerControlComponent::m_canRun, "Can Run", "player config")
					->DataElement(AZ::Edit::UIHandlers::Default, &PlayerControlComponent::m_canJump, "Can Jump", "player config")
					->DataElement(AZ::Edit::UIHandlers::Default, &PlayerControlComponent::m_airControl, "Air Control", "player config")
					->DataElement(AZ::Edit::UIHandlers::Default, &PlayerControlComponent::muzzle_offset, "Muzzle Offset", "")
					->DataElement(AZ::Edit::UIHandlers::Default, &PlayerControlComponent::m_audioShoot, "Sound Shoot", "entity with miniaudio component with sound effect")
					->DataElement(AZ::Edit::UIHandlers::Default, &PlayerControlComponent::m_audioReload, "Sound Reload", "entity with miniaudio component with sound effect")
					->DataElement(AZ::Edit::UIHandlers::Default, &PlayerControlComponent::m_audioWalk, "Sound Walk", "entity with miniaudio component with sound effect")
					->DataElement(AZ::Edit::UIHandlers::Default, &PlayerControlComponent::m_audioHeal, "Sound Heal", "entity with miniaudio component with sound effect")
					->DataElement(AZ::Edit::UIHandlers::Default, &PlayerControlComponent::m_audioCoin, "Sound Coin", "entity with miniaudio component with sound effect")
					->DataElement(AZ::Edit::UIHandlers::Default, &PlayerControlComponent::m_gameUi, "Game UI", "game UI entity")
                ;
            }
        }
    }

    void PlayerControlComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided) {
        provided.push_back(AZ_CRC_CE("PlayerControlComponentService"));
    }

    void PlayerControlComponent::GetIncompatibleServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& incompatible) {
        //--
    }

    void PlayerControlComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required) {
        //--
    }

    void PlayerControlComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent) {
        //--
    }
}
