#include "Maze.h"

using namespace std;

// Global Maze Variables
static int orientation;  // Current Direction (Up Down Left Right)
static int current_node = 0; // Current Node Location
static int previous_node = 0;
static int blockCount = 0; // Discovered Block Count - implement Sorting 
static int V = 36;

// Definition: Block Discovered on vertex, all edges to vertex closed in Adjacency graph representation 
void blockVertex(int index, vector<vector<int>>&graph) {
    int xpos, ypos;
    if (index < 6) {
        xpos = 0;
        ypos = index;
    } else {
        xpos = floor(index / 6);
        ypos = index % 6;
    }
    // Setting all indices in adjacency matrix row to 0
    graph[xpos].assign(graph[xpos].size(),1e9); // Need to test If this is the correct Update Value for closing a branch
    // Setting all indices in adjacency matrix row to 0
    for (int i = 0; i < graph.size(); i++) {
        graph[i][ypos] = 1e9; 
    }
}

//Definiton: Breadth First Search Dijkstra Hybrid For Finding Shortest Distance to Destination Node;
void bfs(vector<vector<int> >& graph, int S,
         vector<int>& par, vector<int>& dist)
{
    // PAth Quese
    queue<int> q;
    dist[S] = 0;
    q.push(S); // Pushing Initial Source Node
    // Iterate till the queue is not empty
    while (!q.empty()) {
        // Pop the node at the front of the queue
        int node = q.front();
        q.pop();
        // Explore all the neighbours of the current node
        for (int neighbour : graph[node]) {
            // Check if the neighbouring node is not visited
            if (dist[neighbour] == 1e9) {
                // Mark the current node as the parent of
                // the neighbouring node
                par[neighbour] = node;
                // Mark the distance of the neighbouring
                // node as distance of the current node + 1
                dist[neighbour] = dist[node] + 1;
                // Insert the neighbouring node to the queue
                q.push(neighbour);
            }
        }
    }
}

// Definition: Outputs Shortest Path to Destination node using BFS/Dijkstra
vector<int> ShortestDistance(vector<vector<int> >& graph,
                           int S, int D, int V)
{
    // par[] array stores the parent of nodes
    vector<int> par(V, -1);

    // dist[] array stores distance of nodes from S
    vector<int> dist(V, 1e9);

    // function call to find the distance of all nodes and
    // their parent nodes
    bfs(graph, S, par, dist);

    if (dist[D] == 1e9) {
        Serial.println("Source and Destination are not connected");
        return {};
    }
    // vector path stores the shortest path
    vector<int> path;
    int currentNode = D;
    path.push_back(D);
    while (par[currentNode] != -1) {
        path.push_back(par[currentNode]);
        currentNode = par[currentNode];
    }

    return path;
}

// Definition: Handles Turn @ vertex based on vertex approach direction, and nexthop orientation
void vertexTurn(int nexthop) {

    if (nexthop == (current_node+6)) {
        // Turn DOWN
        switch(orientation){
            case RIGHT:
            turnConsistent(turn90R,RIGHT);
            break;
            case LEFT:
            turnConsistent(turn90L,LEFT);
            break;
            case UP:
            turnConsistent(turn180R,RIGHT);
            break;
            case DOWN:
            Serial.println("NoTurn");
            // No Turn
            break;

        }
        orientation  = DOWN;
    } else if (nexthop == (current_node-6)){
        // Turn Up
        switch(orientation){
                case RIGHT:
                turnConsistent(turn90L,LEFT);
                break;
                case LEFT:
                turnConsistent(turn90R,RIGHT);
                break;
                case UP:
                Serial.println("No Turn");
                break;
                case DOWN:
                turnConsistent(turn180R,RIGHT);
                break;
        }
        orientation = UP;
    } else if (nexthop == (current_node-1)){
    // Turn Left
    switch(orientation){
                case RIGHT:
                turnConsistent(turn180R,RIGHT);
                break;
                case LEFT:
                Serial.println("No Turn");
                break;
                case UP:
                turnConsistent(turn90L,LEFT);
                break;
                case DOWN:
                turnConsistent(turn90R,RIGHT);
                break;
        }
        orientation = LEFT;
    
    }else if (nexthop == (current_node+1)){
    // Turn Right
    switch(orientation){
                case RIGHT:
                Serial.println("No Turn");
                break;
                case LEFT:
                turnConsistent(turn180R,RIGHT);
                break;
                case UP:
                turnConsistent(turn90R,RIGHT);
                break;
                case DOWN:
                turnConsistent(turn90L,LEFT);
                break;
        }
        orientation = RIGHT;
    }
}

