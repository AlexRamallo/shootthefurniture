#include "Source/PlayerControlComponent.h"
#include "Source/FPSCameraComponent.h"
#include <AzCore/Component/ComponentApplicationBus.h>
#include <AzCore/Component/Entity.h>
#include <AzFramework/Physics/CharacterBus.h>
#include <AzCore/Component/TransformBus.h>
#include <PhysX/CharacterGameplayBus.h>
#include <AzFramework/Physics/PhysicsScene.h>
#include <DebugDraw/DebugDrawBus.h>
#include <MiniAudio/MiniAudioPlaybackBus.h>

#include "UIPlayerStatsComponent.h"

#include <AtomLyIntegration/CommonFeatures/CoreLights/AreaLightBus.h>
#include <AzFramework/Components/ConsoleBus.h>
#include <AzFramework/Input/Buses/Requests/InputSystemCursorRequestBus.h>

#include "Source/CombatStatusComponent.h"
#include "Source/GhettoParticleSystem.h"
#include "LD55/LD55Helpers.h"

namespace LD55 {
	void PlayerControlComponent::Init()
	{
		//--
	}

	void PlayerControlComponent::Activate()
	{
		StartingPointInput::InputEventNotificationBus::MultiHandler::BusConnect(MoveFwdEventId);
		StartingPointInput::InputEventNotificationBus::MultiHandler::BusConnect(MoveRightEventId);
		StartingPointInput::InputEventNotificationBus::MultiHandler::BusConnect(LookUpEventId);
		StartingPointInput::InputEventNotificationBus::MultiHandler::BusConnect(LookRightEventId);
		StartingPointInput::InputEventNotificationBus::MultiHandler::BusConnect(InteractEventId);
		StartingPointInput::InputEventNotificationBus::MultiHandler::BusConnect(CancelEventId);
		StartingPointInput::InputEventNotificationBus::MultiHandler::BusConnect(DoRunEventId);
		StartingPointInput::InputEventNotificationBus::MultiHandler::BusConnect(JumpEventId);
		StartingPointInput::InputEventNotificationBus::MultiHandler::BusConnect(SwitchWeaponEventId);
		StartingPointInput::InputEventNotificationBus::MultiHandler::BusConnect(ReloadWeaponEventId);
		AZ::TickBus::Handler::BusConnect();
		CombatStatusNotificationBus::Handler::BusConnect(GetEntityId());
		CollectibleNotificationBus::Handler::BusConnect(GetEntityId());
	}


	void PlayerControlComponent::Deactivate()
	{
		AZ::TickBus::Handler::BusDisconnect();
		StartingPointInput::InputEventNotificationBus::MultiHandler::BusDisconnect(MoveFwdEventId);
		StartingPointInput::InputEventNotificationBus::MultiHandler::BusDisconnect(MoveRightEventId);
		StartingPointInput::InputEventNotificationBus::MultiHandler::BusDisconnect(LookUpEventId);
		StartingPointInput::InputEventNotificationBus::MultiHandler::BusDisconnect(LookRightEventId);
		StartingPointInput::InputEventNotificationBus::MultiHandler::BusDisconnect(InteractEventId);
		StartingPointInput::InputEventNotificationBus::MultiHandler::BusDisconnect(CancelEventId);
		StartingPointInput::InputEventNotificationBus::MultiHandler::BusDisconnect(DoRunEventId);
		StartingPointInput::InputEventNotificationBus::MultiHandler::BusDisconnect(JumpEventId);
		StartingPointInput::InputEventNotificationBus::MultiHandler::BusDisconnect(SwitchWeaponEventId);
		StartingPointInput::InputEventNotificationBus::MultiHandler::BusDisconnect(ReloadWeaponEventId);
		EMotionFX::Integration::ActorComponentNotificationBus::Handler::BusDisconnect(m_armsEntityId);
		EMotionFX::Integration::ActorNotificationBus::Handler::BusDisconnect(m_armsEntityId);
		CombatStatusNotificationBus::Handler::BusDisconnect(GetEntityId());
		CollectibleNotificationBus::Handler::BusDisconnect(GetEntityId());
	}

