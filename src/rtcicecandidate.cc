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
#include <webrtc/p2p/base/candidate.h>
#include <webrtc/p2p/base/port.h>

#include "common.h"
#include "rtcicecandidate.h"

static const char sRTCIceCandidate[] = "RTCIceCandidate";

static const char kCandidate[] = "candidate";
static const char kSdpMid[] = "sdpMid";
static const char kSdpMLineIndex[] = "sdpMLineIndex";
static const char kFoundation[] = "foundation";
static const char kPriority[] = "priority";
static const char kIp[] = "ip";
static const char kProtocol[] = "protocol";
static const char kPort[] = "port";
static const char kType[] = "type";
static const char kTcpType[] = "tcpType";
static const char kRelatedAddress[] = "relatedAddress";
static const char kRelatedPort[] = "relatedPort";

static const char eBothAreNull[] =
    "Both 'sdpMid' and 'sdpMLineIndex' properties are null.";
static const char eSerialize[] =
    "Failed to serialize ICE candidate.";

NAN_MODULE_INIT(RTCIceCandidate::Init) {
  Local<FunctionTemplate> ctor = Nan::New<FunctionTemplate>(New);

  ctor->SetClassName(LOCAL_STRING(sRTCIceCandidate));
  ctor->InstanceTemplate()->SetInternalFieldCount(1);

  Local<ObjectTemplate> instance = ctor->InstanceTemplate();
  Nan::SetAccessor(instance, LOCAL_STRING(kCandidate), GetCandidate);
  Nan::SetAccessor(instance, LOCAL_STRING(kSdpMid), GetSdpMid);
  Nan::SetAccessor(instance, LOCAL_STRING(kSdpMLineIndex), GetSdpMLineIndex);

  Local<ObjectTemplate> prototype = ctor->PrototypeTemplate();
  Nan::SetAccessor(prototype, LOCAL_STRING(kFoundation), GetCandidate);
  Nan::SetAccessor(prototype, LOCAL_STRING(kPriority), GetPriority);
  Nan::SetAccessor(prototype, LOCAL_STRING(kIp), GetIp);
  Nan::SetAccessor(prototype, LOCAL_STRING(kProtocol), GetProtocol);
  Nan::SetAccessor(prototype, LOCAL_STRING(kPort), GetPort);
  Nan::SetAccessor(prototype, LOCAL_STRING(kType), GetType);
  Nan::SetAccessor(prototype, LOCAL_STRING(kTcpType), GetTcpType);
  Nan::SetAccessor(prototype, LOCAL_STRING(kRelatedAddress), GetRelatedAddress);
  Nan::SetAccessor(prototype, LOCAL_STRING(kRelatedPort), GetRelatedPort);

  constructor().Reset(Nan::GetFunction(ctor).ToLocalChecked());

  Nan::Set(target, LOCAL_STRING(sRTCIceCandidate),
           ctor->GetFunction());
}

RTCIceCandidate::RTCIceCandidate(
  const webrtc::IceCandidateInterface *iceCandidate)
  : _iceCandidate(iceCandidate) {
}

RTCIceCandidate::~RTCIceCandidate() {
  delete _iceCandidate;
}

Local<Object> RTCIceCandidate::Create(
  std::string sdpMid, int sdpMLineIndex, std::string candidate) {
  Local<Function> cons = Nan::New(RTCIceCandidate::constructor());

  Local<Object> candidateInitDict = Nan::New<Object>();
  candidateInitDict->Set(LOCAL_STRING(kSdpMid),
    LOCAL_STRING(sdpMid));
  candidateInitDict->Set(LOCAL_STRING(kSdpMLineIndex),
    Nan::New<Integer>(sdpMLineIndex));
  candidateInitDict->Set(LOCAL_STRING(kCandidate),
    LOCAL_STRING(candidate));

  const int argc = 1;
  Local<Value> argv[1] = { candidateInitDict };
  Local<Object> instance = Nan::NewInstance(cons, argc, argv).ToLocalChecked();

  return instance;
}

NAN_METHOD(RTCIceCandidate::New) {
  CONSTRUCTOR_HEADER("RTCIceCandidate")

  ASSERT_CONSTRUCT_CALL;

  ASSERT_SINGLE_ARGUMENT;
  ASSERT_OBJECT_ARGUMENT(0, candidateInitDict);
  ASSERT_OBJECT_PROPERTY(candidateInitDict, kCandidate, candidateVal);
  ASSERT_PROPERTY_STRING(kCandidate, candidateVal, cand);

  if (!HAS_OWN_PROPERTY(candidateInitDict, kSdpMid) &&
      !HAS_OWN_PROPERTY(candidateInitDict, kSdpMLineIndex)) {
    errorStream << eBothAreNull;
    return Nan::ThrowError(errorStream.str().c_str());
  }

  DECLARE_OBJECT_PROPERTY(candidateInitDict, kSdpMid, sdpMidVal);
  DECLARE_OBJECT_PROPERTY(candidateInitDict, kSdpMLineIndex, sdpMLineIndexVal);

  if (IS_STRICTLY_NULL(sdpMidVal) && IS_STRICTLY_NULL(sdpMLineIndexVal)) {
    errorStream << eBothAreNull;
    return Nan::ThrowError(errorStream.str().c_str());
  }

  String::Utf8Value sdpMid(sdpMidVal->ToString());
  int32_t sdpMLineIndex = sdpMLineIndexVal->Int32Value();

  webrtc::SdpParseError error;
  webrtc::IceCandidateInterface *iceCandidate;
  iceCandidate = webrtc::CreateIceCandidate(
      IS_STRICTLY_NULL(sdpMidVal) ? "" : *sdpMid, sdpMLineIndex, *cand, &error);

  if (!iceCandidate) {
    errorStream << error.description;
    return Nan::ThrowError(errorStream.str().c_str());
  }

  RTCIceCandidate *rtcIceCandidate = new RTCIceCandidate(iceCandidate);
  rtcIceCandidate->Wrap(info.This());
  info.GetReturnValue().Set(info.This());
}

