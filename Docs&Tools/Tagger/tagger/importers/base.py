from pathlib import Path
from typing import Dict
from ..resource import ImageResource, IMAGE_SUFFIXES, VIDEO_SUFFIXES

from PySide6.QtWidgets import QDialog, QVBoxLayout, QComboBox, QLabel, QLineEdit, QHBoxLayout, QPushButton, QMessageBox

_REGISTERED_IMPORTERS = {}
_BASE_DICT = {"type": "", "pregnant": "no", "participants": "any", "outfit": "none", 'fallback': False, "futa": "no",
              "tied": "no"}


class ImporterBase:
    def __init__(self, name: str, display: str):
        self._images = []
        self.name = name
        self.display = display
        self.error_count = 0

    def produce_guesses(self, directory: Path):
        directory = Path(directory)
        if not self.validate(directory):
            raise RuntimeError("The given directory does not appear to be compatible with the importer.")

        self.error_count = 0
        for file_name in directory.rglob("*"):      # type: Path
            if (file_name.suffix not in IMAGE_SUFFIXES) and (file_name.suffix not in VIDEO_SUFFIXES):
                continue
            else:
                relative = file_name.relative_to(directory)
                guess = self.guess_for_image(relative)
                if guess is not None:
                    yield file_name, guess
                else:
                    self.error_count += 1
                    yield file_name, {}

    def guess_for_image(self, relative: Path):
        raise NotImplementedError()

    def validate(self, path: Path):
        """
        This function checks if the given directory really seems to contain a pack compatible with the importer.
        """
        raise NotImplementedError()


def register(importer: ImporterBase):
    _REGISTERED_IMPORTERS[importer.name] = importer


class SetupImportDialog(QDialog):
    def __init__(self, source):
        super().__init__()
        self.setWindowTitle("Import")
        layout = QVBoxLayout()
        importer_names = []
        self.lookup = {}
        for importer in _REGISTERED_IMPORTERS.values():
            importer_names.append(importer.display)
            self.lookup[importer.display] = importer

        layout.addWidget(QLabel("Directory:"))
        layout.addWidget(QLabel(str(source)))

        layout.addWidget(QLabel("Pack Type"))
        self._select = QComboBox()
        self._select.addItems(importer_names)
        layout.addWidget(self._select)

        layout.addWidget(QLabel("Default Source"))
        self._source = QLineEdit()
        layout.addWidget(self._source)

        buttons = QHBoxLayout()
        layout.addLayout(buttons)
        imp = QPushButton("Import")
        imp.clicked.connect(self.accept)
        cnc = QPushButton("Cancel")
        cnc.clicked.connect(self.reject)
        buttons.addWidget(cnc)
        buttons.addWidget(imp)
        self.setLayout(layout)

        self.importer = None  # type: ImporterBase
        self.source = ""
        self.finished.connect(self._set_result)

    def _set_result(self, code):
        if code == 1:
            self.importer = self.lookup[self._select.currentText()]
            self.source = self._source.text()


def handle_import(directory):
    dialog = SetupImportDialog(directory)
    result = dialog.exec_()
    if not result:
        return False

    importer = dialog.importer
    default_source = dialog.source
    images = []

    for file, guess in importer.produce_guesses(directory):
        merged = {**_BASE_DICT, **guess}

        # currently, we don't have outfit support
        if "outfit" in merged:
            del merged["outfit"]
        if "position" in merged:
            del merged["position"]

        images.append(ImageResource(file=file, **merged, source=default_source, comment="Automatically Labelled"))

    if importer.error_count != 0:
        msg = QMessageBox.warning(None, "Unidentified image types",
                                  f"Could not guess the types of {importer.error_count} images!")
    return images
