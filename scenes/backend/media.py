class Media:
    def __init__(self, name: str, id: int, file: str, type: str):
        self.id = id
        self.name = name
        self.file = file
        self.type = type

    def __repr__(self):
        return f"Media: <{self.id}: {self.name}, {self.file}, {self.type}>"
