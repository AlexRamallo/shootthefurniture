#pragma once
#include <AzCore/Component/Component.h>
#include <AzCore/Component/TickBus.h>
#include <AzCore/Math/Vector3.h>
#include "Source/CombatStatusComponent.h"

namespace LD55
{
	class UIPlayerStatsNotifications
		: public AZ::ComponentBus
	{
	public:
		virtual void OnHealthChanged([[maybe_unused]] float cur, [[maybe_unused]] float max, [[maybe_unused]] bool hurt) {};
		virtual void OnAmmoChanged([[maybe_unused]] int clip, [[maybe_unused]] int clipSize, [[maybe_unused]] int bullets) {};
		virtual void OnCoinsChanged([[maybe_unused]] int new_count) {};
	};
	using UIPlayerStatsNotificationBus = AZ::EBus<UIPlayerStatsNotifications>;

	class UIPlayerStatsComponent
		: public AZ::Component
		, public UIPlayerStatsNotificationBus::Handler
		, public AZ::TickBus::Handler
	{
	public:
		AZ_COMPONENT(LD55::UIPlayerStatsComponent, "{63e6256c-98ad-4ab9-a68f-a149210c2f90}");
        static void Reflect(AZ::ReflectContext* context);

		void Init() override;
		void Activate() override;
		void Deactivate() override;
		void OnTick(float deltaTime, AZ::ScriptTimePoint time) override;

		void OnHealthChanged(float cur, float max, bool hurt) override;
		void OnAmmoChanged(int clip, int clipSize, int bullets) override;
		void OnCoinsChanged(int new_count) override;

	protected:
		AZ::EntityId m_coinCount;
		AZ::EntityId m_ammoCount;
		AZ::EntityId m_hurtOverlay;
		float m_hurtOverlayTime = 1.0f;
		float m_hurtOverlayTime_counter = 0.0f;

		float m_healthbarTargetWidth = 300.0f;
		float m_healthbarTargetTime = 0.3f;
		float m_healthbarTargetTime_counter = 0.0f;
	};

}