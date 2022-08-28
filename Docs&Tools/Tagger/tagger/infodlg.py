from pathlib import Path
from typing import Dict

from PySide6.QtCore import QStringListModel
from PySide6.QtGui import QPixmap, Qt, QImageReader
from PySide6.QtWidgets import QDialog, QLineEdit, QVBoxLayout, QCompleter, QLabel, QComboBox

from .resource import TagSpec, read_tag_specs, get_canonical_name
import networkx as nx
import numpy as np
import matplotlib.pyplot as plt


def generate_fallback_tree(start: str, participants: str, repo: Dict[str, TagSpec]):
    visited = set()
    queue = list()

    graph = nx.Graph()
    graph.add_node(start)
    cost = 0.0
    queue.append((start, None, cost))
    base_layout = {}

    while len(queue) > 0:
        # find the lowest cost item
        cheapest = 1e100
        index = -1
        for i, (e, p, c) in enumerate(queue):
            if c < cheapest:
                cheapest = c
                index = i
        node, parent, cost = queue[index]
        del queue[index]
        if node in visited or cost > 100:
            continue
        visited.add(node)

        graph.add_node(node)
        base_layout[node] = np.array([0.01*(np.random.rand() - 0.5), cost])
        if parent is not None:
            graph.add_edge(parent, node)
        for fb in repo[node].fallback:
            # don't look at conditional fallbacks now
            if fb.required is None or fb.required == participants:
                queue.append((fb.target, node, fb.cost + cost))

    layout = base_layout
    for _ in range(10):
        layout = nx.spring_layout(graph, k=150, pos=layout, fixed=[start], iterations=1)
        for k in layout:
            layout[k][1] = 10*base_layout[k][1]

    labels = {}
    for k in graph.nodes:
        labels[k] = repo[k].display

    fig = plt.figure()
    nx.draw(graph, with_labels=True, font_weight='bold', pos=layout, node_size=1000, font_size=10, width=1.5, labels=labels)
    plt.savefig("fallback-graph-visualization.png", dpi=300)
    plt.close(fig)


if __name__ == "__main__":
    repo = read_tag_specs(Path("../../../Resources/Data/ImageTypes.xml"))
    generate_fallback_tree("sexdoggy", repo)


class VisualizeFallbackDlg(QDialog):
    def __init__(self, repo:  Dict[str, TagSpec]):
        super().__init__()
        self.repo = repo

        self.edit = QLineEdit()

        layout = QVBoxLayout()
        self.edit.editingFinished.connect(self._on_update_type)

        model = QStringListModel()
        options = set()
        for s in self.repo.values():
            options.add(s.tag)
            options.add(s.display)

        model.setStringList(options)
        completer = QCompleter()
        completer.setModel(model)
        self.edit.setCompleter(completer)
        layout.addWidget(self.edit)

        layout.addWidget(QLabel("Participants"))
        self.participants = QComboBox()
        self.participants.addItems(["Solo", "Lesbian", "Hetero", "FFM", "MMF", "Gangbang", "Les_group",
                                    "Orgy", "Any"])
        layout.addWidget(self.participants)

        self.setLayout(layout)

        self.pixmap = QPixmap()
        self.label = QLabel("Fallback Graph")
        self.label.setMinimumWidth(800)
        self.label.setMinimumHeight(600)
        self.label.setAlignment(Qt.AlignHCenter | Qt.AlignVCenter)
        layout.addWidget(self.label)

    def _on_update_type(self):
        tag = get_canonical_name(self.edit.text(), self.repo)
        if tag is None:
            return

        generate_fallback_tree(tag, self.participants.currentText().lower(), self.repo)
        reader = QImageReader("fallback-graph-visualization.png")
        reader.setAutoTransform(True)
        image = reader.read()
        image = image.scaled(800, 600, aspectMode=Qt.KeepAspectRatio)
        self.pixmap = QPixmap.fromImage(image)
        self.label.setPixmap(self.pixmap)


class ShowAllTags(QDialog):
    def __init__(self, repo:  Dict[str, TagSpec]):
        super().__init__()
        self.setWindowTitle("Tag Info")
        self.repo = repo
        self.edit = QLineEdit()
        self.edit.setToolTip("Start typing to see suggestions for tag names.")

        layout = QVBoxLayout()
        self.edit.editingFinished.connect(self._on_update_type)

        model = QStringListModel()
        options = set()
        for s in self.repo.values():
            options.add(s.tag)
            options.add(s.display)

        model.setStringList(options)
        completer = QCompleter()
        completer.setModel(model)
        self.edit.setCompleter(completer)
        layout.addWidget(self.edit)

        self.setLayout(layout)

        self.full_name = QLabel("Full Name")
        self.full_name.setToolTip("The name of the tag as displayed to pack makers/players.")
        layout.addWidget(self.full_name)

        self.tag_name = QLabel("Tag")
        self.tag_name.setToolTip("The name of the tag used by the game engine.")
        layout.addWidget(self.tag_name)
        layout.addWidget(QLabel("Description:"))
        self.description = QLabel("<Description>")
        layout.addWidget(self.description)

        #self.pixmap = QPixmap()
        #self.label = QLabel("<Description>")
        #self.label.setMinimumWidth(800)
        #self.label.setMinimumHeight(600)
        #self.label.setAlignment(Qt.AlignHCenter | Qt.AlignVCenter)
        #layout.addWidget(self.label)

    def _on_update_type(self):
        tag = get_canonical_name(self.edit.text(), self.repo)
        if tag is None:
            return

        self.tag_name.setText(f"Tag: {tag}")
        self.full_name.setText(f"Name: {self.repo[tag].display}")
        self.description.setText(self.repo[tag].description)

        #self.pixmap = QPixmap.fromImage(image)
        #self.label.setPixmap(self.pixmap)