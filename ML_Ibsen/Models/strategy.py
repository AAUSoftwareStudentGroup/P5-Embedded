class Strategy:

    def __init__(self, func):
        if func:
            self.execute = func

    def execute(self):
        raise NotImplementedError()
        # will be overwritten