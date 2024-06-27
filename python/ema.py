class EMA:
    def __init__(self, b=0.9):
        self.b = b
        self.n = 0
        self.avg = 0

    def next(self, v):
        self.n += 1
        self.avg = self.b * v + (1 - self.b) * self.avg
        return self.avg

    def avg(self):
        return self.avg