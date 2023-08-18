// Fill out your copyright notice in the Description page of Project Settings.


#include "RenderComp.h"

#include "Kismet/KismetMathLibrary.h"

URenderComp::URenderComp()
{
	
	PrimaryComponentTick.bCanEverTick = true;

	SceneMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("SceneMesh"));
	WallMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("WallMesh"));
	PathMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("PathMesh"));
}


void URenderComp::BeginPlay()
{
	Super::BeginPlay();
}



void URenderComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}



void URenderComp::SetParams(int32 NumRows_, int32 NumColumns_, float TileSize_)
{
	NumRows = NumRows_;
	NumColumns = NumColumns_;
	TileSize = TileSize_;
}

// 生成网格
void URenderComp::GenerateGridMesh()
{
    TArray<FVector> Vertices;
    TArray<int32> Triangles;
	TArray<int32> Triangles1;
    TArray<FVector> Normals;
    TArray<FVector2D> UVs;
    TArray<FLinearColor> VertexColors;

	for (int32 Row = 0; Row <= NumRows; Row++)
	{
		FVector Start = FVector(-0.5 *NumRows * TileSize + Row * TileSize, -0.5 *NumColumns * TileSize, 300); 
		FVector End = FVector(-0.5 *NumRows * TileSize + Row * TileSize, 0.5 *NumColumns * TileSize, 300);
		

		DrawDebugLine(
			GetWorld(),
			Start,
			End,
			FColor::Black,
			true, // 是否持久显示线段
			-1.0f, // 线段的持久显示时间
			0,
			0.5f // 线段的宽度
		);
	}

	for (int32 Col = 0; Col <= NumRows; Col++)
	{
		FVector Start = FVector(-0.5 *NumRows * TileSize , -0.5 *NumColumns * TileSize + Col * TileSize, 300); 
		FVector End = FVector(0.5 *NumRows * TileSize , -0.5 *NumColumns * TileSize + Col * TileSize, 300);
		

		DrawDebugLine(
			GetWorld(),
			Start,
			End,
			FColor::Black,
			true, // 是否持久显示线段
			-1.0f, // 线段的持久显示时间
			0,
			0.5f // 线段的宽度
		);
	}

	
    for (int32 Row = 0; Row < NumRows; Row++)
    {
        for (int32 Column = 0; Column < NumColumns; Column++)
        {
            // 计算当前格子的位置
            FVector Location(-0.5 *NumRows * TileSize + Column * TileSize, -0.5 *NumRows * TileSize + Row * TileSize, 300.0f);
			
            // 添加四个顶点，顺时针方向添加
            Vertices.Add(Location);
            Vertices.Add(Location + FVector(TileSize, 0.0f, 0.0f));
            Vertices.Add(Location + FVector(TileSize, TileSize, 0.0f));
            Vertices.Add(Location + FVector(0.0f, TileSize, 0.0f));

            // 添加三角形索引
            int32 BaseIndex = (Row * NumColumns + Column) * 4;
            Triangles.Add(BaseIndex);
            Triangles.Add(BaseIndex + 1);
            Triangles.Add(BaseIndex + 2);
        	
            Triangles.Add(BaseIndex);
            Triangles.Add(BaseIndex + 2);
            Triangles.Add(BaseIndex + 3);
        	
        	Triangles1.Add(BaseIndex);
        	Triangles1.Add(BaseIndex + 2);
        	Triangles1.Add(BaseIndex + 1);
        	
        	Triangles1.Add(BaseIndex);
        	Triangles1.Add(BaseIndex + 3);
        	Triangles1.Add(BaseIndex + 2);

            /*// 添加法线
            FVector Normal(0.0f, 0.0f, 1.0f);
            for (int32 i = 0; i < 4; i++)
            {
                Normals.Add(Normal);
            }*/
        	
        }
    }
	
    // 设置UProceduralMeshComponent的网格数据
    SceneMesh->CreateMeshSection_LinearColor(0, Vertices, Triangles, Normals, UVs, VertexColors, TArray<FProcMeshTangent>(), true);
	SceneMesh->CreateMeshSection_LinearColor(1, Vertices, Triangles1, Normals, UVs, VertexColors, TArray<FProcMeshTangent>(), true);

    SceneMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly); // 可根据需要启用或禁用碰撞
	SceneMesh->SetCollisionObjectType(ECC_Visibility);
	SceneMesh->SetCollisionResponseToAllChannels(ECR_Block);
	if(IsValid(DefaultMat))
	{
		UMaterialInstanceDynamic* mtl = UMaterialInstanceDynamic::Create(DefaultMat, NULL);
		SceneMesh->SetMaterial(0, mtl);
		SceneMesh->SetMaterial(1, mtl);
	}
}

