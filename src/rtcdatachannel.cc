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
#include <webrtc/api/datachannelinterface.h>
#include "observer/datachannelobserver.h"
#include "rtcdatachannel.h"
#include "common.h"

static const char sRTCDataChannel[] = "RTCDataChannel";

static const char kLabel[] = "label";
static const char kOrdered[] = "ordered";
static const char kReadyState[] = "readyState";
static const char kSend[] = "send";
static const char kClose[] = "close";

NAN_MODULE_INIT(RTCDataChannel::Init) {
  Local<FunctionTemplate> ctor = Nan::New<FunctionTemplate>(New);
  ctor->SetClassName(LOCAL_STRING(sRTCDataChannel));
  ctor->InstanceTemplate()->SetInternalFieldCount(1);

  Local<ObjectTemplate> prototype = ctor->PrototypeTemplate();
  Nan::SetAccessor(prototype, LOCAL_STRING(kLabel), GetLabel);
  Nan::SetAccessor(prototype, LOCAL_STRING(kOrdered), GetOrdered);
  Nan::SetAccessor(prototype, LOCAL_STRING(kReadyState), GetReadyState);

  Nan::SetMethod(prototype, kSend, Send);
  Nan::SetMethod(prototype, kClose, Close);

  constructor().Reset(Nan::GetFunction(ctor).ToLocalChecked());

  Nan::Set(target, LOCAL_STRING(sRTCDataChannel), ctor->GetFunction());
}

RTCDataChannel::RTCDataChannel(
    const rtc::scoped_refptr<webrtc::DataChannelInterface> &datachannel)
    : _datachannel(datachannel) {

    _datachannelObserver = DataChannelObserver::Create();
    _datachannel->RegisterObserver(_datachannelObserver);
}

RTCDataChannel::~RTCDataChannel() {
  _datachannel->UnregisterObserver();
  _datachannelObserver = NULL;
}

Local<Object> RTCDataChannel::Create(
  const rtc::scoped_refptr<webrtc::DataChannelInterface> &datachannel) {
  Local<Function> cons = Nan::New(RTCDataChannel::constructor());
  Local<Object> instance = Nan::NewInstance(cons, 0, NULL).ToLocalChecked();

  RTCDataChannel *_datachannel = new RTCDataChannel(datachannel);
  _datachannel->Wrap(instance);
  _datachannel->_datachannelObserver->SetEventEmitter(_datachannel);

  return instance;
}

NAN_METHOD(RTCDataChannel::New) {
}

NAN_GETTER(RTCDataChannel::GetLabel) {
  UNWRAP_OBJECT(RTCDataChannel, object);
  info.GetReturnValue().Set(LOCAL_STRING(object->_datachannel->label()));
}

NAN_GETTER(RTCDataChannel::GetOrdered) {
  UNWRAP_OBJECT(RTCDataChannel, object);
  info.GetReturnValue().Set(object->_datachannel->ordered());
}

NAN_GETTER(RTCDataChannel::GetReadyState) {
  UNWRAP_OBJECT(RTCDataChannel, object);
  const char* readyState = webrtc::DataChannelInterface::DataStateString(
    object->_datachannel->state());
  info.GetReturnValue().Set(LOCAL_STRING(readyState));
}

NAN_METHOD(RTCDataChannel::Send) {
  METHOD_HEADER("RTCDataChannel", "send");
  UNWRAP_OBJECT(RTCDataChannel, object);

  // FIXME: implement for ArrayBuffer, others?
  ASSERT_STRING_ARGUMENT(0, data);

  webrtc::DataBuffer _buffer(*data);
  object->_datachannel->Send(_buffer);

  info.GetReturnValue().Set(Nan::Null());
}

NAN_METHOD(RTCDataChannel::Close) {
  METHOD_HEADER("RTCDataChannel", "close");
  UNWRAP_OBJECT(RTCDataChannel, object);

  object->_datachannel->Close();

  info.GetReturnValue().Set(Nan::Null());
}
