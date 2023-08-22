// Fill out your copyright notice in the Description page of Project Settings.


#include "QuadTreeActor.h"

#include "Kismet/KismetMathLibrary.h"

AQuadTreeActor::AQuadTreeActor()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AQuadTreeActor::BeginPlay()
{
	Super::BeginPlay();

	TreeRoot = MakeShareable(new QuadTreeNode(FBox2D(Center, Center + FVector2D(Width, Height)), 0));

	GetWorldTimerManager().SetTimer(
		TimerHandleSpawnObj,
		this,
		&AQuadTreeActor::SpawnObj,
		SpawnRate,
		true);

	GetWorldTimerManager().SetTimer(
		TimerHandleChangeVel,
		this,
		&AQuadTreeActor::ChangeVel,
		ChangeVelRate,
		true);
}

void AQuadTreeActor::SpawnObj()
{
	if(ObjNumMax < 0)
	{
		GetWorldTimerManager().ClearTimer(TimerHandleSpawnObj);
		return;
	}

	--ObjNumMax;

	double XPos = UKismetMathLibrary::RandomFloatInRange(10.f  -Width, 10.f +Width);
	double YPos = UKismetMathLibrary::RandomFloatInRange(10.f  -Height, 10.f +Height);

	const FVector Pos = FVector(XPos, YPos, ZPos);

	const int32 ObjType = UKismetMathLibrary::RandomIntegerInRange(0, 1);

	if(ObjType == 0)
	{
		const double Radius = UKismetMathLibrary::RandomFloatInRange(20, 50);
		const TSharedPtr<FObjectStruct> NewObj= MakeShareable(new FObjectStruct(FVector2D(Pos.X, Pos.Y), Radius));
		ObjectsArray.Add(NewObj);
		TreeRoot->InsertObj(NewObj);
	}
	else if(ObjType == 1)
	{
		const double ObjWidth = UKismetMathLibrary::RandomFloatInRange(20, 50);
		const double ObjHeight = UKismetMathLibrary::RandomFloatInRange(20, 50);
		const TSharedPtr<FObjectStruct> NewObj= MakeShareable(new FObjectStruct(FVector2D(Pos.X, Pos.Y), ObjWidth, ObjHeight));
		ObjectsArray.Add(NewObj);
		TreeRoot->InsertObj(NewObj);
	}
}

void AQuadTreeActor::ChangeVel()
{
}

void AQuadTreeActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

