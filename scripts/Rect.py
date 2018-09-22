class Rect:
    def __init__(self, x: int, y: int, width: int, height: int):
        self.x = x
        self.y = y
        self.width = width
        self.height = height

    def sub_im(self, im):
        return im[int(self.y):int(self.y) + int(self.height), int(self.x):int(self.x) + int(self.width)]

    def tl(self):
        return int(self.x), int(self.y)

    def br(self):
        return int(self.x + self.width), int(self.y + self.height)
