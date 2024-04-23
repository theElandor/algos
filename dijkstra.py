import math
from heapq import heappop, heappush
def dij(G, edges, source):
    dist = {i:math.inf for i in range(n)}
    dist[source]=0
    Q = [(0,source)]
    ans = [-1]*n
    while Q:
        distance, u = heappop(Q)
        if ans[u] != -1: continue
        ans[u] = distance
        for v,w in G[u]:
            alt = distance + w
            heappush(Q, (alt,v))
    return ans


edges = [[0,1,4],[0,2,1],[1,3,2],[1,4,3],[1,5,1],[2,3,1],[3,5,3],[4,5,2]]
n = 6
G = {i:[] for i in range(n)}
for source, dest, weight in edges:
    G[source].append((dest, weight))
    G[dest].append((source, weight))
d1 = (dij(G,edges,0))
d2 = (dij(G,edges,n-1))
