#pragma once
#include <AzCore/Component/Component.h>
#include <AzCore/Math/Vector3.h>
#include <AzCore/RTTI/BehaviorContext.h>

namespace LD55
{

	class CombatStatusNotifications
		: public AZ::ComponentBus
	{
	public:
		virtual void OnDie() {};
		virtual void OnHeal([[maybe_unused]] float amount) {};
		virtual void OnDamaged([[maybe_unused]] float amount, [[maybe_unused]] float remaining, [[maybe_unused]] const AZ::Vector3& pos, [[maybe_unused]] const AZ::Vector3& normal) {};
	};
	using CombatStatusNotificationBus = AZ::EBus<CombatStatusNotifications>;

	// This is needed to expose notification bus to scripting
	class BehaviorCombatStatusNotificationBusHandler
		: public CombatStatusNotificationBus::Handler
		, public AZ::BehaviorEBusHandler
	{
	public:
		AZ_EBUS_BEHAVIOR_BINDER(
			BehaviorCombatStatusNotificationBusHandler,
			"{BC01D958-A613-47CB-AE78-7E556DCE8133}",
			AZ::SystemAllocator,
			OnDie,
			OnHeal,
			OnDamaged
		);

		void OnDie() override;
		void OnHeal(float amount) override;
		void OnDamaged(float amount, float remaining, const AZ::Vector3& pos, const AZ::Vector3& normal) override;
	};

	class CombatStatusRequests
		: public AZ::ComponentBus
	{
	public:
		virtual void Hurt(float amount, const AZ::Vector3& pos, const AZ::Vector3& normal) = 0;
		virtual void Heal(float amount) = 0;
		virtual float GetHealth() = 0;
		virtual float GetMaxHealth() = 0;

		virtual float GetAttackStrength() = 0;
		virtual void SetAttackStrength(float set) = 0;
	};
	using CombatStatusRequestBus = AZ::EBus<CombatStatusRequests>;

	class CombatStatusComponent
		: public AZ::Component
		, CombatStatusRequestBus::Handler
	{
	public:
		AZ_COMPONENT(LD55::CombatStatusComponent, "{293615f2-6b13-4647-9245-aa85cf6bf529}");
        static void Reflect(AZ::ReflectContext* context);
        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

		void Init() override;
		void Activate() override;
		void Deactivate() override;

		void Hurt(float amount, const AZ::Vector3& pos, const AZ::Vector3& normal) override;
		void Heal(float amount) override;
		float GetHealth() override;
		float GetMaxHealth() override;
		float GetAttackStrength() override;
		void SetAttackStrength(float set) override;

	protected:
		float m_health = 0.0f;
		float m_maxHealth = 100.0f;
		float m_attack = 15.0f;
		bool m_destroyOnDie = true;

		bool m_isImmortal = false;
		float m_dmgImpulse = 0.0f;

		//--- AUDIO ----------------------------
		AZ::EntityId m_audioDie;
		AZ::EntityId m_audioHurt;
		AZ::EntityId m_audioHeal;
		//--------------------------------------
	};
}