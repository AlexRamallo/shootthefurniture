#include "EnemySentinelAIComponent.h"
#include "LD55/LD55Helpers.h"

#include "Source/ProjectileShooterComponent.h"

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Component/ComponentApplicationBus.h>
#include <AzCore/Component/TransformBus.h>

#include <AzFramework/Entity/GameEntityContextBus.h>

namespace LD55
{
	void EnemySentinelAIComponent::Reflect(AZ::ReflectContext *context)
	{
        if (AZ::SerializeContext* serializeContext = azrtti_cast<AZ::SerializeContext*>(context)) {
            serializeContext->Class<EnemySentinelAIComponent, AZ::Component>()
                ->Version(1)
                ->Field("Shot Cooldown", &EnemySentinelAIComponent::m_shootCooldown)
                ->Field("Projectile Speed", &EnemySentinelAIComponent::m_shotSpeed)
                ->Field("Idle Look A", &EnemySentinelAIComponent::m_idleLook1)
                ->Field("Idle Look B", &EnemySentinelAIComponent::m_idleLook2)
                ->Field("Idle Target Switch Time", &EnemySentinelAIComponent::m_idleTargetSwitchTime)
                ->Field("Idle Target Wait Time", &EnemySentinelAIComponent::m_idleTargetWaitTime)
                ->Field("Forward Axis", &EnemySentinelAIComponent::m_forwardAxis)
                ;

            if (AZ::EditContext* editContext = serializeContext->GetEditContext()) {
                editContext->Class<EnemySentinelAIComponent>("EnemySentinelAIComponent", "Stationary turret that shoots at players")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::Category, "ComponentCategory")
                    ->Attribute(AZ::Edit::Attributes::Icon, "Icons/Components/Component_Placeholder.svg")
                    ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("Game"))
                    ->DataElement(AZ::Edit::UIHandlers::Default, &EnemySentinelAIComponent::m_shootCooldown, "Shot Cooldown", "Time between shots (seconds)")
                    ->DataElement(AZ::Edit::UIHandlers::Default, &EnemySentinelAIComponent::m_shotSpeed, "Projectile Speed", "Speed of projectiles")
                    ->DataElement(AZ::Edit::UIHandlers::Default, &EnemySentinelAIComponent::m_idleLook1, "Idle Look A", "Sentinel will alternate looking between these targets when idle")
                    ->DataElement(AZ::Edit::UIHandlers::Default, &EnemySentinelAIComponent::m_idleLook2, "Idle Look B", "Sentinel will alternate looking between these targets when idle")
                    ->DataElement(AZ::Edit::UIHandlers::Default, &EnemySentinelAIComponent::m_idleTargetSwitchTime, "Idle Target Switch Time", "lerp time between targets")
                    ->DataElement(AZ::Edit::UIHandlers::Default, &EnemySentinelAIComponent::m_idleTargetWaitTime, "Idle Target Wait Time", "time spent at a target before switching")
                    ->DataElement(AZ::Edit::UIHandlers::Default, &EnemySentinelAIComponent::m_forwardAxis, "Forward Axis", "Axis used for idle lookAt calculation")
				;
            }
        }
	}

    void EnemySentinelAIComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required) {
        required.push_back(AZ_CRC_CE("EnemyAIPlayerLocatorComponentService"));
        required.push_back(AZ_CRC_CE("CombatStatusComponentService"));
        required.push_back(AZ_CRC_CE("ProjectileShooterService"));
    }

	void EnemySentinelAIComponent::Init()
	{
		//--
	}

	void EnemySentinelAIComponent::Activate()
	{
		EnemyAIPlayerLocatorNotificationBus::Handler::BusConnect(GetEntityId());
		AZ::TickBus::Handler::BusConnect();
		CombatStatusNotificationBus::Handler::BusConnect(GetEntityId());
	}

	void EnemySentinelAIComponent::Deactivate()
	{
		AZ::TickBus::Handler::BusDisconnect();
		EnemyAIPlayerLocatorNotificationBus::Handler::BusDisconnect(GetEntityId());
		CombatStatusNotificationBus::Handler::BusDisconnect(GetEntityId());
	}

	void EnemySentinelAIComponent::OnDie()
	{
		AZ_TracePrintf("EnemySentinel", "Died!");

	}

	void EnemySentinelAIComponent::OnHeal([[maybe_unused]] float amount)
	{
		//--
	}

	void EnemySentinelAIComponent::OnDamaged([[maybe_unused]] float amount, [[maybe_unused]] float remaining, [[maybe_unused]] const AZ::Vector3& pos, [[maybe_unused]] const AZ::Vector3& normal)
	{
		AZ_TracePrintf("EnemySentinel", "OnDamaged! %f (%f remains)", amount, remaining);
	}

	void EnemySentinelAIComponent::OnTick([[maybe_unused]] float deltaTime, [[maybe_unused]] AZ::ScriptTimePoint time)
	{
		auto trans = GetEntity()->GetTransform();

		if(m_aggroTarget != nullptr)
		{
			AZ::Vector3 lookat_pos = m_aggroTarget->GetTransform()->GetWorldTranslation();
//			auto new_tm = AZ::Transform::CreateLookAt(
//				trans->GetWorldTranslation(),
//				lookat_pos,
//				AZ::Transform::Axis::YPositive
//			);
//			trans->SetWorldTM(new_tm);

			if(m_shootCooldownTimer > 0)
			{
				m_shootCooldownTimer -= deltaTime;
			}
			if(m_shootCooldownTimer <= 0)
			{
				m_shootCooldownTimer = m_shootCooldown;
				AZ::Vector3 fromPos = trans->GetWorldTranslation();
				AZ::EntityId projectile;
				ProjectileShooterRequestBus::EventResult(
					projectile,
					GetEntityId(),
					&ProjectileShooterRequests::ShootProjectile,
					fromPos,
					(lookat_pos - fromPos).GetNormalized(),
					m_shotSpeed,
					true
				);
			}
		}
//		else
//		{
//			AZ::Vector3 m_idleLookPos1;
//			AZ::Vector3 m_idleLookPos2;
//    		if(
//    			get_entity_position(m_idleLook1, m_idleLookPos1) &&
//    			get_entity_position(m_idleLook2, m_idleLookPos2)
//    		)
//			{
//				if(m_idleTargetSwitchTime_counter > 0)
//				{
//					m_idleTargetSwitchTime_counter -= deltaTime;
//				}
//
//				if(m_idleTargetSwitchTime_counter <= 0)
//				{
//					if(m_idleTargetWaitTime_counter > 0)
//					{
//						m_idleTargetWaitTime_counter -= deltaTime;
//					}
//					if(m_idleTargetWaitTime_counter <= 0)
//					{
//						m_idleLookAt1 = !m_idleLookAt1;
//						m_idleTargetWaitTime_counter = m_idleTargetWaitTime;
//						m_idleTargetSwitchTime_counter = m_idleTargetSwitchTime;
//					}
//				}
//
//				AZ::Transform tLookat = AZ::Transform::CreateLookAt(
//					trans->GetWorldTranslation(),
//					m_idleLookAt1 ? m_idleLookPos1 : m_idleLookPos2,
//					m_forwardAxis
//				);
//
//				auto worldTM = trans->GetWorldTM();
//
//				float t = 1.0f - (m_idleTargetSwitchTime_counter / m_idleTargetSwitchTime);
//
//				AZ::Quaternion rot = worldTM.GetRotation().Slerp(tLookat.GetRotation(), t);
//				trans->SetWorldRotationQuaternion(rot);
//			}
//		}

	}

	void EnemySentinelAIComponent::OnFoundPlayer([[maybe_unused]] const AZ::EntityId &player, [[maybe_unused]] const AZ::Vector3 &fromPos, [[maybe_unused]] const AZ::Vector3 &playerPos)
	{
		AZ::Entity *entity = nullptr;
		AZ::ComponentApplicationBus::BroadcastResult(
			entity,
			&AZ::ComponentApplicationRequests::FindEntity,
			player
		);

		if(entity)
		{
			m_aggroTarget = entity;
			m_aggroTargetId = player;
			m_shootCooldownTimer = m_shootCooldown;
		}

	}

	void EnemySentinelAIComponent::OnLostPlayer([[maybe_unused]] const AZ::EntityId &player, [[maybe_unused]] const AZ::Vector3 &fromPos, [[maybe_unused]] const AZ::Vector3 &lastPlayerPos)
	{
		m_aggroTargetId = AZ::EntityId{};
		m_aggroTarget = nullptr;
	}


}