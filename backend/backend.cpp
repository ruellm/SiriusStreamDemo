#include <map>
#include <sstream>
#include <string.h>

#include "backend.h"

#include "client/IChannel.hpp"
#include "client/IChannel.hpp"
#include "client/MiddlewareSDK.hpp"
#include "client/media/Orientation.hpp"
#include "client/IClientApp.hpp"

using namespace proximax::client;
using namespace proximax::core::signals;

static  std::unique_ptr<IClientApp>             _clientApp;
static  ConnectionPtrList                       _connectionPtrs;
static  std::function<backend::OnChannelEvent>  _onChannelInvite;
static  std::function<backend::OnChannelEvent>  _onChannelCreated;
static  std::function<backend::OnChannelEvent>  _onChannelCreationError;

using   RegistrationMap =                       std::map<std::string, UserRegistrationData>;
static  RegistrationMap                         _registrations;

using   ChannelMap =                            std::map<std::string, IClientApp::ChannelPtr>;
static  ChannelMap                              _channels;

using   MessageReceiverList =                   std::map<std::string, std::function<backend::OnChannelEvent>>;
static  MessageReceiverList                     _messageHandlers;

std::string errorCodeToStr( uint8_t errorCode ) {
    std::ostringstream s;
    s << errorCode;
    return s.str( );
}

