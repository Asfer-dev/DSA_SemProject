#include <iostream>
#include <vector>
#include <tuple>
#include <cmath>
#include <fstream>
#include <sstream>
#include <queue>
#include <stack>
#include <omp.h>

using namespace std;

struct LinkedList;

struct Node
{
  int id;
  string code;
  string name;
  double latitude;
  double longitude;
  int population;
  vector<string> adjacentCountries;

  Node()
  {
  }

  Node(int id, string code, string name, double latitude, double longitude, int population, vector<string> adjacentCountries)
  {
    this->id = id;
    this->code = code;
    this->name = name;
    this->latitude = latitude;
    this->longitude = longitude;
    this->population = population;
    this->adjacentCountries = adjacentCountries;
  }
};

double toRadians(double degree)
{
  return degree * M_PI / 180.0;
}

// Function to calculate the Haversine distance between two points
double haversineDistance(double lat1, double lon1, double lat2, double lon2)
{
  const double EarthRadius = 6371.0; // Earth radius in kilometers

  // Convert degrees to radians
  lat1 = toRadians(lat1);
  lon1 = toRadians(lon1);
  lat2 = toRadians(lat2);
  lon2 = toRadians(lon2);

  // Calculate differences in coordinates
  double dLat = lat2 - lat1;
  double dLon = lon2 - lon1;

  // Haversine formula
  double a = sin(dLat / 2) * sin(dLat / 2) +
             cos(lat1) * cos(lat2) * sin(dLon / 2) * sin(dLon / 2);
  double c = 2 * atan2(sqrt(a), sqrt(1 - a));

  // Calculate distance
  double distance = EarthRadius * c;

  return distance;
}
int findCountry(vector<Node> nodes, int start, int end, string key);

class Graph
{
public:
  vector<vector<int>> adjacencyMatrix;
  int numberOfNodes;
  Graph(vector<Node> &nodes, vector<tuple<int, int, int>> &weightedEdges)
  {
    numberOfNodes = nodes.size();
    // Initialize the adjacency matrix with infinity (assuming no direct connection)
    for (int i = 0; i < nodes.size(); ++i)
    {
      vector<int> row(nodes.size(), INT_MAX);
      adjacencyMatrix.push_back(row);
    }

    // Populate the adjacency matrix based on weighted edges
    for (const auto &edge : weightedEdges)
    {
      int from = get<0>(edge);
      int to = get<1>(edge);
      int cost = get<2>(edge);
      adjacencyMatrix[from][to] = cost;
      // If the graph is undirected, uncomment the line below
      // adjacencyMatrix[to][from] = cost;
    }
  }

  void displayMatrix(vector<Node> nodes)
  {
    for (int i = 0; i < adjacencyMatrix.size(); i++)
    {
      cout << nodes[i].name << endl;
      for (int j = 0; j < adjacencyMatrix.size(); j++)
      {
        if (adjacencyMatrix[i][j] != INT_MAX)
        {
          cout << nodes[j].name << ": " << to_string(adjacencyMatrix[i][j]) << "km -- ";
        }
      }
      cout << "N/A" << endl
           << endl;
    }
  }

