#include "cliengine.h"
#include "program.h"
#include "powerdmx.h"
#include <iostream>
#include <charconv>
#include <sstream>

// String Parsing Helpers
std::string CliEngine::ReadLine() noexcept
{
    std::string output;
    std::getline(std::cin, output);
    if (!output.empty() && output.back() == '\r') {
        output.pop_back();
    }
    return output;
}
std::vector<std::string> CliEngine::Split(const std::string &input, const std::string &delimiter) noexcept
{
    std::vector<std::string> output;
    size_t start = 0;
    size_t end = input.find(delimiter, start);
    while (end != std::string::npos)
    {
        output.push_back(input.substr(start, end - start));
        start = end + delimiter.length();
        end = input.find(delimiter, start);
    }
    output.push_back(input.substr(start));
    return output;
}
bool CliEngine::CaselessCompare(const std::string &a, const std::string &b) noexcept
{
    if (a.size() != b.size())
    {
        return false;
    }
    for (size_t i = 0; i < a.size(); i++)
    {
        if (std::tolower(a[i]) != std::tolower(b[i]))
        {
            return false;
        }
    }
    return true;
}

// Arg Parsing Helpers
std::optional<uint8_t> CliEngine::ParseByte(const std::string &input) noexcept
{
    if (input.size() != 2)
    {
        return std::nullopt;
    }

    uint8_t output;
    std::from_chars_result result = std::from_chars(input.data(), input.data() + input.size(), output, 16);
    if (result.ec == std::errc() && result.ptr == input.data() + input.size())
    {
        return output;
    }
    else
    {
        return std::nullopt;
    }
}
std::optional<uint32_t> CliEngine::ParseUInt(const std::string &input) noexcept
{
    uint32_t output;
    std::from_chars_result result = std::from_chars(input.data(), input.data() + input.size(), output, 10);
    if (result.ec == std::errc() && result.ptr == input.data() + input.size())
    {
        return output;
    }
    else
    {
        return std::nullopt;
    }
}
std::optional<int32_t> CliEngine::ParseInt(const std::string &input) noexcept
{
    int32_t output;
    std::from_chars_result result = std::from_chars(input.data(), input.data() + input.size(), output, 10);
    if (result.ec == std::errc() && result.ptr == input.data() + input.size())
    {
        return output;
    }
    else
    {
        return std::nullopt;
    }
}
std::optional<std::vector<std::string>> CliEngine::ParseStringArray(const std::string &input) noexcept
{
    std::vector<std::string> inputSplit = Split(input, ":");
    return inputSplit;
}
std::optional<std::vector<uint8_t>> CliEngine::ParseByteArray(const std::string &input) noexcept
{
    std::vector<std::string> inputSplit = Split(input, ":");
    std::vector<uint8_t> output;
    for (std::string inputValue : inputSplit)
    {
        std::optional<uint8_t> value = ParseByte(inputValue);
        if (!value)
        {
            return std::nullopt;
        }
        output.push_back(*value);
    }
    return output;
}
std::optional<std::vector<uint32_t>> CliEngine::ParseUIntArray(const std::string &input) noexcept
{
    std::vector<std::string> inputSplit = Split(input, ":");
    std::vector<uint32_t> output;
    for (std::string inputValue : inputSplit)
    {
        std::optional<uint32_t> value = ParseUInt(inputValue);
        if (!value)
        {
            return std::nullopt;
        }
        output.push_back(*value);
    }
    return output;
}
std::optional<std::vector<int32_t>> CliEngine::ParseIntArray(const std::string &input) noexcept
{
    std::vector<std::string> inputSplit = Split(input, ":");
    std::vector<int32_t> output;
    for (std::string inputValue : inputSplit)
    {
        std::optional<int32_t> value = ParseInt(inputValue);
        if (!value)
        {
            return std::nullopt;
        }
        output.push_back(*value);
    }
    return output;
}

