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
    for (auto& innerVec : graph) {
        innerVec.erase(remove(innerVec.begin(), innerVec.end(), index), innerVec.end());
    }
    graph[index].clear(); // Clear All Connections From The Node
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
        Serial.print((par[currentNode]));
        Serial.print("  ");
        currentNode = par[currentNode];
    }
    Serial.println();
    return path;
}

// Definition: Handles Turn @ vertex based on vertex approach direction, and nexthop orientation
void vertexTurn(int nexthop, Encoder& enc1, Encoder& enc2) {

    if (nexthop == (current_node+6)) {
        // Turn DOWN
        switch(orientation){
            case RIGHT:
            turnConsistent(turn90R,Right, enc1, enc2);
            break;
            case LEFT:
            turnConsistent(turn90L,Left, enc1, enc2);
            break;
            case UP:
            turnConsistent(turn180R,Right, enc1, enc2);
            break;
            case DOWN:
            //Serial.println("NoTurn");
            // No Turn
            break;

        }
        orientation  = DOWN;
    } else if (nexthop == (current_node-6)){
        // Turn Up
        switch(orientation){
                case RIGHT:
                turnConsistent(turn90L,Left, enc1, enc2);
                break;
                case LEFT:
                turnConsistent(turn90R,Right, enc1, enc2);
                break;
                case UP:
                //Serial.println("No Turn");
                break;
                case DOWN:
                turnConsistent(turn180R,Right, enc1, enc2);
                break;
        }
        orientation = UP;
    } else if (nexthop == (current_node-1)){
    // Turn Left
    switch(orientation){
                case RIGHT:
                turnConsistent(turn180R,Right, enc1, enc2);
                break;
                case LEFT:
                //Serial.println("No Turn");
                break;
                case UP:
                turnConsistent(turn90L,Left, enc1, enc2);
                break;
                case DOWN:
                turnConsistent(turn90R,Right, enc1, enc2);
                break;
        }
        orientation = LEFT;
    
    }else if (nexthop == (current_node+1)){
    // Turn Right
    switch(orientation){
                case RIGHT:
                //Serial.println("No Turn");
                break;
                case LEFT:
                turnConsistent(turn180R,Right, enc1, enc2);
                break;
                case UP:
                turnConsistent(turn90R,Right, enc1, enc2);
                break;
                case DOWN:
                turnConsistent(turn90L,Left, enc1, enc2);
                break;
        }
        orientation = RIGHT;
    }
}

// Definition: Traverses from current_node to next-hop in Path (Follows a single line to the next vertex)
//             Note: 0 Signifies succesfull arrival, 1 signifies block
int traverseEdgeVertex(int nexthop, Encoder& enc1, Encoder& enc2){
    // Orient Facing the next vertex!
    Serial.print("NextHop: ");
    Serial.println(nexthop);
    vertexTurn(nexthop,enc1,enc2);
    int blockPresent = sendRecvSingleMessage(1); // Query Server For Block Detection
    if (blockPresent){
        return 1;
    } else {
        Serial.println("Traversing Edge");
        lineFollowExit(360,1,enc1,enc2);
        current_node = nexthop;
    }
  
    return 0;
}

// Definition: Traverse From current Node, all the way to original destination dynamically updating visitQueue, and Graph
void traversePath(int destination, vector<vector<int>>& graph, vector<int>& visitQueue, Encoder& enc1, Encoder& enc2){
    Serial.print("Current Node is: "); // #Debugging Structure
    Serial.print(current_node);
    Serial.print(" Destination Node is: ");
    Serial.println(destination);
    
    vector<int> path = ShortestDistance(graph,current_node, destination, V);// Determine path to next destination
    // Reverse Vector for in-order path
    reverse(path.begin(),path.end());
    path.erase(path.begin()); // Eliminating Redundant first node
    
    while (current_node != destination){
            // Continually Traverse Edges On Path
            if (traverseEdgeVertex(path[0], enc1, enc2)) { // If TRUE, Block Encountered, Nexthop Needs to be Removed
                // Block Encountered at Node path[0]
                int sub = path[0];
                Serial.println("Blocking Graph Vertex");
                blockVertex(path[0],graph); // Closing all Paths to the vertex nexthop
                Serial.println("After Blocking Vertex");
                blockCount++; // Block Discovered, Iterate Block Count. #Update to iterate based on color
                auto index = find(visitQueue.begin(),visitQueue.end(),path[0]); 
                visitQueue.erase(index); // Node "visited" and closed off. Update VisitQueue
                Serial.println("Before Calculating next Path");
                if (destination == sub){
                    // Destination Node "Reached"... We need a new destination, return to Callee
                    return;
                }

                path = ShortestDistance(graph,current_node,destination,V);
                reverse(path.begin(),path.end());

                path.erase(path.begin()); // Eliminating Redundant Start Node From BFS Path
                Serial.println("After Calculating next Path");
            } else {
                
                auto index = find(visitQueue.begin(),visitQueue.end(),path[0]); //Remove the first element of path and update the VisitQueue
                if (index != visitQueue.end()){ // Index Not Found, Already Visited and Erased
                visitQueue.erase(index);
                }
                current_node = path[0]; // On final edge of path current_node will become path[0], Erase node from path
                path.erase(path.begin());
            }
        }
}

// Definition: 
void solveMaze(Encoder& enc1, Encoder& enc2){
    vector<vector<int>> graph(V); // Maze Graph
    // Current Maze Visit Strategy: Zig-Zag w/ single robot.
    // Update, Update at Duty Split Below, creating two graphs and splitting search among two robots
    vector<int> visitQueue {1,2,3,4,5,
                        11,10,9,8,7,6,
                        12,13,14,15,16,17,
                        // Duty Split Between Heltec 1 & Heltec 2
                        23,22,21,20,19,18,
                        24,25,26,27,28,29,
                        35,34,33,32,31,30};

    // Edge Initialization
    vector<vector<int>> edges = {};

    // If Node 4 Visited, Remove Node 5

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
    /*
    ShortestDistance(graph,2,4,V);
    delay(1000);
    blockVertex(3,graph);
    ShortestDistance(graph,2,4,V);
    delay(1000);

    */
    /*
    // Printing Graph For Display
    Serial.println(" GRAPH BEFORE DELETION");
    for (const auto& innerVec : graph) {
        for (int element : innerVec) {
            Serial.print(element);
            Serial.print(" ");
        }
        Serial.println();
    }
    Serial.println(" GRAPH AFTER DELETION");
    blockVertex(8,graph);
    for (const auto& innerVec : graph) {
        for (int element : innerVec) {
            Serial.print(element);
            Serial.print(" ");
        }
        Serial.println();
    }
    delay(10000);
    */
    // Starting Orientation
    
    orientation = RIGHT;
    current_node = 0; 

    // While VisitQueue Still Has Elements, Continue to Visit Nodes (Breakout when All Maze Nodes Have Been Visited)
    while ((!visitQueue.empty())) {
        int destination = visitQueue[0];
        // Determine Shortest Path to next node
        traversePath(visitQueue[0],graph,visitQueue, enc1, enc2); // Visit Queue Expected to shrink by 1-n values based on length of path
    }
    
    // If We Do not terminate at the Maze Exit, Navigate to exit
    if (current_node != 30) {
        traversePath(30,graph,visitQueue, enc1, enc2); // Node # 30 is grid Exit Value
        vertexTurn(36,enc1,enc2);
    }
    vertexTurn(36,enc1,enc2);
    sendRecvSingleMessage(4);
}




 
 