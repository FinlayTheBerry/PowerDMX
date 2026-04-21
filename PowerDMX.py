import subprocess
import time
import os
import colorsys

class PowerDMXClient:
    def __init__(self):
        binPath = "./bin/PowerDMX"
        if not os.path.isfile("./bin/PowerDMX"):
            binPath = "./bin/PowerDMX.exe"
        self.WORKER = subprocess.Popen(binPath, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, stdin=subprocess.PIPE, shell=True, text=True)
    def _SendCommand(self, command):
        self.WORKER.stdin.write(command + "\n")
        self.WORKER.stdin.flush()
        response = self.WORKER.stdout.readline().rstrip("\n")
        output, error = response.split(";")
        if error != "":
            raise Exception(error)
        else:
            return output
    def Enum(self):
        return int(self._SendCommand("Enum"))
    def Connect(self, index):
        return int(self._SendCommand(f"Connect;{int(index)}"))
    def Disconnect(self, connectionId):
        self._SendCommand(f"Disconnect;{int(connectionId)}")
        return
    def GetType(self, connectionId):
        return int(self._SendCommand(f"GetType;{int(connectionId)}"))
    def GetSerialNumber(self, connectionId):
        return int(self._SendCommand(f"GetSerialNumber;{int(connectionId)}"))
    def SetDmxState(self, connectionId, universe, dmxState):
        self._SendCommand(f"SetDmxState;{int(connectionId)};{int(universe)};{":".join([ f"{b:02x}" for b in bytes(dmxState) ])}")
        return
    def __del__(self):
        if self.WORKER:
            self.WORKER.terminate()
            self.WORKER.wait()
            print("PowerDMX killed.")



client = PowerDMXClient()
client.Enum()
client.Connect(0)
animationTimer = 0
lastTime = 0
while True:
    timeNow = time.time()
    deltaTime = timeNow - lastTime
    print(f"TPF: {10000000 * (deltaTime)} FPS: {1.0 / (deltaTime)}")
    lastTime = timeNow
    animationTimer = (animationTimer + deltaTime) % 2.0
    R, G, B = colorsys.hsv_to_rgb(animationTimer / 2.0, 1.0, 1.0)
    client.SetDmxState(0, 0, [ 255, int(R * 255), int(G * 255), int(B * 255) ])
    timeAfterRender = time.time()
    time.sleep(1 / 100 - (timeAfterRender - timeNow))