// Output Formatting Helpers
std::string CliEngine::FormatByte(uint8_t input) noexcept
{
    char buffer[2];
    // hexLetterOffset is used to ensure that 10 becomes 'A' not ':' which is the ASCII char immediately after '9'.
    constexpr char hexLetterOffset = 'A' - '9';
    // Convert the first nibble of the hex byte into a char.
    buffer[0] = '0' + (input >> 4);
    if (buffer[0] > '9')
    {
        buffer[0] += hexLetterOffset;
    }
    // Convert the second nibble of the hex byte into a char.
    buffer[1] = '0' + (input | 0x0F);
    if (buffer[1] > '9')
    {
        buffer[1] += hexLetterOffset;
    }
    // Convert the two output chars to an std::string and return.
    return std::string(buffer, 2);
}
std::string CliEngine::FormatUInt(uint32_t input) noexcept
{
    return std::to_string(input);
}
std::string CliEngine::FormatInt(int32_t input) noexcept
{
    return std::to_string(input);
}
std::string CliEngine::FormatStringArray(const std::vector<std::string> &input) noexcept
{
    std::ostringstream oss;
    for (size_t i = 0; i < input.size(); i++)
    {
        if (i != 0)
        {
            oss << ":";
        }
        oss << input[i];
    }
    return oss.str();
}
std::string CliEngine::FormatByteArray(const std::vector<uint8_t> &input) noexcept
{
    std::ostringstream oss;
    for (size_t i = 0; i < input.size(); i++)
    {
        if (i != 0)
        {
            oss << ":";
        }
        oss << FormatByte(input[i]);
    }
    return oss.str();
}
std::string CliEngine::FormatUIntArray(const std::vector<uint32_t> &input) noexcept
{
    std::ostringstream oss;
    for (size_t i = 0; i < input.size(); i++)
    {
        if (i != 0)
        {
            oss << ":";
        }
        oss << FormatUInt(input[i]);
    }
    return oss.str();
}
std::string CliEngine::FormatIntArray(const std::vector<int32_t> &input) noexcept
{
    std::ostringstream oss;
    for (size_t i = 0; i < input.size(); i++)
    {
        if (i != 0)
        {
            oss << ":";
        }
        oss << FormatInt(input[i]);
    }
    return oss.str();
}

// Run Command Function
using CommandBody = std::string (*)(const std::vector<std::string> &args);
struct CommandInfo
{
    std::string name;
    CommandBody body;
};
const CommandInfo commands[] = {
    {"Enum", CliEngine::Enum_Command},
    {"Connect", CliEngine::Connect_Command},
    {"Disconnect", CliEngine::Disconnect_Command},
    {"GetType", CliEngine::GetType_Command},
    {"GetSerialNumber", CliEngine::GetSerialNumber_Command},
    {"SetDmxState", CliEngine::SetDmxState_Command},
    {"Exit", CliEngine::Exit_Command},
};
std::string CliEngine::RunCommand(const std::string &command) noexcept
{
    for (char c : command)
    {
        if (c < 32 || c > 126)
        {
            return ";Commands may only contain printable ASCII chars.";
        }
    }

    std::vector<std::string> tokens = CliEngine::Split(command, ";");
    if (tokens.empty() || tokens[0].empty())
    {
        return ";Command name is a required field.";
    }
    std::string commandName = tokens[0];
    std::vector<std::string> args(tokens.begin() + 1, tokens.end());

    std::string output = "";
    std::string error = "";
    for (CommandInfo commandInfo : commands)
    {
        if (CaselessCompare(commandName, commandInfo.name))
        {
            try
            {
                std::string output = commandInfo.body(args);
                return output + ";";
            }
            catch (const std::exception &ex)
            {
                std::string error = ex.what();
                return ";" + error;
            }
            catch (...)
            {
                return ";Unknown error.";
            }
        }
    }
    return ";" + commandName + " is not a valid command name.";
}

