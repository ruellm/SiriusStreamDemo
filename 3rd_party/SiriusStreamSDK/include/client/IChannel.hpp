#pragma once

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#if !defined(PS_SDK_NODISCARD)
/// Define to prevent formatting issues by clang-format since it doesn't handle C++
/// attributes
#if !defined(SWIG)
#define PS_SDK_NODISCARD [[nodiscard]]
#else
#define PS_SDK_NODISCARD
#endif
#endif

namespace proximax::core::signals
{
// Forward declaration which allows not exposing this functionality through the SDK
class ScopedConnection;

/// Alias for a shared_ptr of a connection. This is used in the SDK so that it doesn't
/// require the definition of a connection in order to perform de-registration by
/// destroying the object
using ConnectionPtr = std::shared_ptr<ScopedConnection>;

/// Alias for a vector of connection pointers, which allows holding multiple connections
/// per pointer
using ConnectionPtrList = std::vector<ConnectionPtr>;

/// Alias for a map of connection pointers vectors, which allows holding multiple
/// connections per pointer per key
template <class TKey>
using ConnectionMapPtrList = std::unordered_map<TKey, ConnectionPtrList>;
}

namespace proximax::client
{

/// Enum that defines errors related to channel request
enum class PS_SDK_NODISCARD ChannelRequestErrorID : uint8_t
{
    success = 0,
    channelDenied,
    channelTimeout,
    userNotFound,
    connectionFailure,
    failure,
    channelAlreadyExists,
    invalidConfirm,
};

std::ostream& operator<<(std::ostream& stream, const ChannelRequestErrorID& rhs);

/// This occurs when a channel drops
enum class PS_SDK_NODISCARD ChannelErrorID : uint8_t
{
    userDisconnected = 0,
    channelTimeout,
    connectionFailure,
    rateLimitExceeded,
    channelClosed,
    channelNotConfirmed,
    invalidConfirm,
    invalidDeny,
};

std::ostream& operator<<(std::ostream& stream, const ChannelErrorID& rhs);

/// This enum is used to determine the security to the network, which ultimately
/// translates to the number of hops when creating a circuit
enum class PS_SDK_NODISCARD ChannelSecurity : uint8_t
{
    /// This mechanism creates secure circuits, which means 3 hops for all circuits,
    /// making IP and who is talking to who effectively untraceable
    secure = 0,

    /// This mechanism uses only a single hop to a circuit, which means hop for all
    /// circuits, thus exposing the IP of the caller, so if this node were compromised, it
    /// is potentially traceable.
    insecure
};

std::ostream& operator<<(std::ostream& stream, const ChannelSecurity& rhs);

enum class PS_SDK_NODISCARD VideoStreamErrorID : uint8_t
{
    success = 0,

    /// General connection failure
    connectionFailure,

    /// This code is used only for 1:1 channel communications and occurs when requesting
    /// the stream of a partner and the partner then denies this request
    partnerDenied,

    /// This code is used only for 1:1 channel communications and occurs when the partner
    /// rejected viewing your stream, after sharing
    partnerRejected,

    /// This code is used only for 1:1 channel communications and occurs when trying to
    /// share a stream that isn't currently available.
    notCurrentlyStreaming,

    /// This code is used only for 1:1 channel communications and occurs when sending a
    /// duplicate  or unitiated request to share stream. You can only share stream once
    /// per channel without confirmation to avoid flooding.
    invalidShare,

    /// This code is used only for 1:1 channel communications and occurs when sending a
    /// duplicate or unitiated request to request stream. You can only request stream once
    /// per channel without confirmation to avoid flooding.
    invalidRequest,

    /// This code is used only for 1:1 channel communications and occurs when trying to
    /// share or request a stream with a partner who you don't have a confirmed channel
    /// with
    channelNotConfirmed,

    /// This code is used only for 1:1 channel communications and occurs when you confirm
    /// a stream that hasn't been shared with you
    streamNotShared,
};

std::ostream& operator<<(std::ostream& stream, const VideoStreamErrorID& rhs);

using Image = std::vector<uint8_t>;

/// A channel represents a conceptual (but indirect) connection to another client. It is a
/// conceptual wrapper around a group of circuits, where underneath the hood, a client
/// creates a circuit which ends in a rendezvous node.
class IChannel
{
public:
    using VideoStreamID = std::string;
    using ChannelPtr = std::shared_ptr<IChannel>;

