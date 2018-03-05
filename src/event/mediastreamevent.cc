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
#include "mediastreamevent.h"
#include "eventemitter.h"
#include "rtcmediastream.h"

using namespace v8;

static const char kStream[] = "stream";

MediaStreamEvent::MediaStreamEvent(EventEmitter *eventEmitter,
  const rtc::scoped_refptr<webrtc::MediaStreamInterface> &mediastream)
  : EmitterEvent(eventEmitter), _mediastream(mediastream) {
}

void MediaStreamEvent::Handle() {
  Nan::HandleScope scope;

  // FIXME: make proper MediaStreamEvent ?
  Local<Object> e = Nan::New<Object>();
  // FIXME: move to observer ?
  Nan::Persistent<Object> mediastream(RTCMediaStream::Create(_mediastream));
  e->Set(LOCAL_STRING(kStream), Nan::New(mediastream));

  _eventEmitter->EmitData(LOCAL_STRING(_type), e);
}
