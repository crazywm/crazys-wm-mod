from .resource import ImageResource


_GROUP_PTS = ["mmf", "ffm", "orgy", "gangbang", "lesgroup"]
_HUMAN_SEX_TAGS = ["anal", "bdsm", "sex", "oral", "titty", "hand", "foot", "deepthroat", "dildo", "strapon",
                   "sixtynine", "lick", "finger", "fisting", "suckballs", "rape", "cumshot",
                   "cowgirl", "revcowgirl", "doggy", "missionary", "scissor"]
_SEX_TAGS = _HUMAN_SEX_TAGS + ["beast", "tentacle", "rapebeast"]
def wm6_filename_from_tags(image: ImageResource):
    candidates = image.type.split(";")[0]

    if image.pregnant.lower() == "yes":
        return "preg" + name


def base_from_tag(tag: str, participants: str):
    if tag  in ["tentacle", "beast"]:
        return "beast"

    if tag in ["profile", "portrait"]:
        return "profile"

    if participants in _GROUP_PTS:
        if tag in _HUMAN_SEX_TAGS:
            if tag != "bdsm":
                return "group"
            else:
                return "bdsm"
        return tag

    if participants == "lesbian":
        if tag == "sixtynine":
            return "les69ing"
        if tag in _HUMAN_SEX_TAGS:
            return "les"

    tag = tag.lower()


# "anal*.", "bdsm*.", "sex*.", "group*.", "les*.", "torture*.",
# "death*.", "profile*.", "combat*.", "oral*.", "ecchi*.", "strip*.", "kiss*.", "maid*.", "sing*.",
# "wait*.", "card*.", "bunny*.", "nude*.", "mast*.", "titty*.", "milk*.", "hand*.",
# "foot*.", "bed*.", "farm*.", "herd*.", "cook*.", "craft*.", "swim*.", "bath*.",
# "nurse*.", "formal*.", "shop*.", "magic*.", "sign*.", "presented*.", "dom*.",
# "deep*.", "eatout*.", "dildo*.", "sub*.", "strapon*.", "les69ing*.", "lick*.", "balls*.",
# "cowgirl*.", "revcowgirl*.", "sexdoggy*.", "jail*.", "puppygirl*.", "ponygirl*.", "catgirl*.",
# "brand*.", "rape*.", "beastrape*.", "birthhuman*.", "birthmultiplehuman*.", "birthbeast*.",
#     "impregsex*.", "impreggroup*.", "impregbdsm*.", "impregbeast*.",
#     "virginsex*.", "virgingroup*.", "virginbdsm*.", "virginbeast*.",
#     "escort*.", "sport*.", "study*.", "teacher*.",
#     "massage*.", "studiocrew*.", "cameramage*.", "director*.", "crystalpurifier*.", "stagehand*.",
# "piano*.", "music*.", "refuse*.", "drink*.", "dinner*.",
