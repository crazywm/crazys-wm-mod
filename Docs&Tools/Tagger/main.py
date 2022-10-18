# This import should help with setting up environment variables needed to successfully pick the right QT version.
import tagger.qtimport

from pathlib import Path
import sys
import os
from PySide6 import QtGui
from PySide6.QtWidgets import QApplication, QMainWindow, QFileDialog, QMessageBox
from tagger.main_widget import MainWidget
from tagger.resource import read_tag_specs, read_file_translations, guess_type_by_file_name, ResourcePack, \
    save_image_pack, FILE_SUFFIXES
from tagger.dialogs import VisualizeFallbackDlg, ShowAllTags, PackMetaDlg

app = QApplication([])


# ensures that we can package everything into a single exe
def resource_path(relative_path):
    """ Get absolute path to resource, works for dev and for PyInstaller """
    if hasattr(sys, "_MEIPASS"):
        # PyInstaller creates a temp folder and stores path in _MEIPASS
        base_path = sys._MEIPASS
    else:
        base_path = os.path.abspath(".")

    return Path(os.path.join(base_path, relative_path))


def requires_open_pack(f):
    def wrapped(self, *args, **kwargs):
        if self.tagger.pack_data is None:
            QMessageBox.critical(None, "No Image Pack", "This function is only available when there is an active pack")
        else:
            return f(self, *args, **kwargs)
    return wrapped


