#ifndef POWERDMX
#define POWERDMX
#include <string>
#include <vector>
#include <cstdint>

namespace PowerDmx {
	uint32_t Enum();
	uint32_t Connect(uint32_t index);
	void Disconnect(uint32_t connectionId);
	uint32_t GetType(uint32_t connectionId);
	uint32_t GetSerialNumber(uint32_t connectionId);
	void SetDmxState(uint32_t connectionId, uint32_t universe, const std::vector<uint8_t>& dmxState);
}
#endif // POWERDMX