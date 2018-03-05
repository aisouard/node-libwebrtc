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
#include "common.h"
#include "rtcsessiondescription.h"

static const char sRTCSessionDescription[] = "RTCSessionDescription";

const char RTCSessionDescription::kSdp[] = "sdp";
const char RTCSessionDescription::kType[] = "type";

const char RTCSessionDescription::kAnswer[] = "answer";
const char RTCSessionDescription::kOffer[] = "offer";
const char RTCSessionDescription::kPranswer[] = "pranswer";
const char RTCSessionDescription::kRollback[] = "rollback";

NAN_MODULE_INIT(RTCSessionDescription::Init) {
  Local<FunctionTemplate> ctor = Nan::New<FunctionTemplate>(New);
  ctor->SetClassName(LOCAL_STRING(sRTCSessionDescription));
  ctor->InstanceTemplate()->SetInternalFieldCount(1);

  Local<ObjectTemplate> tpl = ctor->InstanceTemplate();
  Nan::SetAccessor(tpl, LOCAL_STRING(kSdp), GetSdp);
  Nan::SetAccessor(tpl, LOCAL_STRING(kType), GetType);

  constructor().Reset(Nan::GetFunction(ctor).ToLocalChecked());
  Nan::Set(target, LOCAL_STRING(sRTCSessionDescription), ctor->GetFunction());
}

RTCSessionDescription::RTCSessionDescription(
    webrtc::SessionDescriptionInterface *sessionDescription)
    : _sessionDescription(sessionDescription) {
}

RTCSessionDescription::~RTCSessionDescription() {
  delete _sessionDescription;
}

webrtc::SessionDescriptionInterface*
RTCSessionDescription::session_description() {
  std::string sdp;
  _sessionDescription->ToString(&sdp);
  std::string type = _sessionDescription->type();

  // FIXME: handle error...
  webrtc::SdpParseError error;
  return webrtc::CreateSessionDescription(type, sdp, &error);
}

Local<Object> RTCSessionDescription::Create(const std::string &type,
                                            const std::string &sdp) {
  Local<Function> cons = Nan::New(RTCSessionDescription::constructor());
  Local<Object> descriptionInitDict = Nan::New<Object>();
  descriptionInitDict->Set(LOCAL_STRING(kSdp), LOCAL_STRING(sdp));
  descriptionInitDict->Set(LOCAL_STRING(kType), LOCAL_STRING(type));

  const int argc = 1;
  Local<Value> argv[1] = { descriptionInitDict };
  return Nan::NewInstance(cons, argc, argv).ToLocalChecked();
}

NAN_METHOD(RTCSessionDescription::New) {
  CONSTRUCTOR_HEADER("RTCSessionDescription");

  ASSERT_CONSTRUCT_CALL;
  ASSERT_SINGLE_ARGUMENT;

  ASSERT_OBJECT_ARGUMENT(0, descriptionInitDict);

  ASSERT_OBJECT_PROPERTY(descriptionInitDict, kType, typeVal);
  ASSERT_PROPERTY_STRING(kType, typeVal, type);

  if (strcmp(kAnswer, *type) &&
      strcmp(kOffer, *type) &&
      strcmp(kPranswer, *type) &&
      strcmp(kRollback, *type)) {
    errorStream << "The provided value '";
    errorStream << std::string(*type);
    errorStream << "' is not a valid enum value of type RTCSdpType.";
    return Nan::ThrowTypeError(errorStream.str().c_str());
  }

  ASSERT_OBJECT_PROPERTY(descriptionInitDict, kSdp, sdpVal);
  ASSERT_PROPERTY_STRING(kSdp, sdpVal, sdp);

  webrtc::SdpParseError error;
  webrtc::SessionDescriptionInterface *desc;
  desc = webrtc::CreateSessionDescription(*type, *sdp, &error);

  if (!desc) {
    errorStream << error.description;
    return Nan::ThrowTypeError(errorStream.str().c_str());
  }

  RTCSessionDescription *rtcIceCandidate = new RTCSessionDescription(desc);
  rtcIceCandidate->Wrap(info.This());

  info.GetReturnValue().Set(info.This());
}

NAN_GETTER(RTCSessionDescription::GetType) {
  UNWRAP_OBJECT(RTCSessionDescription, object);

  info.GetReturnValue().Set(LOCAL_STRING(object->_sessionDescription->type()));
}

NAN_GETTER(RTCSessionDescription::GetSdp) {
  UNWRAP_OBJECT(RTCSessionDescription, object);
  std::string sdp;

  object->_sessionDescription->ToString(&sdp);
  info.GetReturnValue().Set(LOCAL_STRING(sdp));
}
