#pragma once
#include "SDK.hpp"

namespace cheat
{
	static SDK::UWorld* world = {};
	static SDK::UCanvas* canvas = {};
	static SDK::AActor* local_player = {};
	static SDK::APlayerController* controller = {};
	static SDK::APlayerCameraManager* camera = {};
	static SDK::AFortAthenaVehicle* target_car = {};
	static SDK::AFortAthenaZipline* target_zipline = {};
	static SDK::UObject* target_wall = {};
	static SDK::UClass* wall_class = {};
	static SDK::UKismetMathLibrary* math = {};
	static SDK::UGameplayStatics* stats = {};
	static SDK::UEngine* engine = {};
	static SDK::UKismetRenderingLibrary* renderer = {};
	static SDK::FKey F6;
	static SDK::FKey W, A, S, D;
	static SDK::UFont* Font;
	static SDK::UFortKismetLibrary* lib;

	namespace SilentAim_Leak
	{
		static SDK::AFortWeaponRanged* local_weapon = {};
		static void init_silentAim(SDK::AFortWeapon* weapon)
		{
			local_weapon = reinterpret_cast<SDK::AFortWeaponRanged*>(weapon);
			
		}
	}

	namespace MyCanvas
	{
		SDK::UCanvas* Draw{};
		SDK::FDrawToRenderTargetContext Context{}; // storage
		SDK::UTextureRenderTarget2D* rendering_target{};

		// Cast all drawings
		void Init(SDK::AHUD* hud)
		{
			// window format
			auto windowSize = SDK::FVector2D{ (float)hud->DebugCanvas->SizeX, (float)hud->DebugCanvas->SizeY };

			// render target
			rendering_target = renderer->CreateRenderTarget2D(world, windowSize.X, windowSize.Y,
				SDK::ETextureRenderTargetFormat{});

			// colors
			rendering_target->RenderTargetFormat = SDK::ETextureRenderTargetFormat::RTF_MAX;

			// init & make background clear
			renderer->ClearRenderTarget2D(world, rendering_target, SDK::FLinearColor{ 0.f, 0.f, 0.f, 0.f });
			renderer->BeginDrawCanvasToRenderTarget(world, rendering_target, &Draw,
				&windowSize, &Context);

			// scaling
			Context.RenderTarget->TargetGamma = 1.f;

			// font
			if (!Font)
			{
				Font = reinterpret_cast<SDK::UEngine*>(world->OwningGameInstance->Outer)->MediumFont;
			}
		}
	}

	namespace decoy_test
	{
		/*
		* requires some way more work to allow for us to actually controll it from our GUI.
		*/

		static SDK::AFortAIPawn* decoy_pawn{};
		static SDK::UClass* decoy_class{};
		void create_decoy(SDK::FVector cord_spawn)
		{
			if (!decoy_class) decoy_class = SDK::AFortAIPawn::StaticClass();
			decoy_pawn =
				reinterpret_cast<SDK::AFortAIPawn*>(stats->SpawnObject(decoy_class, decoy_class->Outer));

			/*
			* setup settings
			*/
			decoy_pawn->AutoPossessAI = SDK::EAutoPossessAI::PlacedInWorld;
			decoy_pawn->K2_TeleportTo(cord_spawn, {/* no set rotation in the future this should be changed*/});
			decoy_pawn->PrototypeCharacterMovement(SDK::EMovementMode::MOVE_Walking, local_player->GetVelocity()); /*
																											 begings walking toward us
																											 */

			/*
			* lets give our actor some basic settings in case they aren't auto-set
			*/

			decoy_pawn->SetActorTickEnabled(true);
			decoy_pawn->SetActorTickInterval(1);
			decoy_pawn->ResetAIRotationRateToDefault();
			decoy_pawn->SetCanInteract(true);
			decoy_pawn->SetCanSleep(false); // lets keep our actor constantly active
			decoy_pawn->SetActorScale3D(local_player->RootComponent->RelativeScale3D);

			/*
			* lets make some basic settings for our "AI" to follow
			*/

			// only need the scale of the length not the height.
			decoy_pawn->AddMovementInput(math->Conv_RotatorToVector(decoy_pawn->RootComponent->RelativeRotation), decoy_pawn->RootComponent->RelativeScale3D.X, true);
			decoy_pawn->SetNetDormancy(SDK::ENetDormancy::DORM_Awake); /*
																	   no disconnect
																	   */

			if (decoy_pawn->BasedMovement.bServerHasBaseComponent)
			{
				/*
				* server has loaded our decoy player
				*/
			}
		}
	}

