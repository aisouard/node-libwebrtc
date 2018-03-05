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

#ifndef OBSERVER_DATACHANNELOBSERVER_H_
#define OBSERVER_DATACHANNELOBSERVER_H_

#include <webrtc/api/datachannelinterface.h>
#include "event/emitterevent.h"

using namespace v8;

class DataChannelObserver : public rtc::RefCountInterface,
                            public webrtc::DataChannelObserver {
 public:
  static DataChannelObserver *Create();

  void SetEventEmitter(
      EventEmitter* eventEmitter);

  // The data channel state have changed.
  void OnStateChange();

  //  A data buffer was successfully received.
  void OnMessage(const webrtc::DataBuffer& buffer);

  // The data channel's buffered_amount has changed.
  void OnBufferedAmountChange(uint64_t previous_amount);

 private:
  EventEmitter* _eventEmitter;

 protected:
  DataChannelObserver();
  ~DataChannelObserver();
};

#endif  // OBSERVER_DATACHANNELOBSERVER_H_
