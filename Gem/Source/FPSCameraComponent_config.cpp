
#include "Source/FPSCameraComponent.h"

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/RTTI/BehaviorContext.h>

namespace LD55 {
    void FPSCameraComponent::Reflect(AZ::ReflectContext* context) {
        if (AZ::SerializeContext* serializeContext = azrtti_cast<AZ::SerializeContext*>(context)) {
            serializeContext->Class<FPSCameraComponent, AZ::Component>()
                ->Version(1)
                ->Field("Sensitivity", &FPSCameraComponent::sensitivity)
                ->Field("Enable View Slerp", &FPSCameraComponent::slerp_view)
                ->Field("View Slerp Speed", &FPSCameraComponent::slerp_speed)
                ->Field("Invert Horizontal", &FPSCameraComponent::invert_h)
                ->Field("Invert Vertical", &FPSCameraComponent::invert_v)
                ;

            if (AZ::EditContext* editContext = serializeContext->GetEditContext()) {
                editContext->Class<FPSCameraComponent>("FPSCameraComponent", "Implements FPS camera features")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::Category, "ComponentCategory")
                    ->Attribute(AZ::Edit::Attributes::Icon, "Icons/Components/Component_Placeholder.svg")
                    ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("Game"))
                    ->DataElement(AZ::Edit::UIHandlers::Default, &FPSCameraComponent::sensitivity, "Camera Sensitivity", "Controls horizontal/vertical camera sensitivity")
                    ->DataElement(AZ::Edit::UIHandlers::Default, &FPSCameraComponent::invert_h, "Invert Horizontal", "If enabled, horizontal view inputs will be inverted")
                    ->DataElement(AZ::Edit::UIHandlers::Default, &FPSCameraComponent::invert_v, "Invert Vertical", "If enabled, vertical view inputs will be inverted")
                    ->DataElement(AZ::Edit::UIHandlers::Default, &FPSCameraComponent::slerp_view, "Enable View Slerp", "If enabled, camera will slerp towards view target rather than instantly move")
                    ->DataElement(AZ::Edit::UIHandlers::Default, &FPSCameraComponent::slerp_speed, "Slerp Speed", "Slerp speed factor (only if Enable View Slerp is on)")
                ;
            }
        }

        if (AZ::BehaviorContext* behaviorContext = azrtti_cast<AZ::BehaviorContext*>(context)) {
            behaviorContext->Class<FPSCameraComponent>("FPSCameraComponent")
                ->RequestBus("FPSCameraRequestBus")
                ;

            behaviorContext->EBus<FPSCameraRequestBus>("FPSCameraRequestBus")
                ->Event("LookAt", &FPSCameraRequestBus::Events::LookAt, "Turn the camera to look at a specific point")
                ->Event("RotateView", &FPSCameraRequestBus::Events::RotateView, "Add to view vector")
                ->Event("SetInvertHorizontal", &FPSCameraRequestBus::Events::SetInvertHorizontal, "Set horizontal invert value")
                ->Event("GetInvertHorizontal", &FPSCameraRequestBus::Events::GetInvertHorizontal, "Get horizontal invert value")
                ->Event("SetInvertVertical", &FPSCameraRequestBus::Events::SetInvertVertical, "Set vertical invert value")
                ->Event("GetInvertVertical", &FPSCameraRequestBus::Events::GetInvertVertical, "Get vertical invert value")
                ->Event("SetSensitivity", &FPSCameraRequestBus::Events::GetSensitivity, "Set camera sensitivity")
                ->Event("GetSensitivity", &FPSCameraRequestBus::Events::GetSensitivity, "Get camera sensitivity")
            ;
        }
    }

    void FPSCameraComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided) {
        provided.push_back(AZ_CRC_CE("FPSCameraComponentService"));
    }

    void FPSCameraComponent::GetIncompatibleServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& incompatible) {
        //--
    }

    void FPSCameraComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required) {
        required.push_back(AZ_CRC("CameraService"));
    }

    void FPSCameraComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent) {
        //--
    }
}
