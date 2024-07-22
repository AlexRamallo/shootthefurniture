#pragma once
#include <AzCore/Component/Entity.h>
#include <AzCore/Component/Component.h>
#include <AzCore/Component/ComponentBus.h>
#include <AzCore/Math/Vector3.h>
#include <AzFramework/Spawnable/SpawnableEntitiesInterface.h>
#include <Atom/RPI.Public/Scene.h>
#include <Atom/RPI.Public/Model/Model.h>

namespace LD55
{
	class GhettoParticle;

	enum class GhettoEffectType
	{
		None,
		Burst,
		MuzzleFlash,
	};

	enum class GhettoEase
	{
		LinearInterpolation,
		QuadraticEaseIn,
		QuadraticEaseOut,
		QuadraticEaseInOut,
		CubicEaseIn,
		CubicEaseOut,
		CubicEaseInOut,
		QuarticEaseIn,
		QuarticEaseOut,
		QuarticEaseInOut,
		QuinticEaseIn,
		QuinticEaseOut,
		QuinticEaseInOut,
		SineEaseIn,
		SineEaseOut,
		SineEaseInOut,
		CircularEaseIn,
		CircularEaseOut,
		CircularEaseInOut,
		ExponentialEaseIn,
		ExponentialEaseOut,
		ExponentialEaseInOut,
		ElasticEaseIn,
		ElasticEaseOut,
		ElasticEaseInOut,
		BackEaseIn,
		BackEaseOut,
		BackEaseInOut,
		BounceEaseIn,
		BounceEaseOut,
		BounceEaseInOut,
	};

	struct GhettoParams {
		int count = -1;

		static constexpr float def_f = AZStd::numeric_limits<float>::lowest();
		static constexpr GhettoEase def_ease = GhettoEase::LinearInterpolation;
		static constexpr AZ::Vector3 def_vec3 = {};

		float m_lifetime = def_f;
		float m_baseScale = def_f;
		float m_startScale = def_f;
		float m_endScale = def_f;
		float m_startAlpha = def_f;
		float m_endAlpha = def_f;

		AZ::Vector3 m_gravity = def_vec3;
		GhettoEase m_easeScale = def_ease;
		GhettoEase m_easeAlpha = def_ease;
		AZ::Vector3 m_randomSpawnOffset = def_vec3;
		AZ::Vector3 m_spawnVelocity = def_vec3;
		AZ::Vector3 m_randomVelocity = def_vec3;

		//enables overriding default values for an effect
		void override(const GhettoParams &other);

		void print() const;
	};

	class GhettoParticleSystemRequests
        : public AZ::ComponentBus
	{
        AZ_RTTI(LD55::GhettoParticleSystemRequests, "{03ed8af9-d0e5-4d86-9832-9140f9c87666}");
	public:
		virtual void SpawnParticlesAt(GhettoEffectType type, const AZ::Vector3& pos, const AZ::Vector3& normal, const GhettoParams& params) = 0;
		virtual void SpawnParticlesAtDefault(GhettoEffectType type, const AZ::Vector3& pos, const AZ::Vector3& normal) = 0;
		virtual void KillAllParticles() = 0;
	};
    using GhettoParticleSystemRequestBus = AZ::EBus<GhettoParticleSystemRequests>;

	class GhettoParticleSystem
		: public AZ::Component
		, GhettoParticleSystemRequestBus::Handler
	{
	public:
		AZ_COMPONENT(LD55::GhettoParticleSystem, "{2d19bdc7-6b31-4d10-a361-b46a9490d00b}");
        static void Reflect(AZ::ReflectContext* context);

		// GhettoParticleSystemRequests
		void SpawnParticlesAtDefault(GhettoEffectType type, const AZ::Vector3& pos, const AZ::Vector3& normal) override;
		void SpawnParticlesAt(GhettoEffectType type, const AZ::Vector3& pos, const AZ::Vector3& normal, const GhettoParams& params) override;
		void KillAllParticles() override;
		//----------------------------------------

	protected:
		void Init() override;
		void Activate() override;
		void Deactivate() override;

		void fill_pool();
		void empty_pool();

		AZ::EntityId m_particleLookAtTarget;
		AZ::Data::Asset<AZ::RPI::MaterialAsset> m_particleMat;

		int m_poolSize = 50;
		AZ::Data::Asset<AzFramework::Spawnable> m_spawnableAsset;
		AZStd::vector<GhettoParticle*> pool;
	};

}