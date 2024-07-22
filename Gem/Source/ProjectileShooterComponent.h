#pragma once
#include <AzCore/Component/Component.h>
#include <AzCore/Math/Vector3.h>
#include <AzCore/Component/TickBus.h>
#include <AtomLyIntegration/CommonFeatures/Material/MaterialComponentBus.h>
#include <AzFramework/Spawnable/SpawnableEntitiesInterface.h>
#include <Atom/RPI.Public/Scene.h>
#include <Atom/RPI.Public/Model/Model.h>

namespace LD55
{
	/**
	 * This is basically just a copy&paste of GhettoParticle/System
	 *
	 * TODO: create a generic global entity pool system to consolidate these two
	 * */

	class ProjectileComponent;

	class ProjectileShooterRequests
		: public AZ::ComponentBus
	{
	public:
		virtual AZ::EntityId ShootProjectile(const AZ::Vector3 &at, const AZ::Vector3 &dir, float speed, bool hitPlayer) = 0;
	};
	using ProjectileShooterRequestBus = AZ::EBus<ProjectileShooterRequests>;

	class ProjectileShooterComponent
		: public AZ::Component
		, public ProjectileShooterRequestBus::Handler
	{
	public:
		AZ_COMPONENT(LD55::ProjectileShooterComponent, "{92a4f7da-0a0a-4842-a629-c3c806877410}");
        static void Reflect(AZ::ReflectContext* context);
        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);

		AZ::EntityId ShootProjectile(const AZ::Vector3 &at, const AZ::Vector3 &dir, float speed, bool hitPlayer) override;

	protected:
		void Init() override;
		void Activate() override;
		void Deactivate() override;

		void fill_pool();
		void empty_pool();
		int m_poolSize = 10;
		AZ::Data::Asset<AzFramework::Spawnable> m_spawnableAsset;
		AZStd::vector<ProjectileComponent*> pool;
        AZ::Vector3 projectile_offset = AZ::Vector3{0,0,0.5f};
	};
}
