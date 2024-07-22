#include "LD55/LD55Helpers.h"

#include <AzCore/Component/Entity.h>
#include <AzCore/Component/ComponentApplicationBus.h>
#include <AzCore/Component/TransformBus.h>

namespace LD55
{
    bool get_entity_position(const AZ::EntityId &id, AZ::Vector3 &out)
	{
		AZ::Entity *e = nullptr;
		AZ::ComponentApplicationBus::BroadcastResult(
			e,
			&AZ::ComponentApplicationRequests::FindEntity,
			id
		);
		if(e)
		{
			out = e->GetTransform()->GetWorldTranslation();
			return true;
		}
		else
		{
			return false;
		}
	}
}