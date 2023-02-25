// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelEntrance.h"
#include "ZombieTown/Actors/TownCharacter.h"
#include "ZombieTown/AI/ZombieAIController.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/PlayerCameraManager.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
ALevelEntrance::ALevelEntrance()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");
}

// Called when the game starts or when spawned
void ALevelEntrance::BeginPlay()
{
	Super::BeginPlay();
	
}

// This function is called when a player enters a level through a level entrance
void ALevelEntrance::EnterLevel()
{
    // If TeleportPlayer is set to true, teleport the player to the entrance location
    if (TeleportPlayer)
    {
        AActor* player = UGameplayStatics::GetPlayerPawn(this, 0);
        if (player)
        {
            // Teleport the player to the entrance location with the same rotation and without sweeping
            player->TeleportTo(GetActorLocation(), player->GetActorRotation(), false, true);
        }
    }

    // If TeleportZombies is set to true, teleport all the zombies in the level to the entrance location
    if (TeleportZombies)
    {
        // Get all the zombie AI controllers in the level
        TArray<AActor*> zombies;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), AZombieAIController::StaticClass(), zombies);

        // Set up variables for teleporting zombies in a circle around the entrance location
        int idx = 0; // Index of the current zombie in the loop
        double teleportRadius = 200; // Distance from the entrance to teleport the zombies
        int numZombies = zombies.Num(); // Total number of zombies in the level
        double dTheta = 0; // Change in angle between each zombie
        if (numZombies > 0)
        {
            // Calculate the angle between each zombie in the circle
            dTheta = 2.0f * PI / numZombies;
        }

        // Loop through all the zombies and teleport them to the entrance location
        for (AActor* zombie : zombies)
        {
            // Check if the actor is actually a zombie AI controller
            AZombieAIController* zombieAI = Cast<AZombieAIController>(zombie);
            if (!zombieAI)
            {
                continue;
            }

            // Get the character controlled by the zombie AI controller
            ATownCharacter* character = zombieAI->GetTownCharacter();

            // Check if the zombie is dead, and if so, destroy it and its character if there is one
            if (zombieAI->GetIsDead())
            {
                if (character)
                {
                    character->Destroy();
                }
                zombieAI->Destroy();
                continue;
            }

            // If the zombie is not dead and has a character, teleport it to the entrance location with an offset
            if (!character)
            {
                continue;
            }
            FVector offset(sin(idx * dTheta + PI) * teleportRadius, cos(idx * dTheta + PI) * teleportRadius, 0);
            character->TeleportTo(GetActorLocation() + offset, character->GetActorRotation(), false, true);

            // If there is a player in the level, summon the zombie towards the player
            AActor* player = UGameplayStatics::GetPlayerPawn(this, 0);
            if (player)
            {
                zombieAI->SummonTowardThisFrame(player);
            }

            // Increment the index for the next zombie in the loop
            idx++;
        }
    }
}
