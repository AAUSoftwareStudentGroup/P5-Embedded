class Strategy:

    def __init__(self, func):
        if func:
            self.execute = func

    def execute(self):
        # method should be overwritten
        raise NotImplementedError()