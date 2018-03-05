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

#ifndef COMMON_H_
#define COMMON_H_

#include <sstream>
#include <webrtc/base/logging.h>

#define ERROR_INVOKE_WITHOUT_NEW \
  "Class constructors cannot be invoked without 'new'"

#define ERROR_CONSTRUCT_PREFIX(NAME) \
  "Failed to construct '" NAME "': "

#define ERROR_CALL_PREFIX(CLASS, METHOD) \
  "Failed to execute '" METHOD "' on '" CLASS "': "

#define ERROR_NOT_SINGLE_ARGUMENT(COUNT) \
  "1 argument required, but only " << COUNT << " present."

#define ERROR_NOT_ENOUGH_ARGUMENTS(I, COUNT) \
  I << " arguments required, but only " << COUNT << " present."

#define ERROR_PROPERTY_NOT_A_NUMBER(NAME) \
  "The '" << NAME << "' property is not a number."

#define ERROR_PROPERTY_NOT_STRING(NAME) \
  "The '" << NAME << "' property is not a string, or is empty."

#define ERROR_PROPERTY_NOT_BOOLEAN(NAME) \
  "The '" << NAME << "' property is not a boolean."

#define ERROR_PROPERTY_NOT_OBJECT(NAME) \
  "The '" << NAME << "' property is not an object."

#define ERROR_PROPERTY_NOT_ARRAY(NAME) \
  "The '" << NAME << "' property is not an object."

#define ERROR_PROPERTY_NOT_UINT8ARRAY(NAME) \
  "The '" << NAME << "' property is not a Uint8Array."

#define ERROR_PROPERTY_NOT_DEFINED(NAME) \
  "The '" << NAME << "' property is undefined."

#define ERROR_ARGUMENT_NOT_OBJECT(INDEX, NAME) \
  "parameter " << INDEX << " ('" << NAME << "') is not an object."

#define ERROR_ARGUMENT_NOT_FUNCTION(INDEX, NAME) \
  "parameter " << INDEX << " ('" << NAME << "') is not a function."

#define ERROR_ARGUMENT_NOT_STRING(INDEX, NAME) \
  "parameter " << INDEX << " ('" << NAME << "') is not a string."

#ifdef DEBUG
#define CONSTRUCTOR_HEADER(NAME) \
  LOG(LS_INFO) << __PRETTY_FUNCTION__; \
  std::stringstream errorStream; \
  errorStream << ERROR_CONSTRUCT_PREFIX(NAME);
#define METHOD_HEADER(CLASS, METHOD) \
  LOG(LS_INFO) << __PRETTY_FUNCTION__; \
  std::stringstream errorStream; \
  errorStream << ERROR_CALL_PREFIX(CLASS, METHOD);
#else
#define CONSTRUCTOR_HEADER(NAME) \
  std::stringstream errorStream; \
  errorStream << ERROR_CONSTRUCT_PREFIX(NAME);
#define METHOD_HEADER(CLASS, METHOD) \
  std::stringstream errorStream; \
  errorStream << ERROR_CALL_PREFIX(CLASS, METHOD);
#endif

#define ASSERT_CONSTRUCT_CALL \
  if (!info.IsConstructCall()) { \
    errorStream << ERROR_INVOKE_WITHOUT_NEW; \
    return Nan::ThrowError(errorStream.str().c_str()); \
  }

#define ASSERT_SINGLE_ARGUMENT \
  if (info.Length() < 1) { \
    errorStream << ERROR_NOT_SINGLE_ARGUMENT(info.Length()); \
    return Nan::ThrowError(errorStream.str().c_str()); \
  }

#define ASSERT_REJECT_SINGLE_ARGUMENT \
  if (info.Length() < 1) { \
    errorStream << ERROR_NOT_SINGLE_ARGUMENT(info.Length()); \
    resolver->Reject(Nan::GetCurrentContext(), \
                     Nan::TypeError(errorStream.str().c_str())); \
    return; \
  }

#define ASSERT_ARGUMENTS_COUNT(I) \
  if (info.Length() < I) { \
    errorStream << ERROR_NOT_ENOUGH_ARGUMENTS(I, info.Length()); \
    return Nan::ThrowError(errorStream.str().c_str()); \
  }

