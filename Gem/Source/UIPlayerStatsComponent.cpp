#include "UIPlayerStatsComponent.h"

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>

#include <LyShine/Bus/UiTransform2dBus.h>
#include <LyShine/Bus/UiImageBus.h>
#include <LyShine/Bus/UiTextBus.h>

#include "Source/Easing.h"

namespace LD55
{
	void UIPlayerStatsComponent::Reflect(AZ::ReflectContext *context)
	{
        if (AZ::SerializeContext* serializeContext = azrtti_cast<AZ::SerializeContext*>(context)) {
            serializeContext->Class<UIPlayerStatsComponent, AZ::Component>()
                ->Version(1)
                ->Field("Ammo Count", &UIPlayerStatsComponent::m_ammoCount)
                ->Field("Coin Count", &UIPlayerStatsComponent::m_coinCount)
                ->Field("Hurt Overlay", &UIPlayerStatsComponent::m_hurtOverlay)
                ->Field("Hurt Overlay Time", &UIPlayerStatsComponent::m_hurtOverlayTime)
                ;

            if (AZ::EditContext* editContext = serializeContext->GetEditContext()) {
                editContext->Class<UIPlayerStatsComponent>("UIPlayerStatsComponent", "Implements enemy AI for LD55")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::Category, "ComponentCategory")
                    ->Attribute(AZ::Edit::Attributes::Icon, "Icons/Components/Component_Placeholder.svg")
                    ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("UI"))
                    ->DataElement(AZ::Edit::UIHandlers::Default, &UIPlayerStatsComponent::m_ammoCount, "Ammo Count", "ammo count text label")
                    ->DataElement(AZ::Edit::UIHandlers::Default, &UIPlayerStatsComponent::m_coinCount, "Coin Count", "coin count text label")
                    ->DataElement(AZ::Edit::UIHandlers::Default, &UIPlayerStatsComponent::m_hurtOverlay, "Hurt Overlay", "hurt overlay image")
                    ->DataElement(AZ::Edit::UIHandlers::Default, &UIPlayerStatsComponent::m_hurtOverlayTime, "Hurt Overlay Time", "seconds to show hurt overlay when damaged")
				;
            }
        }
	}

	void UIPlayerStatsComponent::Init()
	{
		//--
	}

	void UIPlayerStatsComponent::Activate()
	{
		AZ_TracePrintf("UIPlayerStatsComponent", "Activate");
		UIPlayerStatsNotificationBus::Handler::BusConnect(GetEntityId());
		AZ::TickBus::Handler::BusConnect();
	}

	void UIPlayerStatsComponent::Deactivate()
	{
		AZ_TracePrintf("UIPlayerStatsComponent", "Deactivate");
		UIPlayerStatsNotificationBus::Handler::BusDisconnect(GetEntityId());
		AZ::TickBus::Handler::BusDisconnect();
	}

	void UIPlayerStatsComponent::OnTick([[maybe_unused]] float deltaTime, [[maybe_unused]] AZ::ScriptTimePoint time)
	{
		if(m_hurtOverlayTime_counter > 0)
		{
			float t = 1.0f - (m_hurtOverlayTime_counter / m_hurtOverlayTime);
			m_hurtOverlayTime_counter -= deltaTime;

			float alpha = 0;

			if(m_hurtOverlayTime_counter <= 0)
			{
				alpha = 0;
			}
			else
			{
				if(t < 0.5f)
				{
					alpha = Ease::CubicEaseOut(t / 0.5f);
				}
				else
				{
					alpha = 1.0f - Ease::CubicEaseIn((t - 0.5f) / 0.5f);
				}
			}

			UiImageBus::Event(m_hurtOverlay, &UiImageBus::Events::SetAlpha, alpha);
		}
	}

	void UIPlayerStatsComponent::OnHealthChanged(float health, float max_health, bool hurt)
	{
		float f = health / max_health;

		//AZ_TracePrintf("UIPlayerStatsComponent", "update_health_display: %f, hurt: %d", f, (int) hurt);

		if(hurt)
		{
			m_hurtOverlayTime_counter = m_hurtOverlayTime;
		}

		UiTransform2dBus::Event(
			GetEntityId(),
			&UiTransform2dInterface::SetLocalWidth,
			300.0f * f
		);
	}

	void UIPlayerStatsComponent::OnAmmoChanged(int clip, int clipSize, int bullets)
	{
//		AZ_TracePrintf("UIPlayerStatsComponent", "OnAmmoChanged: %d/%d | %d", clip, clipSize, bullets);
		char buffer[128];
		azsnprintf(buffer, 128, "Ammo: %d/%d | %d", clip, clipSize, bullets);
		UiTextBus::Event(
			m_ammoCount,
			&UiTextInterface::SetText,
			AZStd::string(buffer)
		);

		UiTextBus::Event(
			m_ammoCount,
			&UiTextInterface::SetColor,
			bullets > 0 ? AZ::Colors::Green : AZ::Colors::Red
		);
	}

	void UIPlayerStatsComponent::OnCoinsChanged(int new_count)
	{
		char buffer[128];
		azsnprintf(buffer, 128, "Beepers: %d / 9", new_count);
		UiTextBus::Event(
			m_coinCount,
			&UiTextInterface::SetText,
			AZStd::string(buffer)
		);
	}
}