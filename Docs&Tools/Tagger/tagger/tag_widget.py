from PySide6.QtCore import Qt, QStringListModel
from PySide6.QtWidgets import QDialog, QComboBox, QVBoxLayout, QLineEdit, QWidget, QGroupBox, QCheckBox, QHBoxLayout, \
    QGridLayout, QListWidget, QLabel, QPushButton, QTabWidget, QScrollArea, QCompleter
from collections import defaultdict

from typing import Set, List
from . import resource
from .resource import ImageResource, get_canonical_name


def translate_check(source: str):
    if source.lower() == "yes": return Qt.Checked
    if source.lower() == "no": return Qt.Unchecked
    if source.lower() == "maybe": return Qt.PartiallyChecked


def check_to_string(state: Qt.CheckState):
    if state == Qt.Checked: return "yes"
    if state == Qt.Unchecked: return "no"
    if state == Qt.PartiallyChecked: return "maybe"


class TagViewWidget(QWidget):
    def __init__(self, repo):
        super().__init__()
        self.repo = repo
        self.edit = QLineEdit()

        layout = QVBoxLayout()
        self.edit.returnPressed.connect(self.on_update_type)

        model = QStringListModel()
        options = set()
        for s in self.repo.values():
            options.add(s.tag)
            options.add(s.display)

        model.setStringList(options)
        completer = QCompleter()
        completer.setModel(model)

        self.tag_completer = completer

        main_tags_layout = QVBoxLayout()

        self.edit.setCompleter(completer)
        self.edit.setToolTip("Start typing to get suggestions for image tags.\nPress <Enter> to add the tag.")
        main_tags_layout.addWidget(QLabel("Main Tags"), 0)
        main_tags_layout.addWidget(self.edit, 0)
        self.display_type = QLabel("")
        self.display_type.setMinimumHeight(100)
        main_tags_layout.addWidget(self.display_type, 1)

        self.clear_tags = QPushButton("Clear Tags")
        self.clear_tags.clicked.connect(self.on_clear_tags)
        main_tags_layout.addWidget(self.clear_tags, 0)
        layout.addLayout(main_tags_layout, 0)

        layout.addStretch(1)
        layout.addWidget(QLabel("Participants"))
        self.participants = QComboBox()
        self.participants.addItems(resource.KNOW_PARTICIPANT_VALUES)
        layout.addWidget(self.participants)
        layout.addStretch(1)

        # pregnant variation
        layout.addWidget(QLabel("Flags"))

        self._check_boxes = {}
        layout.addWidget(self.generate_checkbox("pregnant", "Pregnant",
                                                "Is the girl visibly pregnant in the image?\n"
                                                "Set to maybe state if it cannot be decided from the image."))

        layout.addWidget(self.generate_checkbox("futa", "Futa",
                                                "Can it be seen in the image that the character is a futa?\n"
                                                "Set to maybe state if it cannot be decided from the image."))
        layout.addWidget(self.generate_checkbox("tied", "Tied-Up",
                                                "Is she tied up in the image?\nIf only implied or in the background, "
                                                "use the middle state."))

        self.fallback = QCheckBox()
        self.fallback.setText("Fallback")
        self.fallback.setToolTip("Fallback images will be considered by the game only if there are no better images available.")
        layout.addWidget(self.fallback)
        layout.addStretch(1)

        self.setLayout(layout)

        self.active_tags_list = []
        self._update_tag_display()

    def generate_checkbox(self, attribute, title, tool_tip):
        box = QCheckBox()
        box.setTristate(True)
        box.setText(title)
        box.setToolTip(tool_tip)
        self._check_boxes[attribute] = box
        return box

    def on_update_type(self):
        tag = get_canonical_name(self.edit.text(), self.repo)
        if tag is None:
            self.display_type.setText("ERROR")
            return

        self.active_tags_list.append(tag)
        self.active_tags_list = list(set(self.active_tags_list))
        self._update_tag_display()
        self.edit.setText("")

    def on_clear_tags(self):
        self.active_tags_list = []
        self._update_tag_display()

    def _update_tag_display(self):
        if len(self.active_tags_list) == 0:
            self.display_type.setText("<No Tags>")
            return

        display = "\n".join(map(lambda x: self.repo[x].display, self.active_tags_list))
        self.display_type.setText(display)

    def update_resource(self, resource: ImageResource):
        self.active_tags_list = [x for x in set(resource.type.split(";")) if x in self.repo]
        self.edit.setText(resource.type)
        self._update_tag_display()

        for key, val in self._check_boxes.items():
            val.setCheckState(translate_check(getattr(resource, key)))

        self.fallback.setCheckState(Qt.Checked if resource.fallback else Qt.Unchecked)
        self.participants.setCurrentText(_participants_to_display(resource.participants))

    def write_changes(self, resource: ImageResource):
        resource.type = ";".join(sorted(set(self.active_tags_list)))
        for key, val in self._check_boxes.items():
            setattr(resource, key, check_to_string(val.checkState()))

        resource.fallback = self.fallback.checkState() == Qt.Checked
        print(self.participants.currentText())
        resource.participants = self.participants.currentText().lower()


def _participants_to_display(value):
    if value.lower() in ["mmf", "ffm"]:
        return value.upper()
    return value.title()
