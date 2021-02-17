#pragma once

#include "Enums.hpp"
#include "IChannel.hpp"
#include "IVideoStreamer.hpp"

#include "media/Frame.hpp"

#include <optional>
#include <string>

namespace proximax::routing
{
struct XPXNode;

namespace circuit
{
class CircuitException;
}

namespace stream
{
class StreamManager;
}
}

namespace proximax::routing::video {
    class VideoStream;
}

namespace proximax::client
{

class Client;
class IClient;

/// Data returned when a user registers. This should be stored securely by the client
/// SDK
struct UserRegistrationData
{
    using ClientIdentity = std::string;
    using Ed25519SecretKeyString = std::string;
    using RawCertificate = std::vector<uint8_t>;

    /// Identity of the user. Generally represented in the following way:
    ///     peerstream.client.account.XzNvt72twcU4fzYfGMnDccfkZKkURQQJFBxS9xCtyUdSyaKFG
    /// Consists of the following parts:
    ///     - namespace
    ///     - sub namespace
    ///     - identity/node type (only client will be valid for this)
    ///     - base58 hash of public signing key along with checksum
    ClientIdentity identity{};

    /// Private Ed25519 master key
    Ed25519SecretKeyString masterSecKey{};

    /// Private Ed25519 signing key
    Ed25519SecretKeyString signingSecKey{};

    /// Certificate created upon registration
    RawCertificate cert{};
};

class UserRegistrationDataBuilder
{
public:
    UserRegistrationDataBuilder& identity(UserRegistrationData::ClientIdentity rhs)
    {
        value_.identity = std::move(rhs);
        return *this;
    }

    UserRegistrationDataBuilder& masterSecKey(
        UserRegistrationData::Ed25519SecretKeyString rhs)
    {
        value_.masterSecKey = std::move(rhs);
        return *this;
    }

    UserRegistrationDataBuilder& signingSecKey(
        UserRegistrationData::Ed25519SecretKeyString rhs)
    {
        value_.signingSecKey = std::move(rhs);
        return *this;
    }

    UserRegistrationDataBuilder& cert(UserRegistrationData::RawCertificate rhs)
    {
        value_.cert = std::move(rhs);
        return *this;
    }

    UserRegistrationData build()
    {
        return std::move(value_);
    }

private:
    UserRegistrationData value_;
};

/// Allows you to set the security mode for announcing presence. Insecure uses one hop
/// circuits, while secure mode uses the full three hops to maintain anonymity.
enum class PS_SDK_NODISCARD PresenceSecurityMode : uint8_t
{
    insecure = 0,
    secure
};

struct NetworkStatus
{
    float networkConnectivity;
    size_t numEntryNodesConnected;
};

class NetworkStatusBuilder
{
public:
    NetworkStatusBuilder& networkConnectivity(float rhs)
    {
        value_.networkConnectivity = std::move(rhs);
        return *this;
    }

    NetworkStatusBuilder& numEntryNodesConnected(size_t rhs)
    {
        value_.numEntryNodesConnected = std::move(rhs);
        return *this;
    }

    NetworkStatus build()
    {
        return std::move(value_);
    }

private:
    NetworkStatus value_;
};

enum class PS_SDK_NODISCARD LogLevel : uint8_t
{
    trace = 0,
    debug,
    info,
    warning,
    error,
    fatal,
};

std::ostream& operator<<(std::ostream& stream, const LogLevel& rhs);

enum class PS_SDK_NODISCARD AppExitCode : uint8_t
{
    /// The application exited successfully
    success = 0,

    /// The application is exiting due to a generic failure
    fatal,

    /// The application is exiting due to a network error
    networkError,

    /// The application is exiting due to a failure in the discovery subsystem
    discoveryFailure,

    /// The application is exiting due to a failure with presence announcements subsystem
    presenceFailure,

    /// The application is exiting due to failures with the channel subsystem
    channelFailure,

    /// The application is exiting due to a failure with registering a new user
    registrationFailure,

    /// The application is exiting due to a failure to login
    loginFailure,
};

std::ostream& operator<<(std::ostream& stream, const AppExitCode& rhs);

struct BootstrapNode
{
    /// Fingerprint of node, as a hex string
    std::string fingerprint;

    /// Handshake key of node, as a hex string
    std::string handshakeKey;

    /// Vector of addresses of node, as a host:port string
    ///     e.g.: discovery:6996
    std::vector<std::string> addresses;

    /// Identity of node in PKI Identity format
    /// e.g.: namespace.sub-namespace.type.base58Hash
    std::string identity;

    /// Mode of this node. Possible current values:
    /// client, onion, discovery, auth, oracle
    std::string mode;
};

class BootstrapNodeBuilder
{
public:
    BootstrapNodeBuilder& fingerprint(std::string rhs)
    {
        value_.fingerprint = std::move(rhs);
        return *this;
    }

    BootstrapNodeBuilder& handshakeKey(std::string rhs)
    {
        value_.handshakeKey = std::move(rhs);
        return *this;
    }

