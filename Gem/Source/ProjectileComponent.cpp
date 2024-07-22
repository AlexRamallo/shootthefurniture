#include "ProjectileComponent.h"

#include <AzCore/Component/TransformBus.h>
#include <AzFramework/Physics/RigidBodyBus.h>
#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzFramework/Physics/PhysicsSystem.h>

#include "LD55/LD55Helpers.h"
#include "Source/CombatStatusComponent.h"

namespace LD55
{
	void ProjectileComponent::Reflect(AZ::ReflectContext *context)
	{
        if (AZ::SerializeContext* serializeContext = azrtti_cast<AZ::SerializeContext*>(context)) {
            serializeContext->Class<ProjectileComponent, AZ::Component>()
                ->Version(1)
                ->Field("Lifetime", &ProjectileComponent::m_lifetimeStart)
                ;

            if (AZ::EditContext* editContext = serializeContext->GetEditContext()) {
                editContext->Class<ProjectileComponent>("ProjectileComponent", "A spawnable projectile")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::Category, "ComponentCategory")
                    ->Attribute(AZ::Edit::Attributes::Icon, "Icons/Components/Component_Placeholder.svg")
                    ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("Game"))
                    ->DataElement(AZ::Edit::UIHandlers::Default, &ProjectileComponent::m_lifetimeStart, "Lifetime", "Lifetime of projectile")
				;
            }
        }
	}

    ProjectileComponent::ProjectileComponent()
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

	void ProjectileComponent::Init()
	{
		m_moveRaycastRequest.m_queryType = AzPhysics::SceneQuery::QueryType::Static;
		m_moveRaycastRequest.m_collisionGroup = AzPhysics::CollisionGroup("playerwall");
	}

	void ProjectileComponent::Activate()
	{
		AZ::TickBus::Handler::BusConnect();
		Physics::RigidBodyNotificationBus::Handler::BusConnect(GetEntityId());
	}

	void ProjectileComponent::OnTriggerEnter(const AzPhysics::TriggerEvent& triggerEvent)
	{
		AZ::Vector3 pos = GetEntity()->GetTransform()->GetWorldTranslation();
		AZ::Vector3 norm = triggerEvent.m_triggerBody->GetPosition() - pos;

		CombatStatusRequestBus::Event(
			triggerEvent.m_otherBody->GetEntityId(),
			&CombatStatusRequests::Hurt,
			m_damage,
			pos,
			norm
		);

		reset();
	}

	void ProjectileComponent::Deactivate()
	{
		AZ::TickBus::Handler::BusDisconnect();
		Physics::RigidBodyNotificationBus::Handler::BusDisconnect(GetEntityId());
	}

	void ProjectileComponent::OnTriggerExit([[maybe_unused]] const AzPhysics::TriggerEvent& triggerEvent)
	{
		//--
	}

    void ProjectileComponent::OnPhysicsEnabled(const AZ::EntityId& entityId)
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
    void ProjectileComponent::OnPhysicsDisabled([[maybe_unused]] const AZ::EntityId& entityId)
    {
        m_onTriggerEnterHandler.Disconnect();
        m_onTriggerExitHandler.Disconnect();
    }

	void ProjectileComponent::reset()
	{
		m_lifetime = 0;
		GetEntity()->GetTransform()->SetWorldTranslation(AZ::Vector3(0,0,-10000)); //projectile hell
	}

	void ProjectileComponent::spawn(float damage, const AZ::Vector3 &pos, [[maybe_unused]] const AZ::Vector3 &dir, [[maybe_unused]] float speed, [[maybe_unused]] bool hitPlayer)
	{
		auto trans = GetEntity()->GetTransform();
		trans->SetWorldTranslation(pos);
		m_hitPlayer = hitPlayer;
		m_moveDir = dir;
        m_speed = speed;
        m_damage = damage;
		m_lifetime = m_lifetimeStart;
	}

	bool ProjectileComponent::is_alive() const
	{
		return m_lifetime > 0;
	}

	bool ProjectileComponent::do_raycast(AZ::Vector3 &to, float dist)
	{
		m_moveRaycastRequest.m_start = GetEntity()->GetTransform()->GetWorldTranslation();
		auto dir = to - m_moveRaycastRequest.m_start;
		m_moveRaycastRequest.m_direction = m_moveRaycastRequest.m_start + dir.GetNormalized();
		m_moveRaycastRequest.m_distance = dist;
		m_moveRaycastRequest.m_reportMultipleHits = false;

		auto* sceneInterface = AZ::Interface<AzPhysics::SceneInterface>::Get();
		AZ_Assert(sceneInterface, "Physics system must be initialized");
		AzPhysics::SceneHandle sceneHandle = sceneInterface->GetSceneHandle(AzPhysics::DefaultPhysicsSceneName);
		AZ_Assert(sceneHandle != AzPhysics::InvalidSceneHandle, "Default Physics world must be created");

		AzPhysics::SceneQueryHits results = sceneInterface->QueryScene(sceneHandle, &m_moveRaycastRequest);
		return results.m_hits.size() > 0;

//		DebugDraw::DebugDrawRequestBus::Broadcast
//		(
//			&DebugDraw::DebugDrawRequests::DrawLineLocationToLocation,
//			m_moveRaycastRequest.m_start,
//			m_moveRaycastRequest.m_start + (m_moveRaycastRequest.m_direction * m_lineOfSightMaxDistance),
//			saw ? AZ::Colors::Green : (results.m_hits.empty() ? AZ::Colors::Orange : AZ::Colors::Red),
//			1.0f
//		);
	}

	void ProjectileComponent::OnTick([[maybe_unused]] float deltaTime, [[maybe_unused]] AZ::ScriptTimePoint time)
	{
		if(m_lifetime > 0)
		{
			m_lifetime -= deltaTime;
			auto trans = GetEntity()->GetTransform();

			auto to = trans->GetWorldTranslation() + (m_moveDir * m_speed * deltaTime);
			float dist = (to - trans->GetWorldTranslation()).GetLength() + 0.2f;
			if(!do_raycast(to, dist)){
				trans->SetWorldTranslation(to);
			}else{
				m_lifetime = -1;
			}

			if(m_lifetime <= 0)
			{
				reset();
			}
		}
	}
}