	void PlayerControlComponent::DoShoot()
	{
		auto* sceneInterface = AZ::Interface<AzPhysics::SceneInterface>::Get();
		AZ_Assert(sceneInterface, "Physics system must be initialized");

		AzPhysics::SceneHandle sceneHandle = sceneInterface->GetSceneHandle(AzPhysics::DefaultPhysicsSceneName);
		AZ_Assert(sceneHandle != AzPhysics::InvalidSceneHandle, "Default Physics world must be created");

		AZ::TransformInterface *camT = m_fpscamEntity->GetTransform();
		const AZ::Transform &camWorld = camT->GetWorldTM();

		AzPhysics::RayCastRequest request;
		request.m_direction = camWorld.GetBasisY();
		request.m_start = camWorld.GetTranslation();
		request.m_distance = 1000.0f;
		request.m_queryType = AzPhysics::SceneQuery::QueryType::StaticAndDynamic;
		request.m_collisionGroup = AzPhysics::CollisionGroup("shootable");
		request.m_reportMultipleHits = false;
		request.m_filterCallback = [this](const AzPhysics::SimulatedBody* body, [[maybe_unused]] const Physics::Shape* shape){
			if(body->GetEntityId() == GetEntityId()){
				return AzPhysics::SceneQuery::QueryHitType::None;
			}
			return AzPhysics::SceneQuery::QueryHitType::Block;
		};

		MiniAudio::MiniAudioPlaybackRequestBus::Event(m_audioShoot, &MiniAudio::MiniAudioPlaybackRequestBus::Events::Play);

		//MuzzleFlash
		if(m_gunState.gun_bone_idx < 0)
		{
			m_gunState.gun_bone_idx = (int32_t) m_actorRequests->GetJointIndexByName("gun_bone");
		}

		m_animationGraph->SetNamedParameterBool("gun_shoot", m_gunState.shooting);

		EBUS_EVENT_ID(
			m_muzzleFlashEntityId,
			AZ::Render::AreaLightRequestBus,
			SetIntensityAndMode,
			10.0f,
			AZ::Render::PhotometricUnit::Lumen
		);

		AZ::Vector3 muzzle_flash_pos;

		AZ::Transform gunTrans = m_actorRequests->GetJointTransform(m_gunState.gun_bone_idx, EMotionFX::Integration::Space::WorldSpace);
		muzzle_flash_pos = gunTrans.GetTranslation();

		muzzle_flash_pos += (gunTrans.GetBasisX() * muzzle_offset.GetX());
		muzzle_flash_pos += (gunTrans.GetBasisY() * muzzle_offset.GetY());
		muzzle_flash_pos += (gunTrans.GetBasisZ() * muzzle_offset.GetZ());

		GhettoParticleSystemRequestBus::Broadcast(
			&GhettoParticleSystemRequests::SpawnParticlesAtDefault,
			GhettoEffectType::MuzzleFlash,
			muzzle_flash_pos,
			request.m_direction
		);

		AzPhysics::SceneQueryHits results = sceneInterface->QueryScene(sceneHandle, &request);

		float damage_amount = 1.0f;
		CombatStatusRequestBus::EventResult(
			damage_amount,
			GetEntityId(),
			&CombatStatusRequests::GetAttackStrength
		);

		for(const AzPhysics::SceneQueryHit &hit: results.m_hits)
		{
//			DebugDraw::DebugDrawRequestBus::Broadcast
//			(
//				&DebugDraw::DebugDrawRequests::DrawLineLocationToLocation,
//				hit.m_position,
//				hit.m_position + hit.m_normal,
//				AZ::Colors::Red,
//				10.0f
//			);

			CombatStatusRequestBus::Event(
				hit.m_entityId,
				&CombatStatusRequests::Hurt,
				damage_amount,
				hit.m_position,
				(request.m_start - hit.m_position).GetNormalized()
//				hit.m_normal
			);

			GhettoParticleSystemRequestBus::Broadcast(
				&GhettoParticleSystemRequests::SpawnParticlesAtDefault,
				GhettoEffectType::Burst,
				hit.m_position,
				hit.m_normal
			);
		}
	}

