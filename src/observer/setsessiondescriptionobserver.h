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

#ifndef OBSERVER_SETSESSIONDESCRIPTIONOBSERVER_H_
#define OBSERVER_SETSESSIONDESCRIPTIONOBSERVER_H_

#include <nan.h>
#include <string>
#include <webrtc/api/peerconnectioninterface.h>

using namespace v8;

class SetSessionDescriptionEvent;
class SetSessionDescriptionObserver :
    public webrtc::SetSessionDescriptionObserver {
 public:
  static SetSessionDescriptionObserver *Create(
      Persistent<Promise::Resolver> *resolver);
  static SetSessionDescriptionObserver *Create(
      Persistent<Function> *successCallback,
      Persistent<Function> *failureCallback);

  void OnSuccess();
  void OnFailure(const std::string& error);

 private:
  SetSessionDescriptionEvent *_event;

 protected:
  explicit SetSessionDescriptionObserver(
      Persistent<Promise::Resolver> *resolver);

  SetSessionDescriptionObserver(Persistent<Function> *successCallback,
                                      Persistent<Function> *failureCallback);
};

#endif  // OBSERVER_SETSESSIONDESCRIPTIONOBSERVER_H_
