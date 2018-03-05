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

#ifndef RTCSESSIONDESCRIPTION_H_
#define RTCSESSIONDESCRIPTION_H_

#include <nan.h>
#include <webrtc/api/jsep.h>
#include <string>

using namespace v8;

class RTCSessionDescription : public Nan::ObjectWrap {
 public:
  static NAN_MODULE_INIT(Init);

  static Local<Object> Create(const std::string& type, const std::string &sdp);

  static inline Nan::Persistent<v8::Function>& constructor() {
    static Nan::Persistent<v8::Function> _constructor;
    return _constructor;
  }

  static const char kSdp[];
  static const char kType[];

  static const char kAnswer[];
  static const char kOffer[];
  static const char kPranswer[];
  static const char kRollback[];

  webrtc::SessionDescriptionInterface* session_description();

 private:
  explicit RTCSessionDescription(
      webrtc::SessionDescriptionInterface* sessionDescription);
  ~RTCSessionDescription();

  webrtc::SessionDescriptionInterface* _sessionDescription;

  static NAN_METHOD(New);

  static NAN_GETTER(GetType);
  static NAN_GETTER(GetSdp);
};

#endif  // RTCSESSIONDESCRIPTION_H_