	void PlayerControlComponent::OnPlayerInteract()
	{
		if(!m_gunState.reloading && m_gunState.can_shoot && m_gunState.equipped && !m_gunState.shooting)
		{
			if(m_gunState.m_clip > 0)
			{
				m_gunState.m_clip -= 1;
				DoShoot();
				m_gunState.shoot_timer = m_gunState.shoot_cooldown;
				m_gunState.shooting = true;
				UIPlayerStatsNotificationBus::Broadcast(
					&UIPlayerStatsNotifications::OnAmmoChanged,
					m_gunState.m_clip,
					m_gunState.m_clipSize,
					m_gunState.m_bullets
				);
			}
			else
			{
				DoReload();
			}
		}
//		else
//		{
//			if(m_gunState.reloading){
//				AZ_TracePrintf("shoot", "Couldn't shoot: reloading is true");
//			}
//			else if(!m_gunState.can_shoot){
//				AZ_TracePrintf("shoot", "Couldn't shoot: can_shoot is false");
//			}
//			else if(!m_gunState.equipped){
//				AZ_TracePrintf("shoot", "Couldn't shoot: equipped is false");
//			}
//			else if(m_gunState.shooting){
//				AZ_TracePrintf("shoot", "Couldn't shoot: shooting is true");
//			}
//		}
	}

	void PlayerControlComponent::DoReload()
	{
		if(!m_gunState.reloading)
		{
			if(m_gunState.m_bullets > 0)
			{
				MiniAudio::MiniAudioPlaybackRequestBus::Event(m_audioReload, &MiniAudio::MiniAudioPlaybackRequestBus::Events::Play);
				m_gunState.reloading = true;
				int needed = m_gunState.m_clipSize - m_gunState.m_clip;
				int got = AZStd::min(m_gunState.m_bullets, needed);
				m_gunState.m_clip += got;
				m_gunState.m_bullets -= got;

				UIPlayerStatsNotificationBus::Broadcast(
					&UIPlayerStatsNotifications::OnAmmoChanged,
					m_gunState.m_clip,
					m_gunState.m_clipSize,
					m_gunState.m_bullets
				);
			}
			else
			{
				//TODO: play sound?
			}
		}
	}

	void PlayerControlComponent::OnPlayerCancel()
	{
		AZ_TracePrintf("PlayerControlComponent", "OnPlayerCancel()");
	}

