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
#include "datachannelobserver.h"
#include "event/messageevent.h"
#include "globals.h"

static const char kMessage[] = "message";

DataChannelObserver::DataChannelObserver() {
}

DataChannelObserver::~DataChannelObserver() {
  _eventEmitter = NULL;
}

void DataChannelObserver::OnStateChange() {
  std::cout << "DataChannel OnStateChange" << std::endl;
}

void DataChannelObserver::OnMessage(const webrtc::DataBuffer& buffer) {
  MessageEvent* _event = new MessageEvent(_eventEmitter);
  _event->SetType(kMessage);

  rtc::CopyOnWriteBuffer data = buffer.data;
  std::string _data(reinterpret_cast<char *>(data.data()), data.size());;

  _event->SetData(_data);
  Globals::GetEventQueue()->PushEvent(_event);
}

void DataChannelObserver::OnBufferedAmountChange(uint64_t previous_amount) {
  std::cout << "DataChannel OnBufferedAmountChange" << std::endl;
}

DataChannelObserver *DataChannelObserver::Create() {
  return new rtc::RefCountedObject<DataChannelObserver>();
}

void DataChannelObserver::SetEventEmitter(
    EventEmitter* eventEmitter) {
  _eventEmitter = eventEmitter;
}
