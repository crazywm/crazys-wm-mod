from dataclasses import dataclass, field
from typing import List, Optional, Union, Dict
from pathlib import Path
from xml.etree import ElementTree as ET
from collections import defaultdict
FilePath = Union[Path, str]


_OPT_TO_BOOL = {
    "yes": True, "no": False
}

IMAGE_SUFFIXES = [
    ".png", ".jpg", ".jpeg", ".gif", ".bmp", ".tiff", "webp"
]
VIDEO_SUFFIXES = [
    ".mp4", ".webm", ".avi", ".mov"
]
FILE_SUFFIXES = IMAGE_SUFFIXES + VIDEO_SUFFIXES

@dataclass
class ImageResource:
    file: str
    type: str
    pregnant: str
    futa: str
    tied: str
    participants: str
    fallback: bool = False
    source: Optional[str] = None
    comment: Optional[str] = None
    style: Optional[str] = None


@dataclass
class ResourcePack:
    path: Path
    creator: str = ''
    comment: str = ''
    date: str = ''
    images: List[ImageResource] = field(default_factory=list)


@dataclass
class FallbackSpec:
    cost: int
    target: str
    required: Optional[str] = None


@dataclass
class TagSpec:
    description: str
    tag: str
    display: str
    fallback: List[FallbackSpec] = field(default_factory=list)


@dataclass
class FileNamePattern:
    type: str
    pregnant: str
    futa: str
    tied: str
    participants: str
    patterns: List[str] = field(default_factory=list)


def read_tag_specs(source: FilePath) -> Dict[str, TagSpec]:
    doc = ET.parse(source).getroot()

    specs = {}
    for image_type in doc.findall('Type'):
        name = image_type.attrib['Name'].lower()
        spec = TagSpec(tag=name, description="", display=name)
        for child in image_type:
            if child.tag == "Display":
                spec.display = child.text
            elif child.tag == "Fallback":
                spec.fallback.append(FallbackSpec(target=child.text, cost=int(child.attrib.get("Cost", 10)),
                                                  required=child.attrib.get("RequiredParticipants")))
            elif child.tag == "Description":
                spec.description = child.text

        specs[name] = spec
    return specs


def read_file_translations(source: FilePath) -> List[FileNamePattern]:
    doc = ET.parse(source).getroot()

    def optional(source, default):
        if source is None:
            return default
        return source.text.lower()

    file_patterns = []
    for image_type in doc.findall('FileType'):
        img_type = image_type.find('Type').text.lower()
        preg_val = optional(image_type.find('Pregnant'), "no")
        futa_val = optional(image_type.find('Futa'), "no")
        tied_val = optional(image_type.find('TiedUp'), "no")
        parts = "any"
        if image_type.find('Participants') is not None:
            parts = image_type.find('Participants').text
        patterns = []
        for p in image_type.findall('Pattern'):
            patterns.append(p.text)
        file_patterns.append(FileNamePattern(type=img_type, pregnant=preg_val, futa=futa_val,
                                             tied=tied_val, participants=parts, patterns=patterns))
    return file_patterns


def load_image(node) -> ImageResource:
    a = node.attrib
    return ImageResource(
        file=a["File"], type=a["Type"], pregnant=a["Pregnant"],
        futa=a.get("Futa", "no"), tied=a.get("TiedUp", "no"),
        fallback=_OPT_TO_BOOL[a.get("Fallback", "no")],
        source=a.get("Source", None), comment=a.get("Comment", None),
        style=a.get("Style", None),
        participants=a.get("Participants", "any"),
    )


def _get_optional_text(doc, element, default=""):
    elem = doc.find(element)
    if elem is None:
        return default
    else:
        return elem.text


def load_image_pack(path) -> ResourcePack:
    doc = ET.parse(path).getroot()
    images = []
    for image in doc.findall('Image'):
        images.append(load_image(image))

    creator = _get_optional_text(doc, "Creator")
    comment = _get_optional_text(doc, "Comment")
    date = _get_optional_text(doc, "Date")
    return ResourcePack(path=Path(path), creator=creator, comment=comment, date=date, images=images)


def guess_type_by_file_name(file_name: str, translation: List[FileNamePattern]) -> dict:
    import re
    file_name = Path(file_name).name.lower()
    for translate in translation:  # type: FileNamePattern
        for pattern in translate.patterns:
            p = re.compile(pattern)
            if p.match(file_name):
                if "les" in file_name:
                    print(translate)
                return {"type": translate.type, "pregnant": translate.pregnant,
                        "futa": translate.futa, "participants": translate.participants,
                        "tied": translate.tied}

    if file_name.startswith("preg"):
        base = guess_type_by_file_name(file_name[4:], translation)
        base["pregnant"] = "yes"
        return base

    return {"type": "", "pregnant": "no", "futa": "no", "participants": "any", "tied": "no"}


def _image_resource_dict(image: ImageResource):
    d = {"File": image.file, "Type": image.type, "Pregnant": image.pregnant,
         "Futa": image.futa, "TiedUp": image.tied,
         "Participants": image.participants}
    if image.source is not None:
        d["Source"] = image.source
    if image.comment is not None:
        d["Comment"] = image.comment
    if image.style is not None:
        d["Style"] = image.style
    if image.fallback:
        d["Fallback"] = "yes"
    return d


def save_image_pack(path, pack: ResourcePack):
    import datetime
    root = ET.Element("Images")
    ET.SubElement(root, "Creator").text = pack.creator
    ET.SubElement(root, "Comment").text = pack.comment
    ET.SubElement(root, "Date").text = datetime.date.today().isoformat()
    for image in pack.images:  # type: ImageResource
        im_el = ET.SubElement(root, "Image", _image_resource_dict(image))

    #tree = ET.ElementTree(root)
    # tree.indent(2)
    #tree.write(path)
    from xml.dom import minidom

    xmlstr = minidom.parseString(ET.tostring(root, encoding="unicode", xml_declaration=True)).toprettyxml(indent="   ")
    with open(path, "w") as f:
        f.write(xmlstr)


def get_canonical_name(name: str, repo: Dict[str, TagSpec]):
    text = name.strip()
    if text in repo:
        return text
    else:
        for entry in repo.values():
            if entry.display == text:
                return entry.tag
    return None


def _update_stats_subdict(sub, size_kb):
    sub["count"] += 1
    sub["total_kb"] += size_kb
    sub["smallest"] = min(sub["smallest"], size_kb)
    sub["largest"] = max(sub["largest"], size_kb)


def get_stats(root: Path, images: List[ImageResource]):
    result = {
        "images": {"count": 0, "total_kb": 0, "smallest": 1e100, "largest": 0},
        "videos": {"count": 0, "total_kb": 0, "smallest": 1e100, "largest": 0},
        "other": 0,
        "types": defaultdict(lambda: 0)
    }

    for image in images:
        file_path = root.parent / Path(image.file)
        suffix = file_path.suffix
        size_kb = file_path.stat().st_size // 1024
        if suffix in IMAGE_SUFFIXES:
            _update_stats_subdict(result["images"], size_kb)
        elif suffix in VIDEO_SUFFIXES:
            _update_stats_subdict(result["videos"], size_kb)
        else:
            result["other"] += 1
        result["types"][suffix[1:]] += 1
    return result


KNOW_PARTICIPANT_VALUES = ["Solo", "Lesbian", "Hetero", "FFM", "MMF", "Gangbang", "Lesgroup",
                           "Orgy", "Other", "Any"]