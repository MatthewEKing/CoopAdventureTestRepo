// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Transporter.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COOPADVENTURE_API UTransporter : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTransporter();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	
	FVector StartPoint;
	FVector EndPoint;

	bool ArePointsSet;

	UPROPERTY(EditAnywhere)
	float MoveTime;

	UPROPERTY(EditAnywhere)
	bool OwnerIsTriggerActor;

	UPROPERTY(EditAnywhere)	
	TArray<AActor*> TriggerActors;

	UPROPERTY(VisibleAnywhere)
	int ActivatedTriggerCount;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool AllTriggerActorsTriggered;

	UFUNCTION(BlueprintCallable)
	void SetPoints(FVector Point1, FVector Point2);

	UFUNCTION()
	void OnTriggerActorActivated();

	UFUNCTION()
	void OnTriggerActorDeactivated();
};