	namespace chams_objects
	{
		SDK::TArray<SDK::AActor*> containers{};
		
		void load_containers()
		{
			stats->GetAllActorsOfClass(world, SDK::ABuildingContainer::StaticClass(), &containers);

			for (int x = 0; x < containers.Num(); x++)
			{
				SDK::ABuildingActor* actor = reinterpret_cast<SDK::ABuildingActor*>(containers[x]);
				if (!actor) continue;

				// highlight object
				actor->SetQuestHighlight(true); // yk that Blue/Yellow color on chests/ammo boxes sometimes
			}
		}
	}

	/*
	* use same setup for wall wireframes and other base materials like floors, etc..
	*/
	namespace wall_AI
	{
		void make_wall_spam()
		{
			wall_class = SDK::ABuildingWall::StaticClass();
			target_wall = stats->SpawnObject(wall_class, wall_class->Outer); /* creates wall object */
			/* enables wireframe on the walls material */
			reinterpret_cast<SDK::ABuildingWall*>(target_wall)->BaseMaterial->GetBaseMaterial()->Wireframe = true;
		}
	}

	namespace fakeLag_component
	{
		void make_local_lag(SDK::APlayerState* local_state)
		{
			local_state->bShouldUpdateReplicatedPing = false;
			local_state->Ping = 69; /*
									spoof DUH PING
									*/

			/*
			* now lets make our character FAKE LAGGGG YURR
			*/
			auto old_location = local_player->RootComponent->K2_GetComponentLocation();
			reinterpret_cast<SDK::ACharacter*>(local_player)->CharacterMovement->NetworkSmoothingMode = SDK::ENetworkSmoothingMode::Replay;
			for (int x = 0; x < 3; x++)
			{
				// location should change in 3 ticks while this is constantly running
				reinterpret_cast<SDK::ACharacter*>(local_player)->CharacterMovement->AddImpulse(old_location, true);
			}
		}
	}

	namespace aimbot
	{
		static SDK::FRotator targeted_rotation = {};
		static SDK::FVector predicted_location = {};
 		static SDK::FRotator aimbot_rotation = {};

		static SDK::FVector local_location = {};
		static SDK::FRotator local_rotation = {};

		void move_to(SDK::AActor* targeted_player)
		{
			targeted_rotation = targeted_player->RootComponent->RelativeRotation;
			predicted_location = math->GetRightVector(targeted_rotation);

			// proximation estimate
			predicted_location.Y = math->RadiansToDegrees(predicted_location.Y);

			// different from camera this is our actual view point
			controller->GetActorEyesViewPoint( &local_location, &local_rotation);
			local_location = math->GetUpVector(local_rotation);

			// final rotation & corrections
			aimbot_rotation = math->FindLookAtRotation(local_location, predicted_location);

			controller->AddYawInput(aimbot_rotation.Yaw);
			controller->AddPitchInput(aimbot_rotation.Pitch);
		}
	}

	namespace ESP
	{
		static SDK::TArray<SDK::AActor*> player_pawns{};
		static SDK::TArray<SDK::AActor*> rift_pawns{};
		static SDK::TArray<SDK::AActor*> car_pawns{};
		static SDK::TArray<SDK::AActor*> weak_spots{};
		static SDK::TArray<SDK::AActor*> pickup_pawns{};

		// test teleport
		static SDK::AFortAthenaRiftPortal* teleport_device{};

