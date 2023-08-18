// Fill out your copyright notice in the Description page of Project Settings.


#include "AStarPawn.h"

AAStarPawn::AAStarPawn()
{
	PrimaryActorTick.bCanEverTick = true;
	
	RootStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RootStaticMesh"));
	RootComponent = RootStaticMesh;
	SetRootComponent(RootComponent);
	RenderComp = CreateDefaultSubobject<URenderComp>(TEXT("RenderComp"));
	RenderComp->Activate();

	AStarComp = CreateDefaultSubobject<UAStarComp>(TEXT("AStarComp"));
	AStarComp->Activate();

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->Activate();
	CameraComponent->SetupAttachment(RootComponent);
}

void AAStarPawn::BeginPlay()
{
	Super::BeginPlay();
	RenderComp->SetParams(NumRows, NumColumns, TileSize);
	StartLoc = {0, 0};
	GoalLoc = {NumRows  - 1, NumColumns  - 1};

	

	SelectMode = 0;
	RenderComp->GenerateGridMesh();
	FindPath();
}

void AAStarPawn::OnZoomUpFov()
{
	CameraComponent->SetFieldOfView(CameraComponent->FieldOfView - 5.f);
}

void AAStarPawn::OnZoomDownFov()
{
	CameraComponent->SetFieldOfView(CameraComponent->FieldOfView + 5.f);

}

void AAStarPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}



void AAStarPawn::UserClickedPoint(FVector selectPoint)
{
	
	if(SelectMode != 0)
	{
		UserMovePoint(selectPoint);
		return;
	}

	int row = int((selectPoint.X - (-0.5 *NumRows * TileSize))) / TileSize;
	int col = int((selectPoint.Y - (-0.5 *NumColumns * TileSize))) / TileSize;
	
	if(col == StartLoc.Key	&& row == StartLoc.Value)
	{
		SelectMode = 1;
	}
	else if(col == GoalLoc.Key	&& row == GoalLoc.Value)
	{
		SelectMode = 2;
	}
	else
	{
		SelectMode = 0;
	}

	if(SelectMode != 0) return;
	//UE_LOG(LogTemp, Display, TEXT("Hit Point: X=%f, Y=%f, Z=%f"), HitResult.ImpactPoint.X, HitResult.ImpactPoint.Y,HitResult.ImpactPoint.Z);

	UE_LOG(LogTemp, Warning, TEXT("ROW: %d, COL: %d"), row, col);
	TPair<int32, int32> selectLoc(col, row);
	if(walls.Contains(selectLoc) && walls[selectLoc] == WallType)
	{
		walls.Remove(selectLoc);
	}
	else
	{
		walls.Emplace(selectLoc, WallType);
	}

	FindPath();
	
}

void AAStarPawn::UserMovePoint(FVector selectPoint)
{
	if (SelectMode == 0) return;
	
	int row = int((selectPoint.X - (-0.5 *NumRows * TileSize))) / TileSize;
	int col = int((selectPoint.Y - (-0.5 *NumColumns * TileSize))) / TileSize;
	
	TPair<int32, int32> selectLoc(col, row);
	if(walls.Contains(selectLoc))
	{
		walls.Remove(selectLoc);
	}
	
	if(SelectMode == 1)
	{
		StartLoc.Key = col;
		StartLoc.Value = row;
	}
	else if(SelectMode == 2)
	{
		GoalLoc.Key = col;
		GoalLoc.Value = row;
	}
	SelectMode = 0;
	FindPath();
	
}

void AAStarPawn::UserSelectWallType(int32 Type)
{
	WallType = Type;
}

void AAStarPawn::FindPath()
{
	AStarComp->FindPathAStar(NumColumns, NumRows, StartLoc, GoalLoc, walls, PATH);

	TMap<int32, TSet<TTuple<int32, int32>>> WallsData;

	for(auto& kv : walls)
	{
		if(!WallsData.Contains(kv.Value))
			WallsData.Emplace(kv.Value, TSet<TTuple<int32, int32>>());
			
		WallsData[kv.Value].Add(kv.Key);
	}
	
	RenderComp->GenerateWallMesh(WallsData);

	RenderComp->GenerateStartMesh(StartLoc.Key, StartLoc.Value);
	
	RenderComp->GenerateGoalMesh(GoalLoc.Key, GoalLoc.Value);
	

	RenderComp->GeneratePathMesh(PATH);

}

void AAStarPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("zoomUpFov", IE_Pressed, this, &AAStarPawn::OnZoomUpFov);
	PlayerInputComponent->BindAction("zoomDownFov", IE_Pressed, this, &AAStarPawn::OnZoomDownFov);


}

