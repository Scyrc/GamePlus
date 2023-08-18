// Fill out your copyright notice in the Description page of Project Settings.


#include "AStarComp.h"

std::array<GridLocation, 4> SquareGrid::DIRS = {
	/* East, West, North, South */
	GridLocation{1, 0}, GridLocation{-1, 0},
	GridLocation{0, -1}, GridLocation{0, 1}
};

UAStarComp::UAStarComp()
{
	
	PrimaryComponentTick.bCanEverTick = true;

}


void UAStarComp::BeginPlay()
{
	Super::BeginPlay();

	
}

void UAStarComp::FindPathAStar(int width, int height, const TPair<int, int>& startLoc, const TPair<int, int>& goalLoc,
	const TMap<TPair<int, int>, int32>& walls, std::vector<TPair<int, int>>& pathRes)
{
	SquareGrid grid(width, height);

	for (auto& kv : walls)
	{
		GridLocation loc= {kv.Key.Key, kv.Key.Value};
		grid.walls[loc] = kv.Value;
	}

	

	GridLocation start{startLoc.Key, startLoc.Value}, goal{goalLoc.Key, goalLoc.Value};
	std::unordered_map<GridLocation, GridLocation> came_from;
	std::unordered_map<GridLocation, double> cost_so_far;
	a_star_search(grid, start, goal, came_from, cost_so_far);

	std::vector<GridLocation> path = reconstruct_path(start, goal, came_from);
	pathRes.clear();
	for (auto& loc : path)
	{
		pathRes.push_back(TPair<int,int>(loc.x, loc.y));
	}
}


void UAStarComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

