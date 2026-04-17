#include "powerdmx.h"
#include <libusb-1.0/libusb.h>
#include <iostream>
#include <sstream>
#include <utility>
#include <iomanip>
#include <optional>
#include <fstream>
#include <codecvt>
#include <algorithm>
#include <filesystem>
#include <list>

extern const uint32_t etc_winusb_cat_size;
extern const uint8_t etc_winusb_inf_data[];
extern const uint32_t etc_winusb_inf_size;
extern const uint8_t etc_winusb_inf_data[];
extern const uint32_t powercues_udev_setup_sh_size;
extern const uint8_t powercues_udev_setup_sh_data[];

static constexpr uint16_t USB_VID_ETC = 0x14D5;
static constexpr uint16_t USB_PID_ETC_Gadget = 0x0112;
static constexpr uint16_t USB_PID_ETC_Gadget_2 = 0x0114;
static constexpr uint16_t USB_PID_ETC_Gadget_2_CPU2 = 0x011C;
static constexpr uint8_t USB_HID_Interface_Number = 0x00;
static constexpr uint8_t USB_Bulk_Interface_Number = 0x01;
static constexpr uint8_t __attribute__((unused)) USB_HID_Out_Endpoint_Number = 0x01;
static constexpr uint8_t __attribute__((unused)) USB_HID_In_Endpoint_Number = 0x81;
static constexpr uint8_t USB_Bulk_Out_Endpoint_Number = 0x02;
static constexpr uint8_t USB_Bulk_In_Endpoint_Number = 0x82;

static constexpr uint32_t DeviceType_ETC_Gadget = 0;
static constexpr uint32_t DeviceType_ETC_Gadget_2 = 1;
static constexpr uint32_t DeviceType_ETC_Gadget_2_CPU2 = 2;

struct __attribute__((packed)) SetDmxStatePacket
{
    uint8_t magic = 0x20;
    uint8_t universe = 0;
    uint8_t reserved[4] = {0x00, 0x00, 0x00, 0x02};
    uint8_t payload[512] = { };
};
struct __attribute__((packed)) GetSerialNumberRequestPacket
{
    uint8_t magic = 0x00;
    uint8_t command = 0x08;
    uint8_t reserved[2] = {0x00, 0x00};
};
struct __attribute__((packed)) GetSerialNumberResponsePacket
{
    uint8_t magic = 0x00;
    uint8_t command = 0x08;
    uint8_t reserved[2] = {0x00, 0x00};
    uint32_t serialNumber = 0;
};
struct Interface
{
    uint32_t type = 0;
    libusb_device *libusbDevice = nullptr;
};
struct Connection
{
    uint32_t id = 0;
    uint32_t type = 0;
    libusb_device *libusbDevice = nullptr;
    libusb_device_handle *libusbDeviceHandle = nullptr;
};

static libusb_context *libusbContext = nullptr;
static std::vector<Interface> interfaces = {};
static std::list<Connection> connections = {};
static uint32_t nextFreeId = 0;

