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
	class ProjectileComponent
		: public AZ::Component
		, public AZ::TickBus::Handler
		, protected Physics::RigidBodyNotificationBus::Handler
	{
	public:
		ProjectileComponent();
		AZ_COMPONENT(LD55::ProjectileComponent, "{3009ec0f-1c1f-4fed-a3d7-d46268ed55c0}");
        static void Reflect(AZ::ReflectContext* context);

		void spawn(float damage, const AZ::Vector3 &pos, const AZ::Vector3 &dir, float speed, bool hitPlayer);
		void reset();
		bool is_alive() const;

		void OnTick(float deltaTime, AZ::ScriptTimePoint time) override;

		bool do_raycast(AZ::Vector3 &to, float dist);
		AzPhysics::RayCastRequest m_moveRaycastRequest;

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
		AZStd::shared_ptr<AzFramework::EntitySpawnTicket> m_ticket;
		AZ::Vector3 m_moveDir;

		float m_damage = 5.0f;
		float m_speed = 1.0f;
		float m_lifetime = 0.0f;
		float m_lifetimeStart = 10.0f;
		bool m_hitPlayer = true;
	};
}
