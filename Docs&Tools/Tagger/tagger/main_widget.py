from typing import Optional, List

from PySide6.QtGui import QImage, QPixmap, QImageReader
from PySide6.QtMultimedia import QMediaPlayer
from PySide6.QtWidgets import QWidget, QLabel, QVBoxLayout, QHBoxLayout, QListWidget, QFrame, QPushButton, \
    QListWidgetItem, QGridLayout, QMenuBar, QCheckBox, QComboBox, QLineEdit, QFormLayout, QMessageBox, QDialog
from PySide6.QtCore import Qt
from PySide6.QtMultimediaWidgets import QVideoWidget
from .utils import QHLine, ImageDisplay

from . import resource
from .resource import ImageResource, ResourcePack
from .tag_widget import TagViewWidget

from pathlib import Path


def requires_open_pack(f):
    def wrapped(self, *args, **kwargs):
        if self.pack_data is None:
            QMessageBox.critical(None, "No Image Pack", "This function is only available when there is an active pack")
        else:
            return f(self, *args, **kwargs)
    return wrapped


class MainWidget(QWidget):
    def __init__(self, image_width, image_height, repo: dict):
        super().__init__()

        self.image_width = image_width
        self.image_height = image_height

        self.repo = repo
        layout = QGridLayout()

        # pack image list
        self.pack_list = QListWidget()
        self.pack_list.setMinimumWidth(256)
        self.pack_list.currentRowChanged.connect(self._select_image)
        layout.addWidget(self.pack_list, 0, 0, 1, 1)

        # the image display
        middle_column = QVBoxLayout()
        self._video_widget = ImageDisplay(image_width, image_height)
        middle_column.addWidget(self._video_widget)

        layout.addLayout(middle_column, 0, 1, 2, 1)

        self.image_info = QLabel("Image Info")
        middle_column.addWidget(self.image_info)

        image_meta = QFormLayout()
        layout.addLayout(image_meta, 1, 0, 1, 1)

        self.source_edit = QLineEdit("")
        self.source_edit.editingFinished.connect(self._update_source)
        self.source_edit.setToolTip("Put something here that allows players and other pack makers to identify where the image is from.")
        image_meta.addRow("Source", self.source_edit)

        self.style = QComboBox()
        self.style.addItem("-")
        self.style.addItem("2D")
        self.style.addItem("3D")
        self.style.addItem("Real")
        self.style.setToolTip("Define what sort of art style this image is, to enable players to filter by style.")
        image_meta.addRow("Style", self.style)

        self.tag_view = TagViewWidget(repo=repo)
        layout.addWidget(self.tag_view, 0, 2, 2, 1)

        self.setLayout(layout)

        self.pack_data = None   # type: Optional[ResourcePack]
        self.current_image = None

        # This will be temporarily set to true when the list of images is updated, to prevent constant reloading
        # because of row changes.
        self._bulk_update_is_active = False

        self.update_image(0)

    def set_pack_data(self, data, selected_image=0):
        self.pack_data = data
        self.current_image = None
        self._bulk_update_is_active = True
        try:
            self.pack_list.clear()
            for i, image in enumerate(self.pack_data.images):
                new_item = QListWidgetItem(image.file)
                new_item.setData(Qt.UserRole, i)
                self.pack_list.addItem(new_item)
        finally:
            self._bulk_update_is_active = False
        self.update_image(selected_image)

    def set_image_data(self, image_data):
        reader = QImageReader(image_data)
        reader.setAutoTransform(True)
        image = reader.read()
        img_path = Path(image_data)
        self.image_info.setText(f"{img_path.name}: {image.width()}x{image.height()}; {img_path.stat().st_size // 1024}kB")
        self._video_widget.set_image_data(image_data)

    def update_image(self, image):
        if self.pack_data is None:
            self._video_widget.set_image_data(None)
            self.current_image = None
            return
        if self.current_image is not None:
            active_image = self.pack_data.images[self.current_image]
            self.tag_view.write_changes(active_image)

        if len(self.pack_data.images) == 0:
            self.current_image = None
            return
        image_data = self.pack_data.images[image]   # type: ImageResource
        self.tag_view.update_resource(image_data)
        self.source_edit.setText(image_data.source)
        self.set_image_data(str(self.pack_data.path.parent / image_data.file))
        self.current_image = image
        self.pack_list.setCurrentRow(image)

    @requires_open_pack
    def next_image(self):
        target_image = min(self.current_image + 1, len(self.pack_data.images) - 1)
        self.pack_list.setCurrentRow(target_image)

    @requires_open_pack
    def prev_image(self):
        self.pack_list.setCurrentRow(max(0, self.current_image - 1))

    def _select_image(self, item):
        if self._bulk_update_is_active:
            return
        self.update_image(item) #.data(Qt.UserRole))

    @requires_open_pack
    def add_images(self, images: List[ImageResource]):
        old_count = len(self.pack_data.images)
        for image in images:
            image.file = str(self.rel_path(image.file))
            self.pack_data.images.append(image)
        self.set_pack_data(self.pack_data, selected_image=old_count)\

    @requires_open_pack
    def filter_new_images(self, images: List[ImageResource]):
        old_paths = set(image.file for image in self.pack_data.images)
        return [image for image in images if str(self.rel_path(image.file)) not in old_paths]

    @requires_open_pack
    def duplicate_active_image(self):
        from copy import deepcopy
        copied = deepcopy(self.pack_data.images[self.current_image])
        self.pack_data.images.insert(self.current_image, copied)
        ci = self.current_image + 1
        self.set_pack_data(self.pack_data, selected_image=ci)

    @requires_open_pack
    def remove_active_image(self):
        self.pack_data.images = self.pack_data.images[:self.current_image] + self.pack_data.images[self.current_image+1:]
        ci = self.current_image
        self.set_pack_data(self.pack_data, selected_image=ci)

    @requires_open_pack
    def _update_source(self):
        active_image = self.pack_data.images[self.current_image]
        active_image.source = self.source_edit.text()

    @requires_open_pack
    def rel_path(self, path) -> Path:
        """
        Converts `path` into a path relative to the pack root.
        :param path: A pathlike object to convert.
        :return: The relative path as a `Path` object.
        """
        return Path(path).relative_to(self.pack_data.path.parent)

    @requires_open_pack
    def list_missing_images(self):
        reference = set(self.repo.keys())

        all_types = set()
        for im in self.pack_data.images:  # type: ImageResource
            all_types.update(im.type.split(";"))

        missing = reference.difference(all_types)
        msg = QMessageBox()
        msg.setText("\n".join(missing))
        msg.exec()

    @requires_open_pack
    def show_file_stats(self):
        data = resource.get_stats(self.pack_data.path, self.pack_data.images)
        dlg = QDialog()
        dlg.setWindowTitle("Pack Files Stats")
        # image section
        layout = QVBoxLayout()

        def part_stats(source):
            if source["count"] > 0:
                sub_layout = QFormLayout()
                sub_layout.addRow("Count", QLabel(str(source["count"])))
                sub_layout.addRow("Avg. Size", QLabel(f'{round(source["total_kb"] / max(1, source["count"]))} kB'))
                sub_layout.addRow("Smallest", QLabel(f'{source["smallest"]} kB'))
                sub_layout.addRow("Largest", QLabel(f'{source["largest"]} kB'))
                layout.addLayout(sub_layout)
            else:
                layout.addWidget(QLabel("-"))

        layout.addWidget(QLabel("Images"))
        part_stats(data["images"])
        layout.addWidget(QHLine())

        layout.addSpacing(1)
        layout.addWidget(QLabel("Videos"))
        part_stats(data["videos"])
        layout.addWidget(QHLine())
        layout.addSpacing(1)

        layout.addWidget(QLabel("Types"))
        sub_layout = QFormLayout()
        for t, c in data["types"].items():
            sub_layout.addRow(t, QLabel(str(c)))
        layout.addLayout(sub_layout)
        dlg.setLayout(layout)
        dlg.exec_()
