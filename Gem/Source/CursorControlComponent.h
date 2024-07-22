#pragma once
#include <AzCore/Component/Component.h>
#include <AzCore/Math/Vector3.h>
#include <StartingPointInput/InputEventNotificationBus.h>

#include "LD55/LD55Helpers.h"

namespace LD55
{
	/*
		Makes it possible to release the mouse by pressing the escape/enter keys
	*/
	class CursorControlComponent
		: public AZ::Component
		, private StartingPointInput::InputEventNotificationBus::MultiHandler
	{
	public:
		AZ_COMPONENT(LD55::CursorControlComponent, "{7f6b90a6-dff2-443f-bb6e-f77fb6348afb}");
        static void Reflect(AZ::ReflectContext* context);

		void Init() override;
		void Activate() override;
		void Deactivate() override;

        //StartingInput input events
        void OnPressed(float value) override;

	protected:
		bool m_isMouseLocked = true;
	};

}