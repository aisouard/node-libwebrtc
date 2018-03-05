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
#include "datachannelevent.h"
#include "eventemitter.h"
#include "rtcdatachannel.h"

using namespace v8;

static const char kChannel[] = "channel";

DataChannelEvent::DataChannelEvent(EventEmitter *eventEmitter,
  const rtc::scoped_refptr<webrtc::DataChannelInterface> &datachannel)
  : EmitterEvent(eventEmitter), _channel(datachannel) {
}

void DataChannelEvent::Handle() {
  Nan::HandleScope scope;

  // FIXME: make proper DataChannelEvent ?
  Local<Object> e = Nan::New<Object>();
  // FIXME: move to observer ?
  Nan::Persistent<Object> channel(RTCDataChannel::Create(_channel));
  e->Set(LOCAL_STRING(kChannel), Nan::New(channel));

  _eventEmitter->EmitData(LOCAL_STRING(_type), e);
}