uint32_t PowerDmx::Enum()
{
    int errorCode = 0;
    if (libusbContext == nullptr)
    {
        errorCode = libusb_init(&libusbContext);
        if (errorCode != 0)
        {
            throw std::runtime_error(std::string("libusb_init: ") + libusb_error_name(errorCode));
        }
    }

    for (Interface interface : interfaces)
    {
        libusb_unref_device(interface.libusbDevice);
    }
    interfaces = {};

    libusb_device **list;
    ssize_t count = libusb_get_device_list(libusbContext, &list);
    if (count < 0)
    {
        throw std::runtime_error(std::string("libusb_get_device_list: ") + libusb_error_name(static_cast<int>(count)));
    }
    for (ssize_t i = 0; i < count; i++)
    {
        libusb_device_descriptor deviceDescriptor;
        errorCode = libusb_get_device_descriptor(list[i], &deviceDescriptor);
        if (errorCode != 0)
        {
            libusb_free_device_list(list, 1);
            throw std::runtime_error(std::string("libusb_get_device_list: ") + libusb_error_name(errorCode));
        }
        if (deviceDescriptor.idVendor == USB_VID_ETC)
        {
            Interface newInterface = {};
            switch (deviceDescriptor.idProduct)
            {
            case USB_PID_ETC_Gadget:
                newInterface.type = DeviceType_ETC_Gadget;
                break;
            case USB_PID_ETC_Gadget_2:
                newInterface.type = DeviceType_ETC_Gadget_2;
                break;
            case USB_PID_ETC_Gadget_2_CPU2:
                newInterface.type = DeviceType_ETC_Gadget_2_CPU2;
                break;
            default:
                continue;
            }
            newInterface.libusbDevice = libusb_ref_device(list[i]);
            interfaces.push_back(newInterface);
        }
    }
    libusb_free_device_list(list, 1);
    return static_cast<uint32_t>(interfaces.size());
}
uint32_t PowerDmx::Connect(uint32_t index)
{
    if (index >= interfaces.size())
    {
        throw std::runtime_error("index must be less than the number of enumerated interfaces.");
    }

    Interface interface = interfaces[index];
    Connection connection = {};

    int errorCode = libusb_open(interface.libusbDevice, &connection.libusbDeviceHandle);
    if (errorCode != 0)
    {
        throw std::runtime_error(std::string("libusb_open: ") + libusb_error_name(errorCode));
    }

    if (libusb_kernel_driver_active(connection.libusbDeviceHandle, USB_Bulk_Interface_Number) == 1)
    {
        errorCode = libusb_detach_kernel_driver(connection.libusbDeviceHandle, USB_Bulk_Interface_Number);
        if (errorCode != 0)
        {
            libusb_close(connection.libusbDeviceHandle);
            throw std::runtime_error(std::string("libusb_detach_kernel_driver: ") + libusb_error_name(errorCode));
        }
    }
    if (libusb_kernel_driver_active(connection.libusbDeviceHandle, USB_HID_Interface_Number) == 1)
    {
        errorCode = libusb_detach_kernel_driver(connection.libusbDeviceHandle, USB_HID_Interface_Number);
        if (errorCode != 0)
        {
            libusb_close(connection.libusbDeviceHandle);
            throw std::runtime_error(std::string("libusb_detach_kernel_driver: ") + libusb_error_name(errorCode));
        }
    }

    errorCode = libusb_claim_interface(connection.libusbDeviceHandle, USB_Bulk_Interface_Number);
    if (errorCode != 0)
    {
        libusb_close(connection.libusbDeviceHandle);
#ifdef WINDOWS
        if (errorCode == LIBUSB_ERROR_NOT_SUPPORTED)
        {
            throw std::runtime_error("ETC WinUsb drivers are not installed on this PC. Please run the setup command first.");
        }
        else
#endif
        {
            throw std::runtime_error(std::string("libusb_claim_interface: ") + libusb_error_name(errorCode));
        }
    }
    errorCode = libusb_claim_interface(connection.libusbDeviceHandle, USB_HID_Interface_Number);
    if (errorCode != 0)
    {
        libusb_close(connection.libusbDeviceHandle);
#ifdef WINDOWS
        if (errorCode == LIBUSB_ERROR_NOT_SUPPORTED)
        {
            throw std::runtime_error("ETC WinUsb drivers are not installed on this PC. Please run the setup command first.");
        }
        else
#endif
        {
            throw std::runtime_error(std::string("libusb_claim_interface: ") + libusb_error_name(errorCode));
        }
    }

    connection.libusbDevice = libusb_ref_device(interface.libusbDevice);
    connection.type = interface.type;
    connection.id = nextFreeId;
    nextFreeId++;
    connections.push_back(connection);
    return connection.id;
}
void PowerDmx::Disconnect(uint32_t connectionId)
{
    for (std::list<Connection>::iterator i = connections.begin(); i != connections.end(); i++)
    {
        Connection connection = *i;
        if (connection.id == connectionId)
        {
            libusb_close(connection.libusbDeviceHandle);
            libusb_unref_device(connection.libusbDevice);
            connections.erase(i);
            return;
        }
    }
    throw std::runtime_error("connectionId must be a valid connection id returned from Connect.");
}
uint32_t PowerDmx::GetType(uint32_t connectionId)
{
    std::optional<Connection> connection = std::nullopt;
    for (std::list<Connection>::iterator i = connections.begin(); i != connections.end(); i++)
    {
        if ((*i).id == connectionId)
        {
            connection = *i;
            break;
        }
    }
    if (!connection)
    {
        throw std::runtime_error("connectionId must be a valid connection id returned from Connect.");
    }

    return connection->type;
}
uint32_t PowerDmx::GetSerialNumber(uint32_t connectionId)
{
    std::optional<Connection> connection = std::nullopt;
    for (std::list<Connection>::iterator i = connections.begin(); i != connections.end(); i++)
    {
        if ((*i).id == connectionId)
        {
            connection = *i;
            break;
        }
    }
    if (!connection)
    {
        throw std::runtime_error("connectionId must be a valid connection id returned from Connect.");
    }

    GetSerialNumberRequestPacket requestPacket = {};
    uint8_t* requestBuffer = reinterpret_cast<uint8_t*>(&requestPacket);
    int sentBytes = 0;
    int errorCode = libusb_bulk_transfer(connection->libusbDeviceHandle, USB_Bulk_Out_Endpoint_Number, reinterpret_cast<uint8_t*>(&requestPacket), sizeof(requestPacket), &sentBytes, 1000);
    if (errorCode != 0)
    {
        throw std::runtime_error(std::string("libusb_bulk_transfer: ") + libusb_error_name(errorCode));
    }
    if (sentBytes != sizeof(requestPacket))
    {
        throw std::runtime_error("libusb_bulk_transfer: Failed to send all bytes.");
    }

    uint8_t responseBuffer[4096];
    GetSerialNumberResponsePacket *responsePacket = reinterpret_cast<GetSerialNumberResponsePacket*>(responseBuffer);
    int readBytes = 0;
    errorCode = libusb_bulk_transfer(connection->libusbDeviceHandle, USB_Bulk_In_Endpoint_Number, responseBuffer, sizeof(responseBuffer), &readBytes, 1000);
    if (errorCode != 0)
    {
        throw std::runtime_error(std::string("libusb_bulk_transfer: ") + libusb_error_name(errorCode));
    }
    if (readBytes < static_cast<int>(sizeof(GetSerialNumberResponsePacket)))
    {
        throw std::runtime_error("libusb_bulk_transfer: Failed to read all bytes.");
    }
    for (size_t i = 0; i < sizeof(requestPacket); i++){
        if (responseBuffer[i] != requestBuffer[i]) {
            throw std::runtime_error("libusb_bulk_transfer: Response packet was corrupt.");
        }
    }

    return responsePacket->serialNumber;
}
void PowerDmx::SetDmxState(uint32_t connectionId, uint32_t universe, const std::vector<uint8_t> &dmxState)
{
    std::optional<Connection> connection = std::nullopt;
    for (std::list<Connection>::iterator i = connections.begin(); i != connections.end(); i++)
    {
        if ((*i).id == connectionId)
        {
            connection = *i;
            break;
        }
    }
    if (!connection)
    {
        throw std::runtime_error("connectionId must be a valid connection id returned from Connect.");
    }

    if (dmxState.size() > 512)
    {
        throw std::runtime_error("dmxState must be less than 512 bytes in size.");
    }
    if (universe > 1)
    {
        throw std::runtime_error("universe must be less than 2.");
    }

    SetDmxStatePacket setDmxStatePacket = {};
    setDmxStatePacket.universe = universe;
    memcpy(setDmxStatePacket.payload, dmxState.data(), dmxState.size());
    int sentBytes = 0;
    int errorCode = libusb_bulk_transfer(connection->libusbDeviceHandle, USB_Bulk_Out_Endpoint_Number, reinterpret_cast<unsigned char *>(&setDmxStatePacket), sizeof(setDmxStatePacket), &sentBytes, 1000);
    if (errorCode != 0)
    {
        throw std::runtime_error(std::string("libusb_bulk_transfer: ") + libusb_error_name(errorCode));
    }
    if (sentBytes != sizeof(setDmxStatePacket))
    {
        throw std::runtime_error("libusb_bulk_transfer: Failed to send all bytes.");
    }
}