  void bfsTraversal(int vertex)
  {
    if (vertex >= numberOfNodes || vertex < 0)
    {
      return;
    }
    vector<bool> visited(numberOfNodes, false);
    queue<int> q;
    q.push(vertex);
    while (q.empty() == false)
    {
      vertex = q.front();
      q.pop();
      if (visited[vertex] == false)
      {
        visited[vertex] = true;
        cout << vertex << " ";
        for (int i = 0; i < numberOfNodes; i++)
        {
          if (adjacencyMatrix[vertex][i] == true)
          {
            q.push(i);
          }
        }
      }
    }
  }
  void dfsTraversal(int vertex)
  {
    if (vertex >= numberOfNodes || vertex < 0)
    {
      return;
    }
    vector<bool> visited(numberOfNodes, false);
    stack<int> s;
    s.push(vertex);
    while (s.empty() == false)
    {
      vertex = s.top();
      s.pop();
      if (visited[vertex] == false)
      {
        visited[vertex] = true;
        cout << vertex << " ";
        for (int i = 0; i < numberOfNodes; i++)
        {
          if (adjacencyMatrix[vertex][i] == true)
          {
            s.push(i);
          }
        }
      }
    }
  }
  void printPrims(int parent[], int distance[], vector<Node> nodes)
  {
    cout << "Node  :  Parent  :  Weight" << endl;
    for (int i = 0; i < numberOfNodes; i++)
    {
      if (distance[i] != INT_MAX)
      {
        cout << nodes[i].name << " :: " << nodes[parent[i]].name << " :: " << distance[i] << endl;
      }
    }
  }
  void printDijkstra(int parent[], vector<Node> nodes)
  {
    int temp;
    string destination;
    string path = "";
    cout << "Enter your destination country: ";
    getline(cin >> ws, destination);
    cout << endl
         << "Source :: ";
    temp = findCountry(nodes, 0, nodes.size() - 1, destination);
    int counter = 0;
    while (temp != -1)
    {
      counter++;
      path = nodes[temp].name + "  " + path;
      temp = parent[temp];
    }
    if (counter == 1)
    {
      cout << "The countries are not accessible by road ";
    }
    else
    {
      cout << path;
    }
    cout << ":: Destination" << endl;
  }
  int minimumUnvisitedNode(int distance[], bool visited[])
  {
    int minimum;
    int dist = INT_MAX;
    for (int i = 0; i < numberOfNodes; i++)
    {
      if (distance[i] <= dist && visited[i] == false)
      {
        dist = distance[i], minimum = i;
      }
    }
    return minimum;
  }
  void dijkstra(int source, vector<Node> nodes)
  {
    int distance[numberOfNodes];
    bool visited[numberOfNodes];
    int parent[numberOfNodes];

    for (int i = 0; i < numberOfNodes; i++)
    {
      distance[i] = INT_MAX, visited[i] = false, parent[i] = -1;
    }

    distance[source] = 0;

    for (auto i = 0; i < numberOfNodes - 1; i++)
    {
      int vertex = minimumUnvisitedNode(distance, visited);

      visited[vertex] = true;

      for (int j = 0; j < numberOfNodes; j++)
      {
        if (visited[j] == false && adjacencyMatrix[vertex][j] != INT_MAX && distance[vertex] != INT_MAX && distance[vertex] + adjacencyMatrix[vertex][j] < distance[j])
        {
          distance[j] = distance[vertex] + adjacencyMatrix[vertex][j];
          parent[j] = vertex;
        }
      }
    }
    printDijkstra(parent, nodes);
  }
  void prims(int source, vector<Node> nodes)
  {
    int distance[numberOfNodes];
    bool visited[numberOfNodes];
    int parent[numberOfNodes];

    for (int i = 0; i < numberOfNodes; i++)
    {
      distance[i] = INT_MAX, visited[i] = false, parent[i] = -1;
    }

    distance[source] = 0;

    for (auto i = 0; i < numberOfNodes - 1; i++)
    {
      int vertex = minimumUnvisitedNode(distance, visited);

      visited[vertex] = true;

      for (int j = 0; j < numberOfNodes; j++)
      {
        if (visited[j] == false && adjacencyMatrix[vertex][j] != INT_MAX && adjacencyMatrix[vertex][j] < distance[j])
        {
          distance[j] = adjacencyMatrix[vertex][j];
          parent[j] = vertex;
        }
      }
    }
    printPrims(parent, distance, nodes);
  }
};

int findCountry(vector<Node> nodes, int start, int end, string key)
{
  if (start > end)
  {
    return -1;
  }

  else
  {
    int mid = (start + end) / 2;
    // cout << start << " " << end << endl;
    if (nodes[mid].name.compare(key) == 0)
    {
      return mid;
    }
    else if (nodes[mid].name.compare(key) > 0)
    {
      return findCountry(nodes, start, mid + 1, key);
    }
    else
    {
      return findCountry(nodes, mid - 1, end, key);
    }
  }
}

