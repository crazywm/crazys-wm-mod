import os
import sys
from pathlib import Path

if hasattr(sys, "_MEIPASS"):
    os.environ['QT_PLUGIN_PATH'] = str(Path( sys._MEIPASS ) / 'PyQt6/Qt6')
