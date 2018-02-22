// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectCovenant.h"
#include "HumanController.h"
#include "Spectator.h"
#include "SubjectZero.h"
#include "BaseMode.h"

ABaseMode::ABaseMode()
{

}

void ABaseMode::BeginPlay()
{
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), SpawnPoints);
	Logger::Chat("SpawnPoints: " + FString::FromInt(SpawnPoints.Num()));
}

void ABaseMode::PostLogin(APlayerController * NewPlayer)
{
	Super::PostLogin(NewPlayer);
	if(AHumanController * Controller = Cast<AHumanController>(NewPlayer))
	{
		ASpectator * NewPawn = GetWorld()->SpawnActor<ASpectator>(FVector(0.f, 0.f, 100.f), FRotator(0.f, 0.f, 0.f));
		Controller->Possess(NewPawn);
	}
	Logger::Log("Welcome " + NewPlayer->GetNetOwningPlayer()->GetName());

}

/* SpawnPlayer() - Spawns a character at a PlayerStart location.
Spawn location rotates over the course of the game
*/
void ABaseMode::SpawnPlayer(AHumanController * Controller)
{
	Logger::Log("Attempting to spawn...");
	if(HasAuthority())
	{
		if(GetWorld())
		{
			ASubjectZero * NewPawn = GetWorld()->SpawnActor<ASubjectZero>(SubjectZeroBlueprint, SpawnPoints[SpawnCount]->GetActorLocation(), SpawnPoints[SpawnCount]->GetActorRotation());
			Characters.Add(NewPawn);
			APawn * OldPawn = Controller->GetPawn();
			Controller->Possess(NewPawn);
			OldPawn->Destroy();
		}
	}
	SpawnCount = (SpawnCount + 1) % SpawnPoints.Num();
}

void ABaseMode::KillPlayer(AHumanController * Controller)
{
	if(HasAuthority())
	{
		if(GetWorld())
		{
			ASpectator * NewPawn = GetWorld()->SpawnActor<ASpectator>(Controller->GetPawn()->GetActorLocation(), Controller->GetPawn()->GetActorRotation());
			APawn * OldPawn = Controller->GetPawn();
			Controller->Possess(NewPawn);
			if(ASubjectZero * SubjectZero = Cast<ASubjectZero>(OldPawn))
			{
				Characters.Remove(SubjectZero);
				SubjectZero->Kill();
			}
		}
	}
}