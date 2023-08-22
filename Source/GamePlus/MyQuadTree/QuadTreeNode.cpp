#include "QuadTreeNode.h"

#include "Kismet/KismetSystemLibrary.h"

UObject* QuadTreeNode::WorldObject = nullptr;
 float QuadTreeNode::ZPos = 300.f;

QuadTreeNode::~QuadTreeNode()
{
}

void QuadTreeNode::Update()
{
	BActive = false;
	for (auto& Obj : ObjectsArray)
	{
		Obj.Get()->BActive =false;
	}
	
	// Update OBJ which move out of this tree node
	TArray<TSharedPtr<FObjectStruct>> RemovedObjectsArray;
	for (int32 i =0; i< ObjectsArray.Num(); ++i)
	{
		
		if(!TreeBox.IsInside(ObjectsArray[i].Get()->GetAABBBOX())) 
		{
			RemovedObjectsArray.Add(ObjectsArray[i]);
			ObjectsArray.RemoveAtSwap(i);
			--i;
		}
	}

	if(Root != nullptr && RemovedObjectsArray.Num() > 0)
	{
		for (auto& Obj : RemovedObjectsArray)
		{
			Root->InsertObj(Obj);
		}
	}

	if(Root == nullptr && RemovedObjectsArray.Num() > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("some objects over scene bound, put them into root again"));
		for (auto& Obj : RemovedObjectsArray)
		{
			ObjectsArray.Add(Obj);
		}
	}

	// Update OBJ which move into one child
	if(!bIsLeaf) 
	{
		TArray<TSharedPtr<FObjectStruct>> MoveToChildObjectsArray;
		for (int32 i =0; i< ObjectsArray.Num(); ++i)
		{
			if(ChildNodes[TopLeft].Get()->TreeBox.IsInside(ObjectsArray[i].Get()->GetAABBBOX())) 
			{
				ChildNodes[TopLeft].Get()->InsertElementRecursive(ObjectsArray[i]);
				ObjectsArray.RemoveAtSwap(i);
				--i;
				continue;
			}
			if(ChildNodes[TopRight].Get()->TreeBox.IsInside(ObjectsArray[i].Get()->GetAABBBOX())) 
			{
				ChildNodes[TopRight].Get()->InsertElementRecursive(ObjectsArray[i]);
				ObjectsArray.RemoveAtSwap(i);
				--i;
				continue;
			}
			if(ChildNodes[BottomLeft].Get()->TreeBox.IsInside(ObjectsArray[i].Get()->GetAABBBOX())) 
			{
				ChildNodes[BottomLeft].Get()->InsertElementRecursive(ObjectsArray[i]);
				ObjectsArray.RemoveAtSwap(i);
				--i;
				continue;
			}
			if(ChildNodes[BottomRight].Get()->TreeBox.IsInside(ObjectsArray[i].Get()->GetAABBBOX())) 
			{
				ChildNodes[BottomRight].Get()->InsertElementRecursive(ObjectsArray[i]);
				ObjectsArray.RemoveAtSwap(i);
				--i;
				continue;
			}
		}
	}
	// Update Objs in children
	if(!bIsLeaf)
	{
		ChildNodes[TopLeft].Get()->Update();
		ChildNodes[TopRight].Get()->Update();
		ChildNodes[BottomLeft].Get()->Update();
		ChildNodes[BottomRight].Get()->Update();
	}

	
}

void QuadTreeNode::Draw()
{
	// draw TreeBox
	if(WorldObject)
	{
		if(BActive)
		{
			UKismetSystemLibrary::DrawDebugBox(WorldObject, FVector(Center.X, Center.Y, ZPos), FVector(TreeBox.GetExtent().X, TreeBox.GetExtent().Y, 1), FLinearColor::Green, FRotator::ZeroRotator, 0, 2);
		}
		else
		{
			UKismetSystemLibrary::DrawDebugBox(WorldObject, FVector(Center.X, Center.Y, ZPos), FVector(TreeBox.GetExtent().X, TreeBox.GetExtent().Y, 1), FLinearColor::Red, FRotator::ZeroRotator, 0, 0.5);
		}
	}
	
	if(!bIsLeaf)
	{
		ChildNodes[TopLeft].Get()->Draw();
		ChildNodes[TopRight].Get()->Draw();
		ChildNodes[BottomLeft].Get()->Draw();
		ChildNodes[BottomRight].Get()->Draw();
	}
}

void QuadTreeNode::ActiveBoundAndObjs(const FBox2D& Box2D, int32& QueryCount)
{
	++QueryCount;
	if(!TreeBox.Intersect(Box2D))  // 不相交
	{
		return;
	}
	BActive = true;
	
	for (auto& Obj : ObjectsArray)
	{
		++QueryCount;
		if(Obj.Get()->GetAABBBOX().Intersect(Box2D))
		{
			Obj.Get()->BActive =  true;
		}
	}
	if(!bIsLeaf)
	{
		ChildNodes[TopLeft].Get()->ActiveBoundAndObjs(Box2D, QueryCount);
		ChildNodes[TopRight].Get()->ActiveBoundAndObjs(Box2D, QueryCount);
		ChildNodes[BottomLeft].Get()->ActiveBoundAndObjs(Box2D, QueryCount);
		ChildNodes[BottomRight].Get()->ActiveBoundAndObjs(Box2D, QueryCount);
	}
}