// Definition: Traverses from current_node to next-hop in Path (Follows a single line to the next vertex)
//             Note: 0 Signifies succesfull arrival, 1 signifies block
int traverseEdgeVertex(int nexthop){
    // Orient Facing the next vertex!
    vertexTurn(nexthop);

    if (lineFollowExit()) {      // Attempt to Progress to NextHop
        turnConsistent(turn180R,RIGHT);// // Block Encountered, Return to Previous Vertex
        if (orientation == RIGHT) {
            orientation = LEFT;
        } else if (orientation == LEFT) {
            orientation = RIGHT;
        } else if (orientation == UP) {
            orientation = DOWN;
        } else if (orientation == DOWN) {
            orientation = UP;
        }
        lineFollowExit(); // Return to source node;
        return 1;
    } else {
        
        current_node = nexthop;
    }
    // Arrived at node nexthop
    return 0;
}

// Definition: Traverse From current Node, all the way to original destination dynamically updating visitQueue, and Graph
void traversePath(int destination, vector<vector<int>>& graph, vector<int>& visitQueue){
    Serial.print("Current Node is: "); // #Debugging Structure
    Serial.print(current_node);
    Serial.print(" Destination Node is: ");
    Serial.println(destination);
    delay(1000);
    vector<int> path = ShortestDistance(graph,current_node, destination, V);// Determine path to next destination
    // Reverse Vector for in-order path
    reverse(path.begin(),path.end());
    path.erase(path.begin()); // Eliminating Redundant first node
    
    while (current_node != destination){
            
            if (traverseEdgeVertex(path[0])) { // If returns 1, then exited and returned to source node
                // Block Encountered at Node path[0]
                blockVertex(path[0],graph); // Closing all Paths to the vertex
                blockCount++; // Block Discovered, Iterate Block Count. #Update to iterate based on color
                auto index = find(visitQueue.begin(),visitQueue.end(),path[0]); 
                visitQueue.erase(index); // Node "visited" and closed off. Update VisitQueue
                path = ShortestDistance(graph,current_node,destination,V);
                reverse(path.begin(),path.end());
                path.erase(path.begin()); // Eliminating Redundant Start Node From BFS Path
            } else {
                auto index = find(visitQueue.begin(),visitQueue.end(),path[0]); //Remove the first element of path and update the VisitQueue
                visitQueue.erase(index);
                current_node = path[0]; // On final edge of path current_node will become path[0], Erase node from path
                path.erase(path.begin());
            }
        }
}

// Definition: 
void solveMaze(){
    vector<vector<int>> graph(V); // Maze Graph
    // Current Maze Visit Strategy: Zig-Zag w/ single robot.
    // Update, Update at Duty Split Below, creating two graphs and splitting search among two robots
    vector<int> visitQueue {1,2,3,4,5,
                        11,10,9,8,7,6,
                        12,13,14,15,16,17,
                        // Duty Split
                        23,22,21,20,19,18,
                        24,25,26,27,28,29,
                        35,34,33,32,31,30};

    // Edge Initialization
    vector<vector<int>> edges = {};

    for (int i = 0; i < V; i++){
        if((i+1)%6 != 0) {
        edges.push_back({i,i+1});
        }
        if(i<30) {
        edges.push_back({i,i+6});
        }
    }
    // Adjacency Matrix Initializaion: All Paths initially set to open, else INF
    for (auto edge : edges) {
        graph[edge[0]].push_back(edge[1]);
        graph[edge[1]].push_back(edge[0]);
    }

    // Starting Orientation
    orientation = RIGHT;
    current_node = 0; 

    // While VisitQueue Still Has Elements, Continue to Visit Nodes (Breakout when All Maze Nodes Have Been Visited)
    while ((!visitQueue.empty())) {
        
        int destination = visitQueue[0];
        // Determine Shortest Path to next node
        traversePath(visitQueue[0],graph,visitQueue); // Visit Queue Expected to shrink by 1-n values based on length of path
    }

    // If We Do not terminate at the Maze Exit, Navigate to exit
    if (current_node != 30) {
        traversePath(30,graph,visitQueue); // Node # 30 is grid Exit Value
    }
    
}




 
 