	bool PlayerControlComponent::ensure_refs()
	{
		if(m_fpscam == nullptr)
		{
			AZ::ComponentApplicationBus::BroadcastResult(m_fpscamEntity, &AZ::ComponentApplicationBus::Events::FindEntity, m_fpscamEntityId);
			if(m_fpscamEntity)
			{
				m_fpscam = m_fpscamEntity->FindComponent<FPSCameraComponent>();
			}
			else
			{
				AZ_TracePrintf("PlayerControlComponent", "Failed to find FPS Camera Entity!");
				return false;
			}
		}

		if(m_armsEntity == nullptr)
		{
			AZ::ComponentApplicationBus::BroadcastResult(m_armsEntity, &AZ::ComponentApplicationBus::Events::FindEntity, m_armsEntityId);
			if(!m_armsEntity)
			{
				AZ_TracePrintf("PlayerControlComponent", "Failed to find Arms Entity!");
				return false;
			}
			else
			{
				EMotionFX::Integration::ActorComponentNotificationBus::Handler::BusConnect(m_armsEntityId);
				EMotionFX::Integration::ActorNotificationBus::Handler::BusConnect(m_armsEntityId);
			}
		}
		else
		{
			if(m_animationGraph == nullptr)
			{
				m_animationGraph = EMotionFX::Integration::AnimGraphComponentRequestBus::FindFirstHandler(m_armsEntityId);
				if(m_animationGraph == nullptr)
				{
					AZ_TracePrintf("PlayerControlComponent", "Failed to find arms animation graph!");
					return false;
				}
			}

			if(m_actorRequests == nullptr)
			{
				m_actorRequests = EMotionFX::Integration::ActorComponentRequestBus::FindFirstHandler(m_armsEntityId);
				if(m_actorRequests == nullptr)
				{
					AZ_TracePrintf("PlayerControlComponent", "Failed to find arms actorRequests!");
					return false;
				}
			}
		}

		if(m_muzzleFlashEntity == nullptr)
		{
			AZ::ComponentApplicationBus::BroadcastResult(m_muzzleFlashEntity, &AZ::ComponentApplicationBus::Events::FindEntity, m_muzzleFlashEntityId);
			if(!m_muzzleFlashEntity)
			{
				AZ_TracePrintf("PlayerControlComponent", "Failed to find Muzzle Flash light Entity!");
				return false;
			}
		}

		return true;
	}

	void PlayerControlComponent::OnStateEntered([[maybe_unused]] const char* stateName)
	{
		//--
	}

	void PlayerControlComponent::OnStateExited([[maybe_unused]] const char* stateName)
	{
		if(!strcmp(stateName, "GunShoot"))
		{
			m_gunState.shooting = false;
			UpdateAnims();
		}
		else if(!strcmp(stateName, "GunReload"))
		{
			m_gunState.reloading = false;
			UpdateAnims();
		}
	}

	void PlayerControlComponent::UpdateAnims()
	{
		if(!ensure_refs()) return;

		bool isOnGround = false;
		PhysX::CharacterGameplayRequestBus::BroadcastResult(isOnGround, &PhysX::CharacterGameplayRequestBus::Events::IsOnGround);

		if(!m_moveVel.IsZero() && isOnGround){
			m_animationGraph->SetNamedParameterFloat("walk_speed", 1.0f);
			if(!m_audioWalkPlaying)
			{
				m_audioWalkPlaying = true;
				MiniAudio::MiniAudioPlaybackRequestBus::Event(m_audioWalk, &MiniAudio::MiniAudioPlaybackRequestBus::Events::Play);
			}

		}else{
			m_animationGraph->SetNamedParameterFloat("walk_speed", 0.0f);
			if(m_audioWalkPlaying)
			{
				m_audioWalkPlaying = false;
				MiniAudio::MiniAudioPlaybackRequestBus::Event(m_audioWalk, &MiniAudio::MiniAudioPlaybackRequestBus::Events::Stop);
			}
		}

		m_animationGraph->SetNamedParameterBool("gun_equipped", m_gunState.equipped);
		m_animationGraph->SetNamedParameterBool("gun_shoot", m_gunState.shooting);
		m_animationGraph->SetNamedParameterBool("gun_reload", m_gunState.reloading);
	}

