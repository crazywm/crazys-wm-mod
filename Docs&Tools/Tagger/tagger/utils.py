from PySide6.QtGui import QPixmap, QImageReader, QImage
from PySide6.QtWidgets import QFrame, QLabel, QVBoxLayout
from PySide6.QtCore import Qt

class QHLine(QFrame):
    def __init__(self):
        super(QHLine, self).__init__()
        self.setFrameShape(QFrame.HLine)
        self.setFrameShadow(QFrame.Sunken)


def clear_layout(layout):
    if layout is not None:
        while layout.count():
            child = layout.takeAt(0)
            if child.widget() is not None:
                child.widget().deleteLater()
            elif child.layout() is not None:
                clear_layout(child.layout())


class ImageDisplay(QLabel):
    def __init__(self, width, height):
        self.image_width = width
        self.image_height = height
        super().__init__("Active Image")
        self.pixmap = QPixmap()
        self.setMinimumWidth(self.image_width)
        self.setMinimumHeight(self.image_height)
        self.setAlignment(Qt.AlignHCenter | Qt.AlignVCenter)

    def set_image_data(self, image_data):
        if isinstance(image_data, str):
            reader = QImageReader(image_data)
            reader.setAutoTransform(True)
            image = reader.read()
        else:
            image = QImage.fromData(image_data)
        image = image.scaled(self.image_width, self.image_height, aspectMode=Qt.KeepAspectRatio)
        self.pixmap = QPixmap.fromImage(image)
        self.setPixmap(self.pixmap)


class VideoDisplay(QFrame):
    def __init__(self, width, height):
        from PySide6.QtMultimedia import QMediaPlayer
        from PySide6.QtMultimediaWidgets import QVideoWidget
        super().__init__()
        self.image_width = width
        self.image_height = height
        self._player = QMediaPlayer()
        self._video_widget = QVideoWidget()
        self._video_widget.setMinimumWidth(self.image_width)
        self._video_widget.setMinimumHeight(self.image_height)
        self._player.setVideoOutput(self._video_widget)
        self._player.setLoops(True)
        layout = QVBoxLayout()
        layout.addWidget(self._video_widget)
        self.setLayout(layout)

    def set_image_data(self, image_data):
        reader = QImageReader(image_data)
        reader.setAutoTransform(True)
        self._player.setSource(image_data)
        self._player.play()
        self._player.setLoops(True)
