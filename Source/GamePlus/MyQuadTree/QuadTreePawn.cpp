// Fill out your copyright notice in the Description page of Project Settings.


#include "QuadTreePawn.h"

#include "QComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/GameUserSettings.h"


AQuadTreePawn::AQuadTreePawn()
{
	PrimaryActorTick.bCanEverTick = true;

	RootStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RootStaticMesh"));
	RootComponent = RootStaticMesh;
	SetRootComponent(RootComponent);
	RenderComp = CreateDefaultSubobject<UQComponent>(TEXT("RenderComp"));
	RenderComp->Activate();

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->Activate();
	CameraComponent->SetupAttachment(RootComponent);
}

void AQuadTreePawn::BeginPlay()
{
	Super::BeginPlay();
	UGameUserSettings* GameUserSettings = UGameUserSettings::GetGameUserSettings();
	if (GameUserSettings)
	{
		GameUserSettings->SetFrameRateLimit(240); // 设置帧率上限为 240
		GameUserSettings->ApplySettings(false);
	}
	
	SetActorLocation(FVector(0, 0, 500.f + ZPos));
	RenderComp->SceneBox = FBox2D(Center - FVector2D(Width, Height), Center + FVector2D(Width, Height));
	RenderComp->SceneZ = ZPos;
	RenderComp->GenerateSceneMesh();
	TreeRoot = MakeShareable(new QuadTreeNode(FBox2D(Center -  FVector2D(Width, Height), Center + FVector2D(Width, Height)), 0));
	TreeRoot->WorldObject = GetWorld();
	TreeRoot->ZPos = ZPos;
	
	GetWorldTimerManager().SetTimer(
		TimerHandleSpawnObj,
		this,
		&AQuadTreePawn::SpawnObj,
		SpawnRate,
		true);

	GetWorldTimerManager().SetTimer(
		TimerHandleChangeVel,
		this,
		&AQuadTreePawn::ChangeVel,
		ChangeVelRate,
		true);
}

void AQuadTreePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TreeRoot.Get()->Update();
	ActiveObjs(DeltaTime);
	TreeRoot.Get()->Draw();
	DrawObjsMesh();
	ChangePos(DeltaTime);
}

void AQuadTreePawn::DrawObjsMesh()
{
	TArray<FBox2D> RectArray0;
	TArray<FBox2D> RectArray1;

	TArray<UE::Geometry::FCircle2d> CircleArray0;
	TArray<UE::Geometry::FCircle2d> CircleArray1;

	for(auto& ObjStruct : ObjectsArray)
	{
		if(ObjStruct.Get()->ObjectType == Rect)
		{
			if(ObjStruct.Get()->BActive)
			{
				RectArray1.Add(ObjStruct.Get()->GetAABBBOX());
			}
			else
			{
				RectArray0.Add(ObjStruct.Get()->GetAABBBOX());
			}
		}
		else if(ObjStruct.Get()->ObjectType == Circle)
		{
			if(ObjStruct.Get()->BActive)
			{
				CircleArray1.Add(UE::Geometry::FCircle2d(ObjStruct.Get()->Pos, ObjStruct.Get()->Radius));
			}
			else
			{
				CircleArray0.Add(UE::Geometry::FCircle2d(ObjStruct.Get()->Pos, ObjStruct.Get()->Radius));
			}
		}

		
		
	}
	RenderComp->GenerateRectMesh(RectArray0, false);
	RenderComp->GenerateRectMesh(RectArray1, true);
	RenderComp->GenerateCircleMesh(CircleArray0, false);
	RenderComp->GenerateCircleMesh(CircleArray1, true);
	RenderComp->GenerateUserRectMesh(UserRect);
	
}
void AQuadTreePawn::SpawnObj()
{
	if(!BAutoSpawn) return;
	if(ObjNumMax < 0)
	{
		GetWorldTimerManager().ClearTimer(TimerHandleSpawnObj);
		return;
	}

	--ObjNumMax;

	double XPos = UKismetMathLibrary::RandomFloatInRange(50.f  -Width, Width -50.f);
	double YPos = UKismetMathLibrary::RandomFloatInRange(50.f  -Height, Height -50.f);

	const FVector Pos = FVector(XPos, YPos, ZPos);
	SpawnObj(Pos);
}

void AQuadTreePawn::SpawnObj(FVector Pos)
{
	const int32 ObjType = UKismetMathLibrary::RandomIntegerInRange(0, 1);

	if(ObjType == 0)
	{
		const double Radius = UKismetMathLibrary::RandomFloatInRange(5, 10);
		const TSharedPtr<FObjectStruct> NewObj= MakeShareable(new FObjectStruct(FVector2D(Pos.X, Pos.Y), Radius));
		ObjectsArray.Add(NewObj);
		TreeRoot->InsertObj(NewObj);
		//UE_LOG(LogTemp,Warning, TEXT("Spawn circle: POS: %f %f, Raius: %f"), Pos.X, Pos.Y, Radius);
	}
	else if(ObjType == 1)
	{
		const double ObjWidth = UKismetMathLibrary::RandomFloatInRange(10, 20);
		const double ObjHeight = UKismetMathLibrary::RandomFloatInRange(10, 20);
		const TSharedPtr<FObjectStruct> NewObj= MakeShareable(new FObjectStruct(FVector2D(Pos.X, Pos.Y), ObjWidth, ObjHeight));
		ObjectsArray.Add(NewObj);
		TreeRoot->InsertObj(NewObj);
		//UE_LOG(LogTemp,Warning, TEXT("Spawn Rect: POS: %f %f, Width: %f, Height: %f"), Pos.X, Pos.Y, ObjWidth, ObjHeight);

	}
}

