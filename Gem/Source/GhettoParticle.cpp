#include "GhettoParticle.h"
#include "GhettoParticleSystem.h"
#include <AzCore/Component/Entity.h>
#include <AzCore/Component/TransformBus.h>

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/RTTI/BehaviorContext.h>

#ifndef M_PI
	#define M_PI 3.141592653589f
#endif

#ifndef M_PI_2
	#define M_PI_2 1.57079632679f
#endif

namespace LD55
{
	void GhettoParticle::Reflect(AZ::ReflectContext *context)
	{
        if (AZ::SerializeContext* serializeContext = azrtti_cast<AZ::SerializeContext*>(context)) {
            serializeContext->Class<GhettoParticle, AZ::Component>()
                ->Version(1)
                ;

            if (AZ::EditContext* editContext = serializeContext->GetEditContext()) {
                editContext->Class<GhettoParticle>("GhettoParticle", "Crappy particle system")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::Category, "ComponentCategory")
                    ->Attribute(AZ::Edit::Attributes::Icon, "Icons/Components/Component_Placeholder.svg")
                    ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("Game"))
				;
            }
        }
	}

	void GhettoParticle::Init()
	{
		//AZ_TracePrintf("GhettoParticle", "Init");
	}

	void GhettoParticle::Activate()
	{
		//AZ_TracePrintf("GhettoParticle", "Activate");
		AZ::TickBus::Handler::BusConnect();
	}

	void GhettoParticle::Deactivate()
	{
		//AZ_TracePrintf("GhettoParticle", "Deactivate");
		AZ::TickBus::Handler::BusDisconnect();
	}

	bool GhettoParticle::is_alive()
	{
		return m_life > 0;
	}

	static const AZStd::string transparencyParamName = "settings.alpha";

	void GhettoParticle::OnTick([[maybe_unused]] float deltaTime, [[maybe_unused]] AZ::ScriptTimePoint _time)
	{
		if(m_life <= 0)
		{
			return;
		}
		if(params.count <= 0 || params.m_lifetime == 0)
		{
			//AZ_TracePrintf("GhettoParticle", "Invalid particle (alive, but params is invalid)");
			return;
		}

		float time = 1.0f - (m_life / params.m_lifetime);
		m_life -= deltaTime;

		[[maybe_unused]] float e_alpha = m_fpAlphaEase(time);
		[[maybe_unused]] float f_alpha = params.m_startAlpha + ((params.m_endAlpha - params.m_startAlpha) * e_alpha);
		[[maybe_unused]] float e_scale = m_fpScaleEase(time);
		[[maybe_unused]] float f_scale = params.m_startScale + ((params.m_endScale - params.m_startScale) * e_scale);

		EBUS_EVENT_ID(
			GetEntityId(),
			AZ::Render::MaterialComponentRequestBus,
			SetPropertyValue,
			AZ::Render::DefaultMaterialAssignmentId,
			transparencyParamName,
			AZStd::any(f_alpha)
		);

		m_velocity += params.m_gravity * deltaTime;

		AZ::TransformInterface *trans = GetEntity()->GetTransform();
		AZ::Vector3 newPos;

		if (m_life <= 0){
			newPos = AZ::Vector3(0,0,-10000);
		}
		else
		{
			newPos = trans->GetWorldTranslation() + (m_velocity * deltaTime);
		}

		if(m_lookAt != nullptr)
		{
			AZ::Vector3 lookat_pos = m_lookAt->GetTransform()->GetWorldTranslation();
			auto new_tm = AZ::Transform::CreateLookAt(
				newPos,
				lookat_pos,
				AZ::Transform::Axis::YPositive
			);
			new_tm.SetUniformScale(params.m_baseScale * f_scale);
			trans->SetWorldTM(new_tm);
		}else{
			trans->SetLocalUniformScale(params.m_baseScale * f_scale);
			trans->SetWorldTranslation(newPos);
		}


//		newPos = trans->GetWorldTranslation();
//		//AZ_TracePrintf("Ghetto", "[%s] Particle tick: life = %f, scale = %f (e: %f), alpha = %f (e: %f), pos = (%f, %f, %f)",
//			GetEntityId().ToString().c_str(), m_life,
//			params.m_baseScale * f_scale, e_scale,
//			f_alpha, e_alpha,
//			newPos.GetX(), newPos.GetY(), newPos.GetZ()
//		);
//		params.print();
	}

}
