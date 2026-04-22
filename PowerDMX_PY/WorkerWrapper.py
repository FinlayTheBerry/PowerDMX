import subprocess
import os
import platform
from pathlib import Path

class _WorkerWrapper:
    def __init__(self):
        script_dir = Path(__file__).parent.absolute()
        bin_name = "PowerDMX.exe" if platform.system() == "Windows" else "PowerDMX"
        bin_path = os.path.join(script_dir, bin_name)
        if not os.path.exists(bin_path):
            raise Exception(f"PowerDMX binary not found at \"{bin_path}\".")
        self.worker = subprocess.Popen(bin_path, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, stdin=subprocess.PIPE, text=True, bufsize=1)

    def _send(self, command):
        self.worker.stdin.write(command + os.linesep)
        self.worker.stdin.flush()
        response = self.worker.stdout.readline()
        if not response:
            raise Exception("PowerDMX worker terminated.")
        output, error = response.rstrip(os.linesep).split(";", 1)
        if error:
            raise Exception(f"{error}")
        return output
    
    def __del__(self):
        if hasattr(self, "worker") and self.worker:
            self.worker.terminate()
            try:
                self.worker.wait(timeout=1)
            except:
                self.worker.kill()