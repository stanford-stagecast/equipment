from channel import Channel


class Delta:
    def __init__(self, channel: Channel, intensity: int, hue: int):
        self.channel = channel
        self.intensity = intensity
        self.hue = hue
