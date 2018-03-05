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
#include "common.h"
#include "setsessiondescriptionobserver.h"
#include "event/setsessiondescriptionevent.h"
#include "globals.h"

using namespace v8;

SetSessionDescriptionObserver::SetSessionDescriptionObserver(
    Persistent<Promise::Resolver> *resolver) {
  _event = new SetSessionDescriptionEvent(resolver);
}

SetSessionDescriptionObserver::SetSessionDescriptionObserver(
    Persistent<Function> *successCallback,
    Persistent<Function> *failureCallback) {
  _event = new SetSessionDescriptionEvent(successCallback, failureCallback);
}

SetSessionDescriptionObserver *SetSessionDescriptionObserver::
  Create(Persistent<Function> *successCallback,
       Persistent<Function> *failureCallback) {
  return new rtc::RefCountedObject<SetSessionDescriptionObserver>
      (successCallback, failureCallback);
}

SetSessionDescriptionObserver *SetSessionDescriptionObserver::
  Create(Persistent<Promise::Resolver> *resolver) {
  return new rtc::RefCountedObject<SetSessionDescriptionObserver>
      (resolver);
}

void SetSessionDescriptionObserver::OnSuccess() {
  _event->SetSucceeded(true);
  Globals::GetEventQueue()->PushEvent(_event);
}

void SetSessionDescriptionObserver::OnFailure(const std::string &error) {
  _event->SetSucceeded(false);
  _event->SetErrorMessage(error);
  Globals::GetEventQueue()->PushEvent(_event);
}
