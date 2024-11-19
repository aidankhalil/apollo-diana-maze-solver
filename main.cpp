#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <queue>
#include <unordered_set>
#include <stack>
#include <tuple>
#include <unordered_map>

using namespace std;

/* 
    cot4400, project 3 by: Aidan Khalil U9240-8495 
    this project implements an adjacency matrix based graph by reading input in from a text file, and calling a special breadth-first search algorithm in which
    an apollo and diana maze solution must be found. alternating between red and blue arrows, each of which have a direction, the algorithm must start at the top left arrow (graph vertex or "matrix cell") and find a valid
    path to the bullseye "o" at the bottom right of every maze. 
*/

const char colors[] = {'R', 'B'};

/* * * * * *  * * * * * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- * * * * *  * * * * * */
// structure to represent each vertex (arrow) and the connecting edges for graph implementation

struct edge;
struct vertex {
    char color;                 // 'R' for red, 'B' for blue, 'O' for bullseye
    string direction;           // arrow direction.  n e s w ne se sw nw
	int visited;                // visited flag
    vector<edge*> edges; 
    pair<vertex*, int> parent;  // used for backtracking
};

struct edge {
    vertex* v1_ptr;
    vertex* v2_ptr;
    int weight;
};

// graph class representing the maze
class mazeGraph {
private:
    vector<vector<vertex*>> matrix;   // 2D adjacency list (matrix) implementation
    vector<edge*> edgeList;           // edge list

public:
    // constructor to initialize the matrix based on rows and columns (first line of input in "input.txt")
    mazeGraph(int rows, int cols){
        matrix.resize(rows, vector<vertex*>(cols));
    }

    // function to add an arrow (vertex) to the graph
    void addArrow(int row, int col, char color, string direction){
        vertex* new_vertex = new vertex();
        new_vertex->color = color;
        new_vertex->direction = direction;
        new_vertex->visited = 0;
        matrix[row][col] = new_vertex;
    }

    void addEdge(vertex* v1, vertex* v2, int weight){
        edge* new_edge = new edge();
        new_edge->v1_ptr = v1;
        new_edge->v2_ptr = v2;
        new_edge->weight = weight;
        v1->edges.push_back(new_edge);
        edgeList.push_back(new_edge);
    }

    // function to display the graph (made for DEBUGGING use)
    void displayGraph(){
        for (const auto &row : matrix){
            for (const auto &vertex : row){
                cout << vertex->color;
                if (!vertex->direction.empty()){
                    cout << "-" << vertex->direction;
                }
                cout << " ";
            }
            cout << endl;
        }
    }

    void createEdges(){
        unordered_map<string, pair<int, int>> directions = {
        {"N", {-1, 0}}, {"E", {0, 1}}, {"S", {1, 0}}, {"W", {0, -1}},
        {"NE", {-1, 1}}, {"SE", {1, 1}}, {"SW", {1, -1}}, {"NW", {-1, -1}}
        };
        int y_dir, x_dir, y_dis, x_dis;
        for (int i = 0; i < matrix.size(); ++i){
            for (int j = 0; j < matrix[0].size(); ++j){
                if (matrix[i][j]->color == 'O')
                    continue;
                y_dir = directions[matrix[i][j]->direction].first;
                x_dir = directions[matrix[i][j]->direction].second;
                y_dis = x_dis = 0;
                for(int r=i+y_dir, c=j+x_dir; r >= 0 && r < matrix.size() && c >= 0 && c < matrix[0].size(); r+=y_dir, c+=x_dir){
                    if (matrix[i][j]->color != matrix[r][c]->color)
                    {
                        y_dis = (r - i)*y_dir;
                        x_dis = (c - j)*x_dir;
                        if (y_dis != 0)
                            addEdge(matrix[i][j], matrix[r][c], y_dis);
                        else
                            addEdge(matrix[i][j], matrix[r][c], x_dis);
                    }
                }
                    
            }
        }
    }

// BFS FUNCTION DESCRIPTION / LOGIC:
// Start at the top left arrow, and find a path to the bottom right vertex/cell (bullseye),
// represented with color 'O' and direction "", if encounter an endless loop, backtrack. When the travel path is found, return
// series of moves as a string according to output format specifications (amount of spaces moved (weight) followed by which direction taken (direction)),
// i.e. first 3 moves take you 3 spaces east, 3 spaces southwest, and 4 spaces output should begin as follows: 3E 3SW 4SE
// ** Each move must alternate arrow colors (from red to blue to red...) until the bullseye (color == 'O') has been reached. **

