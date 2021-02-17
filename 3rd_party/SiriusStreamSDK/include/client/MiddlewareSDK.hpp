#pragma once

#include "IClientApp.hpp"

#if USING_CLIENT_TEST_SDK
#include <client/test/TestConfig.hpp>
#endif

#include <functional>
#include <memory>

#if !defined(MIDDLEWARE_SDK_EXPORT)
#if defined(_MSC_VER)
#define MIDDLEWARE_SDK_EXPORT __declspec(dllimport)
#else
#define MIDDLEWARE_SDK_EXPORT __attribute((visibility("default")))
#endif
#endif

namespace proximax::client
{

namespace test
{
struct TestConfig;
}

#if !defined(SWIG)
/// SDK entry point factory method to create a client application
/// @param params The SDK initialization parameters used for configuring the application
/// @returns A client application
MIDDLEWARE_SDK_EXPORT std::unique_ptr<IClientApp> createClientApp(
    std::optional<SDKInitializationParameters> params);

/// SDK entry point factory method to create a client application
/// @param params The SDK initialization parameters used for configuring the application
/// @returns A client application
MIDDLEWARE_SDK_EXPORT std::shared_ptr<IClientApp> createClientAppShared(
    SDKInitializationParameters* params);

/// SDK entry point factory method to create a test client application
/// @param params The SDK initialization parameters used for configuring the test app
/// @param config A test config used for configuring how the test application should
/// behave
/// @returns A test client application
MIDDLEWARE_SDK_EXPORT std::unique_ptr<IClientApp> createTestClientApp(
    std::optional<SDKInitializationParameters>, const test::TestConfig& config);
#else
/// SDK entry point factory method to create a client application
/// @param params The SDK initialization parameters used for configuring the application
/// @returns A client application
extern std::shared_ptr<IClientApp> createClientAppShared(
    SDKInitializationParameters* params);
#endif

}
