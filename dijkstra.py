
#Input: n = 3, edges = [[0,1,2],[1,2,1],[0,2,4]], disappear = [1,3,5]
import math
from heapq import heappush, heappop
def build_graph(n,edges, G):
    for i in range(n):
        G[i] = []
    for source, dest, weight in edges:
        if source in G:
            G[source].append((dest,weight))

def build_prio(dist,Q):
    """
        Function that initializes the priority queue.
        -1 means the node is still unreachable.
    """
    for node, distance in dist.items():
        Q.append((distance, node))

def example1():
    return [[0,1,2],[1,2,1],[0,2,4]], 3

def example2():
    return [[0,1,2],[0,2,6],[1,3,5],[2,3,8],[3,5,15],[3,4,10],[4,5,6],[4,6,2],[5,6,6]], 7

edges, n = example2()
dist = {i:math.inf for i in range(n)}
source = 0
dist[source] = 0
G = {}
Q = []
build_graph(n,edges,G)
build_prio(dist,Q)
print("Graph: {}".format(G))
while Q:
    distance, u = heappop(Q)
    print(f"popped {u} with distance {distance}")
    if distance == math.inf:
        # no more nodes reachable
        break
    for v,w in G[u]:
        alt = dist[u] + w
        if alt < dist[v]:
            dist[v] = alt
            heappush(Q, (alt, v))
print(f"Distances: {dist}")