import os
import sys
from pathlib import Path

if hasattr(sys, "_MEIPASS"):
    os.environ['QT_QPA_PLATFORM_PLUGIN_PATH'] = str(Path(sys._MEIPASS) / 'plugins' / "platforms")
