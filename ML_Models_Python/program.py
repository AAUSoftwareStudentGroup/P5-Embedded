from ml import identifyPlayer
from settings import init_globals
if not __name__ == "__main__":
    raise ValueError("This method should only be called from main")

init_globals()
while True:
    identifyPlayer('simplernn 3.0 with parameters')