	void PlayerControlComponent::UpdateMotion(){
		if(!ensure_refs()) return;

		m_fpscam->RotateView(m_lookRight, m_lookUp);
		m_lookRight = 0.0f;
		m_lookUp = 0.0f;

		bool isOnGround = false;
		PhysX::CharacterGameplayRequestBus::BroadcastResult(isOnGround, &PhysX::CharacterGameplayRequestBus::Events::IsOnGround);

		if (m_moveRight != 0 || m_moveFwd != 0){
			auto cam_worldTM = m_fpscamEntity->GetTransform()->GetWorldTM();

			AZ::Vector3 move_vel = (cam_worldTM.GetBasisY() * m_moveFwd) + (cam_worldTM.GetBasisX() * m_moveRight);
			move_vel.SetZ(0.0f);
			move_vel.Normalize();

			if(m_isRunning){
				move_vel *= m_runSpeed;
			}else{
				move_vel *= m_walkSpeed;
			}

			if(isOnGround){
				m_moveVel += move_vel;
			}else{
				AZ::Vector3 new_vec = m_moveVel + (move_vel * m_airControl);
				m_moveVel = new_vec;
			}

			if(m_moveVel.GetLength() > m_walkSpeed){
				m_moveVel = m_moveVel.GetNormalized() * m_walkSpeed;
			}
		}

		if(m_canJump){
			if(m_didJump && isOnGround){
				m_didJump = false;
				m_jumpVel = AZ::Vector3::CreateAxisZ(m_jumpStrength);
				m_moveVel *= m_moveFriction;
			}
		}else{
			m_jumpVel.Set(0,0,0);
		}
	}

	void PlayerControlComponent::OnTick([[maybe_unused]] float deltaTime, [[maybe_unused]] AZ::ScriptTimePoint time) {
		ensure_refs();
		UpdateAnims();

		if(m_gunState.shoot_input_sticky_frames_counter > 0)
		{
			m_gunState.shoot_input_sticky_frames_counter--;
			OnPlayerInteract();
		}

		if(m_gunState.shoot_timer > 0.0f)
		{
			m_gunState.shoot_timer -= deltaTime;
			if(m_gunState.shoot_timer < m_gunState.shoot_cooldown * 0.8){
				EBUS_EVENT_ID(
					m_muzzleFlashEntityId,
					AZ::Render::AreaLightRequestBus,
					SetIntensityAndMode,
					0.0f,
					AZ::Render::PhotometricUnit::Lumen
				);
			}
		}

		bool isOnGround = false;
		PhysX::CharacterGameplayRequestBus::BroadcastResult(isOnGround, &PhysX::CharacterGameplayRequestBus::Events::IsOnGround);

		if(isOnGround){
			m_onGroundFrames += 1;
			if(m_moveVel.GetLength() > 0.01f){
				m_moveVel *= m_moveFriction;
			}else{
				m_moveVel.Set(0,0,0);
			}
		}else{
			m_onGroundFrames = 0;
		}

		if(isOnGround && m_onGroundFrames == 1){
			m_jumpVel.Set(0,0,0);
		}

		AZ::Vector3 cur_vel = (m_moveVel + m_jumpVel) * deltaTime;
		Physics::CharacterRequestBus::Event(
                GetEntityId(),
                &Physics::CharacterRequests::AddVelocityForPhysicsTimestep,
                cur_vel
        );
	}

	void PlayerControlComponent::OnPressed(float value)
	{
		auto inputId = StartingPointInput::InputEventNotificationBus::GetCurrentBusId();
		if(inputId == nullptr) return;

		if(*inputId == InteractEventId)
		{
			m_gunState.shoot_input_sticky_frames_counter = m_gunState.shoot_input_sticky_frames;
			OnPlayerInteract();
		}
		if(*inputId == CancelEventId)
		{
			OnPlayerCancel();
		}

		if(*inputId == MoveFwdEventId) m_moveFwd = value;
		if(*inputId == MoveRightEventId) m_moveRight = value;
		if(*inputId == LookUpEventId) m_lookUp = value;
		if(*inputId == LookRightEventId) m_lookRight = value;
		if(*inputId == LookRightEventId) m_lookRight = value;
		if(*inputId == DoRunEventId) m_isRunning = true;
		if(*inputId == JumpEventId) m_didJump = true;

		if(*inputId == SwitchWeaponEventId)
		{
			m_gunState.equipped = !m_gunState.equipped;
		}

		if(m_gunState.equipped)
		{
			if(*inputId == ReloadWeaponEventId)
			{
				DoReload();
			}
		}

		UpdateMotion();
	}
	void PlayerControlComponent::OnReleased([[maybe_unused]] float value) {
		auto inputId = StartingPointInput::InputEventNotificationBus::GetCurrentBusId();
		if(inputId == nullptr) return;
		
		if(*inputId == MoveFwdEventId) m_moveFwd = 0.0f;
		if(*inputId == MoveRightEventId) m_moveRight = 0.0f;

		if(*inputId == DoRunEventId) m_isRunning = false;
		if(*inputId == JumpEventId) m_didJump = false;
		UpdateMotion();
	}
	void PlayerControlComponent::OnHeld(float value) {
		auto inputId = StartingPointInput::InputEventNotificationBus::GetCurrentBusId();
		if(inputId == nullptr) return;

		if(*inputId == MoveFwdEventId) m_moveFwd = value;
		if(*inputId == MoveRightEventId) m_moveRight = value;
		if(*inputId == LookUpEventId) m_lookUp = value;
		if(*inputId == LookRightEventId) m_lookRight = value;
		UpdateMotion();
	}

