// Copyright 2018 Slightech Co., Ltd. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#ifndef MYNTEYE_GLOBAL_H_
#define MYNTEYE_GLOBAL_H_
#pragma once

#ifdef _WIN32
#define OS_WIN
#ifdef _WIN64
#define OS_WIN64
#else
#define OS_WIN32
#endif
#if defined(__MINGW32__) || defined(__MINGW64__)
#define OS_MINGW
#ifdef __MINGW64__
#define OS_MINGW64
#else
#define OS_MINGW32
#endif
#elif defined(__CYGWIN__) || defined(__CYGWIN32__)
#define OS_CYGWIN
#endif
#elif __APPLE__
#include "TargetConditionals.h"
#if TARGET_IPHONE_SIMULATOR
#define OS_IPHONE
#define OS_IPHONE_SIMULATOR
#elif TARGET_OS_IPHONE
#define OS_IPHONE
#elif TARGET_OS_MAC
#define OS_MAC
#else
#error "Unknown Apple platform"
#endif
#elif __ANDROID__
#define OS_ANDROID
#elif __linux__
#define OS_LINUX
#elif __unix__
#define OS_UNIX
#elif defined(_POSIX_VERSION)
#define OS_POSIX
#else
#error "Unknown compiler"
#endif

#ifdef OS_WIN
#define DECL_EXPORT __declspec(dllexport)
#define DECL_IMPORT __declspec(dllimport)
#define DECL_HIDDEN
#else
#define DECL_EXPORT __attribute__((visibility("default")))
#define DECL_IMPORT __attribute__((visibility("default")))
#define DECL_HIDDEN __attribute__((visibility("hidden")))
#endif

#if defined(OS_WIN) && !defined(OS_MINGW) && !defined(OS_CYGWIN)
#define OS_SEP "\\"
#else
#define OS_SEP "/"
#endif

#define STRINGIFY_HELPER(X) #X
#define STRINGIFY(X) STRINGIFY_HELPER(X)

#define DISABLE_COPY(Class)      \
  Class(const Class &) = delete; \
  Class &operator=(const Class &) = delete;

#define UNUSED(x) (void)x;

template <typename... T>
void unused(T &&...) {}

#endif  // MYNTEYE_GLOBAL_H_
