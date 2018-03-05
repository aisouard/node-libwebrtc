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

#ifndef RTCMEDIASTREAM_H_
#define RTCMEDIASTREAM_H_

#include <nan.h>
#include <webrtc/api/jsep.h>
#include <string>
#include "eventemitter.h"

using namespace v8;

class RTCMediaStream : public EventEmitter {
 public:
  static NAN_MODULE_INIT(Init);

  // static NAN_GETTER(GetLabel);
  // static NAN_GETTER(GetOrdered);
  // static NAN_GETTER(GetReadyState);

  // static NAN_METHOD(Send);
  // static NAN_METHOD(Close);

  static Local<Object> Create(
    const rtc::scoped_refptr<webrtc::MediaStreamInterface>& mediastream);

  static inline Nan::Persistent<v8::Function>& constructor() {
    static Nan::Persistent<v8::Function> _constructor;
    return _constructor;
  }

  const rtc::scoped_refptr<webrtc::MediaStreamInterface> _mediastream;

 private:
  explicit RTCMediaStream(
    const rtc::scoped_refptr<webrtc::MediaStreamInterface>& mediastream);
  ~RTCMediaStream();

  static NAN_METHOD(New);

 protected:
 // rtc::scoped_refptr<DataChannelObserver> _datachannelObserver;
};

#endif  // RTCMEDIASTREAM_H_
