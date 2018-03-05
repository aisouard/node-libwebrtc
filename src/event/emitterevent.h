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

#ifndef EVENT_EMITTEREVENT_H_
#define EVENT_EMITTEREVENT_H_

#include "event.h"
#include "eventemitter.h"

using namespace v8;

class EmitterEvent : public Event {
 public:
  explicit EmitterEvent(EventEmitter *eventEmitter);

  void Handle();
  void SetType(const std::string& type);

 protected:
  EventEmitter *_eventEmitter;
  std::string _type;
};

#endif  // EVENT_EMITTEREVENT_H_
