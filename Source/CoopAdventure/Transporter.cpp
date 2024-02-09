// Fill out your copyright notice in the Description page of Project Settings.


#include "Transporter.h"
#include "PressurePlate.h"
#include "CollectableKey.h"

// Sets default values for this component's properties
UTransporter::UTransporter()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	//Prefered way to turn on replication for an actor COMPONENT in constructor
	SetIsReplicatedByDefault(true);

	MoveTime = 3.f;
	ActivatedTriggerCount = 0;

	ArePointsSet = false;

	StartPoint = FVector::Zero();
	EndPoint = FVector::Zero();

}


// Called when the game starts
void UTransporter::BeginPlay()
{
	Super::BeginPlay();

	if(OwnerIsTriggerActor)
	{
		TriggerActors.Add(GetOwner());
	}

	for (AActor* TriggerActor : TriggerActors)
	{
		APressurePlate* PressurePlateActor = Cast<APressurePlate>(TriggerActor);

		if (PressurePlateActor)
		{
			//bind functions to the delegate on the each pressure plate in the trigger actors array 
			PressurePlateActor->OnActivated.AddDynamic(this, &UTransporter::OnTriggerActorActivated);
			PressurePlateActor->OnDeactivated.AddDynamic(this, &UTransporter::OnTriggerActorDeactivated);
			continue;
		}

		ACollectableKey* CollectableKeyActor = Cast<ACollectableKey>(TriggerActor);

		if (CollectableKeyActor)
		{
			CollectableKeyActor->OnCollected.AddDynamic(this, &UTransporter::OnTriggerActorActivated);
		}
	}
}


// Called every frame
void UTransporter::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (TriggerActors.Num() > 0)
	{
		AllTriggerActorsTriggered = ActivatedTriggerCount >= TriggerActors.Num();
		
		if (AllTriggerActorsTriggered)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::White, FString::Printf(TEXT("All Triggers Activated")));
		}
	}


	//Move The Owner Actor
	AActor* Owner = GetOwner();
	if (Owner && Owner->HasAuthority() && ArePointsSet)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::White, FString::Printf(TEXT("Has Authority and points set")));
		FVector CurrentLocation = Owner->GetActorLocation();
		float Speed = FVector::Distance(StartPoint, EndPoint) / MoveTime;
		FVector TargetLocation = AllTriggerActorsTriggered ? EndPoint : StartPoint;

		if (!CurrentLocation.Equals(TargetLocation))
		{
			FVector NewLocation = FMath::VInterpConstantTo(CurrentLocation, TargetLocation, DeltaTime, Speed);
			Owner->SetActorLocation(NewLocation);
			//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::White, FString::Printf(TEXT("Moving")));
		}
	}
}



void UTransporter::SetPoints(FVector Point1, FVector Point2)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::White, FString::Printf(TEXT("Set Points")));
	if (Point1.Equals(Point2)) {return;}

	StartPoint = Point1;
	EndPoint = Point2;
	ArePointsSet = true;
}



void UTransporter::OnTriggerActorActivated()
{
	ActivatedTriggerCount++;

	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::White, FString::Printf(TEXT("Activated Actors: %d"), ActivatedTriggerCount));
}



void UTransporter::OnTriggerActorDeactivated()
{
	ActivatedTriggerCount--;

	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::White, FString::Printf(TEXT("Activated Plates: %d"), ActivatedTriggerCount));
}