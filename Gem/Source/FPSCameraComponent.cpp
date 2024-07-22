#include "Source/FPSCameraComponent.h"

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/RTTI/BehaviorContext.h>

#include <AzCore/Math/Vector3.h>
#include <AzFramework/Physics/SystemBus.h>
#include <AzCore/Time/ITime.h>

#include <AzCore/Component/TransformBus.h>
#include <AzFramework/Components/CameraBus.h>

static const float PI = 3.14159265f;
static const float PI2 = (PI * 2.0f);
static const float PITCH_CLAMP = PI * (0.5f - AZ::Constants::FloatEpsilon);

namespace LD55
{
	// AZ::Component
	void FPSCameraComponent::Init() {
		SetPlayerBasis(
			AZ::Vector3(0, 0, 1),
			AZ::Vector3(1, 0, 0),
			AZ::Vector3(0, 1, 0) 
		); 
		look_target = AZ::Quaternion::CreateIdentity();
		RotateView(0, 0);
	}
	void FPSCameraComponent::Activate() {
		FPSCameraRequestBus::Handler::BusConnect(GetEntityId());
		AZ::TickBus::Handler::BusConnect();
		EBUS_EVENT_ID_RESULT(fov_initial, GetEntityId(), Camera::CameraRequestBus, GetFovDegrees);
	}
	void FPSCameraComponent::Deactivate() {
		FPSCameraRequestBus::Handler::BusDisconnect(GetEntityId());
		AZ::TickBus::Handler::BusDisconnect();
	}

	void FPSCameraComponent::OnTick([[maybe_unused]] float deltaTime, [[maybe_unused]] AZ::ScriptTimePoint time) {
		if(slerp_view){
			AZ::Quaternion l_quat;
			EBUS_EVENT_ID_RESULT(l_quat, GetEntityId(), AZ::TransformBus, GetLocalRotationQuaternion);
			l_quat = l_quat.Slerp(look_target, deltaTime * slerp_speed);
			EBUS_EVENT_ID(GetEntityId(), AZ::TransformBus, SetLocalRotationQuaternion, l_quat);
		}
	}

	void FPSCameraComponent::SetPlayerBasis(AZ::Vector3 up, AZ::Vector3 right, AZ::Vector3 fwd) {
		q_player = AZ::Quaternion::CreateFromBasis(right, fwd, up);
		Up = up.GetNormalized();
		Right = right.GetNormalized();
		Fwd = fwd.GetNormalized();
	}

	void FPSCameraComponent::SetClampPitch(bool set) {
		if(!clamp_pitch && set){
			clamp_pitch = set;
			RotateView(0, 0);
		}else{
			clamp_pitch = set;
		}
	};

	void FPSCameraComponent::LookAt([[maybe_unused]] AZ::Vector3 pos) {
		// TODO
	}

	void FPSCameraComponent::RotateView(float delta_yaw, float delta_pitch) {
		auto deltaTime = TimeUsToSeconds(AZ::Interface<AZ::ITime>::Get()->GetSimulationTickDeltaTimeUs());

		yaw += delta_yaw * (-sensitivity.GetX()) * deltaTime;
		pitch += delta_pitch * (-sensitivity.GetY()) * deltaTime;
		
		yaw = fmod(yaw, PI2);

		pitch = AZ::GetClamp(fmod(pitch, PI2), -PITCH_CLAMP, PITCH_CLAMP);

		look_target =
			q_player *
			AZ::Quaternion::CreateFromAxisAngle(Up, yaw) *
			AZ::Quaternion::CreateFromAxisAngle(Right, pitch)
		;

		if(!slerp_view){
			EBUS_EVENT_ID(GetEntityId(), AZ::TransformBus, SetLocalRotationQuaternion, look_target);
		}
	}

	void FPSCameraComponent::SetInvertHorizontal(bool set) {
		invert_h = set;
	}
	bool FPSCameraComponent::GetInvertHorizontal() {
		return invert_h;
	}
	void FPSCameraComponent::SetInvertVertical(bool set) {
		invert_v = set;
	}
	bool FPSCameraComponent::GetInvertVertical() {
		return invert_v;
	}
	void FPSCameraComponent::SetSensitivity(AZ::Vector2 set) {
		sensitivity = set;
	}
	AZ::Vector2 FPSCameraComponent::GetSensitivity() {
		return sensitivity;
	}
}