    BootstrapNodeBuilder& addresses(std::vector<std::string> rhs)
    {
        value_.addresses = std::move(rhs);
        return *this;
    }

    BootstrapNodeBuilder& identity(std::string rhs)
    {
        value_.identity = std::move(rhs);
        return *this;
    }

    BootstrapNodeBuilder& mode(std::string rhs)
    {
        value_.mode = std::move(rhs);
        return *this;
    }

    BootstrapNode build()
    {
        return std::move(value_);
    }

private:
    BootstrapNode value_;
};

/// Initialization parameters for SDK
struct SDKInitializationParameters
{
    /// The namespace to use for application identity
    std::string appIdentityNamespace{};

    /// The sub-namespace to use for application sub-identity
    std::string appIdentitySubNamespace{};

    /// Add additional bootstrap nodes through SDK registration
    std::vector<BootstrapNode> bootstrapNodes{};
};

class SDKInitializationParametersBuilder
{
public:
    SDKInitializationParametersBuilder& appIdentityNamespace(std::string rhs)
    {
        value_.appIdentityNamespace = std::move(rhs);
        return *this;
    }

    SDKInitializationParametersBuilder& appIdentitySubNamespace(std::string rhs)
    {
        value_.appIdentitySubNamespace = std::move(rhs);
        return *this;
    }

    SDKInitializationParametersBuilder& bootstrapNodes(std::vector<BootstrapNode> rhs)
    {
        value_.bootstrapNodes = std::move(rhs);
        return *this;
    }

    SDKInitializationParameters build()
    {
        return std::move(value_);
    }

private:
    SDKInitializationParameters value_;
};

struct KeyPair
{
    std::string publicKey;
    std::string privateKey;
};

/// Main entry point of a client application to interface with the onion-routed network,
/// while providing a higher-level abstraction, so most of the details are hidden from the
/// application itself.
class IClientApp
{
public:
    using KeyPair = proximax::client::KeyPair;

    using ChannelPtr = std::shared_ptr<IChannel>;

    using OnApplicationReady = void();
    using OnApplicationExit = void(AppExitCode);

    using OnChannelRequested = void(std::string);
    using OnNetworkStatus = void(NetworkStatus);
    using OnUserPresenceChange = void(bool);
    using OnChannelConfirmed = void(ChannelPtr);
    using OnChannelRequestError = void(ChannelRequestErrorID);

    using OnRegisterSuccess = void(UserRegistrationData);
    using OnRegisterFailure = void(RegisterFailureCode);
    using OnUserLoaded = void(std::string);

    using OnLoginSuccess = void(std::string, std::optional<UserRegistrationData>);
    using OnLoginFailure = void(LoginFailureCode);

    using OnLookupResult = void(bool);

    using VideoStreamID = std::string;
    using Frame = media::sirius_stream::Frame;
    using FrameType = media::sirius_stream::FrameType;
    using SharedConstFrame = std::shared_ptr<Frame>;
    using OnFrameReceived = void(SharedConstFrame);
    using OnViewersChanged = void(size_t);

    using Action = void();

    virtual ~IClientApp()
    {
    }

    /// Call this to start the client app
    virtual void start() = 0;

    /// Call this to stop the client app
    virtual void stop() = 0;

    virtual void setMinLogLevel(const LogLevel& level) = 0;

    /// Set to true to enable STD output, set to false to disable
    virtual void setStdoutLoggingEnabled(bool isEnabled) = 0;

    // TODO: Allow insecure mode, where each circuit has one hop instead of three.

    /// Method to execute an action on IClientApp's processing task, which ensures that
    /// all operations occur in a serial manner
    /// @param action
    virtual void executeAsync(std::function<Action> action) = 0;

    /// Helper method for logging using SDK logging
    /// @param componentName The name of the component to log from
    /// @param file The file of the log location
    /// @param function The function of the log location
    /// @param line The line of the log location
    /// @param level The log level we're logging at
    /// @param msg The log message
    virtual void log(const std::string& componentName,
                     const LogLevel& level,
                     const char* file,
                     const char* function,
                     size_t line,
                     const std::string& msg) = 0;

    /// Method to register a new user.
    /// NOTE: If the user is already logged in, onRegisterFailure() will be called with a
    /// code to indicate the attempt to register an additional user while logged in.
    /// @param onRegisterSucceeded Callback for when registration is successful
    /// @param onRegisterFailure Callback for when registration fails
    virtual void registerUser(std::function<OnRegisterSuccess> onRegisterSucceeded,
                              std::function<OnRegisterFailure> onRegisterFailure) = 0;

    /// Log in a user with the underlying authentication system. This loads a user from
    /// the specified user registration data structure and verifies their identity with
    /// the system, along with a valid certificate.
    /// NOTE: If the user is already logged in and the attempt is to log in the same user,
    /// onLoginSucceeded() is called back and no further processing occurs. NOTE: If the
    /// user is already logged in adn the attempt is to log in a different user,
    /// onLoginFailure() will be called back with an appropriate code to indicate the
    /// attempt to log in a different user while already logged in.
    /// @param userRegData Registration data for the user
    /// @param onLoginSucceeded Callback for when login is successful
    /// @param onLoginFailure Callback for when login fails
    virtual void login(UserRegistrationData userRegData,
                       std::function<OnLoginSuccess> onLoginSucceeded,
                       std::function<OnLoginFailure> onLoginFailure) = 0;

