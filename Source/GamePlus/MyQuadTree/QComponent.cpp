// Fill out your copyright notice in the Description page of Project Settings.


#include "QComponent.h"

#include "ProceduralMeshComponent.h"

UQComponent::UQComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SceneMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("SceneMesh"));
	RectMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("RectMesh"));
	CircleMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("CircleMesh"));
}


void UQComponent::BeginPlay()
{
	Super::BeginPlay();
}


void UQComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UQComponent::GenerateSceneMesh()
{
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<int32> Triangles1;
	TArray<FVector> Normals;
	TArray<FVector2D> UVs;
	TArray<FProcMeshTangent> Tangents;

	TArray<FLinearColor> VertexColors;

	Vertices.Add(FVector(SceneBox.Min.X, SceneBox.Max.Y, SceneZ));
	Vertices.Add(FVector(SceneBox.Max.X, SceneBox.Max.Y, SceneZ));
	Vertices.Add(FVector(SceneBox.Max.X, SceneBox.Min.Y, SceneZ));
	Vertices.Add(FVector(SceneBox.Min.X, SceneBox.Min.Y, SceneZ));

	Triangles.Add(0);
	Triangles.Add(2);
	Triangles.Add(1);
	
	Triangles.Add(0);
	Triangles.Add(3);
	Triangles.Add(2);

	
	Triangles1.Add(0);
	Triangles1.Add(1);
	Triangles1.Add(2);
	
	Triangles1.Add(0);
	Triangles1.Add(2);
	Triangles1.Add(3);

	

	//SceneMesh->ClearMeshSection(0);
	SceneMesh->CreateMeshSection_LinearColor(0, Vertices, Triangles, Normals, UVs, VertexColors, TArray<FProcMeshTangent>(), true);
	SceneMesh->CreateMeshSection_LinearColor(1, Vertices, Triangles1, Normals, UVs, VertexColors, TArray<FProcMeshTangent>(), true);

	SceneMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly); // 可根据需要启用或禁用碰撞
	SceneMesh->SetCollisionObjectType(ECC_Visibility);
	SceneMesh->SetCollisionResponseToAllChannels(ECR_Block);
	SceneMesh->SetMaterial(0, UMaterial::GetDefaultMaterial(MD_Surface));
	if(IsValid(SceneMat))
	{
		UMaterialInstanceDynamic* mtl = UMaterialInstanceDynamic::Create(SceneMat, nullptr);
		SceneMesh->SetMaterial(0, mtl);
		SceneMesh->SetMaterial(1, mtl);

	}
}

void UQComponent::GenerateUserRectMesh(const FBox2D& UserRect)
{
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UVs;
	TArray<FProcMeshTangent> Tangents;

	TArray<FLinearColor> VertexColors;

	FVector TL = FVector(UserRect.Min.X, UserRect.Max.Y, SceneZ);
	FVector TR = FVector(UserRect.Max.X, UserRect.Max.Y, SceneZ);
	FVector BR = FVector(UserRect.Max.X, UserRect.Min.Y, SceneZ);
	FVector BL = FVector(UserRect.Min.X, UserRect.Min.Y, SceneZ);
	
	Vertices.Add(TL);
	Vertices.Add(TR);
	Vertices.Add(BR);
	Vertices.Add(BL);
	
	Triangles.Add(0);
	Triangles.Add(1);
	Triangles.Add(2);
	
	Triangles.Add(0);
	Triangles.Add(2);
	Triangles.Add(3);

	
	SceneMesh->ClearMeshSection(2);
	SceneMesh->CreateMeshSection_LinearColor(2, Vertices, Triangles, Normals, UVs, VertexColors, TArray<FProcMeshTangent>(), false);
	SceneMesh->SetMaterial(2, UMaterial::GetDefaultMaterial(MD_Surface));
	if(IsValid(UserMat))
	{
		UMaterialInstanceDynamic* mtl = UMaterialInstanceDynamic::Create(UserMat, nullptr);
		SceneMesh->SetMaterial(2, mtl);
	}
}

