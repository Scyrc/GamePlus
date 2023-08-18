// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AStarComp.h"
#include "RenderComp.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Pawn.h"
#include "AStarPawn.generated.h"

UCLASS()
class GAMEPLUS_API AAStarPawn : public APawn
{
	GENERATED_BODY()

public:
	AAStarPawn();
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void UserClickedPoint(FVector selectPoint);
	void UserMovePoint(FVector selectPoint);

	UFUNCTION(BlueprintCallable)
	void UserSelectWallType(int32 Type);

	UFUNCTION(BlueprintCallable)
	void FindPath();

protected:
	virtual void BeginPlay() override;

	void OnZoomUpFov();
	void OnZoomDownFov();

private:
	UPROPERTY(EditAnywhere,  Category = "Comp")
	UStaticMeshComponent* RootStaticMesh;
	
	UPROPERTY(EditDefaultsOnly, Category="Comp")
	URenderComp* RenderComp;

	UPROPERTY(EditDefaultsOnly, Category="Comp")
	UAStarComp* AStarComp;
	
	UPROPERTY(EditDefaultsOnly, Category="Comp")
	UCameraComponent* CameraComponent;
	
	UPROPERTY(EditAnywhere, Category="Scene Set")
	int32 WallType = 0;
	
	TPair<int, int> GoalLoc;
	
	TPair<int, int> StartLoc = {0, 0};
	
	TMap<TPair<int32, int32>, int32> walls;

	std::vector<TPair<int, int>> PATH;

	

	UPROPERTY(EditAnywhere, Category="Scene Set")
	int32 NumRows = 20;
	
	UPROPERTY(EditAnywhere, Category="Scene Set")
	int32 NumColumns = 20;
	
	UPROPERTY(EditAnywhere, Category="Scene Set")
	float TileSize = 5.0f; // 每个格子的大小

	UPROPERTY()
	int32 SelectMode = 0;

};
