// Fill out your copyright notice in the Description page of Project Settings.


#include "CollectableKeyHolder.h"

// Sets default values
ACollectableKeyHolder::ACollectableKeyHolder()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicateMovement(true);

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	SetRootComponent(RootComp);

	KeyMesh = CreateDefaultSubobject<UStaticMeshComponent>("Key Mesh");
	KeyMesh->SetupAttachment(RootComp);
	KeyMesh->SetIsReplicated(true);
	KeyMesh->SetCollisionProfileName(FName("OverlapAllDynamic"));

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(RootComp);
	Mesh->SetIsReplicated(true);
	Mesh->SetCollisionProfileName(FName("BlockAllDynamic"));

	RotationSpeed = 100.f;
}

// Called when the game starts or when spawned
void ACollectableKeyHolder::BeginPlay()
{
	Super::BeginPlay();
	
	KeyMesh->SetVisibility(false);

}

// Called every frame
void ACollectableKeyHolder::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		KeyMesh->AddRelativeRotation(FRotator(0.f, RotationSpeed * DeltaTime, 0.f));
	}
}

void ACollectableKeyHolder::ActivateKeyMesh()
{
	KeyMesh->SetVisibility(true);
}