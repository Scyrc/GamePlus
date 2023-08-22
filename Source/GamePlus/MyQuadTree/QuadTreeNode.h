// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
enum EObjectType { Circle = 0, Rect = 1 };

class FObjectStruct
{
public:
	
	EObjectType ObjectType;
	FVector2D Pos;
	FVector2D Vel = FVector2D(0.f, 0.f); 
	float Radius = 0.f;
	float Width = 0.f;
	float Height = 0.f;
	FBox2D AABBBOX;
	bool BActive = false;
	FBox2D GetAABBBOX()
	{
		InitAABB();
		return AABBBOX;
	}
	
	FObjectStruct(FVector2D Position, float R)
	{
		ObjectType = Circle;
		Pos = Position;
		Radius = R;
		InitAABB();
	}
	FObjectStruct(FVector2D Position, float W, float H)
	{
		ObjectType = Rect;
		Pos = Position;
		Width = W;
		Height = H;
		InitAABB();
	}
	void InitAABB()
	{
		if(ObjectType == Circle)
		{
			AABBBOX.Max = {Pos.X + Radius, Pos.Y + Radius};
			AABBBOX.Min = {Pos.X - Radius, Pos.Y - Radius};
		}
		else if(ObjectType == Rect)
		{
			AABBBOX.Max = {Pos.X + Width * 0.5f, Pos.Y + Height * 0.5f};
			AABBBOX.Min = {Pos.X - Width * 0.5f, Pos.Y - Height * 0.5f};
		}
	}
 

};
class GAMEPLUS_API QuadTreeNode:public TSharedFromThis<QuadTreeNode>
{
public:
	static UObject* WorldObject;
	static float ZPos;
	QuadTreeNode() = delete;
	QuadTreeNode(const FBox2D& InBox, int32 DEPTH, TSharedPtr<QuadTreeNode> RootEle = nullptr) : TreeBox(InBox), Depth(DEPTH), Root(RootEle)
	{
		Center = TreeBox.GetCenter();
		bIsLeaf = true;
		BActive = false;
		ChildNodes.Init(nullptr, 4);
		//TreeBox.Max = {CenterPos.X + GirdSize.X, CenterPos.Y + GirdSize.Y};
		//TreeBox.Min = {CenterPos.X - GirdSize.X, CenterPos.Y - GirdSize.Y};
	}
	~QuadTreeNode();
	void Update();
	void Draw();
	void ActiveBoundAndObjs(const FBox2D& Box2D, int32& QueryCount);

	void InsertObj(TSharedPtr<FObjectStruct> Obj);
protected:
	void InsertElementRecursive(TSharedPtr<FObjectStruct> Obj);

	int32 GetQuads(const FBox2d& box, TSharedPtr<QuadTreeNode>& ChildNode);
	void Split();
private:
	enum QuadNames
	{
		TopLeft = 0,
		TopRight = 1,
		BottomLeft = 2,
		BottomRight = 3
	};
	FVector2D Center;
	//FVector2D GirdSize;
	/** AABB of the tree node */
	FBox2D TreeBox;
	bool bIsLeaf;
	int32 Depth = 0;
	int32 MaxElementNum = 4;
	bool BActive = false;
	float MinimumQuadSize = 100.f;
	TArray<TSharedPtr<FObjectStruct>> ObjectsArray;
	TSharedPtr<QuadTreeNode> Root;
	TArray<TSharedPtr<QuadTreeNode>> ChildNodes;
};
