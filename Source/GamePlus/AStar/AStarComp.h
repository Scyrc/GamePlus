// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <iostream>
#include <iomanip>
#include <unordered_map>
#include <unordered_set>
#include <array>
#include <vector>
#include <utility>
#include <queue>
#include <tuple>
#include <algorithm>
#include <cstdlib>

#include "AStarComp.generated.h"

//
// Created by sc on 2023/8/17.
//

struct GridLocation
{
    int x, y;
};
namespace std {
/* implement hash function so we can put GridLocation into an unordered_set */
    template <> struct hash<GridLocation> {
        std::size_t operator()(const GridLocation& id) const noexcept {
            // NOTE: better to use something like boost hash_combine
            return std::hash<int>()(id.x ^ (id.y << 16));
        }
    };
}

inline double heuristic(GridLocation a, GridLocation b) {
    return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}

inline bool operator == (GridLocation a, GridLocation b) {
    return a.x == b.x && a.y == b.y;
}

inline bool operator != (GridLocation a, GridLocation b) {
    return !(a == b);
}

inline bool operator < (GridLocation a, GridLocation b) {
    return std::tie(a.x, a.y) < std::tie(b.x, b.y);
}
struct SquareGrid
{
    static std::array<GridLocation, 4> DIRS;

    int width, height;
    std::unordered_map<GridLocation, int> walls;  // -1 不可通过， 6:海洋 5 沙漠

    SquareGrid(int width_, int height_) : width(width_), height(height_) {}

    bool in_bounds(GridLocation id) const
    {
        return 0 <= id.x && id.x < width &&
               0 <=id.y && id.y < height;
    }

    bool passable(GridLocation id) const
    {
        return walls.find(id) == walls.end() || walls.find(id)->second > 0;
    }

    std::vector<GridLocation> neighbors(GridLocation id) const
    {
        std::vector<GridLocation> results;

        for(GridLocation dir : DIRS)
        {
            GridLocation next{id.x + dir.x, id.y + dir.y};
            if(in_bounds(next) && passable(next))
            {
                results.push_back(next);
            }
        }

        if((id.x + id.y ) % 2  == 0)
        {
            std::reverse(results.begin(), results.end());
        }

        return results;
    }

    double cost(GridLocation from_node, GridLocation to_node) const {
       

        if( walls.find(to_node) == walls.end())
            return 1;
        if( walls.find(to_node)->second == 1)
            return 3;

        if( walls.find(to_node)->second == 2)
            return 6;

        return 1;
    }
};



struct GridWithWeights: SquareGrid {
    std::unordered_set<GridLocation> forests;
    GridWithWeights(int w, int h): SquareGrid(w, h) {}
    double cost(GridLocation from_node, GridLocation to_node) const {
        return forests.find(to_node) != forests.end()? 5 : 1;
    }
};
template<typename Location>
std::vector<Location> reconstruct_path(
        Location start, Location goal,
        std::unordered_map<Location, Location> came_from)
{
    std::vector<Location> path;

    if(came_from.find(goal) == came_from.end())
        return path;

    Location current = came_from[goal];
    while (current != start)
    {
        path.push_back(current);
        current = came_from[current];
    }

    //path.push_back(start);

    std::reverse(path.begin(), path.end());
    return path;
}


template<typename T, typename priority_t>
struct PriorityQueue {
    typedef std::pair<priority_t, T> PQElement;
    std::priority_queue<PQElement, std::vector<PQElement>,
            std::greater<PQElement>> elements;

    inline bool empty() const {
        return elements.empty();
    }

    inline void put(T item, priority_t priority) {
        elements.emplace(priority, item);
    }

    T get() {
        T best_item = elements.top().second;
        elements.pop();
        return best_item;
    }
};

template<typename Location, typename Graph>
void a_star_search
        (Graph graph,
         Location start,
         Location goal,
         std::unordered_map<Location, Location>& came_from,
         std::unordered_map<Location, double>& cost_so_far)
{
    PriorityQueue<Location, double> frontier;
    frontier.put(start, 0);

    came_from[start] = start;
    cost_so_far[start] = 0;

    while (!frontier.empty())
    {
        Location current = frontier.get();

        if(current == goal)
        {
            break;
        }

        for(Location next : graph.neighbors(current))
        {
            double new_cost = cost_so_far[current] + graph.cost(current, next);
            if(cost_so_far.find(next) == cost_so_far.end() || new_cost < cost_so_far[next])
            {
                cost_so_far[next] = new_cost;
                came_from[next] = current;

                double priority = new_cost + heuristic(next, goal);
                frontier.put(next, priority);
            }
        }
    }
}


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GAMEPLUS_API UAStarComp : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAStarComp();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;

public:

    void FindPathAStar(int width, int height, const TPair<int, int>& start, const TPair<int, int>& goal,  const TMap<TPair<int, int>, int32>& walls, std::vector<TPair<int, int>>& path);

		
};
