#pragma once

#include <AzCore/Component/TransformBus.h>
#include <AzCore/Component/Component.h>
#include <AzCore/std/containers/vector.h>
#include <AzCore/std/containers/unordered_map.h>
#include <PhysX/CharacterGameplayBus.h>
#include <AzCore/Component/TickBus.h>
#include <Integration/AnimGraphComponentBus.h>
#include <Integration/AnimationBus.h>
#include <Integration/ActorComponentBus.h>
#include <StartingPointInput/InputEventNotificationBus.h>

#include "Source/CollectibleComponent.h"
#include "Source/CombatStatusComponent.h"

#include "LD55/LD55Helpers.h"

namespace LD55
{
	class FPSCameraComponent;

    class PlayerControlComponent
        : public AZ::Component
		, private StartingPointInput::InputEventNotificationBus::MultiHandler
		, public AZ::TickBus::Handler
        , private EMotionFX::Integration::ActorComponentNotificationBus::Handler
        , private EMotionFX::Integration::AnimGraphComponentNotificationBus::Handler
		, public EMotionFX::Integration::ActorNotificationBus::Handler
		, public CombatStatusNotificationBus::Handler
		, public CollectibleNotificationBus::Handler
    {
    public:
        AZ_COMPONENT(
            LD55::PlayerControlComponent,
            "{a30b4813-8d8e-446c-989c-84507ec0f099}"
        );

        static void Reflect(AZ::ReflectContext* context);
        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

		void OnDie() override;
		void OnHeal([[maybe_unused]] float amount) override;
		void OnDamaged([[maybe_unused]] float amount, [[maybe_unused]] float remaining, [[maybe_unused]] const AZ::Vector3& pos, [[maybe_unused]] const AZ::Vector3& normal) override;

		//Collectibles
		void OnCollected(CollectibleType type, int value, const AZ::Vector3 &pos) override;

	protected:
        void Init() override;
        void Activate() override;
        void Deactivate() override;

	private:
        EMotionFX::Integration::AnimGraphComponentRequests* m_animationGraph = nullptr;
		EMotionFX::Integration::ActorComponentRequests* m_actorRequests = nullptr;

        //! EMotionFX::Integration::AnimGraphComponentNotificationBus::Handler
		void OnStateEntered(const char* /*stateName*/) override;
		void OnStateExited(const char* /*stateName*/) override;


        //StartingInput input events
        void OnPressed(float value) override;
        void OnReleased(float value) override;
        void OnHeld(float value) override;

        bool ensure_refs();

	protected:
		float m_moveFwd = 0.0f;
		float m_moveRight = 0.0f;
		float m_lookUp = 0.0f;
		float m_lookRight = 0.0f;
		float m_moveFriction = 0.8f;
		float m_airControl = 0.04f;

		bool m_didJump = false;
		bool m_isRunning = false;

		float m_walkSpeed = 5.0f;
		float m_runSpeed = 9.0f;
		float m_jumpStrength = 5.0f;
		bool m_canRun = false;
		bool m_canJump = false;

		bool m_isMouseLocked = true;

		//--- AUDIO ----------------------------
		AZ::EntityId m_audioShoot;
		AZ::EntityId m_audioReload;
		AZ::EntityId m_audioWalk;
		AZ::EntityId m_audioHeal;
		AZ::EntityId m_audioCoin;
		bool m_audioWalkPlaying = false;
		//--------------------------------------
		int m_curCoins = 0;
		AZ::EntityId m_gameUi;

		int m_onGroundFrames = 0;

		struct {
			int key = 1;
			bool is_dialing = false;
			bool is_pressed = false;
			char dialed[7] = {-1,-1,-1,-1,-1,-1,-1};

			inline void clear_dial() {
				for(int i = 0; i < 7; i++){
					dialed[i] = -1;
				}
			}
		} m_dialState;

		struct {
			int32_t gun_bone_idx = -1;

			bool equipped = true;
			bool shooting = false;

			//like 'coyote time' but for the shoot button
			int shoot_input_sticky_frames = 3;
			int shoot_input_sticky_frames_counter = 0;

			bool reloading = false;

			int m_clip = 6;
			int m_clipSize = 6;
			int m_bullets = 36;

			bool can_shoot = true;
			float shoot_cooldown = 0.17f;
			float shoot_timer = 0.0f;

		} m_gunState;
		AZ::Vector3 muzzle_offset = AZ::Vector3::CreateZero();

		AZ::Vector3 m_moveVel = AZ::Vector3::CreateZero();
		AZ::Vector3 m_jumpVel = AZ::Vector3::CreateZero();

		void OnPlayerInteract();
		void OnPlayerCancel();

		void DoShoot();
		void DoReload();

		void UpdateMotion();
		void UpdateAnims();
		void OnTick(float deltaTime, AZ::ScriptTimePoint time) override;

		AZ::EntityId m_armsEntityId;
		AZ::Entity *m_armsEntity = nullptr;

		AZ::EntityId m_fpscamEntityId;
		AZ::Entity *m_fpscamEntity = nullptr;

		AZ::EntityId m_muzzleFlashEntityId;
		AZ::Entity *m_muzzleFlashEntity = nullptr;

		FPSCameraComponent *m_fpscam = nullptr;
    };

};