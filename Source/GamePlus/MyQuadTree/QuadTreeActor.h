// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "QuadTreeNode.h"
#include "GameFramework/Actor.h"
#include "QuadTreeActor.generated.h"

UCLASS()
class GAMEPLUS_API AQuadTreeActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AQuadTreeActor();

protected:
	virtual void BeginPlay() override;

	void SpawnObj();
	void ChangeVel();


public:	
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere, Category="Scene set")
	FVector2D Center = FVector2D(0.f, 0.f);
	
	UPROPERTY(EditAnywhere, Category="Scene set")
	float Width = 500.f;
	
	UPROPERTY(EditAnywhere, Category="Scene set")
	float Height= 500.f;

	UPROPERTY(EditAnywhere, Category="Scene set")
	float SpawnRate= 2.f;

	UPROPERTY(EditAnywhere, Category="Scene set")
	float ChangeVelRate= 3.f;

	UPROPERTY(EditAnywhere, Category="Scene set")
	int32 ObjNumMax= 60.f;

	UPROPERTY(EditAnywhere, Category="Scene set")
	float ZPos= 300.f;
	
	TArray<TSharedPtr<FObjectStruct>> ObjectsArray;

	TSharedPtr<QuadTreeNode> TreeRoot;

	FTimerHandle TimerHandleSpawnObj;
	FTimerHandle TimerHandleChangeVel;

};