void UQComponent::GenerateRectMesh(const TArray<FBox2D>& RectArr, const bool BActive)
{
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UVs;
	TArray<FProcMeshTangent> Tangents;

	TArray<FLinearColor> VertexColors;
	int32 Count = 0;
	for(auto& Box2d : RectArr)
	{
		FVector TL = FVector(Box2d.Min.X, Box2d.Max.Y, SceneZ);
		FVector TR = FVector(Box2d.Max.X, Box2d.Max.Y, SceneZ);
		FVector BR = FVector(Box2d.Max.X, Box2d.Min.Y, SceneZ);
		FVector BL = FVector(Box2d.Min.X, Box2d.Min.Y, SceneZ);
		
		Vertices.Add(TL);
		Vertices.Add(TR);
		Vertices.Add(BR);
		Vertices.Add(BL);
		
		Triangles.Add(Count + 0);
		Triangles.Add(Count + 1);
		Triangles.Add(Count + 2);
		
		Triangles.Add(Count + 0);
		Triangles.Add(Count + 2);
		Triangles.Add(Count + 3);
		
		Count += 4;
	}
	int32 MeshIndex;
	if(!BActive)
	{
		MeshIndex = 0;
	}
	else
	{
		MeshIndex = 1;
	}
	RectMesh->ClearMeshSection(MeshIndex);
	RectMesh->CreateMeshSection_LinearColor(MeshIndex, Vertices, Triangles, Normals, UVs, TArray<FLinearColor>(), Tangents, false);
	

	RectMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); 
	RectMesh->SetMaterial(MeshIndex, UMaterial::GetDefaultMaterial(MD_Surface));
	if(MeshIndex >= MeshMatArr.Num()) return;
	if(IsValid(MeshMatArr[MeshIndex]))
	{
		UMaterialInstanceDynamic* mtl = UMaterialInstanceDynamic::Create(MeshMatArr[MeshIndex], nullptr);
		RectMesh->SetMaterial(MeshIndex, mtl);
	}
}

void UQComponent::GenerateCircleMesh(const TArray<UE::Geometry::FCircle2d>& CircleArr, const bool BActive)
{
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UVs;
	TArray<FProcMeshTangent> Tangents;

	TArray<FLinearColor> VertexColors;
	int32 Count = 0;
	float NumSegments = 64;
	for(auto& Circle2d : CircleArr)
	{
		const FVector Center = FVector(Circle2d.Center.X, Circle2d.Center.Y, SceneZ);
		const double Radius = Circle2d.Radius;
		for (int32 SegmentIndex = 0; SegmentIndex < NumSegments; ++SegmentIndex)
		{
			const float Angle = (float)SegmentIndex / NumSegments * 360.0f;
			const float RadAngle = FMath::DegreesToRadians(Angle);

			FVector VertexPosition;
			VertexPosition.X = Center.X + Radius * FMath::Cos(RadAngle);
			VertexPosition.Y = Center.Y + Radius * FMath::Sin(RadAngle);
			VertexPosition.Z = Center.Z;

			Vertices.Add(VertexPosition);
			
			if (SegmentIndex >= 2)
			{
				Triangles.Add(Count + 0);
				Triangles.Add(Count + SegmentIndex);
				Triangles.Add(Count +SegmentIndex - 1);
			}
		}
		Count += NumSegments;
	}
	int32 MeshIndex;
	if(!BActive)
	{
		MeshIndex = 0;
	}
	else
	{
		MeshIndex = 1;
	}
	CircleMesh->ClearMeshSection(MeshIndex);
	CircleMesh->CreateMeshSection_LinearColor(MeshIndex, Vertices, Triangles, Normals, UVs, TArray<FLinearColor>(), Tangents, false);
	

	CircleMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); 
	CircleMesh->SetMaterial(MeshIndex, UMaterial::GetDefaultMaterial(MD_Surface));
	if(MeshIndex >= MeshMatArr.Num()) return;
	if(IsValid(MeshMatArr[MeshIndex]))
	{
		UMaterialInstanceDynamic* mtl = UMaterialInstanceDynamic::Create(MeshMatArr[MeshIndex], nullptr);
		CircleMesh->SetMaterial(MeshIndex, mtl);
	}
}

