from pathlib import Path

from .base import ImporterBase


class VenusImporter(ImporterBase):
    def __init__(self):
        super().__init__("Venus", "Venus Club")

    def guess_for_image(self, relative_path: Path):
        str_path = str(relative_path).lower()
        if str_path.startswith("video"):
            inner_path = relative_path.relative_to("videos/Performances/Work")
            kind = str(inner_path.parent).lower()
            try:
                return _VENUS_TABLE[kind]
            except KeyError:
                pass

        if relative_path.stem.lower() == "portrait":
            return {"type": "profile"}

        if relative_path.stem.lower() == "portrait_small":
            return {"type": "portrait", "futa": "maybe", "pregnant": "maybe"}

        print("Unclear: ", relative_path.parent)
        return None

    def validate(self, path: Path):
        has_vid = (path / "videos").exists()
        has_img = (path / "images").exists()
        has_data = (path / "JSON_data").exists()
        has_text = (path / "texts").exists()
        return has_vid and has_img and has_data and has_text


_VENUS_TABLE = {
    "finish/body/tits": {},
    "finish/facial/swallow": {},
    "finish/facial/face": {},
    "finish/inside/creampie": {},
    "dance/dance": {},
    "dance/topless": {},
    "dance/closer": {},
    "oral/deepthroat": {"type": "deepthroat", "participants": "hetero"},
    "oral/facefuck": {"type": "deepthroat", "participants": "hetero"},
    "oral/bj": {"type": "oral", "participants": "hetero"},
    "pose/naked": {"type": "naked"},
    "pose/toymast": {"type": "dildo", "participants": "solo"},
    "pose/handmast": {"type": "finger", "participants": "solo"},
    "sex/missionary": {"type": "sex", "participants": "hetero", "position": "missionary"},
    "sex/doggy": {"type": "sex", "participants": "hetero", "position": "doggy"},
    "sex/anal": {"type": "anal", "participants": "hetero"},
    "foreplay/hj": {"type": "hand", "participants": "hetero"},
    "foreplay/tj": {"type": "titty", "participants": "hetero"},
    "foreplay/fj": {"type": "foot", "participants": "hetero"},
    "group/3some": {"type": "sex", "participants": "mmf"},
    "group/4some": {"type": "sex", "participants": "orgy"},
}