	static void update_player_ui_stats(AZ::EntityId playerid, bool hurt)
	{
		float health = 1;
		float max_health = 1;

		CombatStatusRequestBus::EventResult(
			max_health,
			playerid,
			&CombatStatusRequests::GetMaxHealth
		);

		CombatStatusRequestBus::EventResult(
			health,
			playerid,
			&CombatStatusRequests::GetHealth
		);

		UIPlayerStatsNotificationBus::Broadcast(
			&UIPlayerStatsNotifications::OnHealthChanged,
			health,
			max_health,
			hurt
		);
	}

	void PlayerControlComponent::OnDie()
	{
		//update_player_ui_stats(GetEntityId(), true);
		AzFramework::ConsoleRequestBus::Broadcast(
			&AzFramework::ConsoleRequestBus::Events::ExecuteConsoleCommand,
			"LoadLevel GameOver"
		);
	}

	void PlayerControlComponent::OnHeal([[maybe_unused]] float amount)
	{
		update_player_ui_stats(GetEntityId(), false);
	}

	void PlayerControlComponent::OnDamaged([[maybe_unused]] float amount, [[maybe_unused]] float remaining, [[maybe_unused]] const AZ::Vector3& pos, [[maybe_unused]] const AZ::Vector3& normal)
	{
		update_player_ui_stats(GetEntityId(), true);
	}

	void PlayerControlComponent::OnCollected(CollectibleType type, int value, [[maybe_unused]] const AZ::Vector3 &pos)
	{
		switch (type)
		{
			case CollectibleType::Health:
				CombatStatusRequestBus::Event(
					GetEntityId(),
					&CombatStatusRequestBus::Events::Heal,
					(float)value
				);
				MiniAudio::MiniAudioPlaybackRequestBus::Event(m_audioHeal, &MiniAudio::MiniAudioPlaybackRequestBus::Events::Play);
				break;
			case CollectibleType::Coin:
				m_curCoins += 1;
				UIPlayerStatsNotificationBus::Broadcast(
					&UIPlayerStatsNotifications::OnCoinsChanged,
					m_curCoins
				);
				MiniAudio::MiniAudioPlaybackRequestBus::Event(m_audioCoin, &MiniAudio::MiniAudioPlaybackRequestBus::Events::Play);
				break;
			case CollectibleType::Ammo:
				m_gunState.m_bullets += value;
				UIPlayerStatsNotificationBus::Broadcast(
					&UIPlayerStatsNotifications::OnAmmoChanged,
					m_gunState.m_clip,
					m_gunState.m_clipSize,
					m_gunState.m_bullets
				);
				MiniAudio::MiniAudioPlaybackRequestBus::Event(m_audioReload, &MiniAudio::MiniAudioPlaybackRequestBus::Events::Play);
				break;
			default:
				return;
		}
	}
};
