for (auto &node : nodes)
  {
    LinkedList *p = NULL;
    if (node.adjacentCountries != NULL)
    {
      p = node.adjacentCountries->next;
    }
    while (p != NULL)
    {
      int fromId = node.id;
      int toId = p->countryNode.id;
      int distance = haversineDistance(node.latitude, node.longitude, p->countryNode.latitude, p->countryNode.longitude);
      tuple<int, int, int> edge = make_tuple(fromId, toId, distance);
      weightedEdges.push_back(edge);
      p = p->next;
    }
  }