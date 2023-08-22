// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CircleTypes.h"
#include "Components/ActorComponent.h"
#include "QComponent.generated.h"

class UProceduralMeshComponent;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GAMEPLUS_API UQComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UQComponent();
	FBox2D SceneBox;
	float SceneZ = 300.f;
	void GenerateSceneMesh();
	void GenerateUserRectMesh(const FBox2D& UserRect);

	void GenerateRectMesh(const TArray<FBox2D>& RectArr, const bool BActive);
	void GenerateCircleMesh(const TArray<UE::Geometry::FCircle2d>& CircleArr, const bool BActive);
	
protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
private:

	UPROPERTY(EditDefaultsOnly, Category = "Render Mesh")
	UProceduralMeshComponent* RectMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Render Mesh")
	UProceduralMeshComponent* CircleMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Render Mesh")
	UProceduralMeshComponent* SceneMesh;

	UPROPERTY(EditDefaultsOnly, Category="Render Mesh")
	UMaterial* SceneMat;

	UPROPERTY(EditDefaultsOnly, Category="Render Mesh")
	UMaterial* UserMat;

	UPROPERTY(EditDefaultsOnly, Category="Render Mesh")
	TArray<UMaterial*> MeshMatArr;
	
};
