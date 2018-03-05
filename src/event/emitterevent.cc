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

#include "common.h"
#include "emitterevent.h"

using namespace v8;

EmitterEvent::EmitterEvent(EventEmitter *eventEmitter)
  : _eventEmitter(eventEmitter) {}

void EmitterEvent::Handle() {
  Nan::HandleScope scope;
  _eventEmitter->Emit(LOCAL_STRING(_type));
}

void EmitterEvent::SetType(const std::string &type) {
  _type = type;
}
