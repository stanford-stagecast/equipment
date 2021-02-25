class Camera:
    def __init__(self, name: str, id: int):
        self.id = id
        self.name = name

    def __repr__(self):
        return f"Cameras: <{self.ids}: {self.name}>"
