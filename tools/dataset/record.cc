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
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "mynteye/glog_init.h"

#include "mynteye/device.h"
#include "mynteye/utils.h"

#include "mynteye/times.h"

#include "dataset/dataset.h"

MYNTEYE_USE_NAMESPACE

int main(int argc, char *argv[]) {
  glog_init _(argc, argv);

  auto &&device = device::select();
  if (!device)
    return 1;
  /*
  {  // auto-exposure
    device->SetOptionValue(Option::EXPOSURE_MODE, 0);
    device->SetOptionValue(Option::MAX_GAIN, 40);  // [0.48]
    device->SetOptionValue(Option::MAX_EXPOSURE_TIME, 120);  // [0,240]
    device->SetOptionValue(Option::DESIRED_BRIGHTNESS, 200);  // [0,255]
  }
  {  // manual-exposure
    device->SetOptionValue(Option::EXPOSURE_MODE, 1);
    device->SetOptionValue(Option::GAIN, 20);  // [0.48]
    device->SetOptionValue(Option::BRIGHTNESS, 20);  // [0,240]
    device->SetOptionValue(Option::CONTRAST, 20);  // [0,255]
  }
  device->SetOptionValue(Option::IR_CONTROL, 80);
  device->SetOptionValue(Option::FRAME_RATE, 25);
  device->SetOptionValue(Option::IMU_FREQUENCY, 500);
  */
  device->LogOptionInfos();

  // Enable this will cache the motion datas until you get them.
  device->EnableMotionDatas();
  device->Start(Source::ALL);

  const char *outdir;
  if (argc >= 2) {
    outdir = argv[1];
  } else {
    outdir = "./dataset";
  }
  tools::Dataset dataset(outdir);

  cv::namedWindow("frame");

  std::size_t img_count = 0;
  std::size_t imu_count = 0;
  auto &&time_beg = times::now();
  while (true) {
    device->WaitForStreams();

    auto &&left_datas = device->GetStreamDatas(Stream::LEFT);
    auto &&right_datas = device->GetStreamDatas(Stream::RIGHT);
    img_count += left_datas.size();

    auto &&motion_datas = device->GetMotionDatas();
    imu_count += motion_datas.size();

    auto &&left_frame = left_datas.back().frame;
    auto &&right_frame = right_datas.back().frame;
    cv::Mat left_img(
        left_frame->height(), left_frame->width(), CV_8UC1, left_frame->data());
    cv::Mat right_img(
        right_frame->height(), right_frame->width(), CV_8UC1,
        right_frame->data());

    cv::Mat img;
    cv::hconcat(left_img, right_img, img);
    cv::imshow("frame", img);

    {  // save
      for (auto &&left : left_datas) {
        dataset.SaveStreamData(Stream::LEFT, left);
      }
      for (auto &&right : right_datas) {
        dataset.SaveStreamData(Stream::RIGHT, right);
      }

      for (auto &&motion : motion_datas) {
        dataset.SaveMotionData(motion);
      }

      std::cout << "\rSaved " << img_count << " imgs"
                << ", " << imu_count << " imus" << std::flush;
    }

    char key = static_cast<char>(cv::waitKey(1));
    if (key == 27 || key == 'q' || key == 'Q') {  // ESC/Q
      break;
    }
  }
  std::cout << " to " << outdir << std::endl;
  auto &&time_end = times::now();

  device->Stop(Source::ALL);

  float elapsed_ms =
      times::count<times::microseconds>(time_end - time_beg) * 0.001f;
  LOG(INFO) << "Time beg: " << times::to_local_string(time_beg)
            << ", end: " << times::to_local_string(time_end)
            << ", cost: " << elapsed_ms << "ms";
  LOG(INFO) << "Img count: " << img_count
            << ", fps: " << (1000.f * img_count / elapsed_ms);
  LOG(INFO) << "Imu count: " << imu_count
            << ", hz: " << (1000.f * imu_count / elapsed_ms);
  return 0;
}
