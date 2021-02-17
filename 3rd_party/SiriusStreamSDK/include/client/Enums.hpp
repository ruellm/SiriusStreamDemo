#pragma once

#include <cstdint>
#include <iosfwd>

#if !defined(PS_SDK_NODISCARD)
/// Define to prevent formatting issues by clang-format since it doesn't handle C++
/// attributes
#if !defined(SWIG)
#define PS_SDK_NODISCARD [[nodiscard]]
#else
#define PS_SDK_NODISCARD
#endif
#endif

namespace proximax::client
{

enum class PS_SDK_NODISCARD RegisterFailureCode : uint8_t
{
    success = 0,

    /// This error indicates a connection issue with registering the user
    connectionError,

    /// This error indicates an issue with the format of the certificate
    certFormatError,

    /// This error indicates that the certificate has an unsupported version
    certUnsupportedVersion,

    /// This error indicates that the certificate has an invalid signature
    certSignatureInvalid,

    /// This error indicates that the certificate request has an internal error
    certInternalError,

    /// This error indicates the user is already logged in, which prohibits calling
    /// register again
    alreadyLoggedIn,
};

std::ostream& operator<<(std::ostream& stream, const RegisterFailureCode& code);

enum class PS_SDK_NODISCARD LoginFailureCode : uint8_t
{
    success = 0,

    /// This error indicates a connection issue with verifying the user
    connectionError,

    /// This code indicates the user is not registered with the blockchain
    notRegistered,

    /// This code indicates the user does not have a valid certificate
    certificateInvalid,

    /// This error indicates the user is already logged in under a different identity,
    /// which prohibits calling login again if the identity is different
    loggedInDiffIdentity,
};

std::ostream& operator<<(std::ostream& stream, const LoginFailureCode& code);

/// Various identity types supported
enum class PS_SDK_NODISCARD IdentityType : uint8_t
{
    account = 1,
    node = 2
};

std::ostream& operator<<(std::ostream& stream, const IdentityType& rhs);
std::istream& operator>>(std::istream& stream, IdentityType& rhs);

/// The format of the PKI suffix
enum class PS_SDK_NODISCARD PkiSuffixFormat
{
    hex,
    base58
};

}
