#pragma once
#include <AzCore/Component/Component.h>
#include <AzCore/Math/Vector3.h>
#include <AzCore/Component/TickBus.h>

#include "Source/CombatStatusComponent.h"
#include "Source/EnemyAIPlayerLocatorComponent.h"
#include "Source/ProjectileShooterComponent.h"

namespace LD55
{
	class EnemySentinelAIComponent
		: public AZ::Component
		, public EnemyAIPlayerLocatorNotificationBus::Handler
		, public AZ::TickBus::Handler
		, public CombatStatusNotificationBus::Handler
	{
	public:
		AZ_COMPONENT(LD55::EnemySentinelAIComponent, "{1d2f6346-d9de-4008-a06f-db8d05ec6e58}");
        static void Reflect(AZ::ReflectContext* context);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);

		void Init() override;
		void Activate() override;
		void Deactivate() override;
		void OnTick(float deltaTime, AZ::ScriptTimePoint time) override;

		//CombatStatusNotifications
		void OnDie() override;
		void OnHeal(float amount) override;
		void OnDamaged(float amount, float remaining, const AZ::Vector3& pos, const AZ::Vector3& normal) override;

		//EnemyAIPlayerLocator
		void OnFoundPlayer(const AZ::EntityId &player, const AZ::Vector3 &fromPos, const AZ::Vector3 &playerPos) override;
		void OnLostPlayer(const AZ::EntityId &player, const AZ::Vector3 &fromPos, const AZ::Vector3 &lastPlayerPos) override;

	protected:
		AZ::EntityId m_aggroTargetId;
		AZ::Entity *m_aggroTarget = nullptr;
		float m_shootCooldown = 1.0f;
		float m_shootCooldownTimer = 0.0f;
		float m_shotSpeed = 0.5f;

		bool m_idleLookAt1 = true;
		AZ::EntityId m_idleLook1;
		AZ::EntityId m_idleLook2;
		float m_idleTargetSwitchTime = 3.0f;
		float m_idleTargetSwitchTime_counter = 0.0f;
		float m_idleTargetWaitTime = 5.0f;
		float m_idleTargetWaitTime_counter = 0.0f;
		AZ::Transform::Axis m_forwardAxis = AZ::Transform::Axis::YPositive;
	};

}