#pragma once
#include <AzCore/Component/Component.h>
#include <AzCore/Math/Vector3.h>
#include <AzCore/Component/TickBus.h>
#include <AtomLyIntegration/CommonFeatures/Material/MaterialComponentBus.h>
#include <AzFramework/Spawnable/SpawnableEntitiesInterface.h>
#include <Atom/RPI.Public/Scene.h>
#include <Atom/RPI.Public/Model/Model.h>

#include <AzFramework/Physics/RigidBodyBus.h>
#include <AzFramework/Physics/Common/PhysicsSimulatedBodyEvents.h>


namespace LD55
{
	enum class CollectibleType
	{
		Health = 0,
		Ammo = 1,
		Coin = 2,
	};

	class CollectibleNotifications
		: public AZ::ComponentBus
	{
	public:
		virtual void OnCollected([[maybe_unused]] CollectibleType type, [[maybe_unused]] int value, [[maybe_unused]] const AZ::Vector3 &pos) {};
	};
	using CollectibleNotificationBus = AZ::EBus<CollectibleNotifications>;

	class CollectibleComponent
		: public AZ::Component
		, public AZ::TickBus::Handler
		, protected Physics::RigidBodyNotificationBus::Handler
	{
	public:
		CollectibleComponent();
		AZ_COMPONENT(LD55::CollectibleComponent, "{4770c246-747f-4f40-bc3f-38a40e1b9833}");
        static void Reflect(AZ::ReflectContext* context);

		void respawn();
		void on_collected();
		void OnTick(float deltaTime, AZ::ScriptTimePoint time) override;

	protected:
		void Init() override;
		void Activate() override;
		void Deactivate() override;

	private:
        void OnPhysicsEnabled(const AZ::EntityId& entityId) override;
        void OnPhysicsDisabled(const AZ::EntityId& entityId) override;
        void OnTriggerEnter(const AzPhysics::TriggerEvent& triggerEvent);
        void OnTriggerExit(const AzPhysics::TriggerEvent& triggerEvent);
        AzPhysics::SimulatedBodyEvents::OnTriggerEnter::Handler m_onTriggerEnterHandler;
        AzPhysics::SimulatedBodyEvents::OnTriggerExit::Handler m_onTriggerExitHandler;

	public:
		float m_respawns = true;
		float m_respawnTimer = 120.0f;
		float m_respawnTimer_counter = 0.0f;
		int m_value = 10;
		CollectibleType m_type = CollectibleType::Health;

		AZ::Transform spawnTransform;
	};
}
