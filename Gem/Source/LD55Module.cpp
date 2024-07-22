
#include <AzCore/Memory/SystemAllocator.h>
#include <AzCore/Module/Module.h>

#include "LD55SystemComponent.h"

#include "FPSCameraComponent.h"
#include "PlayerControlComponent.h"
#include "GhettoParticle.h"
#include "GhettoParticleSystem.h"
#include "CombatStatusComponent.h"
#include "EnemyAIComponent.h"
#include "EnemyAIPlayerLocatorComponent.h"
#include "AI/EnemySentinelAIComponent.h"
#include "ProjectileComponent.h"
#include "ProjectileShooterComponent.h"
#include "CursorControlComponent.h"
#include "UIPlayerStatsComponent.h"
#include "CollectibleComponent.h"

#include <LD55/LD55TypeIds.h>
namespace LD55
{
    class LD55Module
        : public AZ::Module
    {
    public:
        AZ_RTTI(LD55Module, LD55ModuleTypeId, AZ::Module);
        AZ_CLASS_ALLOCATOR(LD55Module, AZ::SystemAllocator);

        LD55Module()
            : AZ::Module()
        {
            // Push results of [MyComponent]::CreateDescriptor() into m_descriptors here.
            m_descriptors.insert(m_descriptors.end(), {
                EnemyAIPlayerLocatorComponent::CreateDescriptor(),
                LD55SystemComponent::CreateDescriptor(),
                FPSCameraComponent::CreateDescriptor(),
                PlayerControlComponent::CreateDescriptor(),
                GhettoParticle::CreateDescriptor(),
                GhettoParticleSystem::CreateDescriptor(),
                CombatStatusComponent::CreateDescriptor(),
                EnemyAIComponent::CreateDescriptor(),
                EnemySentinelAIComponent::CreateDescriptor(),
                ProjectileComponent::CreateDescriptor(),
                ProjectileShooterComponent::CreateDescriptor(),
                CursorControlComponent::CreateDescriptor(),
                UIPlayerStatsComponent::CreateDescriptor(),
                CollectibleComponent::CreateDescriptor(),
            });
        }

        /**
         * Add required SystemComponents to the SystemEntity.
         */
        AZ::ComponentTypeList GetRequiredSystemComponents() const override
        {
            return AZ::ComponentTypeList{
                azrtti_typeid<LD55SystemComponent>(),
            };
        }
    };
}// namespace LD55

AZ_DECLARE_MODULE_CLASS(Gem_LD55, LD55::LD55Module)
