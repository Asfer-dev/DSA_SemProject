#include <iostream>
#include <vector>
#include <tuple>
#include <cmath>
#include <fstream>
#include <sstream>
#include <queue>
#include <stack>
#include <omp.h>
#include <climits>
#include <unordered_set>
#include <algorithm>
#include <cctype>

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
  int area;
  vector<string> adjacentCountries;

  Node()
  {
  }

  Node(int id, string code, string name, double latitude, double longitude, int population, int area, vector<string> adjacentCountries)
  {
    this->id = id;
    this->code = code;
    this->name = name;
    this->latitude = latitude;
    this->longitude = longitude;
    this->population = population;
    this->area = area;
    this->adjacentCountries = adjacentCountries;
  }
  void displayNode()
  {
    cout << id << ". " << name << " (" << code << ")" << endl;
    cout << "Population: " << this->population << endl;
    cout << "Area in KM square: " << this->area << endl;
    cout << "Adjacent Countries: ";
    for (auto &c : adjacentCountries)
    {
      if (c == adjacentCountries[adjacentCountries.size() - 1])
        cout << c;
      else
        cout << c << ", ";
    }
    cout << endl
         << endl;
  }
  void displayNode(int count)
  {
    cout << count << ". " << name << " (" << code << ")" << endl;
    cout << "Population: " << this->population << endl;
    cout << "Area in KM square: " << this->area << endl;
    cout << "Adjacent Countries: ";
    for (auto &c : adjacentCountries)
    {
      if (c == adjacentCountries[adjacentCountries.size() - 1])
        cout << c;
      else
        cout << c << ", ";
    }
    cout << endl
         << endl;
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

int findCountry(vector<Node> &nodes, string key);

class PriorityQueue
{
public:
  deque<Node> primarydq;
  stack<Node> secondarys;
  int priority;
  PriorityQueue(int priority)
  {
    this->priority = priority;
  }
  void enqueue(Node node)
  {
    if (primarydq.empty())
    {
      primarydq.push_back(node);
    }
    else
    {
      bool condition;
      if (this->priority == 0)
        condition = !primarydq.empty() && primarydq.back().population > node.population;
      else
        condition = !primarydq.empty() && primarydq.back().area > node.area;
      while (condition)
      {
        secondarys.push(primarydq.back());
        primarydq.pop_back();
        if (priority == 0)
          condition = !primarydq.empty() && primarydq.back().population > node.population;
        else
          condition = !primarydq.empty() && primarydq.back().area > node.area;
      }
      primarydq.push_back(node);
      while (!secondarys.empty())
      {
        primarydq.push_back(secondarys.top());
        secondarys.pop();
      }
    }
  }
  void display(bool ascend)
  {

    int count = 0;
    if (ascend)
    {
      while (!primarydq.empty())
      {
        count++;
        primarydq.front().displayNode(count);
        primarydq.pop_front();
      }
    }
    else
    {
      while (!primarydq.empty())
      {
        count++;
        primarydq.back().displayNode(count);
        primarydq.pop_back();
      }
    }
  }
};

class Graph
{
public:
  vector<Node> nodes;
  int numberOfNodes;
  vector<vector<int>> adjacencyMatrix;
  Graph(vector<Node> &nodes, vector<tuple<int, int, int>> &weightedEdges)
  {
    this->nodes = nodes;
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
      adjacencyMatrix[to][from] = cost;
    }
  }

  void displayCountry(int countryId)
  {
    cout << countryId << ". " << nodes[countryId].name << " (" << nodes[countryId].code << ")" << endl;
    cout << "Population: " << nodes[countryId].population << endl;
    cout << "Area in KM square: " << nodes[countryId].area << endl;
    for (int j = 0; j < adjacencyMatrix.size(); j++)
    {
      if (adjacencyMatrix[nodes[countryId].id][j] != INT_MAX)
      {
        cout << nodes[j].name << ": " << to_string(adjacencyMatrix[nodes[countryId].id][j]) << "km -- ";
      }
    }
    cout << "N/A" << endl
         << endl;
  }

  void displayCountries()
  {
    for (int i = 0; i < adjacencyMatrix.size(); i++)
    {
      displayCountry(i);
    }
  }

  void searchCountries(string query)
  {
    transform(query.begin(), query.end(), query.begin(), [](unsigned char c)
              { return tolower(c); });
    for (auto &node : nodes)
    {
      string countryName = node.name;
      transform(countryName.begin(), countryName.end(), countryName.begin(), [](unsigned char c)
                { return tolower(c); });
      if (countryName.find(query) != countryName.npos)
      {
        displayCountry(node.id);
      }
    }
  }

  vector<PriorityQueue> filterCountries(bool populationFlag, int populationLimit, bool areaFlag, int areaLimit)
  {
    PriorityQueue populationq(0);
    PriorityQueue areaq(1);
    for (auto &node : nodes)
    {
      if (populationFlag && areaFlag)
      {
        if (node.population >= populationLimit && node.area >= areaLimit)
        {
          populationq.enqueue(node);
          areaq.enqueue(node);
        }
      }
      else if (populationFlag && !areaFlag)
      {
        bool condition;
        if (areaLimit == -1)
          condition = node.population <= populationLimit;
        else
          condition = node.population >= populationLimit && node.area <= areaLimit;
        if (condition)
        {
          populationq.enqueue(node);
          areaq.enqueue(node);
        }
      }
      else if (!populationFlag && areaFlag)
      {
        bool condition;
        if (populationLimit == -1)
          condition = node.area <= areaLimit;
        else
          condition = (node.population <= populationLimit) && (node.area >= areaLimit);
        if (condition)
        {
          populationq.enqueue(node);
          areaq.enqueue(node);
        }
      }
      else
      {
        bool condition;
        if (populationLimit == -1 && areaLimit == -1)
          condition = true;
        else if (populationLimit == -1 && areaLimit != -1)
          condition = node.area <= areaLimit;
        else if (populationLimit != -1 && areaLimit == -1)
          condition = node.population <= populationLimit;
        else
          condition = node.population <= populationLimit && node.area <= areaLimit;
        if (condition)
        {
          populationq.enqueue(node);
          areaq.enqueue(node);
        }
      }
    }
    return {populationq, areaq};
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
        cout << nodes[vertex].name << " -> ";
        for (int i = 0; i < numberOfNodes; i++)
        {
          if (adjacencyMatrix[vertex][i] != INT_MAX)
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
        cout << nodes[vertex].name << " -> ";
        for (int i = 0; i < numberOfNodes; i++)
        {
          if (adjacencyMatrix[vertex][i] != INT_MAX)
          {
            s.push(i);
          }
        }
      }
    }
  }
  void printPrims(int parent[], int distance[], vector<Node> nodes, int source)
  {
    // for (int i = 0; i < numberOfNodes; i++) {
    //   if (parent[i] == -1) {
    //     nodes[findCountry(nodes[i].adjacentCountries[0])]
    //   }
    // }
    vector<tuple<int, int, int>> edges;
    for (int i = 0; i < numberOfNodes; i++)
    {
      if (distance[i] != INT_MAX && parent[i] != -1)
      {
        tuple<int, int, int> edge = make_tuple(i, parent[i], distance[i]);
        edges.push_back(edge);
      }
    }
    Graph minSpan(nodes, edges);
    minSpan.dfsTraversal(source);
  }

  void printDijkstra(int parent[], int distance[], vector<Node> nodes, int destination)
  {
    string path = "";
    cout << endl
         << "Source :: ";

    int counter = 0;
    int totalDistance = 0;
    while (destination != -1)
    {
      counter++;
      path = nodes[destination].name + "  " + path;
      totalDistance += distance[destination];
      destination = parent[destination];
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
    cout << "Total Distance: " << totalDistance << " KM" << endl;
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
  void dijkstra(int source, int destination, vector<Node> nodes)
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
    printDijkstra(parent, distance, nodes, destination);
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
    printPrims(parent, distance, nodes, source);
  }
};