#define ASSERT_OBJECT_ARGUMENT(I, N) \
  if (!info[I]->IsObject()) { \
    errorStream << ERROR_ARGUMENT_NOT_OBJECT(I + 1, #N); \
    return Nan::ThrowTypeError(errorStream.str().c_str()); \
  } \
  \
  Local<Object> N = info[I]->ToObject();

#define ASSERT_FUNCTION_ARGUMENT(I, N) \
  if (!info[I]->IsFunction()) { \
    errorStream << ERROR_ARGUMENT_NOT_FUNCTION(I + 1, #N); \
    return Nan::ThrowTypeError(errorStream.str().c_str()); \
  } \
  \
  Local<Function> N = info[I].As<v8::Function>();

#define ASSERT_STRING_ARGUMENT(I, N) \
  if (!info[I]->IsString()) { \
    errorStream << ERROR_ARGUMENT_NOT_STRING(I + 1, #N); \
    return Nan::ThrowTypeError(errorStream.str().c_str()); \
  } \
  \
  String::Utf8Value N(info[I]->ToString());

#define ASSERT_REJECT_OBJECT_ARGUMENT(I, N) \
  if (!info[I]->IsObject()) { \
    errorStream << ERROR_ARGUMENT_NOT_OBJECT(I + 1, #N); \
    resolver->Reject(Nan::GetCurrentContext(), \
                     Nan::TypeError(errorStream.str().c_str())); \
    return; \
  } \
  \
  Local<Object> N = info[I]->ToObject();

#define ASSERT_OBJECT_PROPERTY(O, P, V) \
  if (!Nan::HasOwnProperty(O, Nan::New(P).ToLocalChecked()).FromJust()) { \
    errorStream << ERROR_PROPERTY_NOT_DEFINED(P); \
    return Nan::ThrowTypeError(errorStream.str().c_str()); \
  } \
  \
  Local<Value> V = O->Get(Nan::New(P).ToLocalChecked());

#define ASSERT_REJECT_OBJECT_PROPERTY(O, P, V) \
  if (!Nan::HasOwnProperty(O, Nan::New(P).ToLocalChecked()).FromJust()) { \
    errorStream << ERROR_PROPERTY_NOT_DEFINED(P); \
    resolver->Reject(Nan::GetCurrentContext(), \
                     Nan::TypeError(errorStream.str().c_str())); \
    return; \
  } \
  \
  Local<Value> V = O->Get(Nan::New(P).ToLocalChecked());

#define ASSERT_PROPERTY_NUMBER(N, V, S) \
  if (!V->IsNumber()) { \
    errorStream << ERROR_PROPERTY_NOT_A_NUMBER(N); \
    return Nan::ThrowTypeError(errorStream.str().c_str()); \
  } \
  \
  Local<Number> S(V->ToNumber());

#define ASSERT_PROPERTY_STRING(N, V, S) \
  if (!V->IsString()) { \
    errorStream << ERROR_PROPERTY_NOT_STRING(N); \
    return Nan::ThrowTypeError(errorStream.str().c_str()); \
  } \
  \
  String::Utf8Value S(V->ToString());

#define ASSERT_PROPERTY_BOOLEAN(N, V, S) \
  if (!V->IsBoolean()) { \
    errorStream << ERROR_PROPERTY_NOT_BOOLEAN(N); \
    return Nan::ThrowTypeError(errorStream.str().c_str()); \
  } \
  \
  Local<Boolean> S(V->ToBoolean());

#define ASSERT_PROPERTY_OBJECT(N, V, S) \
  if (!V->IsObject()) { \
    errorStream << ERROR_PROPERTY_NOT_OBJECT(N); \
    return Nan::ThrowTypeError(errorStream.str().c_str()); \
  } \
  \
  Local<Object> S(V->ToObject());

#define ASSERT_PROPERTY_ARRAY(N, V, S) \
  if (!V->IsArray()) { \
    errorStream << ERROR_PROPERTY_NOT_ARRAY(N); \
    return Nan::ThrowTypeError(errorStream.str().c_str()); \
  } \
  \
  Local<Array> S = Local<Array>::Cast(V);


#define ASSERT_REJECT_PROPERTY_BOOLEAN(N, V, S) \
  if (!V->IsBoolean()) { \
    errorStream << ERROR_PROPERTY_NOT_BOOLEAN(N); \
    resolver->Reject(Nan::GetCurrentContext(), \
                     Nan::TypeError(errorStream.str().c_str())); \
    return; \
  } \
  \
  Local<Boolean> S(V->ToBoolean());

#define ASSERT_REJECT_PROPERTY_NUMBER(N, V) \
  if (!V->IsNumber()) { \
    errorStream << ERROR_PROPERTY_NOT_A_NUMBER(N); \
    resolver->Reject(Nan::GetCurrentContext(), \
                     Nan::TypeError(errorStream.str().c_str())); \
    return; \
  }

#define ASSERT_REJECT_PROPERTY_STRING(N, V, S) \
  if (!V->IsString()) { \
    errorStream << ERROR_PROPERTY_NOT_STRING(N); \
    resolver->Reject(Nan::GetCurrentContext(), \
                     Nan::TypeError(errorStream.str().c_str())); \
    return; \
  } \
  \
  String::Utf8Value S(V->ToString());

#define ASSERT_REJECT_PROPERTY_UINT8ARRAY(N, V, S) \
  if (!V->IsUint8Array()) { \
    errorStream << ERROR_PROPERTY_NOT_UINT8ARRAY(N); \
    resolver->Reject(Nan::GetCurrentContext(), \
                     Nan::TypeError(errorStream.str().c_str())); \
    return; \
  } \
  \
  Local<Uint8Array> S = V.As<Uint8Array>();

#define DECLARE_OBJECT_PROPERTY(O, P, V) \
  Local<Value> V = O->Get(LOCAL_STRING(P));

#define DECLARE_PROMISE_RESOLVER \
  Local<Promise::Resolver> resolver = \
    Promise::Resolver::New(info.GetIsolate()); \
  info.GetReturnValue().Set(resolver->GetPromise());

#define LOCAL_STRING(S) \
  Nan::New(S).ToLocalChecked()

#define HAS_OWN_PROPERTY(O, P) \
  Nan::HasOwnProperty(O, LOCAL_STRING(P)).FromJust()

#define IS_STRICTLY_NULL(V) \
  (V->IsNull() || V->IsUndefined())

#define UNWRAP_OBJECT(T, O) \
    T *O = Nan::ObjectWrap::Unwrap<T>(info.This())

#endif  // COMMON_H_
