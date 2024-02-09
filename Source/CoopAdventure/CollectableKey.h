// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/AudioComponent.h"
#include "CollectableKeyHolder.h"

#include "CollectableKey.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCollectableKeyOnCollected);

UCLASS()
class COOPADVENTURE_API ACollectableKey : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACollectableKey();

	//function needed to replicate variables
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	USceneComponent* RootComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UCapsuleComponent* CapsuleComp;

	UFUNCTION()
	void OnRep_IsCollected();

	UPROPERTY(ReplicatedUsing = OnRep_IsCollected, BlueprintReadWrite, VisibleAnywhere)
	bool IsCollected;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RotationSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ACollectableKeyHolder* KeyHolderRef;

	UPROPERTY()
	FCollectableKeyOnCollected OnCollected;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UAudioComponent* CollectAudio;
};
