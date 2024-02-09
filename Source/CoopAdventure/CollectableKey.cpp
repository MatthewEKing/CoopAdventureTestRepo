// Fill out your copyright notice in the Description page of Project Settings.


#include "CollectableKey.h"
#include "Net/UnrealNetwork.h"
#include "CoopAdventureCharacter.h"


// Sets default values
ACollectableKey::ACollectableKey()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicateMovement(true);

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	SetRootComponent(RootComp);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(RootComp);
	Mesh->SetIsReplicated(true);
	Mesh->SetCollisionProfileName(FName("OverlapAllDynamic"));

	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>("Capsule");
	CapsuleComp->SetupAttachment(RootComp);
	CapsuleComp->SetIsReplicated(true);
	CapsuleComp->SetCollisionProfileName(FName("OverlapAllDynamic"));
	CapsuleComp->SetCapsuleHalfHeight(150.f);
	CapsuleComp->SetCapsuleRadius(100.f);

	CollectAudio = CreateDefaultSubobject<UAudioComponent>("Collect Audio");
	CollectAudio->SetupAttachment(RootComp);
	CollectAudio->SetAutoActivate(false);

	RotationSpeed = 100.f;
}


void ACollectableKey::BeginPlay()
{
	Super::BeginPlay();
	
}


void ACollectableKey::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//*note we use HasAuthority often because its being replicated from server to client. better to manipulate actors from server.
	if (HasAuthority())
	{
		//Rotates Static Mesh
		Mesh->AddRelativeRotation(FRotator(0.f, RotationSpeed * DeltaTime, 0.f));

		TArray<AActor*> OverlappingActors;
		CapsuleComp->GetOverlappingActors(OverlappingActors, ACoopAdventureCharacter::StaticClass());

		if (!OverlappingActors.IsEmpty())
		{
			//player character is overlapping this actor

			if (!IsCollected)
			{
				IsCollected = true;
				OnRep_IsCollected();
			}
		}
	}


	
}

void ACollectableKey::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//Needed to create replicated variables, needs #include "Net/UnrealNetwork.h"
	DOREPLIFETIME(ACollectableKey, IsCollected);
}

void ACollectableKey::OnRep_IsCollected()
{
	//replicated function that is bound to IsCollected variable and is called on the CLIENTS whenever that value changes, must be called manually on the server
	if (HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("OnRep_IsCollected called from the Server"));

		if (IsCollected)
		{
			OnCollected.Broadcast();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("OnRep_IsCollected called from the Client"));
	}

	Mesh->SetVisibility(!IsCollected);
	CollectAudio->Play();

	if (IsCollected && KeyHolderRef)
	{
		KeyHolderRef->ActivateKeyMesh();
	}
}