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

#include "eventemitter.h"
#include "common.h"

using namespace v8;

static const char kEmit[] = "emit";

EventEmitter::EventEmitter() {}

void EventEmitter::Wrap(Local<Object> obj) {
    Nan::ObjectWrap::Wrap(obj);
    _emit = new Nan::Persistent<Function>(
        Local<Function>::Cast(handle()->Get(Nan::New(kEmit).ToLocalChecked())));
}

void EventEmitter::Emit(Local<String> type) {
    Nan::HandleScope scope;
    Local<Value> argv[] = { type };
    Local<Function> emit = Nan::New(*_emit);
    emit->Call(handle(), 1, argv);
}

void EventEmitter::EmitData(Local<String> type, Local<Value> data) {
    Nan::HandleScope scope;
    Local<Value> argv[] = { type, data };
    Local<Function> emit = Nan::New(*_emit);
    emit->Call(handle(), 2, argv);
}


