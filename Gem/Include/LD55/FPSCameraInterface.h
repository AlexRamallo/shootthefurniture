#pragma once

#include <AzCore/Component/ComponentBus.h>
#include <AzCore/Math/Vector2.h>
#include "LD55/FPSCameraInterface.h"

namespace LD55
{
    class FPSCamera
        : public AZ::ComponentBus
    {
    public:
        AZ_RTTI(LD55::FPSCamera, "{3352DADB-EF21-441D-9E51-FE6A06F4BB2E}");
        virtual ~FPSCamera() = default;

        // Put your public request methods here.
        virtual void LookAt(AZ::Vector3 pos) = 0;
        virtual void RotateView(float delta_h, float delta_v) = 0;

        // virtual void Shake(float amount) = 0;
        // virtual void SetShakeMode...
        // TODO

        virtual void SetInvertHorizontal(bool set) = 0;
        virtual bool GetInvertHorizontal() {return false;}
        virtual void SetInvertVertical(bool set) = 0;
        virtual bool GetInvertVertical() {return false;}

        //used if the player e.g. can walk on walls
        virtual void SetPlayerBasis(AZ::Vector3 up, AZ::Vector3 right, AZ::Vector3 fwd) = 0;

        //If true, camera pitch will be clamped to [-PI/2, PI/2]
        virtual void SetClampPitch(bool set) = 0;

        virtual void SetSensitivity(AZ::Vector2 set) = 0;
        virtual AZ::Vector2 GetSensitivity() {return AZ::Vector2(1.0f, 1.0f);}

        // Put notification events here. Examples:
        // void RegisterEvent(AZ::EventHandler<...> notifyHandler);
        // AZ::Event<...> m_notifyEvent1;
    };

    using FPSCameraRequestBus = AZ::EBus<FPSCamera>;

} // namespace LD55
