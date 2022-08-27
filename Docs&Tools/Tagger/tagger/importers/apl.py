from pathlib import Path

from .base import ImporterBase


class APLImporter(ImporterBase):
    def __init__(self):
        super().__init__("APL", "A Photographer's Lies")

    def guess_for_image(self, relative_path: Path):
        parent = relative_path.parent

        parent_name = str(parent.name).lower()
        if parent_name == ".":
            return None

        if parent_name in _APL_TABLE:
            return _APL_TABLE[parent_name]
        elif "avatar" in str(relative_path.name).lower() or "selfie" in str(relative_path.name).lower():
            return {"type": "portrait"}
        else:
            print(str(relative_path.name).lower())
            print("Unclear: ", relative_path.parent)
            return None


_APL_TABLE = {
    "nurse": {"type": "nurse", "outfit": "nurse"},
    "normal": {"type": "profile"},
    "underwear": {"type": "profile", "outfit": "lingerie"},
    "schoolgirl": {"type": "profile", "outfit": "schoolgirl"},
    "xmas": {"type": "profile", "outfit": "xmas"},
    "nude": {"type": "nude"},
    "seduction": {"type": "ecchi"},
    "cosplay": {},
    "bdsmpics": {"type": "bdsm"},
    "bdsm": {"type": "bdsm"},
    "toy": {"type": "dildo"},
    "toys": {"type": "dildo"},
    "cheerleader": {"type": "cheer"},
    "straight": {"type": "sex", "participants": "hetero"},
    "lesbian": {"type": "sex", "participants": "lesbian"},
    "anal": {"type": "anal", "participants": "hetero"},
    "blowjob": {"type": "oral", "participants": "hetero"},
    "deepthroat": {"type": "deepthroat", "participants": "hetero"},
    "titjob": {"type": "titty", "participants": "hetero"},
    "handjob": {"type": "hand", "participants": "hetero"},
    "group": {"type": "group", "participants": "orgy"},
    "cumshot": {},
}
