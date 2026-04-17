import subprocess
import os

class CLibraryWrapper:
    def __init__(self, executable_path="./my_c_program"):
        # Start the process
        self.process = subprocess.Popen(
            [executable_path],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,     # Handles encoding/decoding to strings automatically
            bufsize=1      # Line buffered
        )

    def _send_command(self, command: str) -> str:
        """Internal helper to handle the I/O."""
        if self.process.poll() is not None:
            raise ChildProcessError("The C executable has terminated.")

        # Ensure the command ends with a newline
        self.process.stdin.write(f"{command}\n")
        self.process.stdin.flush()

        # Read exactly one line back
        response = self.process.stdout.readline()
        return response.strip()

    def get_status(self, item_id: int):
        """Example library function."""
        return self._send_command(f"GET_STATUS {item_id}")

    def close(self):
        """Clean up the process."""
        self.process.stdin.close()
        self.process.terminate()
        self.process.wait()