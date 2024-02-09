// Fill out your copyright notice in the Description page of Project Settings.


#include "PressurePlate.h"

// Sets default values
APressurePlate::APressurePlate()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true; // or SetReplicates(true);
	SetReplicateMovement(true);

	IsActivated = false;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	RootComponent = RootComp;

	TriggerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Trigger Mesh Component"));
	TriggerMesh->SetupAttachment(RootComp);
	TriggerMesh->SetIsReplicated(true);

	auto TriggerMeshAsset = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("/Game/StarterContent/Shapes/Shape_Cylinder.Shape_Cylinder"));
	if (TriggerMeshAsset.Succeeded())
	{
		TriggerMesh->SetStaticMesh(TriggerMeshAsset.Object);
		TriggerMesh->SetRelativeScale3D(FVector(3.3, 3.3, 0.2f));
		TriggerMesh->SetRelativeLocation(FVector(0, 0, 10));
	}	

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	Mesh->SetupAttachment(RootComp);
	Mesh->SetIsReplicated(true);

	auto MeshAsset = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("/Game/Stylized_Egypt/Meshes/building/SM_building_part_08.SM_building_part_08"));
	if (MeshAsset.Succeeded())
	{
		Mesh->SetStaticMesh(MeshAsset.Object);
		Mesh->SetRelativeScale3D(FVector(4, 4, 0.5f));
		Mesh->SetRelativeLocation(FVector(0, 0, 7.2));
	}

	Transporter = CreateDefaultSubobject<UTransporter>(TEXT("Transporter"));
	Transporter->MoveTime = 0.25f;
	Transporter->OwnerIsTriggerActor = true;

}

// Called when the game starts or when spawned
void APressurePlate::BeginPlay()
{
	Super::BeginPlay();
	
	TriggerMesh->SetVisibility(false);
	TriggerMesh->SetCollisionProfileName(FName("OverlapAll"));

	Transporter->SetPoints(GetActorLocation(), GetActorLocation() + FVector(0.f, 0.f, -10.f));


}

// Called every frame
void APressurePlate::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		TArray<AActor*> OverlappingActors;
		AActor* TriggerActor = 0;

		TriggerMesh->GetOverlappingActors(OverlappingActors);

		for (int i = 0; i < OverlappingActors.Num(); ++i)
		{
			AActor* Actor = OverlappingActors[i];
			if (Actor->ActorHasTag("TriggerActor"))
			{
				TriggerActor = Actor;
				break;
			}

			//GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, FString::Printf(TEXT("Overlapping Actor: %s"), *Actor->GetName()));
		}

		if (TriggerActor)
		{
			if (!IsActivated)
			{
				IsActivated = true;
				OnActivated.Broadcast();
				//GEngine->AddOnScreenDebugMessage(-1, 1, FColor::White, FString::Printf(TEXT("Activated")));
			}
		}
		else
		{
			if (IsActivated)
			{
				IsActivated = false;
				OnDeactivated.Broadcast();
				//GEngine->AddOnScreenDebugMessage(-1, 1, FColor::White, FString::Printf(TEXT("Deactivated")));
			}
		}
	}
}