namespace backend {

void SetOnChannelInvite(std::function<OnChannelEvent> callback) {
    _onChannelInvite = callback;
}

void SetOnChannelCreated(std::function<OnChannelEvent> callback) {
    _onChannelCreated = callback;
}

void SetOnChannelCreationError(std::function<OnChannelEvent> callback) {
    _onChannelCreationError = callback;
}

void SetMessageHandler(const std::string& identity, std::function<backend::OnChannelEvent> callback) {
    auto iter = _messageHandlers.find(identity);
    if(iter != _messageHandlers.end()) {
        _messageHandlers[identity] = std::move(callback);
        return;
    }

    _messageHandlers.insert( std::make_pair( identity, std::move( callback ) ) );
}

void OnChannelCreated(IClientApp::ChannelPtr channel, const std::string& identity) {

    _connectionPtrs.push_back(channel->registerMessageReceived([=](std::string message){
        auto iter = _messageHandlers.find(identity);
        if(iter == _messageHandlers.end()) {
            return;
        }

        iter->second(message);
    }));

    // call channel created pass
    _onChannelCreated( identity );

    // insert to map
    _channels.insert( std::make_pair( identity, std::move( channel ) ) );
}

void ConfirmChannel(const std::string& identity) {

    _clientApp->confirmChannel(identity,
                               [=](IClientApp::ChannelPtr channel){
        OnChannelCreated(channel, identity);
    },
    [=](ChannelRequestErrorID result){
        std::string error = "Registration failed due to reason: "
                + errorCodeToStr( ( uint8_t ) result );

        _onChannelCreationError( error );

    }, ChannelSecurity::secure);
}

void DenyChannel(const std::string& identity) {
    _clientApp->denyChannel(identity);
}


void Initialize( std::function<OnApplicationReady> onReady ) {

    _clientApp = createClientApp( std::nullopt );

    _connectionPtrs.push_back( _clientApp->registerApplicationReady( onReady ) );

    _connectionPtrs.push_back( _clientApp->registerChannelRequested( _onChannelInvite ) );

    _clientApp->start( );
}

void Register( std::function<void( const std::string& )> onRegisterSuccess,
               std::function<OnErrorEvent> onErrorCallback ) {

    _clientApp->registerUser( [=]( UserRegistrationData regData ) {

        onRegisterSuccess( regData.identity );

        _registrations.insert( std::make_pair( regData.identity, std::move( regData ) ) );

    }, [=]( RegisterFailureCode errorCode ) {

        std::string error = "Registration failed due to reason: " + errorCodeToStr( (uint8_t)errorCode );

        onErrorCallback( error );
    });
}

void Login(const std::string& identity,
           std::function<void(const std::string& )> onSuccess,
           std::function<OnErrorEvent> onError) {

    RegistrationMap::iterator it = _registrations.begin( );

    UserRegistrationData* data = nullptr;
    while( it != _registrations.end( ) ) {
        if( it->first == identity ) {
            data = &it->second;
            break;
        }

        it++;
    }

    if( data ) {
        _clientApp->login( *data, [=]( std::string identity, std::optional<UserRegistrationData> ) {
            onSuccess( identity );
        },
        [=](LoginFailureCode errorCode) {
            std::string error = "Registration failed due to reason: " + errorCodeToStr( ( uint8_t ) errorCode );
            onError(error);
        });
    }else{
        _clientApp->login( UserRegistrationData{}, [=]( std::string identity, std::optional<UserRegistrationData> ) {
            onSuccess( identity );
        },
        [=](LoginFailureCode errorCode) {
            std::string error = "Registration failed due to reason: " + errorCodeToStr( ( uint8_t ) errorCode );
            onError(error);
        });
    }

}

void CreateChannel(const std::string& identity) {

    _clientApp->createChannel(identity,
                              [=](IClientApp::ChannelPtr channel){
        OnChannelCreated(channel, identity);
    },
    [=](ChannelRequestErrorID result){
        std::string error = "Registration failed due to reason: " + errorCodeToStr( ( uint8_t ) result );

        _onChannelCreationError( error );

    }, ChannelSecurity::secure);
}

void SendMessage(const std::string& identity, const std::string& message) {
    auto iter = _channels.begin();

    IClientApp::ChannelPtr ptr = nullptr;
    while(iter != _channels.end()) {
        if(iter->first == identity) {
            ptr = iter->second;
            break;
        }

        iter++;
    }

    if(!ptr)
        return;

    ptr->sendMessage(message);
}

void CreateBroadcastStream(std::function<OnStreamCreated> OnCreated,
                           std::function<OnErrorEvent> onError) {

    VideoStreamParameters param;
    param.width = 640;
    param.height = 360;

    _clientApp->createBroadcastStream(param,
                                      [=](IVideoStreamer::VideoStreamID id) {
        OnCreated(id);
    },
    [=](std::string error) {
        std::string msg = "Error while creating broadcast : " + error;
        onError(msg);
    });
}

void SendVideoFrame(uint8_t* buffer, size_t sz, int type){
    using namespace  proximax::client;
    using namespace proximax::media::sirius_stream;
    using namespace proximax::media::palx_headless;

    // for this demo we are using layer 1 to set the image
    uint8_t* frame_bytes[3] = {0};

    frame_bytes[0] = nullptr;
    frame_bytes[1] = buffer;
    frame_bytes[2] = nullptr;

    int len[3] = {0, (int)sz, 0};

    FrameType acType  = FrameType::VIDEO_IDR;

    if(type == 0)
       acType = FrameType::VIDEO_IDR;
    else if(type == 1)
       acType = FrameType::VIDEO_I;
    else
       acType = FrameType::VIDEO_P;

    IClientApp::SharedConstFrame frame =
            std::make_shared<Frame>(acType,
                                    Frame::Clock::now(),
                                    0,
                                    Orientation::rotate0,
                                    std::move(len),
                                    std::move(frame_bytes)); // bytes


    _clientApp->sendVideoFrame(frame);
}

void CreateViewerStream(std::string streamId,
                        std::function<OnStreamCreated> onCreated,
                        std::function<OnFrameReceived> onFrameReceived,
                        std::function<OnErrorEvent> onError) {

    _clientApp->createViewerStream(streamId,
        [=](IVideoStreamer::VideoStreamID id){

            _connectionPtrs.push_back(_clientApp->registerVideoFrameReceived(id,
                [=](IClientApp::SharedConstFrame frame ){

                MediaFrame mframe;

                mframe.bytes = new uint8_t[frame->bytes[1].size()];
                mframe.bufferSize = frame->bytes[1].size();
                for(size_t i = 0; i < mframe.bufferSize; i++) {
                    mframe.bytes[i] = frame->bytes[1][i];
                }

                mframe.timestamp = frame->timestamp;
                mframe.type = (MediaFrame::Type)((uint8_t)frame->frameType);

                onFrameReceived(mframe);
             }));

              _connectionPtrs.push_back(_clientApp->registerAudioFrameReceived(id,
                                    [=](IClientApp::SharedConstFrame frame ){
                //TODO: support audio
              }));

              onCreated(id);
        },
        [=](std::string errorMsg) {
            std::string error = "Create viewer stream failed with error: " + errorMsg;
            onError( error );
    });
}

}
