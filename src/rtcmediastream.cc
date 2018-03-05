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
#include <webrtc/api/mediastreaminterface.h>
#include "rtcmediastream.h"
#include "common.h"

static const char sRTCMediaStream[] = "RTCMediaStream";

NAN_MODULE_INIT(RTCMediaStream::Init) {
  Local<FunctionTemplate> ctor = Nan::New<FunctionTemplate>(New);
  ctor->SetClassName(LOCAL_STRING(sRTCMediaStream));
  ctor->InstanceTemplate()->SetInternalFieldCount(1);

  // Local<ObjectTemplate> prototype = ctor->PrototypeTemplate();
  // Nan::SetAccessor(prototype, LOCAL_STRING(kLabel), GetLabel);
  // Nan::SetAccessor(prototype, LOCAL_STRING(kOrdered), GetOrdered);
  // Nan::SetAccessor(prototype, LOCAL_STRING(kReadyState), GetReadyState);

  // Nan::SetMethod(prototype, kSend, Send);
  // Nan::SetMethod(prototype, kClose, Close);

  constructor().Reset(Nan::GetFunction(ctor).ToLocalChecked());

  Nan::Set(target, LOCAL_STRING(sRTCMediaStream), ctor->GetFunction());
}

RTCMediaStream::RTCMediaStream(
    const rtc::scoped_refptr<webrtc::MediaStreamInterface> &mediastream)
    : _mediastream(mediastream) {

    //_mediastreamObserver = DataChannelObserver::Create();
    //_mediastream->RegisterObserver(_mediastreamObserver);
}

RTCMediaStream::~RTCMediaStream() {
  //_mediastream->UnregisterObserver();
  //_mediastreamObserver = NULL;
}

Local<Object> RTCMediaStream::Create(
  const rtc::scoped_refptr<webrtc::MediaStreamInterface> &mediastream) {
  
  Local<Function> cons = Nan::New(RTCMediaStream::constructor());
  Local<Object> instance = Nan::NewInstance(cons, 0, NULL).ToLocalChecked();

  RTCMediaStream *_mediastream = new RTCMediaStream(mediastream);
  _mediastream->Wrap(instance);
  //_mediastream->_mediastreamObserver->SetEventEmitter(_mediastream);

  return instance;
}

NAN_METHOD(RTCMediaStream::New) {
}
