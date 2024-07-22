#include "CombatStatusComponent.h"

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/RTTI/BehaviorContext.h>
#include <AzFramework/Entity/GameEntityContextBus.h>
#include <MiniAudio/MiniAudioPlaybackBus.h>
#include <AzFramework/Physics/RigidBodyBus.h>

#include <DebugDraw/DebugDrawBus.h>

namespace LD55
{
	void CombatStatusComponent::Reflect(AZ::ReflectContext *context)
	{
        if (AZ::SerializeContext* serializeContext = azrtti_cast<AZ::SerializeContext*>(context)) {
            serializeContext->Class<CombatStatusComponent, AZ::Component>()
                ->Version(1)
                ->Field("Attack", &CombatStatusComponent::m_attack)
                ->Field("Max Health", &CombatStatusComponent::m_maxHealth)
                ->Field("Health", &CombatStatusComponent::m_health)
                ->Field("Sound Die", &CombatStatusComponent::m_audioDie)
                ->Field("Sound Hurt", &CombatStatusComponent::m_audioHurt)
                ->Field("Sound Heal", &CombatStatusComponent::m_audioHeal)
                ->Field("Destroy On Die", &CombatStatusComponent::m_destroyOnDie)
                ->Field("Immortal", &CombatStatusComponent::m_isImmortal)
                ->Field("Damage Impulse", &CombatStatusComponent::m_dmgImpulse)
                ;

            if (AZ::EditContext* editContext = serializeContext->GetEditContext()) {
                editContext->Class<CombatStatusComponent>("CombatStatusComponent", "tracks an entity's combat stats")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::Category, "ComponentCategory")
                    ->Attribute(AZ::Edit::Attributes::Icon, "Icons/Components/Component_Placeholder.svg")
                    ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("Game"))
                    ->DataElement(AZ::Edit::UIHandlers::Default, &CombatStatusComponent::m_attack, "Attack Strength", "Amount of damage we do when we attack")
                    ->DataElement(AZ::Edit::UIHandlers::Default, &CombatStatusComponent::m_maxHealth, "Max Health", "Maximum amount of health we can have")
                    ->DataElement(AZ::Edit::UIHandlers::Default, &CombatStatusComponent::m_health, "Starting Health", "Amount health we start with")
                    ->DataElement(AZ::Edit::UIHandlers::Default, &CombatStatusComponent::m_audioDie, "Sound Die", "entity with miniaudio component")
                    ->DataElement(AZ::Edit::UIHandlers::Default, &CombatStatusComponent::m_audioHurt, "Sound Hurt", "entity with miniaudio component")
                    ->DataElement(AZ::Edit::UIHandlers::Default, &CombatStatusComponent::m_audioHeal, "Sound Heal", "entity with miniaudio component")
                    ->DataElement(AZ::Edit::UIHandlers::Default, &CombatStatusComponent::m_destroyOnDie, "Destroy On Die", "destroy entity when health reaches zero")
                    ->DataElement(AZ::Edit::UIHandlers::Default, &CombatStatusComponent::m_isImmortal, "Immortal", "if enabled, entity can't be hurt or killed")
                    ->DataElement(AZ::Edit::UIHandlers::Default, &CombatStatusComponent::m_dmgImpulse, "Damage Impulse", "physics impulse multiplier on hit")
				;
            }
        }

		if (AZ::BehaviorContext* behaviorContext = azrtti_cast<AZ::BehaviorContext*>(context))
		{
			behaviorContext->EBus<CombatStatusRequestBus>("CombatStatusRequestBus", "CombatStatusRequestBus")
				->Attribute(AZ::Script::Attributes::Category, "Combat")
				->Event("Hurt", &CombatStatusRequestBus::Events::Hurt)
				->Event("Heal", &CombatStatusRequestBus::Events::Heal)
				->Event("GetHealth", &CombatStatusRequestBus::Events::GetHealth)
				->Event("GetMaxHealth", &CombatStatusRequestBus::Events::GetMaxHealth)
				->Event("GetAttackStrength", &CombatStatusRequestBus::Events::GetAttackStrength)
				->Event("SetAttackStrength", &CombatStatusRequestBus::Events::SetAttackStrength)
			;
			behaviorContext->EBus<CombatStatusNotificationBus>("CombatStatusNotificationBus", "CombatStatusNotificationBus")
				->Attribute(AZ::Script::Attributes::Category, "Combat")
				->Handler<BehaviorCombatStatusNotificationBusHandler>()
			;
		}
	}

    void CombatStatusComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided) {
        provided.push_back(AZ_CRC_CE("CombatStatusComponentService"));
    }

    void CombatStatusComponent::GetIncompatibleServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& incompatible) {
        //--
    }

    void CombatStatusComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required) {
        //--
    }

    void CombatStatusComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent) {
        //--
    }

	//------------------------------------------------------------------------------------------------------------------
	void BehaviorCombatStatusNotificationBusHandler::OnDie()
	{
		Call(FN_OnDie);
	}
	void BehaviorCombatStatusNotificationBusHandler::OnHeal(float amount)
	{
		Call(FN_OnHeal, amount);
	}
	void BehaviorCombatStatusNotificationBusHandler::OnDamaged(float amount, float remaining, const AZ::Vector3& pos, const AZ::Vector3& normal)
	{
		Call(FN_OnDamaged, amount, remaining, pos, normal);
	}
	//------------------------------------------------------------------------------------------------------------------

	void CombatStatusComponent::Init()
	{
		//--
	}

	void CombatStatusComponent::Activate()
	{
		CombatStatusRequestBus::Handler::BusConnect(GetEntityId());
		m_health = m_maxHealth;
	}

	void CombatStatusComponent::Deactivate()
	{
		CombatStatusRequestBus::Handler::BusDisconnect(GetEntityId());
	}

	void CombatStatusComponent::Hurt(float amount, const AZ::Vector3& pos, const AZ::Vector3& normal)
	{
		if(m_isImmortal)
		{
			amount = 0;
		}
		m_health -= amount;

		CombatStatusNotificationBus::Event(
			GetEntityId(),
			&CombatStatusNotifications::OnDamaged,
			amount,
			m_health,
			pos,
			normal
		);

		if(m_health <= 0)
		{
			CombatStatusNotificationBus::Event(
				GetEntityId(),
				&CombatStatusNotifications::OnDie
			);

			if(m_audioDie.IsValid())
			{
				MiniAudio::MiniAudioPlaybackRequestBus::Event(m_audioDie, &MiniAudio::MiniAudioPlaybackRequestBus::Events::Play);
			}

			if(m_destroyOnDie)
			{
				AzFramework::GameEntityContextRequestBus::Broadcast(
					&AzFramework::GameEntityContextRequests::DestroyGameEntity,
					GetEntityId()
				);
			}
		}
		else
		{
			if(m_audioHurt.IsValid())
			{
				MiniAudio::MiniAudioPlaybackRequestBus::Event(m_audioHurt, &MiniAudio::MiniAudioPlaybackRequestBus::Events::Play);
			}

			if(m_dmgImpulse != 0)
			{
				Physics::RigidBodyRequestBus::Event(
					GetEntityId(),
//					&Physics::RigidBodyRequests::ApplyLinearImpulse,
					&Physics::RigidBodyRequests::ApplyLinearImpulseAtWorldPoint,
					-normal * m_dmgImpulse,
					pos
				);

//				DebugDraw::DebugDrawRequestBus::Broadcast(
//					&DebugDraw::DebugDrawRequests::DrawLineLocationToLocation,
//					pos,
//					pos + (-normal * m_dmgImpulse),
//					AZ::Colors::Red,
//					10.0f
//				);
			}
		}
	}

	void CombatStatusComponent::Heal(float amount)
	{
		m_health += amount;
		if(m_health > m_maxHealth)
		{
			m_health = m_maxHealth;
		}

		if(m_audioHeal.IsValid())
		{
			MiniAudio::MiniAudioPlaybackRequestBus::Event(m_audioHeal, &MiniAudio::MiniAudioPlaybackRequestBus::Events::Play);
		}

		CombatStatusNotificationBus::Event(
			GetEntityId(),
			&CombatStatusNotifications::OnHeal,
			amount
		);
	}

	float CombatStatusComponent::GetHealth()
	{
		return m_health;
	}

	float CombatStatusComponent::GetMaxHealth()
	{
		return m_maxHealth;
	}

	float CombatStatusComponent::GetAttackStrength()
	{
		return m_attack;
	}

	void CombatStatusComponent::SetAttackStrength(float set)
	{
		m_attack = set;
	}

}