    string bfs(){
        int rows = matrix.size();
        int cols = matrix[0].size();

        queue<vertex*> q;

        // Start with the top-left corner with red color
        q.push(matrix[0][0]);
        matrix[0][0]->visited = 1;

        vertex* current;
        while (!q.empty()){
            current = q.front();
            q.pop();

            if (current->color == 'O'){
                break;
            }

            for (edge* e : current->edges){
                if (!(e->v2_ptr->visited)){
                    q.push(e->v2_ptr);
                    e->v2_ptr->visited = 1;
                    e->v2_ptr->parent.first = current;
                    e->v2_ptr->parent.second = e->weight;
                }
            }
        }
        
        stack<vertex*> s;
        while (current != matrix[0][0]){
            s.push(current);
            current = current->parent.first;
        }
        stringstream ss;
        while (!s.empty()){
            current = s.top();
            s.pop();
            ss << current->parent.second << current->parent.first->direction << " ";
        }
        return ss.str();
    }

    vector<edge*> getEdgeList(){
        return edgeList;
    }

    // Simple function to return the matrix
    vector<vector<vertex*>>& getMutableMatrix(){
        return matrix;
    }

};


// function to read the maze (from input) and construct graph based on data
mazeGraph* constructGraph(const string &filename){
    ifstream inputFile(filename);
    
    // check if the file is opened successfully
    if (!inputFile.is_open()){
        cerr << "error opening file: " << filename << endl;
        exit(EXIT_FAILURE);
    }

    // declare necessary variables. read and save them from "input.txt" file
    int rows, cols;
    inputFile >> rows >> cols;

    // call maze() method from mazeGraph class to construct 2-dimensional graph (adjacency matrix)
    mazeGraph* maze = new mazeGraph(rows, cols);

    // now read the next "r" lines which contain color and directional info for each arrow (vertex) in the maze (graph (adj. matrix)) 
    // each line has "c" values, where each value represents color of the arrow, followed by a hyphen, followed by the direction (n,e,s,w,ne,se,sw,nw) 
    for (int i = 0; i < rows; ++i){
        
        for (int j = 0; j < cols; ++j){
            char color;
            string direction;

            inputFile >> color;
            
            // check if it's the last iteration and the cell is the bottom-right one (assumed to always be the bullseye marked with color 'o', and no direction)
            if (i == rows - 1 && j == cols - 1){
                // manually set the bullseye arrow
                maze->addArrow(i, j, 'O', "");
            }
            else{
                inputFile.ignore(); // ignore the hyphen
                inputFile >> direction;
                maze->addArrow(i, j, color, direction);
            }
        }
    }

    // close input file and return the constructed maze
    inputFile.close();

    return maze;
}


/* * * * * *  * * * * * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- * * * * *  * * * * * */
int main() {
    // open "input.txt" for reading and call the constructGraph function
    const string filename = "input.txt";
    mazeGraph* maze = constructGraph(filename);
    maze->createEdges();
    
    // vector<vector<vertex*>> matrix = maze->getMutableMatrix();
    // display the constrtucted graph entitled "maze" for DEBUGGING purposes
    // maze->displayGraph();

    // call bfs graph traversal to reach bullseye
    string solution = maze->bfs();
    
    // Write the constructed path to "output.txt" file
    std::ofstream outputFile("output.txt");
    if (outputFile.is_open()){
        outputFile << solution;
        // Close the file
        outputFile.close();
        std::cout << "content written to output.txt successfully." << std::endl;
    } 
    else{
        std::cerr << "error opening output.txt for writing." << std::endl;
    }


    return 0;

}
/* * * * * *  * * * * * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- * * * * *  * * * * * */