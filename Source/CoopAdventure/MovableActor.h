// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Components/ArrowComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Transporter.h"

#include "MovableActor.generated.h"

UCLASS()
class COOPADVENTURE_API AMovableActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMovableActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	USceneComponent* RootComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UArrowComponent* Point1;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UArrowComponent* Point2;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UTransporter* Transporter;
};