void QuadTreeNode::InsertObj(TSharedPtr<FObjectStruct> Obj)
{
	if(!TreeBox.Intersect(Obj.Get()->GetAABBBOX()))  // 不相交
	{
		UE_LOG(LogTemp, Warning, TEXT("some objects not Intersect scene bound"));
		return;
	}
	InsertElementRecursive(Obj);
}

void QuadTreeNode::InsertElementRecursive(TSharedPtr<FObjectStruct> Obj)
{
	
	if(bIsLeaf) // leaves node
	{
		const bool bCanSplitTree = TreeBox.GetSize().SizeSquared() > FMath::Square(MinimumQuadSize);

		if(!bCanSplitTree || ObjectsArray.Num() < MaxElementNum)
		{
			ObjectsArray.Add(Obj);
			if (!bCanSplitTree)
			{
				UE_LOG(LogTemp, Warning, TEXT("Minimum size %f reached for quadtree at %s. Filling beyond capacity %d to %d"), MinimumQuadSize, *Center.ToString(), MaxElementNum, ObjectsArray.Num());
			}
		}
		else
		{
			Split();
			InsertElementRecursive(Obj);
		}
		return;
	}
	TSharedPtr<QuadTreeNode> QuadTreeNodeChild;
	const int32 NumQuads = GetQuads(Obj.Get()->GetAABBBOX(), QuadTreeNodeChild);
	check(NumQuads > 0);
	if(NumQuads == 1)
	{
		QuadTreeNodeChild->InsertElementRecursive(Obj);
	}
	else
	{
		// Overlaps multiple subtrees, store here
		ObjectsArray.Add(Obj);
	}
	
}

int32 QuadTreeNode::GetQuads(const FBox2d& box, TSharedPtr<QuadTreeNode>& ChildNode)
{
	int32 QuadCount = 0;
	check(!bIsLeaf);

	bool bNegX = box.Min.X <= Center.X;
	bool bNegY = box.Min.Y <= Center.Y;
	
	bool bPosX = box.Max.X >= Center.X;
	bool bPosY = box.Max.Y >= Center.Y;


	if(bNegX && bNegY)
	{
		++QuadCount;
		ChildNode = ChildNodes[BottomLeft];
	}

	if(bPosX && bPosY)
	{
		++QuadCount;
		ChildNode = ChildNodes[TopRight];
	}

	if(bPosX && bNegY)
	{
		++QuadCount;
		ChildNode = ChildNodes[BottomRight];
	}

	if(bNegX && bPosY)
	{
		++QuadCount;
		ChildNode = ChildNodes[TopLeft];
	}

	return QuadCount;

}

void QuadTreeNode::Split()
{
	check(bIsLeaf == true)

	const FVector2D Extent = TreeBox.GetExtent();
	const FVector2D XExtent = FVector2D(Extent.X, 0.f); 
	const FVector2D YExtent = FVector2D(0.f, Extent.Y); 
	/************************************************************************
	 *  ___________max
	 * |     |     |
	 * |     |     |
	 * |-----c------
	 * |     |     |
	 * min___|_____|
	 *
	 * We create new quads by adding xExtent and yExtent
	************************************************************************/

	const FVector2D C = Center;
	const FVector2D TM = Center + YExtent;
	const FVector2D ML = Center - XExtent;
	const FVector2D MR = Center + XExtent;
	const FVector2D BM = Center - YExtent;
	const FVector2D BL = TreeBox.Min;
	const FVector2D TR = TreeBox.Max;

	ChildNodes[TopLeft] = MakeShareable(new QuadTreeNode(FBox2D(ML, TM), Depth + 1, Root == nullptr ? this->AsShared() : Root));
	ChildNodes[TopRight] = MakeShareable(new QuadTreeNode(FBox2D(C, TR), Depth + 1, Root == nullptr ? this->AsShared() : Root));
	ChildNodes[BottomLeft] = MakeShareable(new QuadTreeNode(FBox2D(BL, C), Depth + 1, Root == nullptr ? this->AsShared() : Root));
	ChildNodes[BottomRight] = MakeShareable(new QuadTreeNode(FBox2D(BM, MR), Depth + 1, Root == nullptr ? this->AsShared() : Root));

	bIsLeaf = false;

	TArray<TSharedPtr<FObjectStruct>> OverlappingObjs;

	for (const auto& Obj : ObjectsArray)
	{
		TSharedPtr<QuadTreeNode> QuadTreeNodeChild;

		const int32 NumQuads = GetQuads(Obj.Get()->GetAABBBOX(), QuadTreeNodeChild);

		check(NumQuads > 0);

		if(NumQuads == 1)
		{
			QuadTreeNodeChild->ObjectsArray.Add(Obj);
		}
		else
		{
			OverlappingObjs.Add(Obj);
		}
	}
	ObjectsArray = OverlappingObjs;
}
