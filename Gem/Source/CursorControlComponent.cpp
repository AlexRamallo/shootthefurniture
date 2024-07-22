#include "CursorControlComponent.h"

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzFramework/Input/Buses/Requests/InputSystemCursorRequestBus.h>
#include "LD55/LD55Helpers.h"

namespace LD55
{
	void CursorControlComponent::Reflect(AZ::ReflectContext *context)
	{
        if (AZ::SerializeContext* serializeContext = azrtti_cast<AZ::SerializeContext*>(context)) {
            serializeContext->Class<CursorControlComponent, AZ::Component>()
                ->Version(1)
                ;
            if (AZ::EditContext* editContext = serializeContext->GetEditContext()) {
                editContext->Class<CursorControlComponent>("CursorControlComponent", "Implements enemy AI for LD55")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::Category, "ComponentCategory")
                    ->Attribute(AZ::Edit::Attributes::Icon, "Icons/Components/Component_Placeholder.svg")
                    ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("Game"))
				;
            }
        }
	}
	
	void CursorControlComponent::Init()
	{
		//--
	}

	void CursorControlComponent::Activate()
	{
		StartingPointInput::InputEventNotificationBus::MultiHandler::BusConnect(OpenMenuEventId);
		StartingPointInput::InputEventNotificationBus::MultiHandler::BusConnect(InteractEventId);
		StartingPointInput::InputEventNotificationBus::MultiHandler::BusConnect(CancelEventId);
	}

	void CursorControlComponent::Deactivate()
	{
		StartingPointInput::InputEventNotificationBus::MultiHandler::BusDisconnect(OpenMenuEventId);
		StartingPointInput::InputEventNotificationBus::MultiHandler::BusDisconnect(InteractEventId);
		StartingPointInput::InputEventNotificationBus::MultiHandler::BusDisconnect(CancelEventId);
	}

	void CursorControlComponent::OnPressed([[maybe_unused]] float value)
	{
		auto inputId = StartingPointInput::InputEventNotificationBus::GetCurrentBusId();
		if(inputId == nullptr) return;

		if(*inputId == InteractEventId || *inputId == CancelEventId)
		{
			//lock mouse
			if(!m_isMouseLocked)
			{
				m_isMouseLocked = true;
				AzFramework::InputSystemCursorRequestBus::Broadcast(
					&AzFramework::InputSystemCursorRequests::SetSystemCursorState,
					AzFramework::SystemCursorState::ConstrainedAndHidden
				);
			}
		}
		if(*inputId == OpenMenuEventId)
		{
			//unlock mouse
			if(m_isMouseLocked)
			{
				m_isMouseLocked = false;
				AzFramework::InputSystemCursorRequestBus::Broadcast(
					&AzFramework::InputSystemCursorRequests::SetSystemCursorState,
					AzFramework::SystemCursorState::UnconstrainedAndVisible
				);
			}
		}
	}

}