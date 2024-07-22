#pragma once
#include <AzCore/Component/EntityId.h>
#include <AzCore/Math/Vector3.h>
#include <StartingPointInput/InputEventNotificationBus.h>

namespace LD55
{
    const StartingPointInput::InputEventNotificationId OpenMenuEventId("open_menu");
    const StartingPointInput::InputEventNotificationId MoveFwdEventId("move_forward");
    const StartingPointInput::InputEventNotificationId MoveRightEventId("move_right");
    const StartingPointInput::InputEventNotificationId LookUpEventId("look_up");
    const StartingPointInput::InputEventNotificationId LookRightEventId("look_right");
    const StartingPointInput::InputEventNotificationId InteractEventId("interact");
    const StartingPointInput::InputEventNotificationId CancelEventId("cancel");
    const StartingPointInput::InputEventNotificationId DoRunEventId("do_run");
    const StartingPointInput::InputEventNotificationId JumpEventId("jump");
    const StartingPointInput::InputEventNotificationId SwitchWeaponEventId("switch_weapon");
    const StartingPointInput::InputEventNotificationId ReloadWeaponEventId("reload_weapon");

    bool get_entity_position(const AZ::EntityId &id, AZ::Vector3 &out);
}