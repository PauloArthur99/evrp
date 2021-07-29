import networkx as nx # type: ignore
import matplotlib.pyplot as plt # type: ignore

with open("arquivoSoluções.txt") as f:
    f.readline()
    routeContents = f.readlines()

with open("PointCoord.txt") as c:
    c.readline()
    coordContents = c.readlines()

routes = []
for line in routeContents:
    lineSplit = line.split()
    lineSplit = lineSplit[1:]
    vetAux = []
    for str in lineSplit:
        elem = int(str)
        vetAux.append(elem)
    routes.append(vetAux)

coords = {}
for line in coordContents:
    lineSplit = line.split()
    coords[int(lineSplit[0])] = float(lineSplit[1]), float(lineSplit[2])

g = nx.DiGraph()
g.add_node(0)
for vet in routes:
    for elem in vet:
        g.add_node(elem)
    g.add_edge(0, vet[0])
    g.add_edge(vet[-1], 0)
    for i in range(len(vet) - 1):
        g.add_edge(vet[i], vet[i+1])

fig, ax = plt.subplots()
nx.draw_networkx_nodes(g,pos=coords, ax=ax)
nx.draw_networkx_labels(g,pos=coords, ax=ax)
nx.draw_networkx_edges(g,pos=coords, ax=ax)
ax.tick_params(left=True, bottom=True, labelleft=True, labelbottom=True)
plt.grid(True)
plt.show()
plt.savefig("networkx_nodes.png")

"""
nx.draw(g, with_labels = True)
plt.savefig("filename.png")
"""

