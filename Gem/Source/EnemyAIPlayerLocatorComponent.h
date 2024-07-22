#pragma once
#include <AzCore/Component/Component.h>
#include <AzCore/Component/TickBus.h>
#include <AzCore/Math/Vector3.h>

#include <AzFramework/Physics/RigidBodyBus.h>
#include <AzFramework/Physics/Common/PhysicsSimulatedBodyEvents.h>


namespace LD55
{
	class EnemyAIPlayerLocatorNotifications
		: public AZ::ComponentBus
	{
	public:
		virtual void OnFoundPlayer([[maybe_unused]] const AZ::EntityId &player, [[maybe_unused]] const AZ::Vector3 &fromPos, [[maybe_unused]] const AZ::Vector3 &playerPos){};
		virtual void OnLostPlayer([[maybe_unused]] const AZ::EntityId &player, [[maybe_unused]] const AZ::Vector3 &fromPos, [[maybe_unused]] const AZ::Vector3 &lastPlayerPos){};
	};
	using EnemyAIPlayerLocatorNotificationBus = AZ::EBus<EnemyAIPlayerLocatorNotifications>;

	class EnemyAIPlayerLocatorRequests
		: public AZ::ComponentBus
	{
	public:
		AZ_RTTI(LD55::EnemyAIPlayerLocatorRequests, "{e8541033-1202-4720-9548-c1a3dd9efb8c}");

		virtual void ForceFindPlayer([[maybe_unused]] const AZ::EntityId &player) {};
		virtual void ForceFindPlayer([[maybe_unused]] const AZ::EntityId &player, [[maybe_unused]] const AZ::Vector3 &fromPos, [[maybe_unused]] const AZ::Vector3 &playerPos) {};

		virtual void ForceLosePlayer() {};
		virtual void ForceLosePlayer([[maybe_unused]] const AZ::Vector3 &fromPos, [[maybe_unused]] const AZ::Vector3 &playerPos) {};
	};
	using EnemyAIPlayerLocatorRequestBus = AZ::EBus<EnemyAIPlayerLocatorRequests>;

	enum class PlayerLocatorStrategy
	{
		None = 0, //never finds player
		Trigger = 1, //find player if they enter a trigger volume
	};

	enum class PlayerLosingStrategy
	{
		None = 0, //never lose the player
		Trigger = 1, //lose player if they leave trigger volume
		LineOfSight = 2, //lose player if they leave line of sight
	};

	void ReflectAILocatorStrategies(AZ::ReflectContext *context);

	class EnemyAIPlayerLocatorComponent
		: public AZ::Component
		, public EnemyAIPlayerLocatorRequestBus::Handler
		, public AZ::TickBus::Handler
		, protected Physics::RigidBodyNotificationBus::Handler
	{
	public:
		EnemyAIPlayerLocatorComponent();

		AZ_COMPONENT(LD55::EnemyAIPlayerLocatorComponent, "{60b69554-e3fd-464f-ab1f-8ce74f8a7f2b}");
        static void Reflect(AZ::ReflectContext* context);
        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

		void Init() override;
		void Activate() override;
		void Deactivate() override;

		void OnTick(float deltaTime, AZ::ScriptTimePoint time) override;

		void SetLocatorStrategy(PlayerLocatorStrategy set);
		void SetLosingStrategy(PlayerLosingStrategy set);
		void ForceFindPlayer(const AZ::EntityId &player) override;
		void ForceFindPlayer(const AZ::EntityId &player, const AZ::Vector3 &fromPos, const AZ::Vector3 &playerPos) override;
		void ForceLosePlayer() override;
		void ForceLosePlayer(const AZ::Vector3 &fromPos, const AZ::Vector3 &playerPos) override;

	protected:
		PlayerLocatorStrategy m_locateStrategy = PlayerLocatorStrategy::None;
		PlayerLosingStrategy m_loseStrategy = PlayerLosingStrategy::None;

		float m_loseTimeAmount = 0.0f; //seconds lose condition must be true for player to be lost
		float m_locateTimeAmount = 0.0f; //seconds locate condition must be true for player to be located

		float m_locateRadius = 10.0f;

		AZ::EntityId m_trigger; //entity with trigger volume

		struct {
			AZ::EntityId candidate;
			float timer = 0.0f;
		} m_locatePotentialState;

		struct {
			float timer = 0.0f;
		} m_losePotentialState;

		bool last_raycast_saw_player = false;
		int raycast_step = 0;
		int raycast_step_count = 6;

		AzPhysics::RayCastRequest m_lineOfSightCastRequest;
		float m_lineOfSightMaxDistance = 100.0f;

		AZ::EntityId m_player;

        void OnPhysicsEnabled(const AZ::EntityId& entityId) override;
        void OnPhysicsDisabled(const AZ::EntityId& entityId) override;

		bool raycast_to_entity(const AZ::EntityId &id);

		void on_initial_found_condition(const AZ::EntityId& id);
		void on_initial_lost_condition();
		//called when locating strategy condition is met
		void on_found_player(AZ::EntityId player, const AZ::Vector3 &fromPos, const AZ::Vector3 &playerPos);
		//called when losing strategy condition is met
        void on_lost_player(const AZ::Vector3 &fromPos, const AZ::Vector3 &playerPos);

	private:
        void OnTriggerEnter(const AzPhysics::TriggerEvent& triggerEvent);
        void OnTriggerExit(const AzPhysics::TriggerEvent& triggerEvent);
        AzPhysics::SimulatedBodyEvents::OnTriggerEnter::Handler m_onTriggerEnterHandler;
        AzPhysics::SimulatedBodyEvents::OnTriggerExit::Handler m_onTriggerExitHandler;
	};

}