    /// Retrieve the identity of the user
    virtual std::string identity() const = 0;

#if 0
    /// Retrieve the public signing key of the user
    virtual Ed25519PublicKeyString signingKeyPublic() = 0;

    /// Retrieve the public signing key of the user
    virtual Ed25519PrivateKeyString signingKeyPrivate() = 0;
#endif

    /// Retrieve interface to video streamer, used if broadcasting video to the psp
    /// network
//    virtual IVideoStreamer& videoStreamer() = 0;

    /// Use this to create a new channel to the specified user. The user will either
    /// respond by creating the channel or it could fail by one of the mechanisms
    /// described in the ChannelRequestErrorID enum.
    /// @param userId of the User that we would like to create a channel with
    /// @param onChannelConfirmed Callback for when a channel is created
    /// @param onError Callback for when channel creation fails due to a reason specified
    /// in the enum
    /// @param security The security of the channel (relates to number of hops for
    /// underlying circuits)
    virtual void createChannel(const std::string& userId,
                               std::function<OnChannelConfirmed> onChannelConfirmed,
                               std::function<OnChannelRequestError> onError,
                               ChannelSecurity security) = 0;

    /// Use this to confirm a requested channel to the specified user. The user has
    /// already indicated their desire to communicate with you, but it can still fail due
    /// to network errors, disconnections, etc... If it succeeds, you will receive a
    /// callback, and if it fails, there is another callback to handle this.
    /// @param userId of the User that we would like to confirm a channel with
    /// @param onChannelConfirmed Callback for when a channel is created
    /// @param onError Callback for when channel creation fails due to a reason specified
    /// in the enum
    /// @param security The security of the channel (relates to number of hops for
    /// underlying circuits)
    virtual void confirmChannel(const std::string& userId,
                                std::function<OnChannelConfirmed> onChannelConfirmed,
                                std::function<OnChannelRequestError> onError,
                                ChannelSecurity security) = 0;

    /// Use this method to deny a channel request to a specific user. If any network
    /// failures occur, they are transparent to the client.
    /// @param userId The user ID that we are denying
    virtual void denyChannel(const std::string& userId) = 0;

    virtual KeyPair generateKeyPair() const = 0;

    /// Use this to register for a notification that the application is fully started
    virtual core::signals::ConnectionPtr registerApplicationReady(
        std::function<OnApplicationReady> handler) = 0;

    // For someone else initiating with you:
    // registerChannelRequested() ->
    //      onChannelRequested(userId) callback
    //          ->    A. confirmChannel(userId, [](channelPtr) { ... do something with the
    //          channel ...})
    //          -> or B. denyChannel(userId)

    /// @param handler The handler to call back when network status is available
    virtual core::signals::ConnectionPtr registerChannelRequested
    PS_SDK_NODISCARD(std::function<OnChannelRequested> handler) = 0;

    /// Register for network status - e.g.: Number of entry nodes online, and other
    /// statistics?
    /// @param handler The handler to call back when network status is available
    virtual core::signals::ConnectionPtr registerNetworkStatus
    PS_SDK_NODISCARD(std::function<OnNetworkStatus> handler) = 0;

    /// Register for an event to determine the current user presence. Will call back with
    /// initial presence at time of request and presence as it changes
    /// @param The user who you'd like to monitor the presence of
    /// @param handler The handler to call back upon presence changes
    virtual core::signals::ConnectionPtr registerUserPresenceChange PS_SDK_NODISCARD(
        const std::string& userId, std::function<void(bool isActive)> handler) = 0;

    /// Register for an event when general SDK error happened
    /// @param handler The handler to call back upon error happened
    virtual core::signals::ConnectionPtr registerApplicationExit
    PS_SDK_NODISCARD(std::function<OnApplicationExit> handler) = 0;


#pragma region Video stream
    virtual void createBroadcastStream(VideoStreamParameters videoStreamParameters,
                                       std::function<void(VideoStreamID)> onStarted,
                                       std::function<void(std::string)> onError) = 0;

    virtual void createViewerStream( const std::string& token,
                                     std::function<void(VideoStreamID)> onStarted,
                                     std::function<void(std::string)> onError) = 0;

    virtual void sendVideoFrame(SharedConstFrame frame) = 0;

    virtual void sendAudioFrame(SharedConstFrame frame) = 0;

    virtual core::signals::ConnectionPtr registerViewersChanged(
            std::function<OnViewersChanged> onViewersChanged) = 0;

    virtual core::signals::ConnectionPtr registerVideoFrameReceived(
            VideoStreamID videoStream,
            std::function<OnFrameReceived> onFrameReceived) = 0;

    virtual core::signals::ConnectionPtr registerAudioFrameReceived(
            VideoStreamID videoStream,
            std::function<OnFrameReceived> onFrameReceived) = 0;

#pragma endregion

    };
}