int main()
{
  // Dataset reading
  ifstream dataFile;
  dataFile.open("world_coordinates.csv");
  string line = "";
  getline(dataFile, line);
  line = "";
  vector<Node> nodes;
  int i = 0;
  int errcount = 0;
  while (getline(dataFile, line) && i < 300)
  {
    int id;
    string code;
    string name;
    double latitude;
    double longitude;
    int population;
    vector<string> adjacentCountries;
    string ts;

    istringstream lineString(line);

    getline(lineString, code, ',');
    getline(lineString, name, ',');

    try
    {
      getline(lineString, ts, ',');
      latitude = stod(ts);
      ts = "";
    }
    catch (const invalid_argument &ia)
    {
      errcount++;
      cerr << "Invalid argument: " << ia.what() << endl;
      cout << "id: " << (i + 1) << endl;
      latitude = 0;
    }

    try
    {
      getline(lineString, ts, ',');
      longitude = stod(ts);
      ts = "";
    }
    catch (const invalid_argument &ia)
    {
      errcount++;
      cerr << "Invalid argument: " << ia.what() << endl;
      cout << "id: " << (i + 1) << endl;
      longitude = 0;
    }

    getline(lineString, ts, ',');
    population = atoi(ts.c_str());
    ts = "";

    getline(lineString, ts, ',');
    if (!ts.empty() && ts.front() == '"')
    {
      string temp = "";
      getline(lineString, temp, '"');
      ts += ',' + temp;
      ts = ts.substr(1);
      getline(lineString, temp, ',');
    }
    string countryName = "";
    for (int i = 0; i < ts.size(); i++)
    {
      if (ts[i] == ',')
      {
        // for (auto &node : nodes)
        // {
        //   cout << (node.name) << " " << countryName << endl;
        //   if (node.name.compare(countryName) == 0)
        //   {
        //     // adjacentCountries = new LinkedList(node, adjacentCountries);
        //     adjacentCountries.push_back(countryName);
        //   }
        // }
        adjacentCountries.push_back(countryName);
        countryName = "";
      }
      else if (ts[i] != ' ' && ts[i] != '"')
      {
        countryName += ts[i];
      }
    }
    adjacentCountries.push_back(countryName);
    countryName = "";
    ts = "";

    Node node((i), code, name, latitude, longitude, population, adjacentCountries);
    nodes.push_back(node);
    line = "";
    i++;
  }

  // Graph
  vector<tuple<int, int, int>> weightedEdges;
  for (auto &node : nodes)
  {
    for (auto &adjCountryName : node.adjacentCountries)
    {
      for (auto &countryNode : nodes)
      {
        if (countryNode.name.compare(adjCountryName) == 0)
        {

          int fromId = node.id;
          int toId = countryNode.id;
          int distance = (int)haversineDistance(node.latitude, node.longitude, countryNode.latitude, countryNode.longitude);
          tuple<int, int, int> edge = make_tuple(fromId, toId, distance);
          weightedEdges.push_back(edge);
        }
      }
    }
    // LinkedList *p = NULL;
    // if (node.adjacentCountries != NULL)
    // {
    //   p = node.adjacentCountries->next;
    // }
    // do
    // {
    //   cout << (p->countryNode->code) << endl;
    //   int fromId = node.id;
    //   int toId = p->countryNode->id;
    //   cout << node.latitude << endl;
    //   cout << node.longitude << endl;
    //   cout << p->countryNode->latitude << endl;
    //   cout << p->countryNode->longitude << endl;
    //   cout << endl;
    //   int distance = (int)haversineDistance(node.latitude, node.longitude, p->countryNode->latitude, p->countryNode->longitude);
    //   // cout << distance << endl;
    //   tuple<int, int, int> edge = make_tuple(fromId, toId, distance);
    //   weightedEdges.push_back(edge);
    //   p = p->next;
    // } while (p != node.adjacentCountries);
  }

  Graph countriesGraph(nodes, weightedEdges);

  int option;
  while (true)
  {
    cout << "----------------------------------------------------------" << endl;
    cout << "Menu: " << endl;
    cout << "1: Print country data: " << endl;
    cout << "2: BFS Traversal: " << endl;
    cout << "3: DFS Traversal: " << endl;
    cout << "4: Dijkstra Algorithm (Shortest Path): " << endl;
    cout << "5: Prim's Algorithm (Minimum Spanning Tree): " << endl;
    cout << "0: Exit: " << endl
         << endl;
    cout << "Enter: ";
    cin >> option;
    cout << "----------------------------------------------------------" << endl;
    if (option == 1)
    {
      countriesGraph.displayMatrix(nodes);
    }
    if (option == 2)
    {
      cout << "Enter starting point: ";
      int vertex;
      cin >> vertex;
      cout << "BFS traversal: ";
      countriesGraph.bfsTraversal(vertex);
      cout << "...End..." << endl;
    }
    if (option == 3)
    {
      cout << "Enter starting point: ";
      int vertex;
      cin >> vertex;
      cout << "DFS traversal: ";
      countriesGraph.dfsTraversal(vertex);
      cout << "...End..." << endl;
    }
    if (option == 4)
    {
      string source;
      cout << "Enter your source country: ";
      getline(cin >> ws, source);

      int temp = findCountry(nodes, 0, nodes.size() - 1, source);

      countriesGraph.dijkstra(temp, nodes);
    }
    if (option == 5)
    {
      string source;
      cout << "Enter source country: ";
      cin >> source;
      int temp = findCountry(nodes, 0, nodes.size() - 1, source);
      countriesGraph.prims(temp, nodes);
    }
    if (option == 0)
    {
      break;
    }
  }
  return 0;
}