int findCountry(vector<Node> nodes, int start, int end, string key)
{
  while (start <= end)
  {
    int mid = (end + start) / 2;
    if (nodes[mid].name.compare(key) == 0)
    {
      return mid;
    }
    if (nodes[mid].name.compare(key) > 0)
    {
      end = mid - 1;
    }
    else
    {
      start = mid + 1;
    }
  }
  return -1;
}
int findCountry(vector<Node> &nodes, string key)
{
  for (Node n : nodes)
  {
    if (n.name == key)
    {
      return n.id;
    }
  }
  return -1;
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
    int area;
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
      if (ts[ts.size() - 1] == ',')
        ts = ts.substr(1, ts.size() - 2);
      else
        ts = ts.substr(1);
    }
    string countryName = "";
    for (int i = 0; i < ts.size(); i++)
    {
      if (ts[i] == ',')
      {
        adjacentCountries.push_back(countryName);
        countryName = "";
      }
      else if (ts[i] != '"')
      {
        countryName += ts[i];
      }
    }
    adjacentCountries.push_back(countryName);
    countryName = "";
    ts = "";

    getline(lineString, ts, ',');
    ts = "";
    getline(lineString, ts, ',');
    area = atoi(ts.c_str());
    ts = "";

    Node node((i), code, name, latitude, longitude, population, area, adjacentCountries);
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
    cout << "1: Print country data" << endl;
    cout << "2: Search for a Country" << endl;
    cout << "3: Filter Countries" << endl;
    cout << "4: Find Shortest Path between two Countries" << endl;
    cout << "5: Prim's Algorithm (Minimum Spanning Tree)" << endl;
    cout << "0: Exit: " << endl
         << endl;
    cout << "Enter: ";
    cin >> option;
    cout << "----------------------------------------------------------" << endl;
    if (option == 1)
    {
      countriesGraph.displayCountries();
    }
    else if (option == 2)
    {
      string query;
      cout << "Enter your query: ";
      getline(cin >> ws, query);
      cout << endl;
      countriesGraph.searchCountries(query);
    }
    else if (option == 3)
    {
      int option1;
      bool populationFlag = true;
      int populationLimit = -1;
      bool areaFlag = true;
      int areaLimit = -1;
      while (true)
      {
        cout << "1: Enter Population Criteria" << endl;
        cout << "2: Enter Area Criteria" << endl;
        cout << "3: Print List" << endl;
        cout << "0: Back" << endl;
        cout << "Enter: ";
        cin >> option1;
        if (option1 == 1)
        {
          cout << "----------------------------------------------------------" << endl;
          cout << "Enter Population Threshold: ";
          cin >> populationLimit;
          cout << "Do you want countries less than the threshold (0) or greater than it (1)?: ";
          cin >> populationFlag;
          cout << "----------------------------------------------------------" << endl;
        }
        else if (option1 == 2)
        {
          cout << "----------------------------------------------------------" << endl;
          cout << "Enter Area Threshold: ";
          cin >> areaLimit;
          cout << "Do you want countries less than the threshold (0) or greater than it (1)?: ";
          cin >> areaFlag;
          cout << "----------------------------------------------------------" << endl;
        }
        else if (option1 == 3)
        {
          vector<PriorityQueue> queues = countriesGraph.filterCountries(populationFlag, populationFlag, areaFlag, areaLimit);
          PriorityQueue populationq = queues[0];
          PriorityQueue areaq = queues[1];
          int option2;
          while (true)
          {
            cout << "----------------------------------------------------------" << endl;
            cout << "1: Sort by Population" << endl;
            cout << "2: Sort by Area" << endl;
            cout << "0: Back" << endl;
            cout << "Enter: ";
            cin >> option2;

            if (option2 == 1)
            {
              cout << "----------------------------------------------------------" << endl;
              bool ascend;
              cout << "Lowest first (1) or Highest first (0): ";
              cin >> ascend;
              populationq.display(ascend);
              cout << "----------------------------------------------------------" << endl;
            }
            else if (option2 == 2)
            {
              cout << "----------------------------------------------------------" << endl;
              bool ascend;
              cout << "Lowest first (1) or Highest first (0): ";
              cin >> ascend;
              areaq.display(ascend);
              cout << "----------------------------------------------------------" << endl;
            }
            else
            {
              break;
            }
          }
        }
        else if (option1 == 0)
        {
          break;
        }
      }
    }
    else if (option == 4)
    {
      string source;
      cout << "Enter your source country: ";
      getline(cin >> ws, source);

      int sourceId = findCountry(nodes, source);

      if (sourceId != -1)
      {
        string destination;
        cout << "Enter your destination country: ";
        getline(cin >> ws, destination);
        int destId = findCountry(nodes, destination);
        if (destId != -1)
          countriesGraph.dijkstra(sourceId, destId, nodes);
        else
          cout << endl
               << "Country does not exist" << endl;
      }
      else
      {
        cout << endl
             << "Country does not exist" << endl;
      }
    }
    else if (option == 5)
    {
      string source;
      cout << "Enter source country: ";
      cin >> source;
      int temp = findCountry(nodes, source);
      if (temp != -1)
      {
        countriesGraph.prims(temp, nodes);
      }
      else
      {
        cout << endl
             << "Country does not exist" << endl;
      }
    }
    else
    {
      break;
    }
  }
  return 0;
}