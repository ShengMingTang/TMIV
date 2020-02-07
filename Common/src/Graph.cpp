#include <TMIV/Common/Graph.h>

namespace TMIV::Common::Graph {
auto getBitFieldFromGraph(const BuiltIn::Sparse<float> &graph) -> std::vector<unsigned> {
  std::vector<unsigned> bitField;

  for (NodeId nodeId = 0; nodeId < graph.getNumberOfNodes(); nodeId++) {
    unsigned bf = 0;

    for (const auto &l : graph.getNeighbourhood(nodeId)) {
      bf |= (1U << l.node());
    }

    bitField.push_back(bf);
  }

  return bitField;
}

auto getGraphFromBitField(const std::vector<unsigned> &bitField) -> BuiltIn::Sparse<float> {
  BuiltIn::Sparse<float> graph(bitField.size());

  for (NodeId nodeId = 0; nodeId < graph.getNumberOfNodes(); nodeId++) {
    unsigned field = bitField[nodeId];

    for (NodeId otherId = 0; otherId < graph.getNumberOfNodes(); otherId++) {
      if ((field & (1U << otherId)) != 0U) {
        graph.connect(nodeId, otherId, 1., LinkType::Directed);
      }
    }
  }

  return graph;
}

auto getDescendingOrderId(const BuiltIn::Sparse<float> &g) -> std::vector<NodeId> {
  std::vector<NodeId> descendingOrderId;

  // Root
  NodeId rootId = 0U;

  for (NodeId nodeId = 0; nodeId < g.getNumberOfNodes(); nodeId++) {
    if (g.getNeighbourhoodSize(nodeId) == 0) {
      rootId = nodeId;
      break;
    }
  }

  // Pruning order
  std::size_t toMark = g.getNumberOfNodes();
  std::vector<std::uint8_t> hasBeenMarked(g.getNumberOfNodes(), 0U);

  descendingOrderId.push_back(rootId);
  hasBeenMarked[rootId] = 1U;
  toMark--;

  while (toMark != 0) {
    std::vector<NodeId> shoudlBeMarked;

    for (NodeId nodeId = 0; nodeId < g.getNumberOfNodes(); nodeId++) {
      if (hasBeenMarked[nodeId] == 0U) {
        std::size_t nbParentMarked = 0U;

        for (const auto &l : g.getNeighbourhood(nodeId)) {
          if (hasBeenMarked[l.node()] != 0U) {
            nbParentMarked++;
          }
        }

        if (nbParentMarked == g.getNeighbourhoodSize(nodeId)) {
          shoudlBeMarked.push_back(nodeId);
        }
      }
    }

    for (auto nodeId : shoudlBeMarked) {
      descendingOrderId.push_back(nodeId);
      hasBeenMarked[nodeId] = 1U;
      toMark--;
    }
  }

  return descendingOrderId;
}

auto getReversedGraph(const BuiltIn::Sparse<float> &g) -> BuiltIn::Sparse<float> {
  BuiltIn::Sparse<float> reversedGraph(g.getNumberOfNodes());

  for (NodeId sourceId = 0; sourceId < g.getNumberOfNodes(); sourceId++) {
    for (const auto &l : g.getNeighbourhood(sourceId)) {
      reversedGraph.connect(l.node(), sourceId, 1.F, LinkType::Directed);
    }
  }

  return reversedGraph;
}

} // namespace TMIV::Common::Graph