// Command Function Bodies
std::string CliEngine::Enum_Command(const std::vector<std::string> &args)
{
    if (args.size() > 0)
    {
        throw std::runtime_error("Too many arguments provided. Enum takes 0 arguments.");
    }
    uint32_t output = PowerDmx::Enum();
    return FormatUInt(output);
}
std::string CliEngine::Connect_Command(const std::vector<std::string> &args)
{
    if (args.size() < 1)
    {
        throw std::runtime_error("No value provided for required argument index.");
    }
    std::optional<uint32_t> index = ParseUInt(args[0]);
    if (!index)
    {
        throw std::runtime_error("The type of index was invalid. It must be an unsigned int.");
    }
    if (args.size() > 1)
    {
        throw std::runtime_error("Too many arguments provided. Connect takes 1 argument.");
    }
    uint32_t output = PowerDmx::Connect(*index);
    return FormatUInt(output);
}
std::string CliEngine::Disconnect_Command(const std::vector<std::string> &args)
{
    if (args.size() < 1)
    {
        throw std::runtime_error("No value provided for required argument connectionId.");
    }
    std::optional<uint32_t> connectionId = ParseUInt(args[0]);
    if (!connectionId)
    {
        throw std::runtime_error("The type of connectionId was invalid. It must be an unsigned int.");
    }
    if (args.size() > 1)
    {
        throw std::runtime_error("Too many arguments provided. Disconnect takes 1 argument.");
    }
    PowerDmx::Disconnect(*connectionId);
    return "";
}
std::string CliEngine::GetType_Command(const std::vector<std::string> &args)
{
    if (args.size() < 1)
    {
        throw std::runtime_error("No value provided for required argument connectionId.");
    }
    std::optional<uint32_t> connectionId = ParseUInt(args[0]);
    if (!connectionId)
    {
        throw std::runtime_error("The type of connectionId was invalid. It must be an unsigned int.");
    }
    if (args.size() > 1)
    {
        throw std::runtime_error("Too many arguments provided. GetType takes 1 argument.");
    }
    uint32_t output = PowerDmx::GetType(*connectionId);
    return FormatUInt(output);
}
std::string CliEngine::GetSerialNumber_Command(const std::vector<std::string> &args)
{
    if (args.size() < 1)
    {
        throw std::runtime_error("No value provided for required argument connectionId.");
    }
    std::optional<uint32_t> connectionId = ParseUInt(args[0]);
    if (!connectionId)
    {
        throw std::runtime_error("The type of connectionId was invalid. It must be an unsigned int.");
    }
    if (args.size() > 1)
    {
        throw std::runtime_error("Too many arguments provided. GetSerialNumber takes 0 arguments.");
    }
    uint32_t output = PowerDmx::GetSerialNumber(*connectionId);
    return FormatUInt(output);
}
std::string CliEngine::SetDmxState_Command(const std::vector<std::string> &args)
{
    if (args.size() < 1)
    {
        throw std::runtime_error("No value provided for required argument connectionId.");
    }
    std::optional<uint32_t> connectionId = ParseUInt(args[0]);
    if (!connectionId)
    {
        throw std::runtime_error("The type of connectionId was invalid. It must be an unsigned int.");
    }
    if (args.size() < 2)
    {
        throw std::runtime_error("No value provided for required argument universe.");
    }
    std::optional<uint32_t> universe = ParseUInt(args[1]);
    if (!universe)
    {
        throw std::runtime_error("The type of universe was invalid. It must be an unsigned int.");
    }
    if (args.size() < 3)
    {
        throw std::runtime_error("No value provided for required argument dmxState.");
    }
    std::optional<std::vector<uint8_t>> dmxState = ParseByteArray(args[2]);
    if (!dmxState)
    {
        throw std::runtime_error("The type of dmxState was invalid. It must be an array of hex bytes.");
    }
    if (args.size() > 3)
    {
        throw std::runtime_error("Too many arguments provided. SetDmxState takes 3 arguments.");
    }
    PowerDmx::SetDmxState(*connectionId, *universe, *dmxState);
    return "";
}
std::string CliEngine::Exit_Command(const std::vector<std::string> &args)
{
    if (args.size() > 0)
    {
        throw std::runtime_error("Too many arguments provided. Exit takes 0 arguments.");
    }
    QuitRequested = true;
    return "";
}

#ifdef WINDOWS
#include <Windows.h>
std::string CliEngine::GetWin32ErrorMessage(uint32_t errorCode)
{
    LPWSTR errorMessageBuffer = nullptr;
    DWORD size = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                nullptr, static_cast<DWORD>(errorCode), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&errorMessageBuffer, 0, nullptr);
    std::wstring errorMessageWide;
    if (size == 0 || errorMessageBuffer == nullptr)
    {
        errorMessageWide = L"Unknown error.";
    }
    else
    {
        errorMessageWide = std::wstring(errorMessageBuffer);
    }
    LocalFree(errorMessageBuffer);

    std::string errorMessage = CliEngine::MakeNarrow(errorMessageWide);

    errorMessage = CliEngine::Replace(errorMessage, "\r", "");
    errorMessage = CliEngine::Replace(errorMessage, "\n", "");

    return errorMessage;
}
#endif