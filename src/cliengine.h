#ifndef CLIENGINE
#define CLIENGINE
#include <cstdint>
#include <string>
#include <vector>
#include <optional>

namespace CliEngine
{
    std::string ReadLine() noexcept;
    std::vector<std::string> Split(const std::string &input, const std::string &delimiter) noexcept;
    bool CaselessCompare(const std::string &a, const std::string &b) noexcept;

    std::optional<std::string> ParseString(const std::string &input) noexcept;
    std::optional<uint8_t> ParseByte(const std::string &input) noexcept;
    std::optional<uint32_t> ParseUInt(const std::string &input) noexcept;
    std::optional<int32_t> ParseInt(const std::string &input) noexcept;
    std::optional<std::vector<std::string>> ParseStringArray(const std::string &input) noexcept;
    std::optional<std::vector<uint8_t>> ParseByteArray(const std::string &input) noexcept;
    std::optional<std::vector<uint32_t>> ParseUIntArray(const std::string &input) noexcept;
    std::optional<std::vector<int32_t>> ParseIntArray(const std::string &input) noexcept;

    std::string FormatByte(uint8_t input) noexcept;
    std::string FormatUInt(uint32_t input) noexcept;
    std::string FormatInt(int32_t input) noexcept;
    std::string FormatStringArray(const std::vector<std::string> &input) noexcept;
    std::string FormatByteArray(const std::vector<uint8_t> &input) noexcept;
    std::string FormatUIntArray(const std::vector<uint32_t> &input) noexcept;
    std::string FormatIntArray(const std::vector<int32_t> &input) noexcept;

    std::string RunCommand(const std::string &command) noexcept;

    std::string Enum_Command(const std::vector<std::string> &args);
    std::string Connect_Command(const std::vector<std::string> &args);
    std::string Disconnect_Command(const std::vector<std::string> &args);
    std::string GetType_Command(const std::vector<std::string> &args);
    std::string GetSerialNumber_Command(const std::vector<std::string> &args);
    std::string SetDmxState_Command(const std::vector<std::string> &args);
    std::string Exit_Command(const std::vector<std::string> &args);
}
#endif // CLIENGINE