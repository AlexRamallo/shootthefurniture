#pragma once
#include <AzCore/Component/Component.h>
#include <AzCore/Math/Vector3.h>

#include "Source/EnemyAIPlayerLocatorComponent.h"

namespace LD55
{
	enum class EnemyAIType
	{
		None, 			//does nothing, but can die
		Sentinel, 		//static turret that can shoot player if seen + within range
		Brawler,    	//chases player on ground, causing damage on collision
		Kamikaze,   	//chases player by flying, causing damage on collision
		FlyShooter,		//flying, tries to stay within min dist from player, shoots
		GroundShooter,	//grounded, tries to stay within dist from player, shoots
	};



	class EnemyAIComponent
		: public AZ::Component
		, public EnemyAIPlayerLocatorNotificationBus::Handler
	{
	public:
		AZ_COMPONENT(LD55::EnemyAIComponent, "{09489b75-5967-43b7-aa27-d94e56e19569}");
        static void Reflect(AZ::ReflectContext* context);
        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

		void Init() override;
		void Activate() override;
		void Deactivate() override;

		void OnFoundPlayer(const AZ::EntityId &player, const AZ::Vector3 &fromPos, const AZ::Vector3 &playerPos) override;
		void OnLostPlayer(const AZ::EntityId &player, const AZ::Vector3 &fromPos, const AZ::Vector3 &lastPlayerPos) override;

	protected:
		AZ::EntityId m_aggroTarget;
		EnemyAIType type = EnemyAIType::None;
	};

}