#ifdef WINDOWS
#include <setupapi.h>
#pragma comment(lib, "Setupapi.lib")
void PowerDmx::Setup()
{
    // Get file paths
    wchar_t tempPath[MAX_PATH + 2];
    if (GetTempPath2W(MAX_PATH + 2, tempPath) == 0)
    {
        std::cout << CliEngine::GetWin32ErrorMessage(GetLastError()) << std::endl;
    }
    std::wstring etcWinUsbInfPath = tempPath + std::wstring(L"ETC_WinUsb.inf");
    std::wstring etcWinUsbCatPath = tempPath + std::wstring(L"ETC_WinUsb.cat");

    // Extract ETC_WinUsb.inf to target path
    std::ofstream infFile(etcWinUsbInfPath, std::ios::binary | std::ios::trunc);
    infFile.write(reinterpret_cast<const char *>(ETC_WinUsb_inf_Data), ETC_WinUsb_inf_Length);
    infFile.close();
    if (!infFile)
    {
        throw std::runtime_error("Failed to write ETC_WinUsb.inf to temp folder.");
    }

    // Extract ETC_WinUsb.cat to target path
    std::ofstream catFile(etcWinUsbCatPath, std::ios::binary | std::ios::trunc);
    catFile.write(reinterpret_cast<const char *>(ETC_WinUsb_cat_Data), ETC_WinUsb_cat_Length);
    catFile.close();
    if (!catFile)
    {
        throw std::runtime_error("Failed to write ETC_WinUsb.cat to temp folder.");
    }

    // Install driver inf file
    if (!SetupCopyOEMInfW(etcWinUsbInfPath.c_str(), nullptr, SPOST_NONE, 0, nullptr, 0, nullptr, nullptr))
    {
        DWORD errorCode = GetLastError();
        if (errorCode == ERROR_ACCESS_DENIED)
        {
            throw std::runtime_error("Administrator rights are required in order to setup the ETC USB drivers.");
        }
        else
        {
            throw std::runtime_error(std::string("Win32 Error ") + std::to_string(errorCode) + std::string(": ") + CliEngine::GetWin32ErrorMessage(errorCode));
        }
    }

    // Cleanup files
    std::filesystem::remove(etcWinUsbInfPath);
    std::filesystem::remove(etcWinUsbCatPath);

    std::cout << "Successfully installed ETC USB drivers. You may need to reconnect ETC Gadget devices or reboot your PC for newly installed drivers to take effect." << std::endl;

    return;
}
#endif