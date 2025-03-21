#include "windows_udp_transport.h"
#include <iomanip>
#include <memory>
#include <Winsock2.h>
#pragma comment(lib, "ws2_32.lib")

static bool SetSocketNonBlocking(int32_t socketDescriptor);

template <typename T>
void PrintHexDump(const T& container, size_t length = 0) 
{
    const uint8_t* data;

    // Determine the data pointer and length
    if constexpr (std::is_same_v<T, std::vector<uint8_t>>) {
        data = container.data();
        length = container.size();
    } else if constexpr (std::is_same_v<T, std::unique_ptr<char[]>>) {
        data = reinterpret_cast<const uint8_t*>(container.get());
    } else {
        static_assert(std::is_same_v<T, void>, "Unsupported container type");
    }

    // Use the given length if not zero, otherwise process the full vector
    for (size_t i = 0; i < length; ++i) {
        // Print the hex value of the byte
        std::cout << std::hex << std::setw(2) << std::setfill('0') 
                  << static_cast<int>(data[i]) << " ";

        // Optionally, print a newline every 16 bytes for readability
        if ((i + 1) % 16 == 0) {
            std::cout << std::endl;
        }
    }

    // Final newline if the last line didn't end with 16 bytes
    if (length % 16 != 0) {
        std::cout << std::endl;
    }
}

WindowsUdpTransport::WindowsUdpTransport()
{
    _socketDescriptor = INVALID_SOCKET;
}

WindowsUdpTransport::~WindowsUdpTransport()
{
    DeInitialize();
}

bool WindowsUdpTransport::InitializeSocket(const std::string& ipAddress, uint16_t port)
{
    bool retVal = false;
    if (_socketDescriptor == INVALID_SOCKET)
    {
        struct sockaddr_in portAddress;
        WSADATA            wsaData;
        int reuse = 1;

        /* Construct port address structure */
        memset(&portAddress, 0, sizeof(portAddress));
        portAddress.sin_family = AF_INET;
        portAddress.sin_addr.s_addr = INADDR_ANY;
        portAddress.sin_port = htons(port);

        /* Start platform socket library */
        if (0 != WSAStartup(MAKEWORD(2, 2), &wsaData))
        {
            std::cout << "WSAStartup failed\n";
        }
        /* Create socket */
        else if (INVALID_SOCKET == (_socketDescriptor = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)))
        {
            std::cerr << "Failed to create socket. WSA error code: " << WSAGetLastError() << std::endl;
        }

        /* Allow socket reuse */
        /*else if (SOCKET_ERROR == setsockopt(_socketDescriptor, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0) {
            std::cerr << "Failed to set SO_REUSEADDR. WSA error code: " << WSAGetLastError() << std::endl;
        }*/

        /* Bind socket */
        else if (0 != bind(_socketDescriptor, (struct sockaddr*)&portAddress, sizeof(portAddress)))
        {
            std::cerr << "Bind failed. WSA error code: " << WSAGetLastError() << std::endl;
            DeInitialize();
            _socketDescriptor = INVALID_SOCKET;
        }

        else if (false == SetSocketNonBlocking(_socketDescriptor))
        {
            std::cout << "Failed to set socket to non blocking\n";
            DeInitialize();
            _socketDescriptor = INVALID_SOCKET;
        }
        else
        {
            retVal = true;
        }
        std::cout << "Succesfully initialized socket.\n";
    }
    else
    {
        std::cout << "Sockets already initialized!\n";
    }
    return retVal;
}

// Deinitialize the UDP transport layer.
void WindowsUdpTransport::DeInitialize()
{
    _socketDescriptor = closesocket(_socketDescriptor);
}

// Send a message through the UDP transport layer.
int32_t WindowsUdpTransport::TransportSendMessage(const std::string& destIpAddr,
                                                  uint16_t destPort,
                                                  const std::vector<uint8_t>& message)
{
    int retval = -1;
    struct sockaddr_in portAddress;

    /* Populate port address structure */
    memset(&portAddress, 0, sizeof(portAddress));
    portAddress.sin_family = AF_INET;
    portAddress.sin_addr.s_addr = inet_addr(destIpAddr.c_str());
    portAddress.sin_port = htons(destPort);

    // std::cout << "Sending Message:\n";
    // PrintHexDump(message);
    retval = sendto(_socketDescriptor,
                    reinterpret_cast<const char*>(message.data()), // Pointer to raw data
                    static_cast<int>(message.size()),              // Data size
                    0,
                    (const struct sockaddr*)&portAddress,
                    sizeof(portAddress));
    if (0 > retval)
    {
        int errorCode = WSAGetLastError();
        std::cerr << "sendto failed. WSA error code: " << errorCode << std::endl;
    }
    return retval;
}

// Receive a message 
int32_t WindowsUdpTransport::ReceiveMessage(std::unique_ptr<char[]>& inputBuffer,
                                            uint16_t inputBufferSize,
                                            std::string& senderIp,
                                            uint16_t& senderPort)
{
    int retval = -1;
    struct sockaddr_in senderAddress;
    int32_t senderAddressLength = sizeof(struct sockaddr_in);

    if (nullptr == inputBuffer)
    {
        inputBuffer = std::make_unique<char[]>(inputBufferSize);
    }

    retval = recvfrom(_socketDescriptor,
                      inputBuffer.get(),
                      inputBufferSize,
                      0,
                      (struct sockaddr *)&senderAddress,
                      &senderAddressLength);
    if (0 > retval)
    {
        int errorCode = WSAGetLastError();
        std::cerr << "recvfrom failed. WSA error code: " << errorCode << std::endl;
    }
    else
    {
        // std::cout << "Received Message:\n";
        // PrintHexDump(inputBuffer, retval);
        senderIp = std::string(inet_ntoa(senderAddress.sin_addr));
        senderPort = ntohs(senderAddress.sin_port);
    }
    return retval;
}

// Poll the receive socket
bool WindowsUdpTransport::PollReceiveSocket()
{
    if (_socketDescriptor == INVALID_SOCKET) {
        std::cerr << "Receive socket not initialized\n";
        return false;
    }

    fd_set readSet;
    FD_ZERO(&readSet);
    FD_SET(_socketDescriptor, &readSet);

    timeval timeout = {0, 0}; // Non-blocking: immediately return if no data is available

    int result = select(0, &readSet, nullptr, nullptr, &timeout);
    if (result > 0 && FD_ISSET(_socketDescriptor, &readSet)) {
        std::cout << "Received data on the socket\n";
        return true; // Data is available to read
    }

    return false; // No data available
}

static bool SetSocketNonBlocking(int32_t socketDescriptor)
{
    bool retVal = false;
    unsigned long mode = 1;
    if (0 != ioctlsocket(socketDescriptor, FIONBIO, &mode))
    {
        printf("Failed to set udp socket to non blocking mode\n");
    }
    else
    {
        retVal = true;
    }

    return retVal;
}