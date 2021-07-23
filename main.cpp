#include <iostream>
#include <fstream>
#include <set>
#include <ctime>
#include <vector>
#include <string>
#include <algorithm>

class Graph
{
public:
	//Forward Declaration of Vertex class since it needs to be a part of the Edge class
	class Vertex;

	//Edge that acts as a link between Vertices
	//Contains data for target Vertex, a next Node and a capacity for the edge established between the two
	class Edge
	{
	public:
		Vertex* target;
		Edge* next;
		int capacity;
		int flow;

		//Constructor
		Edge(Vertex* t, Edge* n, int c)
		{
			target = t;
			next = n;
			capacity = c;
			flow = 0;
		}
	};

	//Base Data Structure Vertex
	//Contains an id, along with an adjacency list, for all the edges that the Vertex shares
	class Vertex
	{
	public:
		int id;
		Edge* adjacencyList;

		//Constructor
		Vertex(int _id)
		{
			id = _id;
			adjacencyList = nullptr;
		}

		//Add an Edge (an adjacency) to the adjacencyList for the given Vertex with a given capacity
		void addAdjacency(Vertex* v, int capacity)
		{
			if (v != nullptr)
			{
				adjacencyList = new Edge(v, adjacencyList, capacity);
			}
		}
	};

	//Vector for vertices
	std::vector<Vertex*> vertices;
	bool print;

	//Constructor for Graph
	Graph(std::ifstream& file, bool toPrint)
	{
		print = toPrint;
		std::string fileName = "";
		std::string line = "";
		bool error = false;

		int lineNumber = 0;
		int numVertices = 0;

		//Do-while loop for entering file path
		//Loops as long as the user enters a valid file path
		do
		{
			//Error message
			if (error)
			{
				std::cout << "\nInvalid file path entered. Please try again" << std::endl;
			}

			//Input file path
			//Append .txt if not already present or if fileName entered is less than 4 characters long to prevent exception
			std::cout << "Enter file path : ";
			std::cin >> fileName;
			if (fileName.length() < 4 || fileName.substr(fileName.length() - 4) != ".txt")
			{
				fileName += ".txt";
			}

			//Open file with given fileName
			file.open(fileName);
			error = true;

		} while (!file);

		std::cout << std::endl;

		//While loop for getting lines from the file
		while (std::getline(file, line))
		{
			//Finds the number of vertices needed for the graph, and adds as many vertices
			if (lineNumber == 0)
			{
				numVertices = std::stoi(line);
				for (int i = 0; i < numVertices; i++)
				{
					addVertex();
				}
			}
			//For all other lines, call function parse
			if (line != "" && lineNumber != 0)
			{
				parse(line);
			}
			lineNumber++;
		}
		std::cout << "The graph has been populated with " << numVertices << " vertices\n";
		std::cout << std::endl;
	}

	//Function to parse the line read from the data file
	//Parses the line and assigns the values for from, to and capacity
	//Calls function addEdge with these values
	void parse(std::string line)
	{
		int numberOfSpaces = 0;
		int from = 0;
		int to = 0;
		int capacity = 0;
		std::string temp = "";

		for (int i = 0; i < line.length(); i++)
		{
			if (line[i] != ' ')
			{
				temp += line[i];
			}
			if (line[i] == ' ')
			{
				if (numberOfSpaces == 0)
				{
					from = std::stoi(temp);
					temp = "";
				}
				else if (numberOfSpaces == 1)
				{
					to = std::stoi(temp);
					temp = "";
				}
				numberOfSpaces++;
			}
			else if (i == line.length() - 1)
			{
				capacity = std::stoi(temp);
				temp = "";
				addEdge(from, to, capacity);
			}
		}
	}

	//Function to add vertex to the vertices array
	void addVertex()
	{
		vertices.push_back(new Vertex(vertices.size()));
	}

	//Function to add Edge between two vertices with a capacity
	void addEdge(int from, int to, int capacity)
	{
		//Prints edge information is print is set to true
		if (print)
		{
			std::cout << "Adding edge from vertex " << vertices[from]->id << " to vertex " << vertices[to]->id << " with capacity " << capacity << std::endl;
		}
		vertices[from]->addAdjacency(vertices[to], capacity);
	}

