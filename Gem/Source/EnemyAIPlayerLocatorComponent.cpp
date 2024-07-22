#include "EnemyAIPlayerLocatorComponent.h"
#include "LD55/LD55Helpers.h"

#include <AzCore/Component/ComponentApplicationBus.h>
#include <AzCore/Serialization/SerializeContext.h>
#include <AzFramework/Physics/PhysicsSystem.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Component/TransformBus.h>
#include <AzCore/Component/Entity.h>
#include <DebugDraw/DebugDrawBus.h>

namespace LD55
{
	void EnemyAIPlayerLocatorComponent::Reflect(AZ::ReflectContext *context)
	{
        if (AZ::SerializeContext* serializeContext = azrtti_cast<AZ::SerializeContext*>(context)) {
            serializeContext->Class<EnemyAIPlayerLocatorComponent, AZ::Component>()
                ->Version(1)
                ->Field("Trigger Volume", &EnemyAIPlayerLocatorComponent::m_trigger)
                ->Field("Line of Sight Distance", &EnemyAIPlayerLocatorComponent::m_lineOfSightMaxDistance)
                ->Field("Raycast Tick Step", &EnemyAIPlayerLocatorComponent::raycast_step_count)
                ->Field("Locate Strategy", &EnemyAIPlayerLocatorComponent::m_locateStrategy)
                ->Field("Lose Strategy", &EnemyAIPlayerLocatorComponent::m_loseStrategy)
                ->Field("Lose Countdown", &EnemyAIPlayerLocatorComponent::m_loseTimeAmount)
                ->Field("Locate Countdown", &EnemyAIPlayerLocatorComponent::m_locateTimeAmount)
                ;

            if (AZ::EditContext* editContext = serializeContext->GetEditContext()) {
				editContext->Class<EnemyAIPlayerLocatorComponent>("EnemyAIPlayerLocatorComponent", "Implements AI logic for locating player")
					->ClassElement(AZ::Edit::ClassElements::EditorData, "")
					->Attribute(AZ::Edit::Attributes::Category, "ComponentCategory")
					->Attribute(AZ::Edit::Attributes::Icon, "Icons/Components/Component_Placeholder.svg")
					->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("Game"))
					->DataElement(AZ::Edit::UIHandlers::Default, &EnemyAIPlayerLocatorComponent::m_trigger, "Trigger Volume", "Trigger volume used by trigger strategies")
					->DataElement(AZ::Edit::UIHandlers::Default, &EnemyAIPlayerLocatorComponent::m_lineOfSightMaxDistance, "Line of Sight Distance", "Maximum distance for line of sight raycast")
					->DataElement(AZ::Edit::UIHandlers::Default, &EnemyAIPlayerLocatorComponent::m_locateStrategy, "Locate Strategy", "Strategy AI uses to locate a player")
					->DataElement(AZ::Edit::UIHandlers::Default, &EnemyAIPlayerLocatorComponent::m_loseStrategy, "Losing Strategy", "Strategy for determining when player is lost from AI")
					->DataElement(AZ::Edit::UIHandlers::Default, &EnemyAIPlayerLocatorComponent::m_loseTimeAmount, "Lose Countdown", "(seconds) Lose condition must be sustained for this long for AI to lose player")
					->DataElement(AZ::Edit::UIHandlers::Default, &EnemyAIPlayerLocatorComponent::m_locateTimeAmount, "Locate Countdown", "(seconds) Locate condition must be sustained for this long to locate player")
					->DataElement(AZ::Edit::UIHandlers::Default, &EnemyAIPlayerLocatorComponent::raycast_step_count, "Raycast Tick Step", "Ticks between each raycast for performance tuning")
				;
            }
        }
	}

	void ReflectAILocatorStrategies([[maybe_unused]] AZ::ReflectContext *context)
	{
//        AZ::SerializeContext* serializeContext = azrtti_cast<AZ::SerializeContext*>(context);
//        if (serializeContext)
//        {
//            AZ::EditContext* editContext = serializeContext->GetEditContext();
//            if (editContext)
//            {
//				editContext->Enum<PlayerLocatorStrategy>("PlayerLocatorStrategy", "Strategy AI uses to locate a player")
//					->Value("None", PlayerLocatorStrategy::None)
//					->Value("Trigger", PlayerLocatorStrategy::Trigger)
//				;
//
//				editContext->Enum<PlayerLosingStrategy>("PlayerLosingStrategy", "Strategy for determining when player is lost from AI")
//					->Value("None", PlayerLosingStrategy::None)
//					->Value("Trigger", PlayerLosingStrategy::Trigger)
//					->Value("LineOfSight", PlayerLosingStrategy::LineOfSight)
//				;
//            }
//        }
	}

    void EnemyAIPlayerLocatorComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided) {
        provided.push_back(AZ_CRC_CE("EnemyAIPlayerLocatorComponentService"));
    }

    void EnemyAIPlayerLocatorComponent::GetIncompatibleServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& incompatible) {
        //--
    }

    void EnemyAIPlayerLocatorComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required) {
        //--
    }

    void EnemyAIPlayerLocatorComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent) {
        //--
    }

    //------------------------------------------------------------------------------------------------------------------

    EnemyAIPlayerLocatorComponent::EnemyAIPlayerLocatorComponent()
        : m_onTriggerEnterHandler([this]([[maybe_unused]] AzPhysics::SimulatedBodyHandle bodyHandle,
            const  AzPhysics::TriggerEvent& triggerEvent)
            {
                OnTriggerEnter(triggerEvent);
            })
        , m_onTriggerExitHandler([this]([[maybe_unused]] AzPhysics::SimulatedBodyHandle bodyHandle,
            const  AzPhysics::TriggerEvent& triggerEvent)
            {
                OnTriggerExit(triggerEvent);
            })
	{
		//--
	}

	void EnemyAIPlayerLocatorComponent::Init()
	{
		m_lineOfSightCastRequest.m_distance = m_lineOfSightMaxDistance;
		m_lineOfSightCastRequest.m_queryType = AzPhysics::SceneQuery::QueryType::StaticAndDynamic;
		m_lineOfSightCastRequest.m_filterCallback = [this](const AzPhysics::SimulatedBody* body, [[maybe_unused]] const Physics::Shape* shape){
			//ignore our collider
			if(body->GetEntityId() == GetEntityId()){
				return AzPhysics::SceneQuery::QueryHitType::None;
			}
			//ignore our trigger volume
			if(body->GetEntityId() == m_trigger){
				return AzPhysics::SceneQuery::QueryHitType::None;
			}
			return AzPhysics::SceneQuery::QueryHitType::Block;
		};
	}

	void EnemyAIPlayerLocatorComponent::Activate()
	{
		Physics::RigidBodyNotificationBus::Handler::BusConnect(m_trigger);
		AZ::TickBus::Handler::BusConnect();
	}

	void EnemyAIPlayerLocatorComponent::Deactivate()
	{
		Physics::RigidBodyNotificationBus::Handler::BusDisconnect(m_trigger);
		AZ::TickBus::Handler::BusDisconnect();
	}

    void EnemyAIPlayerLocatorComponent::OnPhysicsEnabled(const AZ::EntityId& entityId)
    {
        if (auto* physicsSystem = AZ::Interface<AzPhysics::SystemInterface>::Get())
        {
            AZStd::pair<AzPhysics::SceneHandle, AzPhysics::SimulatedBodyHandle> foundBody =
                physicsSystem->FindAttachedBodyHandleFromEntityId(entityId);
            if (foundBody.first != AzPhysics::InvalidSceneHandle)
            {
                AzPhysics::SimulatedBodyEvents::RegisterOnTriggerEnterHandler(foundBody.first, foundBody.second, m_onTriggerEnterHandler);
                AzPhysics::SimulatedBodyEvents::RegisterOnTriggerExitHandler(foundBody.first, foundBody.second, m_onTriggerExitHandler);
            }
        }
    }
    void EnemyAIPlayerLocatorComponent::OnPhysicsDisabled([[maybe_unused]] const AZ::EntityId& entityId)
    {
        m_onTriggerEnterHandler.Disconnect();
        m_onTriggerExitHandler.Disconnect();
    }

	void EnemyAIPlayerLocatorComponent::on_initial_found_condition(const AZ::EntityId& id)
	{
		if(m_locateTimeAmount > 0)
		{
			m_locatePotentialState.timer = m_locateTimeAmount;
		}
		else
		{
			AZ::Vector3 playerPos;
			if(get_entity_position(id, playerPos))
			{
				AZ::Vector3 fromPos = GetEntity()->GetTransform()->GetWorldTranslation();
				on_found_player(id, fromPos, playerPos);
			}
			else
			{
				AZ_TracePrintf("EnemyAIPlayerLocatorComponent", "invalid player entity in trigger volume");
			}
		}
	}

	void EnemyAIPlayerLocatorComponent::on_initial_lost_condition()
	{
		if(m_loseTimeAmount > 0)
		{
			m_losePotentialState.timer = m_loseTimeAmount;
		}
		else
		{
			AZ::Vector3 playerPos;
			if(get_entity_position(m_player, playerPos))
			{
				AZ::Vector3 fromPos = GetEntity()->GetTransform()->GetWorldTranslation();
				on_lost_player(fromPos, playerPos);
			}
			else
			{
				AZ_TracePrintf("EnemyAIPlayerLocatorComponent", "invalid player entity in trigger volume");
			}
		}
	}

	void EnemyAIPlayerLocatorComponent::OnTriggerEnter(const AzPhysics::TriggerEvent& triggerEvent)
	{
		if(m_player.IsValid()){
			return; //already located a player
		}
		AZ::EntityId id = triggerEvent.m_otherBody->GetEntityId();
		m_locatePotentialState.candidate = id;
		if(m_locateStrategy == PlayerLocatorStrategy::Trigger)
		{
			on_initial_found_condition(id);
		}
	}

	void EnemyAIPlayerLocatorComponent::OnTriggerExit([[maybe_unused]] const AzPhysics::TriggerEvent& triggerEvent)
	{
		m_locatePotentialState.candidate = AZ::EntityId{};
		if(!m_player.IsValid())
		{
			return; //don't have a player to lose
		}
		if(m_locateStrategy == PlayerLocatorStrategy::Trigger)
		{
			on_initial_lost_condition();
		}
	}

	void EnemyAIPlayerLocatorComponent::OnTick([[maybe_unused]] float deltaTime, [[maybe_unused]] AZ::ScriptTimePoint time)
	{
		if(!m_player.IsValid())
		{
			if(m_locatePotentialState.timer > 0)
			{
				m_locatePotentialState.timer -= deltaTime;
				if(m_locatePotentialState.timer <= 0)
				{
					AZ::Vector3 playerPos;
					if(get_entity_position(m_locatePotentialState.candidate, playerPos))
					{
						AZ::Vector3 fromPos = GetEntity()->GetTransform()->GetWorldTranslation();
						on_found_player(
							m_locatePotentialState.candidate,
							fromPos,
							playerPos
						);
					}
					else
					{
						AZ_TracePrintf("EnemyAIPlayerLocatorComponent", "player CANDIDATE from trigger became invalid before timer completion");
					}
				}
			}
		}
		else
		{
			//Line of sight
			if(m_loseStrategy == PlayerLosingStrategy::LineOfSight)
			{
				if(raycast_step <= 0)
				{
					raycast_step = raycast_step_count;
					last_raycast_saw_player = raycast_to_entity(m_player);
				}
				else
				{
					raycast_step -= 1;
				}

				if(!last_raycast_saw_player && m_losePotentialState.timer <= 0)
				{
					on_initial_lost_condition();
				}
			}

			//handle lose timer
			if(m_losePotentialState.timer > 0)
			{
				m_losePotentialState.timer -= deltaTime;
				if(m_losePotentialState.timer <= 0)
				{
					AZ::Vector3 playerPos;
					if(get_entity_position(m_player, playerPos))
					{
						AZ::Vector3 fromPos = GetEntity()->GetTransform()->GetWorldTranslation();
						on_lost_player(
							fromPos,
							playerPos
						);
					}
					else
					{
						AZ_TracePrintf("EnemyAIPlayerLocatorComponent", "m_player is invalid at time of lose timer completion");
					}
				}
			}
		}
	}

	bool EnemyAIPlayerLocatorComponent::raycast_to_entity(const AZ::EntityId &id)
	{
		AZ::Vector3 playerPos;
		if(get_entity_position(id, playerPos))
		{
			m_lineOfSightCastRequest.m_start = GetEntity()->GetTransform()->GetWorldTranslation();
			m_lineOfSightCastRequest.m_direction = playerPos - m_lineOfSightCastRequest.m_start;
			m_lineOfSightCastRequest.m_reportMultipleHits = false;

			auto* sceneInterface = AZ::Interface<AzPhysics::SceneInterface>::Get();
			AZ_Assert(sceneInterface, "Physics system must be initialized");
			AzPhysics::SceneHandle sceneHandle = sceneInterface->GetSceneHandle(AzPhysics::DefaultPhysicsSceneName);
			AZ_Assert(sceneHandle != AzPhysics::InvalidSceneHandle, "Default Physics world must be created");

			AzPhysics::SceneQueryHits results = sceneInterface->QueryScene(sceneHandle, &m_lineOfSightCastRequest);
			bool saw = false;
			for(AzPhysics::SceneQueryHit &hit : results.m_hits)
			{
				if(hit.m_entityId == id){
					saw = true;
					break;
				}
			}

			DebugDraw::DebugDrawRequestBus::Broadcast
			(
				&DebugDraw::DebugDrawRequests::DrawLineLocationToLocation,
				m_lineOfSightCastRequest.m_start,
				m_lineOfSightCastRequest.m_start + (m_lineOfSightCastRequest.m_direction * m_lineOfSightMaxDistance),
				saw ? AZ::Colors::Green : (results.m_hits.empty() ? AZ::Colors::Orange : AZ::Colors::Red),
				1.0f
			);
			return saw;
		}
		return false;
	}

	//called when locating strategy condition is met
	void EnemyAIPlayerLocatorComponent::on_found_player(AZ::EntityId player, const AZ::Vector3 &fromPos, const AZ::Vector3 &playerPos)
	{
//		AZ_TracePrintf("AILocator", "Found the player!");
		m_player = player;
		EnemyAIPlayerLocatorNotificationBus::Event(
			GetEntityId(),
			&EnemyAIPlayerLocatorNotifications::OnFoundPlayer,
			player,
			fromPos,
			playerPos
		);
		last_raycast_saw_player = true;
	}

	//called when losing strategy condition is met
	void EnemyAIPlayerLocatorComponent::on_lost_player(const AZ::Vector3 &fromPos, const AZ::Vector3 &playerPos)
	{
//		AZ_TracePrintf("AILocator", "Lost the player...");
		EnemyAIPlayerLocatorNotificationBus::Event(
			GetEntityId(),
			&EnemyAIPlayerLocatorNotifications::OnLostPlayer,
			m_player,
			fromPos,
			playerPos
		);
		m_player = AZ::EntityId{};

		/*
		 * It's possible for line of sight to be lost while player is still inside trigger volume,
		 * meaning OnTriggerEnter won't fire again until the player exits the volume.
		 * */
		if(m_loseStrategy == PlayerLosingStrategy::LineOfSight && m_locatePotentialState.candidate.IsValid())
		{
			on_initial_found_condition(m_locatePotentialState.candidate);
		}
	}

	void EnemyAIPlayerLocatorComponent::SetLocatorStrategy(PlayerLocatorStrategy set)
	{
		m_locateStrategy = set;
	}

	void EnemyAIPlayerLocatorComponent::SetLosingStrategy(PlayerLosingStrategy set)
	{
		m_loseStrategy = set;
	}

	void EnemyAIPlayerLocatorComponent::ForceFindPlayer(const AZ::EntityId &player)
	{
		AZ::Vector3 playerPos;
		if(get_entity_position(player, playerPos))
		{
			AZ::Vector3 fromPos = GetEntity()->GetTransform()->GetWorldTranslation();
			ForceFindPlayer(player, fromPos, playerPos);
		}
		else
		{
			AZ_TracePrintf("EnemyAIPlayerLocatorComponent", "ForceFindPlayer called with invalid player EntityId");
		}
	}
	void EnemyAIPlayerLocatorComponent::ForceFindPlayer(const AZ::EntityId &player, const AZ::Vector3 &fromPos, const AZ::Vector3 &playerPos)
	{
		on_found_player(player, fromPos, playerPos);
	}
	void EnemyAIPlayerLocatorComponent::ForceLosePlayer()
	{
		if(!m_player.IsValid())
		{
			AZ_TracePrintf("EnemyAIPlayerLocatorComponent", "ForceLosePlayer called, but enemy has no player");
			return;
		}

		AZ::Vector3 playerPos;
		if(get_entity_position(m_player, playerPos))
		{
			AZ::Vector3 fromPos = GetEntity()->GetTransform()->GetWorldTranslation();
			ForceLosePlayer(fromPos, playerPos);
		}
		else
		{
			AZ_TracePrintf("EnemyAIPlayerLocatorComponent", "ForceFindPlayer called with invalid player EntityId");
		}
	}
	void EnemyAIPlayerLocatorComponent::ForceLosePlayer(const AZ::Vector3 &fromPos, const AZ::Vector3 &playerPos)
	{
		on_lost_player(fromPos, playerPos);
	}


}