		/*
		* its seperate so it can be based on toggles
		*/
		void world_esp()
		{
			stats->GetAllActorsOfClass(world, SDK::AFortAthenaRiftPortal::StaticClass(), &rift_pawns);
			stats->GetAllActorsOfClass(world, SDK::AFortAthenaVehicle::StaticClass(), &car_pawns);
			stats->GetAllActorsOfClass(world, SDK::ABuildingWeakSpot::StaticClass(), &weak_spots);
			stats->GetAllActorsOfClass(world, SDK::AFortPickupAthena::StaticClass(), &pickup_pawns);

			for (int x = 0; x < rift_pawns.Num(); x++)
			{
				SDK::AFortAthenaRiftPortal* actor = reinterpret_cast<SDK::AFortAthenaRiftPortal*>(rift_pawns[x]);
				if (!actor) continue;

				teleport_device = actor;
			}

			for (int x = 0; x < car_pawns.Num(); x++)
			{
				SDK::AFortAthenaVehicle* actor = reinterpret_cast<SDK::AFortAthenaVehicle*>(car_pawns[x]);
				if (!actor) continue;

				// no car damage
				actor->DamageSet->HealthDamageScale.CurrentValue = 0;
			
				// car collosion
				actor->SetActorEnableCollision(true);
			
				// car less weight
				actor->GravityMultiplier = 0;

				// instant switch seats
				actor->SeatSwitchCooldown = 0;

				// car fly
				SDK::FVector loc = actor->RootComponent->RelativeLocation;
				SDK::FVector upper_right = math->GetRightVector(actor->RootComponent->RelativeRotation);
				float steering_angle = actor->GetSteeringAngle();
				if (controller->IsInputKeyDown(W))
				{
					loc.X = upper_right.X + (15 * steering_angle);
				}
				else if (controller->IsInputKeyDown(A))
				{
					loc.Y = upper_right.Y - (15 * steering_angle);
				}
				else if (controller->IsInputKeyDown(S))
				{
					loc.X = upper_right.X - (15 * steering_angle);
				}
				else if (controller->IsInputKeyDown(D))
				{
					loc.Y = upper_right.Y + (15 * steering_angle);
				}

				/*
				* game already updates rotation lol
				*/
				teleport_device->TeleportVehicle(actor, { loc });

				// teleport to plane intersect
				SDK::FVector plane_intersect = { 0,0,0 };
				teleport_device->TeleportVehicle(actor, plane_intersect);
			}

			for (int x = 0; x < weak_spots.Num(); x++)
			{
				SDK::ABuildingWeakSpot* actor = reinterpret_cast<SDK::ABuildingWeakSpot*>(weak_spots[x]);
				if (!actor) continue;

				// weakspot aimbot
				if (actor->bActive && !actor->bHidden)
				{
					aimbot::move_to( actor );
				}
			}
			for (int x = 0; x < pickup_pawns.Num(); x++)
			{
				SDK::AFortPickupAthena* actor = reinterpret_cast<SDK::AFortPickupAthena*>(pickup_pawns[x]);
				if (!actor) continue;
				if (!actor->PrimaryPickupItemEntry.ItemDefinition) continue;
				
				// loot names
				SDK::FString pickupIdentifier = actor->PrimaryPickupItemEntry.ItemDefinition->DisplayName.TextData;
				SDK::EFortItemTier tier = actor->PrimaryPickupItemEntry.ItemDefinition->Tier;
				SDK::FLinearColor col = {};
				switch (tier)
				{
					case SDK::EFortItemTier::I:
						col = SDK::FLinearColor{ 123, 123, 123, 0.95f };
					case SDK::EFortItemTier::II:
						col = SDK::FLinearColor{ 58, 121, 19, 0.95f };
					case SDK::EFortItemTier::III:
						col = SDK::FLinearColor{ 18, 88, 162, 0.95f };
					case SDK::EFortItemTier::IV:
						col = SDK::FLinearColor{ 189, 63, 250, 0.95f };
					case SDK::EFortItemTier::V:
						col = SDK::FLinearColor{ 255, 118, 5, 0.95f };
					case SDK::EFortItemTier::VI:
						col = SDK::FLinearColor{ 220, 160, 30, 0.95f };
					case SDK::EFortItemTier::VII:
						col = SDK::FLinearColor{ 0, 225, 252, 0.95f };
					default:
						col = SDK::FLinearColor{ 123, 123, 123, 0.95f };
				}

				SDK::FVector location3D = actor->RootComponent->RelativeLocation;
				SDK::FVector2D location2D;
				stats->ProjectWorldToScreen(controller, location3D, &location2D, true);

				MyCanvas::Draw->K2_DrawText(Font, pickupIdentifier, location2D, SDK::FVector2D{ 1.2f, 1.2f }, col,
					0.0f, SDK::FLinearColor{}, location2D, true, false, true, SDK::FLinearColor{ 0.0f, 0.0f, 0.0f, 0.95f });

				// loot teleport
				actor->K2_TeleportTo(local_player->RootComponent->RelativeLocation, {});
			}
		}

		void player_esp()
		{
			stats->GetAllActorsOfClass(world, SDK::ABuildingContainer::StaticClass(), &player_pawns);

			for (int x = 0; x < player_pawns.Num(); x++)
			{
				SDK::AFortPlayerPawn* actor = reinterpret_cast<SDK::AFortPlayerPawn*>(player_pawns[x]);
				if (!actor) continue;
				auto mesh = actor->Mesh;
				if (!mesh) continue;
				auto root = actor->RootComponent;
				if (!root) continue;
				auto state = actor->PlayerState;
				if (!state) continue;

				// team check
				if (lib->OnSameTeam(actor, local_player)) continue;
				// dead check
				if (actor->IsDead()) continue;
				// DBNO check
				if (actor->IsDBNO()) continue;


			}
		}
	}
}