	//Function to print adjacency lists for the graph
	//Prints the vertices and its established edges, along with its flow and capacity
	//Prints the current state of the graph
	void printAdjacencyLists()
	{
		std::cout << "The graph's current state will be printed below\n";
		for (int i = 0; i < vertices.size(); i++)
		{
			std::cout << vertices[i]->id << " -> ";
			Edge* n = vertices[i]->adjacencyList;
			while (n != nullptr)
			{
				std::cout << n->target->id << '(' << n->flow << '/' << n->capacity << ')' << ' ';
				n = n->next;
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;
	}

	//Function to return Edge* given two vertices
	//Searches for vertex w in the adjacency list of vertex v and returns if found
	Edge* getNode(Vertex* v, Vertex* w)
	{
		for (Edge* n = v->adjacencyList; n != nullptr; n = n->next)
		{
			if (n->target->id == w->id)
			{
				return n;
			}
		}
		return nullptr;
	}

	//Function to find the successor value from the adjacency list
	//As long as the current flow of the Edge is less than the capacity, and the Vertex in question doesn't exist in the closed list
	//Returns the Vertex accordingly
	Vertex* findSuccessor(Vertex* v, std::set<Vertex*>& c)
	{
		for (Edge* n = v->adjacencyList; n != nullptr; n = n->next)
		{
			if ((n->flow < n->capacity) && (c.find(n->target) == c.end()))
			{
				return n->target;
			}
		}
		return nullptr;
	}

	//Function to run depth-first search
	//Runs from source Vertex to target Vertex to find a path from source to target
	std::vector<Vertex*> DFS(Vertex* source, Vertex* target)
	{
		std::vector<Vertex*> o;
		std::set<Vertex*> c;

		o.push_back(source);
		c.insert(source);

		//Loop as long as the open list is not empty
		while (!o.empty())
		{
			Vertex* v = o[o.size()-1];
			Vertex* w = findSuccessor(v, c);

			if (w != nullptr)
			{
				o.push_back(w);
				c.insert(w);
				//If w is the target, it means we have found a path
				if (w == target)
				{
					//Printing out the path that has been found
					//Prints if print is set to true
					if (print)
					{
						std::cout << "Path found - ";
						for (int i = 0; i < o.size(); i++)
						{
							std::cout << o[i]->id;
							if (i != o.size() - 1)
							{
								std::cout << " -> ";
							}
						}
						std::cout << std::endl;
					}

					//Return the vector as it contains the nodes along the path
					return o;
				}
			}
			else
			{
				//Delete the last element
				o.pop_back();
			}
		}
		return {};
	}

	//Find the available capacity between vertex v and w
	//Subtract current flow from capacity to get the desired value
	int AvailableCapacity(Vertex* v, Vertex* w)
	{
		return getNode(v, w)->capacity - getNode(v, w)->flow;
	}

	//Function implementing the Ford Fulkerson algorithm to find the maximum flow from the source Vertex to the target Vertex
	int FordFulkerson(Vertex* source, Vertex* target)
	{
		//Stack containing path between source and target Vertex
		std::vector<Vertex*> stack = DFS(source, target);

		//Loop while stack is not empty
		while (!stack.empty())
		{
			//Find the available capacity between first and second values of the stack
			int f = AvailableCapacity(stack[0], stack[1]);

			//Find the minimum flow along the given path
			for (int i = 0; i < stack.size() - 1; i++)
			{
				f = std::min(f, AvailableCapacity(stack[i], stack[i + 1]));
			}

			//Prints flow information is print is set to true
			if (print)
			{
				std::cout << "Adding a flow of " << f << " to the edge\n" << std::endl;
			}

			//Add the minimum flow to all edges along the path
			for (int i = 0; i < stack.size() - 1; i++)
			{
				getNode(stack[i], stack[i + 1])->flow += f;
			}

			//Refresh stack by running DFS on updated graph
			stack = DFS(source, target);
		}

		//Calculating the maximum flow for the network
		int total = 0;
		for (Edge* n = source->adjacencyList; n != nullptr; n = n->next)
		{
			total+=n->flow;
		}

		//Return total
		std::cout << "Maximum flow for the graph has been found - ";
		return total;
	}
};

int main()
{
	//Timer Variables
	std::clock_t start;
	double duration;

	std::ifstream file;

	//Initialise Graph variable on the heap
	//Passing bool variable determines whether additional information will be printed or not
	Graph* g = new Graph(file, false);

	//Printing state of graph after initialisation
	//Commented out to see results faster (Doesn't necessarily affect the runtime value at the end)
	//g->printAdjacencyLists();

	//Starting time after data has been input into file
	//Reduces impact on runtime and potential time loss when user fails to enter right data
	start = std::clock();
	
	//Running Ford Fulkerson algorithm and printing out maximum flow of the graph
	std::cout << g->FordFulkerson(g->vertices[0], g->vertices[g->vertices.size() - 1]) << std::endl << std::endl;

	//Printing state of graph after maximum flow has been found
	//Commented out to prevent it from impacting the runtime
	//g->printAdjacencyLists();

	//Delete g off the heap
	delete g;

	duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;
	std::cout << "\nElapsed Time - " << duration << " seconds" << std::endl;

	return 1;
}