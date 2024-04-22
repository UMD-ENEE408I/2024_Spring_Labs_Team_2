#ifndef MAZE
#define MAZE

#include "LineFollow.h"
#include <Arduino.h>
#include <vector>
#include <queue>

#include <Algorithm>
using namespace std;

const int UP =0;
const int DOWN =1;
const int RIGHT =2;
const int LEFT=  3;

// Shortest Path/Dijkstras Functions
void bfs(vector<vector<int> >& graph, int S,
         vector<int>& par, vector<int>& dist);

vector<int> ShortestDistance(vector<vector<int> >& graph,
                           int S, int D, int V);

// Maze Traversal Functions
void solveMaze();

int traverseEdgeVertex(int nexthop);

void traversePath(int destination);

//void setupMaze();

void print(vector<vector<int>> graph);


#endif