void URenderComp::GenerateGoalMesh(const int32 Row, const int32 Col)
{

	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UVs;
	TArray<FProcMeshTangent> Tangents;
	
	float diff = TileSize * 0.5f;

	FVector Center(-0.5 *NumRows * TileSize + Col * TileSize + diff *0.8 , -0.5 *NumRows * TileSize + Row * TileSize + diff, 300.0f);
	
	float SideLength = TileSize * 0.8f; 
	
	/*FVector Vertex0 = Center + FVector(0.0f, -SideLength / 2.0f, 0.0f);
	FVector Vertex1 = Center + FVector(-SideLength * FMath::Sqrt(3) / 2.0f, SideLength / 2.0f, 0.0f);
	FVector Vertex2 = Center + FVector(SideLength * FMath::Sqrt(3) / 2.0f, SideLength / 2.0f, 0.0f);*/

	const FVector Vertex0 = Center + FVector((FMath::Sqrt(3.0) / 2.0 - 1.0 / (2*FMath::Sqrt(3.0)))  * SideLength, 0.f, 0.0f);
	const FVector Vertex1 = Center + FVector(- 1.0 / (2*FMath::Sqrt(3.0)) * SideLength, - SideLength / 2.0f, 0.0f);
	const FVector Vertex2 = Center + FVector(- 1.0 / (2*FMath::Sqrt(3.0)) * SideLength, SideLength / 2.0f, 0.0f);
	
	Vertices.Add(Vertex0);
	Vertices.Add(Vertex1);
	Vertices.Add(Vertex2);

	
	Triangles.Add(0);
	Triangles.Add(1);
	Triangles.Add(2);

	
	PathMesh->ClearMeshSection(0);
	PathMesh->CreateMeshSection_LinearColor(0, Vertices, Triangles, Normals, UVs, TArray<FLinearColor>(), Tangents, false);
	PathMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); 
	PathMesh->SetMaterial(0, UMaterial::GetDefaultMaterial(MD_Surface));
	if(IsValid(StartMat))
	{
		UMaterialInstanceDynamic* mtl = UMaterialInstanceDynamic::Create(StartMat, nullptr);
		PathMesh->SetMaterial(0, mtl);
	}
	
}

void URenderComp::GenerateStartMesh(const int32 Row, const int32 Col)
{
	float diff = TileSize * 0.5;
	
	FVector Center(-0.5 *NumRows * TileSize + Col * TileSize + diff, -0.5 *NumRows * TileSize + Row * TileSize + diff, 300.0f);

	float Radius = TileSize * 0.4; 
	float NumSegments = 64;

	TArray<FVector> Vertices;
	TArray<int32> Triangles;

	for (int32 SegmentIndex = 0; SegmentIndex < NumSegments; ++SegmentIndex)
	{
		const float Angle = (float)SegmentIndex / NumSegments * 360.0f;
		const float RadAngle = FMath::DegreesToRadians(Angle);

		FVector VertexPosition;
		VertexPosition.X = Center.X + Radius * FMath::Cos(RadAngle);
		VertexPosition.Y = Center.Y + Radius * FMath::Sin(RadAngle);
		VertexPosition.Z = Center.Z;

		Vertices.Add(VertexPosition);

		if (SegmentIndex >= 1)
		{
			Triangles.Add(0);
			Triangles.Add(SegmentIndex + 1);
			Triangles.Add(SegmentIndex);
		}
	}

	TArray<FVector> Normals;
	

	TArray<FVector2D> UVs;
	TArray<FProcMeshTangent> Tangents;

	PathMesh->ClearMeshSection(1);
	PathMesh->CreateMeshSection_LinearColor(1, Vertices, Triangles, Normals, UVs, TArray<FLinearColor>(), Tangents, false);
	PathMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 可根据需要启用或禁用碰撞
	PathMesh->SetMaterial(1, UMaterial::GetDefaultMaterial(MD_Surface)); // 设置默认材质
	if(IsValid(GoalMat))
	{
		UMaterialInstanceDynamic* mtl = UMaterialInstanceDynamic::Create(GoalMat, nullptr);
		PathMesh->SetMaterial(1, mtl);
	}
}


