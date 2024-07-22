#include "ProjectileShooterComponent.h"
#include "ProjectileComponent.h"
#include "Source/CombatStatusComponent.h"

#include <AzCore/Component/TransformBus.h>
#include <AzCore/Serialization/SerializeContext.h>
#include <Atom/RPI.Public/Model/Model.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Asset/AssetSerializer.h>

namespace LD55
{
	void ProjectileShooterComponent::Reflect(AZ::ReflectContext *context)
	{
        if (AZ::SerializeContext* serializeContext = azrtti_cast<AZ::SerializeContext*>(context)) {
            serializeContext->Class<ProjectileShooterComponent, AZ::Component>()
                ->Version(1)
				->Field("Particle Asset", &ProjectileShooterComponent::m_spawnableAsset)
				->Field("Pool Size", &ProjectileShooterComponent::m_poolSize)
                ->Field("Projectile Offset", &ProjectileShooterComponent::projectile_offset)
                ;

            if (AZ::EditContext* editContext = serializeContext->GetEditContext()) {
                editContext->Class<ProjectileShooterComponent>("ProjectileShooterComponent", "Can spawn projectiles")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::Category, "ComponentCategory")
                    ->Attribute(AZ::Edit::Attributes::Icon, "Icons/Components/Component_Placeholder.svg")
                    ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("Game"))
                    ->DataElement(AZ::Edit::UIHandlers::Default, &ProjectileShooterComponent::m_spawnableAsset, "Projectile Asset", "Projectile asset to spawn")
                    ->DataElement(AZ::Edit::UIHandlers::Default, &ProjectileShooterComponent::m_poolSize, "Pool Size", "Size of projectile pool")
                    ->DataElement(AZ::Edit::UIHandlers::Default, &ProjectileShooterComponent::projectile_offset, "Projectile Offset", "Spawn offset of projectile")
				;
            }
        }
	}
	void ProjectileShooterComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
	{
        provided.push_back(AZ_CRC_CE("ProjectileShooterService"));
	}

	void ProjectileShooterComponent::Init()
	{
		//--
	}

	void ProjectileShooterComponent::Activate()
	{
		ProjectileShooterRequestBus::Handler::BusConnect(GetEntityId());
		fill_pool();
//		AZ_TracePrintf("ProjectileShooter", "Pool size: %d", (int)pool.size());
	}
	void ProjectileShooterComponent::Deactivate()
	{
		ProjectileShooterRequestBus::Handler::BusDisconnect(GetEntityId());
		empty_pool();
	}
	void ProjectileShooterComponent::empty_pool()
	{
//		AZ_TracePrintf("ProjectileShooter", "Emptying pool (cur size: %d)", pool.size());
		for(ProjectileComponent *p: pool)
		{
			AzFramework::SpawnableEntitiesInterface::Get()->DespawnAllEntities(*(p->m_ticket));
			p->m_ticket = nullptr;
		}
		pool.clear();
	}
	void ProjectileShooterComponent::fill_pool()
	{
//		AZ_TracePrintf("ProjectileShooter", "Filling pool to size: %d", m_poolSize);
		for(int i = 0; i < m_poolSize; i++){
			auto m_ticket = AZStd::make_shared<AzFramework::EntitySpawnTicket>(m_spawnableAsset);
			auto postSpawnCB = [this, m_ticket](
				[[maybe_unused]] AzFramework::EntitySpawnTicket::Id, AzFramework::SpawnableConstEntityContainerView view
			){
				for(const AZ::Entity *e : view){
					if(!e) continue;
					auto trans = e->GetTransform();
					if(!trans) continue;
					AZStd::vector<AZ::EntityId> children = trans->GetAllDescendants();
					for(AZ::EntityId &child : children){
            			AZ::Entity* childEntity = nullptr;
            			AZ::ComponentApplicationBus::BroadcastResult(childEntity, &AZ::ComponentApplicationBus::Events::FindEntity, child);
            			if(childEntity){
							ProjectileComponent* projectile = childEntity->FindComponent<ProjectileComponent>();
							if (projectile)
							{
								projectile->m_ticket = m_ticket;
								pool.push_back(projectile);
								projectile->reset();
								return;
							}
						}
					}
				}
			};

			if (m_ticket->IsValid())
			{
//				AZ_TracePrintf("ProjectileShooter", "EntitySpawnTicket is VALID");
				AzFramework::SpawnAllEntitiesOptionalArgs optionalArgs;
				optionalArgs.m_completionCallback = AZStd::move(postSpawnCB);
				AzFramework::SpawnableEntitiesInterface::Get()->SpawnAllEntities(*m_ticket, AZStd::move(optionalArgs));
			}
			else
			{
//				AZ_TracePrintf("ProjectileShooter", "EntitySpawnTicket is INVALID");
				AZ_Assert(m_ticket->IsValid(), "Unable to instantiate spawnable asset");
			}
		}
	}

	AZ::EntityId ProjectileShooterComponent::ShootProjectile(const AZ::Vector3 &at, const AZ::Vector3 &dir, float speed, bool hitPlayer)
	{
//		AZ_TracePrintf("ProjectileShooter", "ShootProjectile(pool size: %d)", pool.size());

		for(ProjectileComponent *p: pool)
		{
			if(!p->is_alive())
			{
//				AZ_TracePrintf("ProjectileShooter", "Spawned a projectile!");
				float damage = 10.0;
				CombatStatusRequestBus::EventResult(
					damage,
					GetEntityId(),
					&CombatStatusRequests::GetAttackStrength
				);

				p->spawn(damage, at + projectile_offset, dir, speed, hitPlayer);
				return p->GetEntityId();
			}
		}
//		AZ_TracePrintf("ProjectileShooter", "Didn't spawn anything :(");
		return AZ::EntityId{};
	}

}