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

#ifndef RTCPEERCONNECTION_H_
#define RTCPEERCONNECTION_H_

#include <nan.h>
#include <webrtc/api/jsep.h>
#include <string>
#include "eventemitter.h"

using namespace v8;

class PeerConnectionObserver;
class RTCPeerConnection : public EventEmitter {
 public:
  static NAN_MODULE_INIT(Init);

 private:
  explicit RTCPeerConnection(
      const webrtc::PeerConnectionInterface::RTCConfiguration& config);
  ~RTCPeerConnection();

  static NAN_METHOD(New);
  static NAN_METHOD(CreateOffer);
  static NAN_METHOD(CreateAnswer);
  static NAN_METHOD(SetLocalDescription);
  static NAN_METHOD(SetRemoteDescription);
  static NAN_METHOD(CreateDataChannel);
  static NAN_METHOD(GenerateCertificate);
  static NAN_METHOD(AddStream);

  static NAN_GETTER(GetConnectionState);
  static NAN_GETTER(GetCurrentLocalDescription);
  static NAN_GETTER(GetCurrentRemoteDescription);
  static NAN_GETTER(GetLocalDescription);
  static NAN_GETTER(GetRemoteDescription);
  static NAN_GETTER(GetIceConnectionState);
  static NAN_GETTER(GetIceGatheringState);
  static NAN_GETTER(GetPendingLocalDescription);
  static NAN_GETTER(GetPendingRemoteDescription);
  static NAN_GETTER(GetSignalingState);

  class GenerateCertificateWorker : public Nan::AsyncWorker {
   public:
    explicit GenerateCertificateWorker(Persistent<Promise::Resolver> *resolver,
                                       const rtc::KeyParams& params);
    ~GenerateCertificateWorker() {}

    void Execute();
    void WorkComplete();

   private:
    const rtc::KeyParams _params;
    Persistent<Promise::Resolver> *_resolver;
    rtc::scoped_refptr<rtc::RTCCertificate> _certificate;
  };

  static Nan::Persistent<FunctionTemplate> constructor;

 protected:
  rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface>
      _peerConnectionFactory;
  rtc::scoped_refptr<webrtc::PeerConnectionInterface> _peerConnection;
  rtc::scoped_refptr<PeerConnectionObserver> _peerConnectionObserver;
};

#endif  // RTCPEERCONNECTION_H_
