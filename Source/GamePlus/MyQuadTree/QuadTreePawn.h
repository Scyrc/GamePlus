// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "QuadTreeNode.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Pawn.h"
#include "QuadTreePawn.generated.h"

class UQComponent;
UCLASS()
class GAMEPLUS_API AQuadTreePawn : public APawn
{
	GENERATED_BODY()

public:
	AQuadTreePawn();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	int32 GetObjNum() const {return ObjectsArray.Num();}

	UFUNCTION(BlueprintCallable)
	int32 GetQueryNum() const {return QueryNum;}

	UFUNCTION(BlueprintCallable)
	void SetSpawnObj(bool bActive) {BAutoSpawn = bActive;}
	
	UFUNCTION(BlueprintCallable)
	void SetChangeVel(bool bActive) {BAutoChange = bActive;}

	UFUNCTION(BlueprintCallable)
	void SetUserRect(float width, float height) {UserRectHeight = height; UserRectWidth = width;}

protected:
	virtual void BeginPlay() override;
	void SpawnObj();
	void SpawnObj(FVector Pos);
	void ChangeVel();
	void ChangePos(float DeltaTime);
	void ActiveObjs(float DeltaTime);
	void DrawObjsMesh();

	void OnZoomUpFov();
	void OnZoomDownFov();
	void OnMouseLeftClick();
	TSharedPtr<FObjectStruct> SelectObj;
	bool BSelectMode = false;
	FBox2D UserRect;
	void OnMouseRightClick();
	bool GetHitPoint(FVector& HitPos);
	int32 QueryNum = 0;

private:
	UPROPERTY(EditAnywhere,  Category = "Comp")
	UStaticMeshComponent* RootStaticMesh;
	
	UPROPERTY(EditDefaultsOnly, Category="Comp")
	UQComponent* RenderComp;
	
	UPROPERTY(EditDefaultsOnly, Category="Comp")
	UCameraComponent* CameraComponent;
	
	UPROPERTY(EditAnywhere, Category="Scene set")
	FVector2D Center = FVector2D(0.f, 0.f);
	
	UPROPERTY(EditAnywhere, Category="Scene set")
	float Width = 800.f;
	
	UPROPERTY(EditAnywhere, Category="Scene set")
	float Height= 800.f;

	UPROPERTY(EditAnywhere, Category="Scene set")
	float SpawnRate= 0.5f;

	UPROPERTY(EditAnywhere, Category="Scene set")
	float ChangeVelRate= 2.f;

	UPROPERTY(EditAnywhere, Category="Scene set")
	int32 ObjNumMax= 60.f;

	UPROPERTY(EditAnywhere, Category="Scene set")
	float ZPos= 0.f;

	UPROPERTY(EditAnywhere, Category="Scene set")
	float UserRectWidth= 100.f;

	UPROPERTY(EditAnywhere, Category="Scene set")
	float UserRectHeight =  100.f;

	UPROPERTY(EditAnywhere, Category="Scene set")
	bool BAutoSpawn = false;

	UPROPERTY(EditAnywhere, Category="Scene set")
	bool BAutoChange = false;
	
	TArray<TSharedPtr<FObjectStruct>> ObjectsArray;

	TSharedPtr<QuadTreeNode> TreeRoot;

	FTimerHandle TimerHandleSpawnObj;
	FTimerHandle TimerHandleChangeVel;


};

