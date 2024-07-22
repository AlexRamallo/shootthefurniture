
#pragma once

#include <AzCore/Component/Component.h>

#include <LD55/LD55Bus.h>

namespace LD55
{
    class LD55SystemComponent
        : public AZ::Component
        , protected LD55RequestBus::Handler
    {
    public:
        AZ_COMPONENT_DECL(LD55SystemComponent);

        static void Reflect(AZ::ReflectContext* context);

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

        LD55SystemComponent();
        ~LD55SystemComponent();

    protected:
        ////////////////////////////////////////////////////////////////////////
        // LD55RequestBus interface implementation

        ////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////
        // AZ::Component interface implementation
        void Init() override;
        void Activate() override;
        void Deactivate() override;
        ////////////////////////////////////////////////////////////////////////
    };
}