void AQuadTreePawn::ChangeVel()
{
	if(!BAutoChange) return;
	//FMath::RandInit(FPlatformTime::Seconds());

	for (auto& Obj : ObjectsArray)
	{
		double XVel = UKismetMathLibrary::RandomFloatInRange(-20, 20);
		double YVel = UKismetMathLibrary::RandomFloatInRange(-20, 20);
		Obj.Get()->Vel = FVector2D(XVel, YVel);
	}
}

void AQuadTreePawn::ChangePos(float DeltaTime)
{
	if(!BAutoChange) return;
	for (auto& Obj : ObjectsArray)
	{
		Obj.Get()->Pos += Obj.Get()->Vel * DeltaTime;
		Obj.Get()->Pos.X = UKismetMathLibrary::FClamp(Obj.Get()->Pos.X, -Width, Width);
		Obj.Get()->Pos.Y = UKismetMathLibrary::FClamp(Obj.Get()->Pos.Y, -Height, Height);
		Obj.Get()->InitAABB();
	}
}

void AQuadTreePawn::ActiveObjs(float DeltaTime)
{
	FVector SelectPos;
	if(!GetHitPoint(SelectPos)) return;

	UserRect = FBox2D(FVector2D(SelectPos.X - UserRectWidth * 0.5, SelectPos.Y - UserRectHeight * 0.5), FVector2D(SelectPos.X + UserRectWidth * 0.5, SelectPos.Y + UserRectHeight * 0.5));
	int32 Tmp = 0;
	TreeRoot->ActiveBoundAndObjs(UserRect, Tmp);
	QueryNum = Tmp;

	UE_LOG(LogTemp, Warning, TEXT("Object Num : %d , Query NUM : %d"), ObjectsArray.Num(), QueryNum);
}

void AQuadTreePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("zoomUpFov", IE_Pressed, this, &AQuadTreePawn::OnZoomUpFov);
	PlayerInputComponent->BindAction("zoomDownFov", IE_Pressed, this, &AQuadTreePawn::OnZoomDownFov);
	InputComponent->BindAction("MouseLeftClick", IE_Pressed, this, &AQuadTreePawn::OnMouseLeftClick);
	InputComponent->BindAction("MouseRightClick", IE_Pressed, this, &AQuadTreePawn::OnMouseRightClick);

}


void AQuadTreePawn::OnZoomUpFov()
{
	CameraComponent->SetFieldOfView(CameraComponent->FieldOfView - 5.f);
}

void AQuadTreePawn::OnZoomDownFov()
{
	CameraComponent->SetFieldOfView(CameraComponent->FieldOfView + 5.f);
}

void AQuadTreePawn::OnMouseLeftClick()
{
	FVector SelectObjPos;
	if(!GetHitPoint(SelectObjPos)) return;
	SpawnObj(SelectObjPos);
}

void AQuadTreePawn::OnMouseRightClick()
{
	FVector SelectObjPos;
	if(!GetHitPoint(SelectObjPos)) return;
	
	if(!BSelectMode)  //going to select point
	{
		for (auto& Obj : ObjectsArray)
		{
			
			if(Obj.Get()->GetAABBBOX().IsInsideOrOn({SelectObjPos.X, SelectObjPos.Y}))
			{
				SelectObj = Obj;
				BSelectMode = true;
				break;
			}
		}
	}
	else //move point
	{
		if(SelectObj != nullptr && SelectObj.Get() != nullptr)
		{
			SelectObj.Get()->Pos = {SelectObjPos.X, SelectObjPos.Y};
			BSelectMode = false;
		}
	}
}

bool AQuadTreePawn::GetHitPoint(FVector& HitPos)
{
	FVector2D MousePosition;
	if (Cast<APlayerController>(GetController())->GetMousePosition(MousePosition.X, MousePosition.Y))
	{
		FVector WorldPos;
		FVector WorldDirection;
		float TraceDistance = 2000000.f;
		Cast<APlayerController>(GetController())->DeprojectMousePositionToWorld(WorldPos, WorldDirection);
		{
			FHitResult HitResult;
			FCollisionQueryParams Params;
			//Params.AddIgnoredActor(this);
			// 发射射线
			if (GetWorld()->LineTraceSingleByChannel(HitResult, WorldPos, WorldPos + WorldDirection * TraceDistance, ECC_Visibility, Params))
			{
				//UE_LOG(LogTemp,Warning, TEXT("Hit POS: %f, %f, %f"), HitResult.ImpactPoint.X, HitResult.ImpactPoint.Y, HitResult.ImpactPoint.Z);
				HitPos = HitResult.ImpactPoint;
				return true;
			}
		}
	}

	return false;
}
