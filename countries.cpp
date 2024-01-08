#include <iostream>
#include <vector>
#include <tuple>
#include <cmath>
#include <fstream>
#include <sstream>

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

struct LinkedList
{
  Node *countryNode;
  LinkedList *next;
  LinkedList(Node node)
  {
    this->countryNode = &node;
    next = this;
  }
  LinkedList(Node node, LinkedList *end)
  {
    this->countryNode = &node;
    if (end != NULL)
    {
      this->next = end;
      end->next = this;
    }
    else
    {
      this->next = this;
    }
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

class Graph
{
public:
  vector<vector<int>> adjacencyMatrix;
  Graph(vector<Node> &nodes, vector<tuple<int, int, int>> &weightedEdges)
  {
    // Initialize the adjacency matrix with infinity (assuming no direct connection)
    for (int i = 0; i < nodes.size(); ++i)
    {
      vector<int> row(nodes.size(), -1);
      row[i] = 0; // Set diagonal elements to 0
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

  void displayMatrix() const
  {
    int i = 0;
    for (const auto &row : adjacencyMatrix)
    {
      if (i > 9)
        break;
      for (int val : row)
      {
        cout << (val == -1 ? "INF" : to_string(val)) << " ";
      }
      cout << endl;
      i++;
    }
  }
};

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
  countriesGraph.displayMatrix(); // only prints the first 10 rows for simplicity
}