from typing import List


class Channel:
    @staticmethod
    def get_defaults() -> List["Channel"]:
        names = ["Aiyana", "Audrey", "JJ", "Justine", "Sam"]
        channels = [
            ["Left", "Right"],
            ["BGLeft", "BGRight"],
        ]
        return [
            Channel(i + 1, f"{names[(i % 10)//2]}-{channels[i // 10][i % 2]}")
            for i in range(0, 20)
        ]

    def __init__(self, number: int, name: str):
        self.number = number
        self.name = name

    def __repr__(self):
        return f"Channel: <{self.number}: {self.name}>"
