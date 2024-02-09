// Fill out your copyright notice in the Description page of Project Settings.


#include "MovableActor.h"

// Sets default values
AMovableActor::AMovableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicateMovement(true);

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	SetRootComponent(RootComp);

	Point1 = CreateDefaultSubobject<UArrowComponent>("Point 1");
	Point1->SetupAttachment(RootComp);
	Point1->SetRelativeLocation(FVector(0, 0, 0));

	Point2 = CreateDefaultSubobject<UArrowComponent>("Point 2");
	Point2->SetupAttachment(RootComp);
	Point2->SetRelativeLocation(FVector(0, 0, 300));

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(RootComp);
	Mesh->SetIsReplicated(true);

	Transporter = CreateDefaultSubobject<UTransporter>("Transporter");
}

// Called when the game starts or when spawned
void AMovableActor::BeginPlay()
{
	Super::BeginPlay();
	
	//World Location of Point 1
	FVector StartPoint = GetActorLocation() + Point1->GetRelativeLocation();
	FVector EndPoint = GetActorLocation() + Point2->GetRelativeLocation();

	//if (Transporter)
	//{
	Transporter->SetPoints(StartPoint, EndPoint);
	//}
}

// Called every frame
void AMovableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