    using OnRawReceived = void(std::vector<uint8_t>);
    using OnMessageReceived = void(std::string);
    using OnChannelError = void(ChannelErrorID);

    using OnVideoStreamCreated = void(VideoStreamID);
    using OnVideoStreamError = void(VideoStreamErrorID);
    using OnVideoStreamRequested = void();
    using OnVideoStreamShared = void(VideoStreamID);
    using OnVideoStreamDenied = void();

    using OnChannelConfirmed = void(ChannelPtr);
    using OnChannelRequestError = void(ChannelRequestErrorID);

    virtual ~IChannel()
    {
    }

    virtual void sendRawData(const std::vector<uint8_t>& rawData) = 0;

    virtual void sendMessage(const std::string& msg) = 0;

    virtual void closeChannel() = 0;

    virtual void confirmChannel(std::function<OnChannelConfirmed> onChannelConfirmed,
                                std::function<OnChannelRequestError> onError) = 0;

    virtual void denyChannel() = 0;

    /// Method to share your stream with another user
    /// @param onCreate Called when a stream is successfully created
    /// @param onError called when the stream creation fails
    virtual void shareStream(std::function<OnVideoStreamCreated> onCreate,
                             std::function<OnVideoStreamError> onError) = 0;

    /// Method to request to view the stream of another user
    /// @param onCreate Called when a stream is successfully created
    /// @param onError called when the stream creation fails
    virtual void requestStream(std::function<OnVideoStreamCreated> onCreate,
                               std::function<OnVideoStreamError> onError) = 0;

    /// Method to confirm the other user from viewing your stream after their request
    /// @param onCreate Called when a stream is successfully created
    /// @param onError called when the stream creation fails
    virtual void confirmVideoStreamRequest(std::function<OnVideoStreamCreated> onCreate,
                                           std::function<OnVideoStreamError> onError) = 0;

    /// Method to deny a user from viewing your stream after their request
    virtual void denyVideoStreamRequest() = 0;

    /// Method to confirm viewing of the other user's stream that has been shared with you
    /// @param onCreate Called when a stream is successfully created
    /// @param onError called when the stream creation fails
    virtual void confirmVideoStreamShare(std::function<OnVideoStreamCreated> onCreate,
                                         std::function<OnVideoStreamError> onError) = 0;

    /// Method to decline viewing of the other user's stream that has been shared with you
    virtual void denyVideoStreamShare() = 0;

    /// Method to stop viewing the other user's stream (if you're already viewing)
    virtual void stopViewingStream() = 0;

    /// Method to retrieve the identity of the user on the channel
    virtual std::string identity() const = 0;

    /// Method to retrieve whether or not the channel is confirmed
    virtual bool isConfirmed() const = 0;

    /// Register for raw data received notification
    /// @param handler The handler to call back when raw data is received
    virtual core::signals::ConnectionPtr registerRawDataReceived
    PS_SDK_NODISCARD(std::function<OnRawReceived> handler) = 0;

    /// Register for a message received notification
    /// @param handler The handler to call back when a message is received
    virtual core::signals::ConnectionPtr registerMessageReceived
    PS_SDK_NODISCARD(std::function<OnMessageReceived> handler) = 0;

    /// Register for a channel error notification
    /// @param handler The handler to call back when a channel errors out
    virtual core::signals::ConnectionPtr registerChannelError
    PS_SDK_NODISCARD(std::function<OnChannelError> handler) = 0;

    /// Register for a notification of a video stream that has been requested
    /// @param handler The handler to call back when a stream is requested
    virtual core::signals::ConnectionPtr registerVideoStreamRequested
    PS_SDK_NODISCARD(std::function<OnVideoStreamRequested> handler) = 0;

    /// Register for a notification of a video stream that has been shared
    /// @param handler The handler to call back when a stream is requested
    virtual core::signals::ConnectionPtr registerVideoStreamShared
    PS_SDK_NODISCARD(std::function<OnVideoStreamShared> handler) = 0;
};

}
