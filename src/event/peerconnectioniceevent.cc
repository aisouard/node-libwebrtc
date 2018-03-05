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

#include "common.h"
#include "peerconnectioniceevent.h"
#include "eventemitter.h"
#include "rtcicecandidate.h"

#include <webrtc/p2p/base/candidate.h>
#include <iostream>

using namespace v8;

static const char kCandidate[] = "candidate";

PeerConnectionIceEvent::PeerConnectionIceEvent(EventEmitter *eventEmitter)
  : EmitterEvent(eventEmitter) {
}

void PeerConnectionIceEvent::Handle() {
  Nan::HandleScope scope;

  // FIXME: make proper PeerConnectionIceEvent ?
  Local<Object> e = Nan::New<Object>();

  if (_candidate.length() && _sdpMid.length()) {
    e->Set(LOCAL_STRING(kCandidate),
         RTCIceCandidate::Create(_sdpMid, _sdpMLineIndex, _candidate));
  } else {
    e->Set(LOCAL_STRING(kCandidate), Nan::Null());
  }

  _eventEmitter->EmitData(LOCAL_STRING(_type), e);
}

void PeerConnectionIceEvent::SetCandidate(
    const webrtc::IceCandidateInterface* candidate) {
  candidate->ToString(&_candidate);
  _sdpMid = candidate->sdp_mid();
  _sdpMLineIndex = candidate->sdp_mline_index();
}