class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()

        self.setWindowTitle("Tagger")

        try:
            self.repo = read_tag_specs(resource_path("ImageTypes.xml"))
            self.translator, presets = read_file_translations(resource_path("ImageFiles.xml"))
        except FileNotFoundError:
            QMessageBox.critical(self, "Missing Files", "Could not find 'ImageTypes.xml' and 'ImageFiles.xml'!")
            exit(1)

        self.tagger = MainWidget(768, 768, self.repo, presets)
        self.setup_menu_bar()

        # Set the central widget of the Window.
        self.setCentralWidget(self.tagger)

    def setup_menu_bar(self):
        file_menu = self.menuBar().addMenu("&File")
        new_action = QtGui.QAction('&New', self)
        new_action.setShortcut('Ctrl+N')
        new_action.setStatusTip('New pack')
        new_action.triggered.connect(self._new_pack_dlg)
        file_menu.addAction(new_action)

        load_action = QtGui.QAction('&Load', self)
        load_action.setShortcut('Ctrl+L')
        load_action.setStatusTip('Load pack')
        load_action.triggered.connect(self._load_pack_dlg)
        file_menu.addAction(load_action)

        save_action = QtGui.QAction('&Save', self)
        save_action.setShortcut('Ctrl+S')
        save_action.setStatusTip('Save current pack')
        save_action.triggered.connect(lambda: self._save_pack_dlg(save_as=False))
        file_menu.addAction(save_action)

        action = QtGui.QAction('&Import', self)
        action.setShortcut('Ctrl+Shift+I')
        action.setStatusTip('Imports an existing pack')
        action.triggered.connect(self._import_dlg)
        file_menu.addAction(action)

        exit_action = QtGui.QAction('&Exit', self)
        exit_action.setShortcut('Ctrl+Q')
        exit_action.setStatusTip('Exit application')
        exit_action.triggered.connect(self.close)
        file_menu.addAction(exit_action)

        pack_menu = self.menuBar().addMenu("&Images")
        action = QtGui.QAction('&Add Image', self)
        action.setShortcut('Ctrl+A')
        action.setStatusTip('Add image')
        action.triggered.connect(self._add_image_dlg)
        pack_menu.addAction(action)

        action = QtGui.QAction('&Remove Image', self)
        action.setShortcut('Ctrl+F')
        action.setStatusTip('Removes an image from the pack')
        action.triggered.connect(self.tagger.remove_active_image)
        pack_menu.addAction(action)

        action = QtGui.QAction('&Duplicate Image', self)
        action.setShortcut('Ctrl+D')
        action.setStatusTip('Duplicates the image and all associated tags')
        action.triggered.connect(self.tagger.duplicate_active_image)
        pack_menu.addAction(action)

        action = QtGui.QAction('Add &Directory', self)
        action.setShortcut('Ctrl+Shift+A')
        action.setStatusTip('Add all images from directory')
        action.triggered.connect(self._add_dir_dlg)
        pack_menu.addAction(action)

        action = QtGui.QAction('&Next Image', self)
        action.setShortcut('M')
        action.setStatusTip('Go to the next image')
        action.triggered.connect(self.tagger.next_image)
        pack_menu.addAction(action)

        action = QtGui.QAction('&Previous Image', self)
        action.setShortcut('N')
        action.setStatusTip('Go to the previous image')
        action.triggered.connect(self.tagger.prev_image)
        pack_menu.addAction(action)

        stats_menu = self.menuBar().addMenu("&Pack")
        action = QtGui.QAction('&List Missing', self)
        action.setStatusTip('Display the list of all image types without at least a single image')
        action.triggered.connect(self.tagger.list_missing_images)
        stats_menu.addAction(action)
        action = QtGui.QAction('&File Stats', self)
        action.setStatusTip('Display an overview of the files used in this pack')
        action.triggered.connect(self.tagger.show_file_stats)
        stats_menu.addAction(action)
        action = QtGui.QAction('&Meta Data', self)
        action.setStatusTip('Display the packs metadata')
        action.triggered.connect(self._meta_dlg)
        stats_menu.addAction(action)

        info_menu = self.menuBar().addMenu("&Info")
        action = QtGui.QAction('&Fallbacks', self)
        action.setStatusTip('Show Fallback Graph')
        action.triggered.connect(self._show_fallbacks)
        info_menu.addAction(action)

        action = QtGui.QAction('&Tags', self)
        action.setStatusTip('Show all available image tags')
        action.triggered.connect(self._show_all_tags)
        info_menu.addAction(action)

    def _load_pack_dlg(self):
        pack_file, _ = QFileDialog.getOpenFileName(self, "Open Pack", ".", "Pack Files (images*.xml)")
        if pack_file:
            from tagger.resource import load_image_pack
            self.tagger.set_pack_data(load_image_pack(pack_file))

    @requires_open_pack
    def _add_image_dlg(self):
        image_file, _ = QFileDialog.getOpenFileName(self, "Add Image", str(self.tagger.pack_data.path.parent),
                                                    "Image (*.jpg *.jpeg *.png *.webp *.mp4 *.webm *.avi)")
        if image_file:
            from tagger.resource import ImageResource
            image = ImageResource(file=image_file, **guess_type_by_file_name(image_file, self.translator))
            self.tagger.add_images([image])

    @requires_open_pack
    def _add_dir_dlg(self):
        directory = QFileDialog.getExistingDirectory(self, "Add Directory", str(self.tagger.pack_data.path.parent))
        if directory:
            self._add_from_directory(directory)

    @requires_open_pack
    def _save_pack_dlg(self, save_as=True):
        if save_as:
            target, _ = QFileDialog.getSaveFileName(self, "Save Pack", str(self.tagger.pack_data.path), "Pack Files (images.xml)")
        else:
            target = self.tagger.pack_data.path

        self.tagger.save_active_image()
        save_image_pack(target, self.tagger.pack_data)

    def _new_pack_dlg(self):
        target, _ = QFileDialog.getSaveFileName(self, "New Pack", ".", "Pack Files (images.xml)")
        if target:
            target = Path(target)
            self.tagger.set_pack_data(ResourcePack(target))
            self._add_from_directory(target.parent)
            save_image_pack(target, self.tagger.pack_data)

    @requires_open_pack
    def _add_from_directory(self, directory):
        from tagger.resource import ImageResource
        images = []
        for file in Path(directory).iterdir():
            if file.suffix not in FILE_SUFFIXES:
                continue
            image = ImageResource(file=file, **guess_type_by_file_name(file, self.translator))
            images.append(image)
        images = self.tagger.filter_new_images(images)
        self.tagger.add_images(sorted(images, key=lambda x: x.file))

    def _import_dlg(self):
        directory = QFileDialog.getExistingDirectory(self, "Import From")
        if directory:
            from tagger.importers.base import handle_import
            if self.tagger.pack_data is None:
                self.tagger.set_pack_data(ResourcePack(Path(directory)))
            self.tagger.add_images(sorted(handle_import(directory), key=lambda x: x.file))

    @requires_open_pack
    def _meta_dlg(self):
        dlg = PackMetaDlg(self.tagger.pack_data)
        dlg.exec_()

    def _show_fallbacks(self):
        dlg = VisualizeFallbackDlg(self.repo)
        dlg.exec_()

    def _show_all_tags(self):
        dlg = ShowAllTags(self.repo)
        dlg.exec_()


# Create a Qt widget, which will be our window.
window = MainWindow()
window.show()

# Start the event loop.
app.exec()
