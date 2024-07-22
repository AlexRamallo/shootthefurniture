#include "GhettoParticleSystem.h"
#include "Source/GhettoParticle.h"

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/RTTI/BehaviorContext.h>
#include <AzCore/Asset/AssetSerializer.h>
#include <AzCore/Math/Transform.h>
#include <AzFramework/Components/TransformComponent.h>
#include <AzCore/std/smart_ptr/make_shared.h>
#include <AzCore/Math/Random.h>
#include <Atom/RPI.Public/Model/Model.h>
#include <Atom/RPI.Public/Scene.h>


namespace LD55
{

	void GhettoParticleSystem::Reflect(AZ::ReflectContext *context)
	{
		if (AZ::SerializeContext* serializeContext = azrtti_cast<AZ::SerializeContext*>(context)) {
			serializeContext->Class<GhettoParticleSystem, AZ::Component>()
				->Version(1)
				->Field("Particle Asset", &GhettoParticleSystem::m_spawnableAsset)
				->Field("Pool Size", &GhettoParticleSystem::m_poolSize)
				->Field("LookAt Target", &GhettoParticleSystem::m_particleLookAtTarget)
				->Field("Particle Material Asset", &GhettoParticleSystem::m_particleMat)
			;

			if (AZ::EditContext* editContext = serializeContext->GetEditContext()) {
				editContext->Class<GhettoParticleSystem>("GhettoParticleSystem", "Crappy particle system")
					->ClassElement(AZ::Edit::ClassElements::EditorData, "")
					->Attribute(AZ::Edit::Attributes::Category, "ComponentCategory")
					->Attribute(AZ::Edit::Attributes::Icon, "Icons/Components/Component_Placeholder.svg")
					->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("Game"))
                    ->DataElement(AZ::Edit::UIHandlers::Default, &GhettoParticleSystem::m_spawnableAsset, "Particle Asset", "Particle asset to spawn")
                    ->DataElement(AZ::Edit::UIHandlers::Default, &GhettoParticleSystem::m_particleMat, "Particle Material Asset", "Material asset that will be instantiated for spawned particle")
                    ->DataElement(AZ::Edit::UIHandlers::Default, &GhettoParticleSystem::m_poolSize, "Pool Size", "Size of particle pool")
                    ->DataElement(AZ::Edit::UIHandlers::Default, &GhettoParticleSystem::m_particleLookAtTarget, "LookAt Target", "Entity that particles will look at for billboard effect")
				;
			}
		}
	}

	void GhettoParticleSystem::Init()
	{
		//--
	}
	void GhettoParticleSystem::Activate()
	{
		GhettoParticleSystemRequestBus::Handler::BusConnect(GetEntityId());
		fill_pool();
		AZ_TracePrintf("GhettoSystem", "Pool size: %d", (int)pool.size());
	}
	void GhettoParticleSystem::Deactivate()
	{
		GhettoParticleSystemRequestBus::Handler::BusDisconnect(GetEntityId());
		empty_pool();
	}
	void GhettoParticleSystem::empty_pool()
	{
		AZ_TracePrintf("GhettoSys", "Emptying pool (cur size: %d)", pool.size());
		for(GhettoParticle *particle: pool)
		{
			AzFramework::SpawnableEntitiesInterface::Get()->DespawnAllEntities(*(particle->m_ticket));
			particle->m_ticket = nullptr;
		}
		pool.clear();
	}
	void GhettoParticleSystem::fill_pool()
	{
		AZ_TracePrintf("GhettoSys", "Filling pool to size: %d", m_poolSize);
		for(int i = 0; i < m_poolSize; i++){
			auto m_ticket = AZStd::make_shared<AzFramework::EntitySpawnTicket>(m_spawnableAsset);
			auto postSpawnCB = [this, m_ticket](
				[[maybe_unused]] AzFramework::EntitySpawnTicket::Id, AzFramework::SpawnableConstEntityContainerView view
			){

				AZ::Entity* m_lookAt = nullptr;
				AZ::ComponentApplicationBus::BroadcastResult(m_lookAt, &AZ::ComponentApplicationBus::Events::FindEntity, m_particleLookAtTarget);
				if(!m_lookAt)
				{
					AZ_TracePrintf("GhettoSys", "Particle LookAt target not found!");
				}

				for(const AZ::Entity *e : view){
					if(!e) continue;

					auto trans = e->GetTransform();
					if(!trans) continue;
					AZStd::vector<AZ::EntityId> children = trans->GetAllDescendants();
					for(AZ::EntityId &child : children){

            			AZ::Entity* childEntity = nullptr;
            			AZ::ComponentApplicationBus::BroadcastResult(childEntity, &AZ::ComponentApplicationBus::Events::FindEntity, child);
            			if(childEntity){
							GhettoParticle* particle = childEntity->FindComponent<GhettoParticle>();
							if (particle)
							{

								particle->m_life = 0;
								particle->m_ticket = m_ticket;
								particle->m_lookAt = m_lookAt;
								pool.push_back(particle);
								particle->m_materialInstance = AZ::RPI::Material::Create(m_particleMat);

								AZ::Render::MaterialAssignmentMap map = {
										{
											AZ::Render::DefaultMaterialAssignmentId,
											{
												/*asset*/ particle->m_materialInstance->GetAsset(),
												/*instance*/ particle->m_materialInstance
											}
										}
								};

								EBUS_EVENT_ID(
									child,
									AZ::Render::MaterialComponentRequestBus,
									SetMaterialMap,
									map
								);

								return;
							}
						}
					}
				}
			};

			if (m_ticket->IsValid())
			{
//				AZ_TracePrintf("GhettoSys", "EntitySpawnTicket is VALID");
				AzFramework::SpawnAllEntitiesOptionalArgs optionalArgs;
				optionalArgs.m_completionCallback = AZStd::move(postSpawnCB);
				AzFramework::SpawnableEntitiesInterface::Get()->SpawnAllEntities(*m_ticket, AZStd::move(optionalArgs));
			}
			else
			{
				AZ_TracePrintf("GhettoSys", "EntitySpawnTicket is INVALID");
				AZ_Assert(m_ticket->IsValid(), "Unable to instantiate spawnable asset");
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	static AHEasingFunction get_ease_func(GhettoEase ease)
	{
		switch (ease) {
            default: return Ease::LinearInterpolation;
			case GhettoEase::LinearInterpolation: return Ease::LinearInterpolation;
			case GhettoEase::QuadraticEaseIn: return Ease::QuadraticEaseIn;
			case GhettoEase::QuadraticEaseOut: return Ease::QuadraticEaseOut;
			case GhettoEase::QuadraticEaseInOut: return Ease::QuadraticEaseInOut;
			case GhettoEase::CubicEaseIn: return Ease::CubicEaseIn;
			case GhettoEase::CubicEaseOut: return Ease::CubicEaseOut;
			case GhettoEase::CubicEaseInOut: return Ease::CubicEaseInOut;
			case GhettoEase::QuarticEaseIn: return Ease::QuarticEaseIn;
			case GhettoEase::QuarticEaseOut: return Ease::QuarticEaseOut;
			case GhettoEase::QuarticEaseInOut: return Ease::QuarticEaseInOut;
			case GhettoEase::QuinticEaseIn: return Ease::QuinticEaseIn;
			case GhettoEase::QuinticEaseOut: return Ease::QuinticEaseOut;
			case GhettoEase::QuinticEaseInOut: return Ease::QuinticEaseInOut;
			case GhettoEase::SineEaseIn: return Ease::SineEaseIn;
			case GhettoEase::SineEaseOut: return Ease::SineEaseOut;
			case GhettoEase::SineEaseInOut: return Ease::SineEaseInOut;
			case GhettoEase::CircularEaseIn: return Ease::CircularEaseIn;
			case GhettoEase::CircularEaseOut: return Ease::CircularEaseOut;
			case GhettoEase::CircularEaseInOut: return Ease::CircularEaseInOut;
			case GhettoEase::ExponentialEaseIn: return Ease::ExponentialEaseIn;
			case GhettoEase::ExponentialEaseOut: return Ease::ExponentialEaseOut;
			case GhettoEase::ExponentialEaseInOut: return Ease::ExponentialEaseInOut;
			case GhettoEase::ElasticEaseIn: return Ease::ElasticEaseIn;
			case GhettoEase::ElasticEaseOut: return Ease::ElasticEaseOut;
			case GhettoEase::ElasticEaseInOut: return Ease::ElasticEaseInOut;
			case GhettoEase::BackEaseIn: return Ease::BackEaseIn;
			case GhettoEase::BackEaseOut: return Ease::BackEaseOut;
			case GhettoEase::BackEaseInOut: return Ease::BackEaseInOut;
			case GhettoEase::BounceEaseIn: return Ease::BounceEaseIn;
			case GhettoEase::BounceEaseOut: return Ease::BounceEaseOut;
			case GhettoEase::BounceEaseInOut: return Ease::BounceEaseInOut;
		}
	}

	void GhettoParams::print() const
	{
		AZ_TracePrintf("GhettoParams", "=== GhettoParams::print() ===");
		AZ_TracePrintf("GhettoParams", "\tcount: %d", count);
		AZ_TracePrintf("GhettoParams", "\tm_lifetime: %f", m_lifetime);
		AZ_TracePrintf("GhettoParams", "\tm_baseScale: %f", m_baseScale);
		AZ_TracePrintf("GhettoParams", "\tm_startScale: %f", m_startScale);
		AZ_TracePrintf("GhettoParams", "\tm_endScale: %f", m_endScale);
		AZ_TracePrintf("GhettoParams", "\tm_startAlpha: %f", m_startAlpha);
		AZ_TracePrintf("GhettoParams", "\tm_endAlpha: %f", m_endAlpha);
		AZ_TracePrintf("GhettoParams", "\tm_gravity: [%f, %f, %f]", m_gravity.GetX(), m_gravity.GetY(), m_gravity.GetZ());
		AZ_TracePrintf("GhettoParams", "\tm_randomSpawnOffset: [%f, %f, %f]", m_randomSpawnOffset.GetX(), m_randomSpawnOffset.GetY(), m_randomSpawnOffset.GetZ());
		AZ_TracePrintf("GhettoParams", "\tm_spawnVelocity: [%f, %f, %f]", m_spawnVelocity.GetX(), m_spawnVelocity.GetY(), m_spawnVelocity.GetZ());
		AZ_TracePrintf("GhettoParams", "\tm_randomVelocity: [%f, %f, %f]", m_randomVelocity.GetX(), m_randomVelocity.GetY(), m_randomVelocity.GetZ());
	}

	static AZStd::unordered_map<GhettoEffectType, GhettoParams> default_particle_effect_config = {
		{
			GhettoEffectType::Burst, {
                /*.count*/ 10,
                /*.m_lifetime*/ 0.30f,
                /*.m_baseScale*/ 10.0f,
                /*.m_startScale*/ 0.0f,
				/*.m_endScale*/ 1.0f,
				/*.m_startAlpha*/ 1.0f,
				/*.m_endAlpha*/ 0.0f,
				/*.m_gravity*/ AZ::Vector3(0, 0, -9.8f),
				/*.m_easeScale*/ GhettoEase::CubicEaseOut,
				/*.m_easeAlpha*/ GhettoEase::CubicEaseOut,
				/*.m_randomSpawnOffset*/ AZ::Vector3::CreateZero(),
                /*.m_spawnVelocity*/ AZ::Vector3(0.0f, 0.0f, 0.0f),
                /*.m_randomVelocity*/ AZ::Vector3(1.0f, 1.0f, 1.0f),
			}
		},
		{
			GhettoEffectType::MuzzleFlash, {
                /*.count*/ 10,
                /*.m_lifetime*/ 0.20f,
                /*.m_baseScale*/ 1.0f,
                /*.m_startScale*/ 0.2f,
				/*.m_endScale*/ 1.0f,
				/*.m_startAlpha*/ 1.0f,
				/*.m_endAlpha*/ 0.0f,
				/*.m_gravity*/ AZ::Vector3::CreateZero(),
				/*.m_easeScale*/ GhettoEase::CubicEaseOut,
				/*.m_easeAlpha*/ GhettoEase::CubicEaseOut,
				/*.m_randomSpawnOffset*/ AZ::Vector3::CreateZero(),
                /*.m_spawnVelocity*/ AZ::Vector3(0.0f, 0.0f, 0.0f),
                /*.m_randomVelocity*/ AZ::Vector3(2.0f, 2.0f, 2.0f),
			}
		},
	};

	void GhettoParams::override(const GhettoParams &other)
	{
		if(other.count >= 0) count = other.count;
		if(other.m_lifetime != GhettoParams::def_f) m_lifetime = other.m_lifetime;
		if(other.m_baseScale != GhettoParams::def_f) m_baseScale = other.m_baseScale;
		if(other.m_startScale != GhettoParams::def_f) m_startScale = other.m_startScale;
		if(other.m_endScale != GhettoParams::def_f) m_endScale = other.m_endScale;
		if(other.m_startAlpha != GhettoParams::def_f) m_startAlpha = other.m_startAlpha;
		if(other.m_endAlpha != GhettoParams::def_f) m_endAlpha = other.m_endAlpha;
		if(other.m_gravity != GhettoParams::def_vec3) m_gravity = other.m_gravity;
		if(other.m_easeScale != GhettoParams::def_ease) m_easeScale = other.m_easeScale;
		if(other.m_easeAlpha != GhettoParams::def_ease) m_easeAlpha = other.m_easeAlpha;
		if(other.m_randomSpawnOffset != GhettoParams::def_vec3) m_randomSpawnOffset = other.m_randomSpawnOffset;
		if(other.m_spawnVelocity != GhettoParams::def_vec3) m_spawnVelocity = other.m_spawnVelocity;
		if(other.m_randomVelocity != GhettoParams::def_vec3) m_randomVelocity = other.m_randomVelocity;
	}

	static AZ::SimpleLcgRandom random;

	static void setup_particle(
		GhettoParticle *particle,
		[[maybe_unused]] GhettoEffectType type,
		const AZ::Vector3 &pos,
		[[maybe_unused]] const AZ::Vector3 &normal,
		const GhettoParams &params
	){
		particle->params = params;

		AZ::Entity *entity = particle->GetEntity();
		AZ::TransformInterface *trans =entity->GetTransform();

		AZ::Vector3 offset = AZ::Vector3(
			random.GetRandomFloat() - 0.5f,
			random.GetRandomFloat() - 0.5f,
			random.GetRandomFloat() - 0.5f
		);
		AZ::Vector3 spawn_pos = pos + (params.m_randomSpawnOffset * offset);
		AZ::Vector3 vel_rand = params.m_randomVelocity * AZ::Vector3(
			random.GetRandomFloat() - 0.5f,
			random.GetRandomFloat() - 0.5f,
			random.GetRandomFloat() - 0.5f
		);
		AZ::Vector3 spawn_velocity = params.m_spawnVelocity + vel_rand;

		particle->m_life = params.m_lifetime;
		particle->m_velocity = spawn_velocity;
		particle->m_fpAlphaEase = get_ease_func(params.m_easeScale);
		particle->m_fpScaleEase = get_ease_func(params.m_easeAlpha);
		trans->SetWorldTranslation(spawn_pos);

		switch(type)
		{
			default: return;
			case GhettoEffectType::Burst:
				particle->m_velocity += normal * 4.0f * (0.5f + random.GetRandomFloat());
				break;
		}
	}

	//------------------------------------------------------------------------------------------------------------------

	void GhettoParticleSystem::SpawnParticlesAtDefault(GhettoEffectType type, const AZ::Vector3& pos, const AZ::Vector3& normal)
	{
		GhettoParams params;
		SpawnParticlesAt(type, pos, normal, params);
	}

	void GhettoParticleSystem::SpawnParticlesAt(GhettoEffectType type, const AZ::Vector3 &pos, const AZ::Vector3& normal, const GhettoParams &params_in)
	{
		if(type == GhettoEffectType::None)
		{
			return;
		}

		GhettoParams params = default_particle_effect_config[type];
		params.override(params_in);

//		params.print();

		int count = params.count;
		for(GhettoParticle *p: pool)
		{
			if(count <= 0)
			{
				break;
			}

			if(!p->is_alive())
			{
				count -= 1;
				setup_particle(p, type, pos, normal, params);
			}
		}

		if(count > 0)
		{
			AZ_TracePrintf("GhettoSys", "Failed to create %d particles (pool size: %d)", count, pool.size());
		}
	}

	void GhettoParticleSystem::KillAllParticles()
	{
		for(GhettoParticle *p: pool)
		{
			p->m_life = 0;
		}
	}

}