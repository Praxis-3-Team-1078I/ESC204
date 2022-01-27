import numpy as np
import matplotlib.pyplot as plt
import networkx as nx

palette = {"node_color":"#36558F", "edge_color":"#40376E", "font_color":"#000000", "face_color":"#96ACB7"}
plt.figure(figsize=(16,16))
ax = plt.axes()
ax.set_facecolor(palette["face_color"])

def read_csv(file_name="actors.csv"):
    """Take an actor network csv and read it into a dictionary"""
    with open(file_name) as file:
        data = file.readlines()[1:]

    nodes = []
    for line in data:
        name, connections, relations, show = [section.strip() for section in line.split(",")[1:]]
        show = eval(show) # Check if connection labels should be shown
        connections = list(map(int, connections.split()))
        relations = relations.split() if show else [""]*len(connections)
        nodes.append((name, *[(c, r if r != "None" else "") for c, r in zip(connections, relations)]))

    return nodes


def create_graph(nodes):
    """From a set of nodes in dict form create a DiGraph"""
    graph = nx.DiGraph()
    for n, node in enumerate(nodes):
        graph.add_node(n, label=node[0])

    for n, node in enumerate(nodes):
        for connection in node[1:]:
            graph.add_edge(n, connection[0], label=connection[1])

    return graph


def plot_graph(graph):
    """Plot a DiGraph in matplotlib using networkx"""
    # neato, dot, twopi, circo, fdp, nop, wc, acyclic, gvpr, gvcolor, ccomps, sccmap, tred, sfdp, unflatten
    pos = nx.nx_agraph.graphviz_layout(graph, prog="neato")
    nx.draw_networkx(graph, pos, with_labels=False, arrows=True, node_size=1000, node_color=palette["node_color"], arrowstyle="->", edge_color=palette["edge_color"])

    labels = {node[0]:node[1]["label"] for node in graph.nodes(data=True)}
    nx.draw_networkx_labels(graph, pos, labels, font_size=11, font_color=palette["font_color"])

    edge_labels = dict([((n1, n2), d['label']) for n1, n2, d in graph.edges(data=True)])
    nx.draw_networkx_edge_labels(graph, pos, edge_labels, font_size=7)


nodes = read_csv("actors.csv")
graph = create_graph(nodes)
plot_graph(graph)

plt.savefig("actor_network.pdf")
# plt.savefig("plot.png", dpi=700)
plt.show()