NAN_GETTER(RTCIceCandidate::GetCandidate) {
  UNWRAP_OBJECT(RTCIceCandidate, object);
  std::string candidate;

  if (!object->_iceCandidate->ToString(&candidate)) {
    return Nan::ThrowError(eSerialize);
  }

  info.GetReturnValue().Set(LOCAL_STRING(candidate));
}

NAN_GETTER(RTCIceCandidate::GetSdpMid) {
  UNWRAP_OBJECT(RTCIceCandidate, object);

  if (!object->_iceCandidate->sdp_mid().length()) {
    info.GetReturnValue().SetNull();
    return;
  }

  info.GetReturnValue().Set(LOCAL_STRING(object->_iceCandidate->sdp_mid()));
}

NAN_GETTER(RTCIceCandidate::GetSdpMLineIndex) {
  UNWRAP_OBJECT(RTCIceCandidate, object);

  info.GetReturnValue().Set(object->_iceCandidate->sdp_mline_index());
}

NAN_GETTER(RTCIceCandidate::GetFoundation) {
  UNWRAP_OBJECT(RTCIceCandidate, object);
  const cricket::Candidate &cand = object->_iceCandidate->candidate();

  info.GetReturnValue().Set(LOCAL_STRING(cand.foundation()));
}

NAN_GETTER(RTCIceCandidate::GetPriority) {
  UNWRAP_OBJECT(RTCIceCandidate, object);
  const cricket::Candidate &cand = object->_iceCandidate->candidate();

  info.GetReturnValue().Set(cand.priority());
}

NAN_GETTER(RTCIceCandidate::GetIp) {
  UNWRAP_OBJECT(RTCIceCandidate, object);
  const cricket::Candidate &cand = object->_iceCandidate->candidate();

  info.GetReturnValue().Set(LOCAL_STRING(cand.address().ipaddr().ToString()));
}

NAN_GETTER(RTCIceCandidate::GetProtocol) {
  UNWRAP_OBJECT(RTCIceCandidate, object);
  const cricket::Candidate &cand = object->_iceCandidate->candidate();

  info.GetReturnValue().Set(LOCAL_STRING(cand.protocol()));
}

NAN_GETTER(RTCIceCandidate::GetPort) {
  UNWRAP_OBJECT(RTCIceCandidate, object);
  const cricket::Candidate &cand = object->_iceCandidate->candidate();

  info.GetReturnValue().Set(cand.address().port());
}

NAN_GETTER(RTCIceCandidate::GetType) {
  UNWRAP_OBJECT(RTCIceCandidate, object);
  const cricket::Candidate &cand = object->_iceCandidate->candidate();

  info.GetReturnValue().Set(LOCAL_STRING(cand.type()));
}

NAN_GETTER(RTCIceCandidate::GetTcpType) {
  UNWRAP_OBJECT(RTCIceCandidate, object);
  const cricket::Candidate &cand = object->_iceCandidate->candidate();
  const std::string &tcptype = cand.tcptype();

  if (!tcptype.length()) {
    info.GetReturnValue().SetNull();
    return;
  }

  info.GetReturnValue().Set(LOCAL_STRING(tcptype));
}

NAN_GETTER(RTCIceCandidate::GetRelatedAddress) {
  UNWRAP_OBJECT(RTCIceCandidate, object);
  const cricket::Candidate &cand = object->_iceCandidate->candidate();

  if (!strcmp(cricket::LOCAL_PORT_TYPE, cand.type().c_str()) ||
      cand.related_address().IsNil()) {
    info.GetReturnValue().SetNull();
    return;
  }

  info.GetReturnValue().Set(
      LOCAL_STRING(cand.related_address().ipaddr().ToString()));
}

NAN_GETTER(RTCIceCandidate::GetRelatedPort) {
  UNWRAP_OBJECT(RTCIceCandidate, object);
  const cricket::Candidate &cand = object->_iceCandidate->candidate();

  if (!strcmp(cricket::LOCAL_PORT_TYPE, cand.type().c_str()) ||
      cand.related_address().IsNil()) {
    info.GetReturnValue().SetNull();
    return;
  }

  info.GetReturnValue().Set(cand.related_address().port());
}
