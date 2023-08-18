// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProceduralMeshComponent.h"

#include "RenderComp.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GAMEPLUS_API URenderComp : public UActorComponent
{
	GENERATED_BODY()

public:	
	URenderComp();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void SetParams(int32 NumRows_, int32 NumColumns_, float TileSize_);
	void GenerateGridMesh();

	void GenerateStartMesh(const int32 Row, const int32 Col);

	void GenerateGoalMesh(const int32 Row, const int32 Col);

	
	void GenerateWallMesh(TMap<int32, TSet<TTuple<int32, int32>>>& Walls);

	void GeneratePathMesh(std::vector<TPair<int, int>>& PATH);
protected:
	virtual void BeginPlay() override;

private:
	
	UPROPERTY(EditDefaultsOnly, Category = "Render Mesh")
	UProceduralMeshComponent* SceneMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Render Mesh")
	UProceduralMeshComponent* WallMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Render Mesh")
	UProceduralMeshComponent* PathMesh;
	

	UPROPERTY(EditDefaultsOnly, Category = "Render Mesh")
	bool btestGpt = false;
	
	UPROPERTY(EditDefaultsOnly, Category="Render Mesh")
	UMaterial* DefaultMat;

	UPROPERTY(EditDefaultsOnly, Category="Render Mesh")
	UMaterial* RoadMat;

	UPROPERTY(EditDefaultsOnly, Category="Render Mesh")
	UMaterial* StartMat;

	UPROPERTY(EditDefaultsOnly, Category="Render Mesh")
	UMaterial* GoalMat;
	
	UPROPERTY(EditDefaultsOnly, Category="Render Mesh")
	TArray<UMaterial*>  WallMatList;

	
	int32 NumRows;
	int32 NumColumns;
	float TileSize; 
	
};
