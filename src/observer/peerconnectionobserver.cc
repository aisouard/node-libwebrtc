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

#include <iostream>
#include "peerconnectionobserver.h"
#include "globals.h"

static const char kSignalingStateChange[] = "signalingstatechange";
static const char kIceConnectionStateChange[] = "iceconnectionstatechange";
static const char kIceGatheringStateChange[] = "icegatheringstatechange";
static const char kNegociationNeeded[] = "negotiationneeded";
static const char kDataChannel[] = "datachannel";
static const char kIceCandidate[] = "icecandidate";
// FIXME: not part of W3C spec ?
static const char kAddStream[] = "addstream";
static const char kRemoveStream[] = "removestream";

PeerConnectionObserver::PeerConnectionObserver() {
}

PeerConnectionObserver::~PeerConnectionObserver() {
  _eventEmitter = NULL;
}

void PeerConnectionObserver::OnSignalingChange(
  webrtc::PeerConnectionInterface::SignalingState new_state) {
  EmitterEvent* _event = new EmitterEvent(_eventEmitter);
  _event->SetType(kSignalingStateChange);
  Globals::GetEventQueue()->PushEvent(_event);
}

void PeerConnectionObserver::OnAddStream(
    rtc::scoped_refptr<webrtc::MediaStreamInterface> stream) {
  std::cout << "OnAddStream" << std::endl;
  MediaStreamEvent* _event = new MediaStreamEvent(_eventEmitter, stream);
  _event->SetType(kAddStream);
  Globals::GetEventQueue()->PushEvent(_event);
}

void PeerConnectionObserver::OnRemoveStream(
    rtc::scoped_refptr<webrtc::MediaStreamInterface> stream) {
  std::cout << "OnRemoveStream" << std::endl;
  MediaStreamEvent* _event = new MediaStreamEvent(_eventEmitter, stream);
  _event->SetType(kRemoveStream);
  Globals::GetEventQueue()->PushEvent(_event);
}

void PeerConnectionObserver::OnDataChannel(
    rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel) {
  DataChannelEvent* _event = new DataChannelEvent(_eventEmitter, data_channel);
  _event->SetType(kDataChannel);
  Globals::GetEventQueue()->PushEvent(_event);
}

void PeerConnectionObserver::OnRenegotiationNeeded() {
  EmitterEvent* _event = new EmitterEvent(_eventEmitter);
  _event->SetType(kNegociationNeeded);
  Globals::GetEventQueue()->PushEvent(_event);
}

void PeerConnectionObserver::OnIceConnectionChange(
    webrtc::PeerConnectionInterface::IceConnectionState new_state) {
  EmitterEvent* _event = new EmitterEvent(_eventEmitter);
  _event->SetType(kIceConnectionStateChange);
  Globals::GetEventQueue()->PushEvent(_event);
}

void PeerConnectionObserver::OnIceGatheringChange(
    webrtc::PeerConnectionInterface::IceGatheringState new_state) {
  switch (new_state) {
    case webrtc::PeerConnectionInterface::kIceGatheringNew:
      break;

    case webrtc::PeerConnectionInterface::kIceGatheringGathering:
      break;

    case webrtc::PeerConnectionInterface::kIceGatheringComplete:
      {
        // emit null ice candidate
        // as per https://www.w3.org/TR/webrtc/#dom-rtcpeerconnectioniceevent
        PeerConnectionIceEvent* _iceEvent
          = new PeerConnectionIceEvent(_eventEmitter);
        _iceEvent->SetType(kIceCandidate);
        Globals::GetEventQueue()->PushEvent(_iceEvent);
        break;
      }

    default:
      break;
  }

  EmitterEvent* _event = new EmitterEvent(_eventEmitter);
  _event->SetType(kIceGatheringStateChange);
  Globals::GetEventQueue()->PushEvent(_event);
}

void PeerConnectionObserver::OnIceCandidate(
    const webrtc::IceCandidateInterface *candidate) {
  PeerConnectionIceEvent* _event = new PeerConnectionIceEvent(_eventEmitter);
  _event->SetType(kIceCandidate);
  _event->SetCandidate(candidate);
  Globals::GetEventQueue()->PushEvent(_event);
}

void PeerConnectionObserver::OnIceCandidatesRemoved(
    const std::vector<cricket::Candidate> &candidates) {
  std::cout << "OnIceCandidatesRemoved" << std::endl;
}

void PeerConnectionObserver::OnIceConnectionReceivingChange(bool receiving) {
  std::cout << "OnIceConnectionReceivingChange" << std::endl;
}

PeerConnectionObserver *PeerConnectionObserver::Create() {
  return new rtc::RefCountedObject<PeerConnectionObserver>();
}

void PeerConnectionObserver::SetEventEmitter(
    EventEmitter* eventEmitter) {
  _eventEmitter = eventEmitter;
}
