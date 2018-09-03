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
#include "api/processor/processor.h"

#include <glog/logging.h>

#include <exception>
#include <utility>

#include "internal/strings.h"
#include "internal/times.h"

MYNTEYE_BEGIN_NAMESPACE

Processor::Processor(std::int32_t proc_period)
    : proc_period_(std::move(proc_period)),
      activated_(false),
      input_ready_(false),
      idle_(true),
      dropped_count_(0),
      input_(nullptr),
      output_(nullptr),
      output_result_(nullptr),
      pre_callback_(nullptr),
      post_callback_(nullptr),
      callback_(nullptr),
      parent_(nullptr) {
  VLOG(2) << __func__;
}

Processor::~Processor() {
  VLOG(2) << __func__;
  Deactivate();
  input_.reset(nullptr);
  output_.reset(nullptr);
  output_result_.reset(nullptr);
  childs_.clear();
}

std::string Processor::Name() {
  return "Processor";
}

void Processor::AddChild(const std::shared_ptr<Processor> &child) {
  child->parent_ = this;
  childs_.push_back(child);
}

void Processor::RemoveChild(const std::shared_ptr<Processor> &child) {
  childs_.remove(child);
}

std::list<std::shared_ptr<Processor>> Processor::GetChilds() {
  return childs_;
}

void Processor::SetPreProcessCallback(PreProcessCallback callback) {
  pre_callback_ = std::move(callback);
}

void Processor::SetPostProcessCallback(PostProcessCallback callback) {
  post_callback_ = std::move(callback);
}

void Processor::SetProcessCallback(ProcessCallback callback) {
  callback_ = std::move(callback);
}

void Processor::Activate(bool parents) {
  if (activated_)
    return;
  if (parents) {
    // Activate all parents
    Processor *parent = parent_;
    while (parent != nullptr) {
      parent->Activate();
      parent = parent->parent_;
    }
  }
  activated_ = true;
  thread_ = std::thread(&Processor::Run, this);
  // thread_.detach();
}

void Processor::Deactivate(bool childs) {
  if (!activated_)
    return;
  if (childs) {
    // Deactivate all childs
    iterate_processors(GetChilds(), [](std::shared_ptr<Processor> proc) {
      proc->Deactivate();
    });
  }
  activated_ = false;
  {
    std::lock_guard<std::mutex> lk(mtx_input_ready_);
    input_ready_ = true;
  }
  cond_input_ready_.notify_all();
  thread_.join();
}

bool Processor::IsActivated() {
  return activated_;
}

bool Processor::IsIdle() {
  std::lock_guard<std::mutex> lk(mtx_state_);
  return idle_;
}

bool Processor::Process(const Object &in) {
  if (!activated_)
    return false;
  if (!idle_) {
    std::lock_guard<std::mutex> lk(mtx_state_);
    if (!idle_) {
      ++dropped_count_;
      return false;
    }
  }
  if (!in.DecValidity()) {
    LOG(WARNING) << Name() << " process with invalid input";
    return false;
  }
  {
    std::lock_guard<std::mutex> lk(mtx_input_ready_);
    input_.reset(in.Clone());
    input_ready_ = true;
  }
  cond_input_ready_.notify_all();
  return true;
}

std::shared_ptr<Object> Processor::GetOutput() {
  std::lock_guard<std::mutex> lk(mtx_result_);
  return std::shared_ptr<Object>(std::move(output_result_));
}

std::uint64_t Processor::GetDroppedCount() {
  std::lock_guard<std::mutex> lk(mtx_state_);
  return dropped_count_;
}

void Processor::Run() {
  VLOG(2) << Name() << " thread start";

  auto sleep = [this](const times::system_clock::time_point &time_beg) {
    if (proc_period_ > 0) {
      static times::system_clock::time_point time_prev = time_beg;
      auto &&time_elapsed_ms =
          times::count<times::milliseconds>(times::now() - time_prev);
      time_prev = time_beg;

      if (time_elapsed_ms < proc_period_) {
        VLOG(2) << Name() << " process cost "
                << times::count<times::milliseconds>(times::now() - time_beg)
                << " ms, sleep " << (proc_period_ - time_elapsed_ms) << " ms";
        std::this_thread::sleep_for(
            std::chrono::milliseconds(proc_period_ - time_elapsed_ms));
        return;
      }
    }
    VLOG(2) << Name() << " process cost "
            << times::count<times::milliseconds>(times::now() - time_beg)
            << " ms";
  };

  while (true) {
    std::unique_lock<std::mutex> lk(mtx_input_ready_);
    cond_input_ready_.wait(lk, [this] { return input_ready_; });

    auto &&time_beg = times::now();

    if (!activated_) {
      SetIdle(true);
      input_ready_ = false;
      break;
    }
    SetIdle(false);

    if (!output_) {
      output_.reset(OnCreateOutput());
    }

    if (pre_callback_) {
      pre_callback_(input_.get());
    }
    bool ok = false;
    try {
      if (callback_) {
        if (callback_(input_.get(), output_.get(), parent_)) {
          ok = true;
        } else {
          ok = OnProcess(input_.get(), output_.get(), parent_);
        }
      } else {
        ok = OnProcess(input_.get(), output_.get(), parent_);
      }
      // CV_Assert(false);
    } catch (const std::exception &e) {
      std::string msg(e.what());
      strings::rtrim(msg);
      LOG(ERROR) << Name() << " process error \"" << msg << "\"";
    }
    if (!ok) {
      VLOG(2) << Name() << " process failed";
      continue;
    }
    if (post_callback_) {
      post_callback_(output_.get());
    }
    {
      std::unique_lock<std::mutex> lk(mtx_result_);
      output_result_.reset(output_->Clone());
    }

    if (!childs_.empty()) {
      for (auto child : childs_) {
        child->Process(*output_);
      }
    }

    SetIdle(true);
    input_ready_ = false;

    sleep(time_beg);
  }
  VLOG(2) << Name() << " thread end";
}

void Processor::SetIdle(bool idle) {
  std::lock_guard<std::mutex> lk(mtx_state_);
  idle_ = idle;
}

MYNTEYE_END_NAMESPACE
