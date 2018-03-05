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

#ifndef EVENT_MESSAGEEVENT_H_
#define EVENT_MESSAGEEVENT_H_

#include "emitterevent.h"
#include "eventemitter.h"
#include <webrtc/api/jsep.h>

using namespace v8;

class MessageEvent : public EmitterEvent {
 public:
  explicit MessageEvent(EventEmitter *eventEmitter);

  void Handle();
  void SetData(const std::string& data);

 private:
  std::string _data;
};

#endif  // EVENT_MESSAGEEVENT_H_
