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

#ifndef OBSERVER_PEERCONNECTIONOBSERVER_H_
#define OBSERVER_PEERCONNECTIONOBSERVER_H_

#include <webrtc/api/peerconnectioninterface.h>
#include "event/emitterevent.h"
#include "event/peerconnectioniceevent.h"
#include "event/datachannelevent.h"
#include "event/mediastreamevent.h"

using namespace v8;

class PeerConnectionObserver : public rtc::RefCountInterface,
                               public webrtc::PeerConnectionObserver {
 public:
  static PeerConnectionObserver *Create();

  void SetEventEmitter(
      EventEmitter* eventEmitter);

  // Triggered when the SignalingState changed.
  void OnSignalingChange(
      webrtc::PeerConnectionInterface::SignalingState new_state);

  // Triggered when media is received on a new stream from remote peer.
  void OnAddStream(
      rtc::scoped_refptr<webrtc::MediaStreamInterface> stream);

  // Triggered when a remote peer close a stream.
  void OnRemoveStream(
      rtc::scoped_refptr<webrtc::MediaStreamInterface> stream);

  // Triggered when a remote peer opens a data channel.
  void OnDataChannel(
      rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel);

  // Triggered when renegotiation is needed. For example, an ICE restart
  // has begun.
  void OnRenegotiationNeeded();

  // Called any time the IceConnectionState changes.
  void OnIceConnectionChange(
      webrtc::PeerConnectionInterface::IceConnectionState new_state);

  // Called any time the IceGatheringState changes.
  void OnIceGatheringChange(
      webrtc::PeerConnectionInterface::IceGatheringState new_state);

  // A new ICE candidate has been gathered.
  void OnIceCandidate(const webrtc::IceCandidateInterface* candidate);

  // Ice candidates have been removed.
  void OnIceCandidatesRemoved(
      const std::vector<cricket::Candidate>& candidates);

  // Called when the ICE connection receiving status changes.
  void OnIceConnectionReceivingChange(bool receiving);

 private:
  EventEmitter* _eventEmitter;

 protected:
  PeerConnectionObserver();
  ~PeerConnectionObserver();
};

#endif  // OBSERVER_PEERCONNECTIONOBSERVER_H_
