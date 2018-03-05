/*
 * Copyright (c) 2017 Axel Isouard <axel@isouard.fr>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <cstring>
#include <memory>
#include <iostream>
#include <webrtc/api/test/fakeconstraints.h>
#include "common.h"
#include "globals.h"
#include "observer/createsessiondescriptionobserver.h"
#include "observer/setsessiondescriptionobserver.h"
#include "observer/peerconnectionobserver.h"
#include "rtccertificate.h"
#include "rtcpeerconnection.h"
#include "rtcsessiondescription.h"
#include "rtcdatachannel.h"
#include "rtcmediastream.h"

Nan::Persistent<FunctionTemplate> RTCPeerConnection::constructor;

static const char sRTCPeerConnection[] = "RTCPeerConnection";

static const char kCreateOffer[] = "createOffer";
static const char kCreateAnswer[] = "createAnswer";
static const char kSetLocalDescription[] = "setLocalDescription";
static const char kSetRemoteDescription[] = "setRemoteDescription";
static const char kCreateDataChannel[] = "createDataChannel";
static const char kAddStream[] = "addStream";
static const char kGenerateCertificate[] = "generateCertificate";

static const char kIceServers[] = "iceServers";
static const char kIceServerUrls[] = "urls";
static const char kCertificates[] = "certificates";

static const char kName[] = "name";
static const char kRSA[] = "RSASSA-PKCS1-v1_5";
static const char kHash[] = "hash";
static const char kSHA256[] = "SHA-256";
static const char kModulusLength[] = "modulusLength";
static const char kPublicExponent[] = "publicExponent";
static const char kECDSA[] = "ECDSA";
static const char kNamedCurve[] = "namedCurve";
static const char kP256[] = "P-256";

static const char kConnectionState[] = "connectionState";
static const char kCurrentLocalDescription[] = "currentLocalDescription";
static const char kCurrentRemoteDescription[] = "currentRemoteDescription";
static const char kLocalDescription[] = "localDescription";
static const char kRemoteDescription[] = "remoteDescription";
static const char kIceConnectionState[] = "iceConnectionState";
static const char kIceGatheringState[] = "iceGatheringState";
static const char kPendingLocalDescription[] = "pendingLocalDescription";
static const char kPendingRemoteDescription[] = "pendingRemoteDescription";
static const char kSignalingState[] = "signalingState";

static const char kNew[] = "new";
static const char kChecking[] = "checking";
static const char kConnected[] = "connected";
static const char kCompleted[] = "completed";
static const char kFailed[] = "failed";
static const char kDisconnected[] = "disconnected";
static const char kClosed[] = "closed";
static const char kUnknown[] = "unknown";

static const char kStable[] = "stable";
static const char kHaveLocalOffer[] = "have-local-offer";
static const char kHaveLocalPranswer[] = "have-local-pranswer";
static const char kHaveRemoteOffer[] = "have-remote-offer";
static const char kHaveRemotePranswer[] = "have-remote-pranswer";

static const char kIceRestart[] = "iceRestart";

static const char eCurve[] = "EcKeyGenParams: Unrecognized namedCurve";
static const char eHash[] = "Algorithm: Unrecognized hash";
static const char eName[] = "Algorithm: Unrecognized name";
static const char eUnsupported[] = "The 1st argument provided is an "
    "AlgorithmIdentifier with a supported algorithm name, but the parameters "
    "are not supported.";

static const char eGenerateCertificateFailure[] = 
    "Failed to generate the certificate.";
static const char eConstructorFailure[] =
    "Failed to construct 'RTCPeerConnection'";

NAN_MODULE_INIT(RTCPeerConnection::Init) {
  Local<FunctionTemplate> ctor = Nan::New<FunctionTemplate>(New);
  ctor->SetClassName(LOCAL_STRING(sRTCPeerConnection));
  ctor->InstanceTemplate()->SetInternalFieldCount(1);

  Nan::SetMethod(ctor, kGenerateCertificate, GenerateCertificate);

  Local<ObjectTemplate> prototype = ctor->InstanceTemplate();
  Nan::SetMethod(prototype, kCreateOffer, CreateOffer);
  Nan::SetMethod(prototype, kCreateAnswer, CreateAnswer);
  Nan::SetMethod(prototype, kSetLocalDescription, SetLocalDescription);
  Nan::SetMethod(prototype, kSetRemoteDescription, SetRemoteDescription);
  Nan::SetMethod(prototype, kCreateDataChannel, CreateDataChannel);
  Nan::SetMethod(prototype, kAddStream, AddStream);

  Local<ObjectTemplate> tpl = ctor->InstanceTemplate();
  Nan::SetAccessor(tpl, LOCAL_STRING(kConnectionState),
                   GetConnectionState);
  Nan::SetAccessor(tpl, LOCAL_STRING(kCurrentLocalDescription),
                   GetCurrentLocalDescription);
  Nan::SetAccessor(tpl, LOCAL_STRING(kCurrentRemoteDescription),
                   GetCurrentRemoteDescription);
  Nan::SetAccessor(tpl, LOCAL_STRING(kLocalDescription),
                   GetLocalDescription);
  Nan::SetAccessor(tpl, LOCAL_STRING(kRemoteDescription),
                   GetRemoteDescription);
  Nan::SetAccessor(tpl, LOCAL_STRING(kIceConnectionState),
                   GetIceConnectionState);
  Nan::SetAccessor(tpl, LOCAL_STRING(kIceGatheringState),
                   GetIceGatheringState);
  Nan::SetAccessor(tpl, LOCAL_STRING(kPendingLocalDescription),
                   GetPendingLocalDescription);
  Nan::SetAccessor(tpl, LOCAL_STRING(kPendingRemoteDescription),
                   GetPendingRemoteDescription);
  Nan::SetAccessor(tpl, LOCAL_STRING(kSignalingState),
                   GetSignalingState);

  constructor.Reset(ctor);
  Nan::Set(target, LOCAL_STRING(sRTCPeerConnection), ctor->GetFunction());
}

RTCPeerConnection::RTCPeerConnection(
    const webrtc::PeerConnectionInterface::RTCConfiguration& config) {

  _peerConnectionFactory = webrtc::CreatePeerConnectionFactory(
      Globals::GetSignalingThread(), Globals::GetWorkerThread(),
      NULL, NULL, NULL);

  _peerConnectionObserver = PeerConnectionObserver::Create();
  _peerConnection = _peerConnectionFactory->CreatePeerConnection(
          config, NULL, NULL, _peerConnectionObserver);
}

RTCPeerConnection::~RTCPeerConnection() {
  _peerConnection = NULL;
  _peerConnectionObserver = NULL;
  _peerConnectionFactory = NULL;
}

NAN_METHOD(RTCPeerConnection::New) {
  CONSTRUCTOR_HEADER("RTCPeerConnection")
  webrtc::FakeConstraints constraints;
  webrtc::PeerConnectionInterface::RTCConfiguration _config;

  if (info.Length() > 0) {
    ASSERT_OBJECT_ARGUMENT(0, config);

    DECLARE_OBJECT_PROPERTY(config, kIceServers, iceServersVal);

    if (!iceServersVal->IsNull() && !iceServersVal->IsUndefined()) {
      ASSERT_PROPERTY_ARRAY(kIceServers, iceServersVal, iceServers);

      for (unsigned int i = 0; i < iceServers->Length(); i = i + 1) {
        Local<Value> iceServerVal = iceServers->Get(i);
        ASSERT_PROPERTY_OBJECT(kIceServers, iceServerVal, iceServer);

        webrtc::PeerConnectionInterface::IceServer server;

        DECLARE_OBJECT_PROPERTY(iceServer, kIceServerUrls, iceServerUrlsVal);
        ASSERT_PROPERTY_ARRAY(kIceServerUrls, iceServerUrlsVal, iceServerUrls);

        for (unsigned int j = 0; j < iceServerUrls->Length(); j = j + 1) {
          Local<Value> iceServerUrlVal = iceServerUrls->Get(j);
          // FIXME: validate URL
          ASSERT_PROPERTY_STRING(kIceServerUrls, iceServerUrlVal, iceServerUrl);
          server.urls.push_back(*iceServerUrl);
          // FIXME: add username / password for TURN servers
        }
        _config.servers.push_back(server);
      }
    }

    DECLARE_OBJECT_PROPERTY(config, kCertificates, certificatesVal);

    if (!certificatesVal->IsNull() && !certificatesVal->IsUndefined()) {
      ASSERT_PROPERTY_ARRAY(kCertificates, certificatesVal, certificates);

      for (unsigned int i = 0; i < certificates->Length(); i = i + 1) {
        Local<Value> certificateVal = certificates->Get(i);
        ASSERT_PROPERTY_OBJECT(kCertificates, certificateVal, certificate);
        // FIXME: validate it's a RTCCertificate object
        RTCCertificate* _certificate
          = Nan::ObjectWrap::Unwrap<RTCCertificate>(certificate);

        _config.certificates.push_back(_certificate->_certificate);
      }
    }
  }

  RTCPeerConnection *rtcPeerConnection = new RTCPeerConnection(_config);

  if (!rtcPeerConnection->_peerConnection) {
    return Nan::ThrowError(eConstructorFailure);
  }

  rtcPeerConnection->Wrap(info.This());

  rtcPeerConnection->_peerConnectionObserver->SetEventEmitter(
    rtcPeerConnection);

  info.GetReturnValue().Set(info.This());
}

NAN_METHOD(RTCPeerConnection::CreateOffer) {
  METHOD_HEADER("RTCPeerConnection", "createOffer");
  UNWRAP_OBJECT(RTCPeerConnection, object);

  bool iceRestart = false;
  unsigned char start = 0;
  rtc::scoped_refptr<webrtc::CreateSessionDescriptionObserver> observer;

  if (info.Length() < 2) {
    DECLARE_PROMISE_RESOLVER;

    if (info.Length() == 1) {
      ASSERT_REJECT_OBJECT_ARGUMENT(0, options);
      DECLARE_OBJECT_PROPERTY(options, kIceRestart, iceRestartVal);
      iceRestart = iceRestartVal->ToBoolean()->BooleanValue();
    }

    observer = CreateSessionDescriptionObserver::Create(
        new Nan::Persistent<Promise::Resolver>(resolver));
  } else if (info.Length() > 1) {
    if (info.Length() > 2) {
      start = 1;

      ASSERT_OBJECT_ARGUMENT(0, options);
      DECLARE_OBJECT_PROPERTY(options, kIceRestart, iceRestartVal);
      iceRestart = iceRestartVal->ToBoolean()->BooleanValue();
    }

    ASSERT_FUNCTION_ARGUMENT(start, successCallback);
    ASSERT_FUNCTION_ARGUMENT(start + 1, failureCallback);

    observer = CreateSessionDescriptionObserver::Create(
        new Nan::Persistent<Function>(successCallback),
        new Nan::Persistent<Function>(failureCallback));
  }

  webrtc::FakeConstraints constraints;
  constraints.AddOptional(webrtc::MediaConstraintsInterface::kIceRestart,
                          iceRestart);

  object->_peerConnection->CreateOffer(observer, &constraints);
}

NAN_METHOD(RTCPeerConnection::CreateAnswer) {
  METHOD_HEADER("RTCPeerConnection", "createAnswer");
  UNWRAP_OBJECT(RTCPeerConnection, object);

  unsigned char start = 0;
  rtc::scoped_refptr<webrtc::CreateSessionDescriptionObserver> observer;

  if (info.Length() < 2) {
    DECLARE_PROMISE_RESOLVER;

    observer = CreateSessionDescriptionObserver::Create(
        new Nan::Persistent<Promise::Resolver>(resolver));
  } else if (info.Length() > 1) {
    if (info.Length() > 2) {
      start = 1;
    }

    ASSERT_FUNCTION_ARGUMENT(start, successCallback);
    ASSERT_FUNCTION_ARGUMENT(start + 1, failureCallback);

    observer = CreateSessionDescriptionObserver::Create(
        new Nan::Persistent<Function>(successCallback),
        new Nan::Persistent<Function>(failureCallback));
  }

  webrtc::FakeConstraints constraints;
  object->_peerConnection->CreateAnswer(observer, &constraints);
}

// FIXME: factorize SetLocalDescription and SetRemoteDescription

NAN_METHOD(RTCPeerConnection::SetLocalDescription) {
  METHOD_HEADER("RTCPeerConnection", "setLocalDescription");
  UNWRAP_OBJECT(RTCPeerConnection, object);

  rtc::scoped_refptr<webrtc::SetSessionDescriptionObserver> observer;

  // FIXME: Promise implementation only
  DECLARE_PROMISE_RESOLVER;
  ASSERT_REJECT_OBJECT_ARGUMENT(0, sessionDescription);

  // FIXME: validate it's a RTCSessionDescription object
  RTCSessionDescription* _sessionDescription;
  _sessionDescription = Nan::ObjectWrap::Unwrap<RTCSessionDescription>(
    sessionDescription);

  observer = SetSessionDescriptionObserver::Create(
    new Nan::Persistent<Promise::Resolver>(resolver));

  object->_peerConnection->SetLocalDescription(observer,
    _sessionDescription->session_description());
}

NAN_METHOD(RTCPeerConnection::SetRemoteDescription) {
  METHOD_HEADER("RTCPeerConnection", "setRemoteDescription");
  UNWRAP_OBJECT(RTCPeerConnection, object);

  rtc::scoped_refptr<webrtc::SetSessionDescriptionObserver> observer;

  // FIXME: Promise implementation only
  DECLARE_PROMISE_RESOLVER;
  ASSERT_REJECT_OBJECT_ARGUMENT(0, sessionDescription);

  // FIXME: validate it's a RTCSessionDescription object
  RTCSessionDescription* _sessionDescription;
  _sessionDescription = Nan::ObjectWrap::Unwrap<RTCSessionDescription>(
    sessionDescription);

  observer = SetSessionDescriptionObserver::Create(
    new Nan::Persistent<Promise::Resolver>(resolver));

  object->_peerConnection->SetRemoteDescription(observer,
    _sessionDescription->session_description());
}

NAN_METHOD(RTCPeerConnection::CreateDataChannel) {
  METHOD_HEADER("RTCPeerConnection", "createDataChannel");
  UNWRAP_OBJECT(RTCPeerConnection, object);

  ASSERT_STRING_ARGUMENT(0, name);

  // FIXME: add init options
  const webrtc::DataChannelInit init;

  rtc::scoped_refptr<webrtc::DataChannelInterface> _channel
    = object->_peerConnection->CreateDataChannel(*name, &init);

  Local<Object> datachannel = RTCDataChannel::Create(_channel);

  info.GetReturnValue().Set(datachannel);
}

NAN_METHOD(RTCPeerConnection::AddStream) {
  METHOD_HEADER("RTCPeerConnection", "addStream");
  UNWRAP_OBJECT(RTCPeerConnection, object);

  ASSERT_OBJECT_ARGUMENT(0, stream);

  // FIXME: validate...
  RTCMediaStream* _stream = Nan::ObjectWrap::Unwrap<RTCMediaStream>(
    stream);

  std::cout << "ADDING STREAM" << std::endl;
  bool ok = object->_peerConnection->AddStream(_stream->_mediastream);
  if(ok) {
    std::cout << "stream added" << std::endl;
  }else{
    std::cout << "stream NOT added" << std::endl;
  }

  info.GetReturnValue().Set(Nan::Null());
}

NAN_GETTER(RTCPeerConnection::GetConnectionState) {
  // FIXME: implement
  info.GetReturnValue().Set(LOCAL_STRING("new"));
}

NAN_GETTER(RTCPeerConnection::GetCurrentLocalDescription) {
  info.GetReturnValue().Set(Nan::Null());
}

NAN_GETTER(RTCPeerConnection::GetLocalDescription) {
  METHOD_HEADER("RTCPeerConnection", "getLocalDescription");
  UNWRAP_OBJECT(RTCPeerConnection, object);

  const webrtc::SessionDescriptionInterface *session
    = object->_peerConnection->local_description();

  const std::string type = session->type();
  std::string sdp;
  session->ToString(&sdp);

  Local<Object> desc = RTCSessionDescription::Create(type, sdp);

  info.GetReturnValue().Set(desc);
}

NAN_GETTER(RTCPeerConnection::GetRemoteDescription) {
  METHOD_HEADER("RTCPeerConnection", "getRemoteDescription");
  UNWRAP_OBJECT(RTCPeerConnection, object);

  const webrtc::SessionDescriptionInterface *session
    = object->_peerConnection->remote_description();

  const std::string type = session->type();
  std::string sdp;
  session->ToString(&sdp);

  Local<Object> desc = RTCSessionDescription::Create(type, sdp);

  info.GetReturnValue().Set(desc);
}

NAN_GETTER(RTCPeerConnection::GetCurrentRemoteDescription) {
  info.GetReturnValue().Set(Nan::Null());
}

NAN_GETTER(RTCPeerConnection::GetIceConnectionState) {
  UNWRAP_OBJECT(RTCPeerConnection, object);

  std::string iceConnectionState;
  webrtc::PeerConnectionInterface::IceConnectionState state =
      object->_peerConnection->ice_connection_state();

  switch (state) {
    case webrtc::PeerConnectionInterface::kIceConnectionNew:
      iceConnectionState = kNew;
      break;

    case webrtc::PeerConnectionInterface::kIceConnectionChecking:
      iceConnectionState = kChecking;
      break;

    case webrtc::PeerConnectionInterface::kIceConnectionConnected:
      iceConnectionState = kConnected;
      break;

    case webrtc::PeerConnectionInterface::kIceConnectionCompleted:
      iceConnectionState = kCompleted;
      break;

    case webrtc::PeerConnectionInterface::kIceConnectionFailed:
      iceConnectionState = kFailed;
      break;

    case webrtc::PeerConnectionInterface::kIceConnectionDisconnected:
      iceConnectionState = kDisconnected;
      break;

    case webrtc::PeerConnectionInterface::kIceConnectionClosed:
      iceConnectionState = kClosed;
      break;

    default:
      iceConnectionState = kUnknown;
      break;
  }

  info.GetReturnValue().Set(LOCAL_STRING(iceConnectionState));
}

NAN_GETTER(RTCPeerConnection::GetIceGatheringState) {
  UNWRAP_OBJECT(RTCPeerConnection, object);

  std::string iceGatheringState;
  webrtc::PeerConnectionInterface::IceGatheringState state =
      object->_peerConnection->ice_gathering_state();

  switch (state) {
    case webrtc::PeerConnectionInterface::kIceGatheringNew:
      iceGatheringState = kNew;
      break;

    case webrtc::PeerConnectionInterface::kIceGatheringGathering:
      iceGatheringState = kChecking;
      break;

    case webrtc::PeerConnectionInterface::kIceGatheringComplete:
      iceGatheringState = kConnected;
      break;

    default:
      iceGatheringState = kUnknown;
      break;
  }

  info.GetReturnValue().Set(LOCAL_STRING(iceGatheringState));
}

NAN_GETTER(RTCPeerConnection::GetPendingLocalDescription) {
  info.GetReturnValue().Set(Nan::Null());
}

NAN_GETTER(RTCPeerConnection::GetPendingRemoteDescription) {
  info.GetReturnValue().Set(Nan::Null());
}

NAN_GETTER(RTCPeerConnection::GetSignalingState) {
  UNWRAP_OBJECT(RTCPeerConnection, object);

  std::string signalingState;
  webrtc::PeerConnectionInterface::SignalingState state =
      object->_peerConnection->signaling_state();

  switch (state) {
    case webrtc::PeerConnectionInterface::kStable:
      signalingState = kStable;
      break;

    case webrtc::PeerConnectionInterface::kHaveLocalOffer:
      signalingState = kHaveLocalOffer;
      break;

    case webrtc::PeerConnectionInterface::kHaveLocalPrAnswer:
      signalingState = kHaveLocalPranswer;
      break;

    case webrtc::PeerConnectionInterface::kHaveRemoteOffer:
      signalingState = kHaveRemoteOffer;
      break;

    case webrtc::PeerConnectionInterface::kHaveRemotePrAnswer:
      signalingState = kHaveRemotePranswer;
      break;

    default:
      signalingState = kUnknown;
      break;
  }

  info.GetReturnValue().Set(LOCAL_STRING(signalingState));
}

RTCPeerConnection::GenerateCertificateWorker::GenerateCertificateWorker
    (Persistent<Promise::Resolver> *resolver, const rtc::KeyParams& params)
    : Nan::AsyncWorker(nullptr), _params(params), _resolver(resolver) {
}

void RTCPeerConnection::GenerateCertificateWorker::Execute() {
  rtc::RTCCertificateGenerator *generator = Globals::GetCertificateGenerator();

  _certificate = generator->GenerateCertificate(_params,
                                                rtc::Optional<uint64_t>());
}

void RTCPeerConnection::GenerateCertificateWorker::WorkComplete() {
  Nan::HandleScope scope;
  Local<Promise::Resolver> resolver = Nan::New(*_resolver);

  if (!_certificate.get()) {
    std::stringstream errorStream;

    errorStream << eGenerateCertificateFailure;
    resolver->Reject(Nan::TypeError(errorStream.str().c_str()));
  } else {
    resolver->Resolve(RTCCertificate::Create(_certificate));
  }

  Isolate::GetCurrent()->RunMicrotasks();
  _resolver->Reset();
  delete _resolver;
}

NAN_METHOD(RTCPeerConnection::GenerateCertificate) {
  METHOD_HEADER("RTCPeerConnection", "generateCertificate");
  DECLARE_PROMISE_RESOLVER;

  ASSERT_REJECT_SINGLE_ARGUMENT;
  ASSERT_REJECT_OBJECT_ARGUMENT(0, keygenAlgorithm);
  ASSERT_REJECT_OBJECT_PROPERTY(keygenAlgorithm, kName, nameVal);
  ASSERT_REJECT_PROPERTY_STRING(kName, nameVal, name);

  rtc::KeyParams keyParams;
  if (!stricmp(kECDSA, (*name))) {
    ASSERT_REJECT_OBJECT_PROPERTY(keygenAlgorithm, kNamedCurve, namedCurveVal);
    ASSERT_REJECT_PROPERTY_STRING(kNamedCurve, namedCurveVal, namedCurve);

    if (stricmp(kP256, *namedCurve)) {
      errorStream << eCurve;
      resolver->Reject(Nan::GetCurrentContext(),
                       Nan::Error(errorStream.str().c_str()));
      return;
    }

    keyParams = rtc::KeyParams::ECDSA(rtc::ECCurve::EC_NIST_P256);
  } else if (!stricmp(kRSA, (*name))) {
    ASSERT_REJECT_OBJECT_PROPERTY(keygenAlgorithm, kHash, hashVal);
    ASSERT_REJECT_PROPERTY_STRING(kHash, hashVal, hash);

    if (stricmp(kSHA256, *hash)) {
      errorStream << eHash;
      resolver->Reject(Nan::GetCurrentContext(),
                       Nan::Error(errorStream.str().c_str()));
      return;
    }

    ASSERT_REJECT_OBJECT_PROPERTY(keygenAlgorithm, kModulusLength,
                                  modulusLengthVal);
    ASSERT_REJECT_PROPERTY_NUMBER(kModulusLength, modulusLengthVal);
    ASSERT_REJECT_OBJECT_PROPERTY(keygenAlgorithm, kPublicExponent,
                                  publicExponentVal);
    ASSERT_REJECT_PROPERTY_UINT8ARRAY(kPublicExponent, publicExponentVal,
                                      publicExponentArray);

    int modulus = modulusLengthVal->Int32Value();
    if (modulus < rtc::kRsaMinModSize || modulus > rtc::kRsaMaxModSize) {
      errorStream << eUnsupported;

      resolver->Reject(Nan::GetCurrentContext(),
                       Nan::Error(errorStream.str().c_str()));
      return;
    }

    size_t publicExponentLength = publicExponentArray->Length();
    if (publicExponentLength < 1 || publicExponentLength > 4) {
      errorStream << eUnsupported;

      resolver->Reject(Nan::GetCurrentContext(),
                       Nan::Error(errorStream.str().c_str()));
      return;
    }

    uint8_t publicExponent[4] = { 0, 0, 0, 0 };
    for (uint32_t i = 0; i < publicExponentLength; ++i) {
      publicExponent[i] =
          static_cast<uint8_t>(publicExponentArray->Get(i)->Uint32Value());
    }

    uint32_t publicExponentValue = (publicExponent[3] << 24)
                                   + (publicExponent[2] << 16)
                                   + (publicExponent[1] << 8)
                                   + publicExponent[0];

    keyParams = rtc::KeyParams::RSA(modulus, publicExponentValue);
  } else {
    errorStream << eName;
    resolver->Reject(Nan::GetCurrentContext(),
                     Nan::Error(errorStream.str().c_str()));
    return;
  }

  Nan::AsyncQueueWorker(new GenerateCertificateWorker(
      new Nan::Persistent<Promise::Resolver>(resolver), keyParams));
}
