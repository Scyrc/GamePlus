// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"

#include "AStarPawn.h"

void AMyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	InputComponent->BindAction("MouseLeftClick", IE_Pressed, this, &AMyPlayerController::OnMouseLeftClick);
	//InputComponent->BindAction("MouseLeftClick", IE_Released, this, &AMyPlayerController::OnMouseLeftReleased);

}
void AMyPlayerController::OnMouseLeftClick()
{
	// 获取鼠标位置
	FVector2D MousePosition;
	if (GetMousePosition(MousePosition.X, MousePosition.Y))
	{
		// 在这里可以使用MousePosition进行处理
		//UE_LOG(LogTemp, Warning, TEXT("mouse Position: X=%f, Y=%f"), MousePosition.X, MousePosition.Y);

		FVector WorldPos;
		FVector WorldDirection;
		float TraceDistance = 2000000.f;
		DeprojectMousePositionToWorld(WorldPos, WorldDirection);
		//UE_LOG(LogTemp, Warning, TEXT("World Position: X=%f, Y=%f, Z=%f"), WorldPos.X, WorldPos.Y, WorldPos.Z);
		//UE_LOG(LogTemp, Warning, TEXT("WorldDirection: X=%f, Y=%f, Z=%f"), WorldDirection.X, WorldDirection.Y, WorldDirection.Z);

		{
			FHitResult HitResult;
			FCollisionQueryParams Params;
			Params.AddIgnoredActor(this); // 忽略玩家控制器自身

			FVector Start = WorldPos;
			FVector End = WorldPos + WorldDirection * TraceDistance;
		

			/*DrawDebugLine(
				GetWorld(),
				Start,
				End,
				FColor::Black,
				false, // 是否持久显示线段
				10.f, // 线段的持久显示时间
				0,
				0.1f // 线段的宽度
			);*/

			// 发射射线
			if (GetWorld()->LineTraceSingleByChannel(HitResult, WorldPos, WorldPos + WorldDirection * TraceDistance, ECC_Visibility, Params))
			{
				// 检测到碰撞，可以在这里处理选中物体的逻辑
				auto HitComp = HitResult.GetComponent();
				if (HitComp)
				{
					// 在这里对选中的物体进行处理
					//UE_LOG(LogTemp, Display, TEXT("Selected Actor: %s"), *HitComp->GetName());
					//UE_LOG(LogTemp, Display, TEXT("Hit Point: X=%f, Y=%f, Z=%f"), HitResult.ImpactPoint.X, HitResult.ImpactPoint.Y,HitResult.ImpactPoint.Z);

				}

				auto pawn = Cast<AAStarPawn>(GetPawn());
				if(pawn)
				{
					pawn->UserClickedPoint(HitResult.ImpactPoint);
				}
			}
		}
	}
}

void AMyPlayerController::OnMouseLeftReleased()
{
	// 获取鼠标位置
	FVector2D MousePosition;
	if (GetMousePosition(MousePosition.X, MousePosition.Y))
	{
		

		FVector WorldPos;
		FVector WorldDirection;
		float TraceDistance = 2000000.f;
		DeprojectMousePositionToWorld(WorldPos, WorldDirection);
	

		{
			FHitResult HitResult;
			FCollisionQueryParams Params;
			Params.AddIgnoredActor(this); // 忽略玩家控制器自身

			FVector Start = WorldPos;
			FVector End = WorldPos + WorldDirection * TraceDistance;
			

			// 发射射线
			if (GetWorld()->LineTraceSingleByChannel(HitResult, WorldPos, WorldPos + WorldDirection * TraceDistance, ECC_Visibility, Params))
			{
				auto pawn = Cast<AAStarPawn>(GetPawn());
				if(pawn)
				{
					pawn->UserMovePoint(HitResult.ImpactPoint);
				}
			}
		}
	}
}