void URenderComp::GenerateWallMesh(TMap<int32, TSet<TTuple<int32, int32>>>& Walls)
{
	WallMesh->ClearAllMeshSections();

	for(auto& kv : Walls)
	{
		auto& WallSet = kv.Value;
		TArray<FVector> Vertices;
		TArray<int32> Triangles;

	    TArray<FVector> Normals;
	    TArray<FVector2D> UVs;
	    TArray<FLinearColor> VertexColors;

	  
		int32 BaseIndex = 0;

	    for (int32 Row = 0; Row < NumRows; Row++)
	    {
	        for (int32 Column = 0; Column < NumColumns; Column++)
	        {
        		if(!WallSet.Contains(TTuple<int32, int32>(Row, Column))) continue;
        		
	            // 计算当前格子的位置
	            FVector Location(-0.5 *NumRows * TileSize + Column * TileSize, -0.5 *NumRows * TileSize + Row * TileSize, 300.0f);
				
	            // 添加四个顶点，顺时针方向添加
	            Vertices.Add(Location);
	            Vertices.Add(Location + FVector(TileSize, 0.0f, 0.0f));
	            Vertices.Add(Location + FVector(TileSize, TileSize, 0.0f));
	            Vertices.Add(Location + FVector(0.0f, TileSize, 0.0f));

	            // 添加三角形索引
        		
        		Triangles.Add(BaseIndex);
        		Triangles.Add(BaseIndex + 2);
        		Triangles.Add(BaseIndex + 1);
        		
        		Triangles.Add(BaseIndex);
        		Triangles.Add(BaseIndex + 3);
        		Triangles.Add(BaseIndex + 2);
        		
        		BaseIndex += 4;
	        }
	    }
		
		auto& WallType = kv.Key;

	  
	    WallMesh->CreateMeshSection_LinearColor(WallType, Vertices, Triangles, Normals, UVs, VertexColors, TArray<FProcMeshTangent>(), false);

	    WallMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); 
	    WallMesh->SetMaterial(WallType, UMaterial::GetDefaultMaterial(MD_Surface));
		if(IsValid(WallMatList[WallType]))
		{
			UMaterialInstanceDynamic* mtl = UMaterialInstanceDynamic::Create(WallMatList[WallType], nullptr);
			WallMesh->SetMaterial(WallType, mtl);
		}
	}
}

void URenderComp::GeneratePathMesh(std::vector<TPair<int, int>>& PATH)
{
	int32 BaseIndex = 0;
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UVs;
	TArray<FLinearColor> VertexColors;
	float diff = TileSize * 0.15;
	for (auto& path : PATH)
	{
		
		int Row = path.Key;
		int Column = path.Value;
		
		// 计算当前格子的位置
		FVector Location(-0.5 *NumRows * TileSize + Column * TileSize + diff, -0.5 *NumRows * TileSize + Row * TileSize + diff, 300.0f);
		
		// 添加四个顶点，顺时针方向添加
		Vertices.Add(Location);
		Vertices.Add(Location + FVector(TileSize - diff * 2, 0.0f, 0.0f));
		Vertices.Add(Location + FVector(TileSize - diff * 2, TileSize- diff * 2, 0.0f));
		Vertices.Add(Location + FVector(0.0f, TileSize - diff * 2, 0.0f));

		// 添加三角形索引
        
		Triangles.Add(BaseIndex);
		Triangles.Add(BaseIndex + 2);
		Triangles.Add(BaseIndex + 1);
        
		Triangles.Add(BaseIndex);
		Triangles.Add(BaseIndex + 3);
		Triangles.Add(BaseIndex + 2);
		
        		
		BaseIndex += 4;
	}
	
	PathMesh->ClearMeshSection(2);
	PathMesh->CreateMeshSection_LinearColor(2, Vertices, Triangles, Normals, UVs, VertexColors, TArray<FProcMeshTangent>(), false);
	PathMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); 
	PathMesh->SetMaterial(2, UMaterial::GetDefaultMaterial(MD_Surface));
	if(IsValid(RoadMat))
	{
		UMaterialInstanceDynamic* mtl = UMaterialInstanceDynamic::Create(RoadMat, nullptr);
		PathMesh->SetMaterial(2, mtl);
	}
}
