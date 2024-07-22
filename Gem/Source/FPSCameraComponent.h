
#pragma once

#include <AzCore/Component/Component.h>
#include <LD55/FPSCameraInterface.h>
#include <AzCore/Component/TickBus.h>
#include <AzCore/Math/Quaternion.h>

namespace LD55
{

    class FPSCameraComponent
        : public AZ::Component
        , public FPSCameraRequestBus::Handler
        , public AZ::TickBus::Handler
    {

    public:
        AZ_COMPONENT(
            LD55::FPSCameraComponent,
            "{0076B5F2-4069-44E2-AD63-082E631EC6F5}"
        );

        ////////////
        void OnTick(float deltaTime, AZ::ScriptTimePoint time) override;

        static void Reflect(AZ::ReflectContext* context);
        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

        void LookAt(AZ::Vector3 pos) override;
        void RotateView(float delta_h, float delta_v) override;

        void SetInvertHorizontal(bool set) override;
        bool GetInvertHorizontal() override;
        void SetInvertVertical(bool set) override;
        bool GetInvertVertical() override;
        void SetSensitivity(AZ::Vector2 set) override;
        AZ::Vector2 GetSensitivity() override;

        void SetPlayerBasis(AZ::Vector3 up, AZ::Vector3 right, AZ::Vector3 fwd) override;

        void SetClampPitch(bool set) override;

    protected:
        void Init() override;
        void Activate() override;
        void Deactivate() override;

        AZ::Quaternion q_player, look_target;
        AZ::Vector3 Up, Right, Fwd;

        float yaw = 0.0f;
        float pitch = 0.0f;

        bool clamp_pitch = true;
        bool slerp_view = true;
        float slerp_speed = 15.0f;

        float fov_initial = 0.0f;

        AZ::Vector2 sensitivity = AZ::Vector2(1.0f, 1.0f);
        bool invert_h = false;
        bool invert_v = false;
    };
}
