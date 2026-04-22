from .WorkerWrapper import _WorkerWrapper
_workerWrapper = _WorkerWrapper()

def Enum():
    return int(_workerWrapper._send("Enum"))

def Connect(index):
    return int(_workerWrapper._send(f"Connect;{int(index)}"))

def Disconnect(connection_id):
    _workerWrapper._send(f"Disconnect;{int(connection_id)}")

def GetType(connection_id):
    return int(_workerWrapper._send(f"GetType;{int(connection_id)}"))

def GetSerialNumber(connection_id):
    return int(_workerWrapper._send(f"GetSerialNumber;{int(connection_id)}"))

def SetDmxState(connection_id, universe, dmx_state):
    _workerWrapper._send(f"SetDmxState;{int(connection_id)};{int(universe)};{":".join([f"{b:02x}" for b in bytes(dmx_state)])}")

__all__ = ["Enum", "Connect", "Disconnect", "GetType", "GetSerialNumber", "SetDmxState"]