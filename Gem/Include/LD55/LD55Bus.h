
#pragma once

#include <LD55/LD55TypeIds.h>

#include <AzCore/EBus/EBus.h>
#include <AzCore/Interface/Interface.h>

namespace LD55
{
    class LD55Requests
    {
    public:
        AZ_RTTI(LD55Requests, LD55RequestsTypeId);
        virtual ~LD55Requests() = default;
        // Put your public methods here
    };

    class LD55BusTraits
        : public AZ::EBusTraits
    {
    public:
        //////////////////////////////////////////////////////////////////////////
        // EBusTraits overrides
        static constexpr AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;
        static constexpr AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;
        //////////////////////////////////////////////////////////////////////////
    };

    using LD55RequestBus = AZ::EBus<LD55Requests, LD55BusTraits>;
    using LD55Interface = AZ::Interface<LD55Requests>;

} // namespace LD55
