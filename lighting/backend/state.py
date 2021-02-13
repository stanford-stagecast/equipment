from cue_list import CueList

cues = CueList()


class State():
    @staticmethod
    def get() -> CueList:
        return cues

    @staticmethod
    def set(data: CueList):
        global cues
        cues = data

