import subprocess
import os
import platform

class PowerDMXClient:
    def __init__(self):
        binPath = "./PowerDMX"
        if platform.system() == "Windows":
            binPath = "./PowerDMX.exe"
        self.WORKER = subprocess.Popen(binPath, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, stdin=subprocess.PIPE, text=True)
    def _SendCommand(self, command):
        self.WORKER.stdin.write(command + os.linesep)
        self.WORKER.stdin.flush()
        response = self.WORKER.stdout.readline()
        output, error = response.rstrip(os.linesep).split(";")
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