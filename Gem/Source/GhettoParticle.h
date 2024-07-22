#pragma once
#include <AzCore/Component/Component.h>
#include <AzCore/Math/Vector3.h>
#include <AzCore/Component/TickBus.h>
#include <AtomLyIntegration/CommonFeatures/Material/MaterialComponentBus.h>
#include <AzFramework/Spawnable/SpawnableEntitiesInterface.h>
#include <Atom/RPI.Public/Scene.h>
#include <Atom/RPI.Public/Model/Model.h>

#include "Source/Easing.h"
#include "Source/GhettoParticleSystem.h"

namespace LD55
{
	class GhettoParticle
		: public AZ::Component
		, public AZ::TickBus::Handler
	{
	public:
		AZ_COMPONENT(LD55::GhettoParticle, "{ed200e1e-9c6c-4479-91ed-95afb5f3fc6f}");
        static void Reflect(AZ::ReflectContext* context);

        bool is_alive();

	protected:
		void Init() override;
		void Activate() override;
		void Deactivate() override;
		void OnTick(float deltaTime, AZ::ScriptTimePoint time) override;

	public:
		AZStd::shared_ptr<AzFramework::EntitySpawnTicket> m_ticket;
		GhettoParams params;
		AZ::Data::Instance<AZ::RPI::Material> m_materialInstance;
		AZ::Entity *m_lookAt = nullptr;
		float m_life = 0.0f;
		AZ::Vector3 m_velocity = AZ::Vector3::CreateAxisZ(0.0f);
		AHEasingFunction m_fpScaleEase = Ease::LinearInterpolation;
		AHEasingFunction m_fpAlphaEase = Ease::LinearInterpolation;
	};
}
