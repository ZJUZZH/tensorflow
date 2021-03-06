/* Copyright 2020 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include "tensorflow/lite/delegates/gpu/cl/device_info.h"

#include <algorithm>
#include <string>
#include <vector>

#include "absl/strings/numbers.h"
#include "absl/strings/str_split.h"

namespace tflite {
namespace gpu {
namespace cl {
namespace {
AdrenoGpu GetAdrenoGpuVersion(const std::string& device_name) {
  const std::map<std::string, AdrenoGpu> kMapping = {
      // Adreno 6xx series
      {"685", AdrenoGpu::kAdreno685},
      {"680", AdrenoGpu::kAdreno680},
      {"675", AdrenoGpu::kAdreno675},
      {"650", AdrenoGpu::kAdreno650},
      {"640", AdrenoGpu::kAdreno640},
      {"630", AdrenoGpu::kAdreno630},
      {"620", AdrenoGpu::kAdreno620},
      {"616", AdrenoGpu::kAdreno618},
      {"616", AdrenoGpu::kAdreno616},
      {"615", AdrenoGpu::kAdreno615},
      {"612", AdrenoGpu::kAdreno612},
      {"610", AdrenoGpu::kAdreno610},
      {"605", AdrenoGpu::kAdreno605},
      // Adreno 5xx series
      {"540", AdrenoGpu::kAdreno540},
      {"530", AdrenoGpu::kAdreno530},
      {"512", AdrenoGpu::kAdreno512},
      {"510", AdrenoGpu::kAdreno510},
      {"509", AdrenoGpu::kAdreno509},
      {"508", AdrenoGpu::kAdreno508},
      {"506", AdrenoGpu::kAdreno506},
      {"505", AdrenoGpu::kAdreno505},
      {"504", AdrenoGpu::kAdreno504},
      // Adreno 4xx series
      {"430", AdrenoGpu::kAdreno430},
      {"420", AdrenoGpu::kAdreno420},
      {"418", AdrenoGpu::kAdreno418},
      {"405", AdrenoGpu::kAdreno405},
      // Adreno 3xx series
      {"330", AdrenoGpu::kAdreno330},
      {"320", AdrenoGpu::kAdreno320},
      {"308", AdrenoGpu::kAdreno308},
      {"306", AdrenoGpu::kAdreno306},
      {"305", AdrenoGpu::kAdreno305},
      {"304", AdrenoGpu::kAdreno304},
      // Adreno 2xx series
      {"225", AdrenoGpu::kAdreno225},
      {"220", AdrenoGpu::kAdreno220},
      {"205", AdrenoGpu::kAdreno205},
      {"203", AdrenoGpu::kAdreno203},
      {"200", AdrenoGpu::kAdreno200},
      // Adreno 1xx series
      {"130", AdrenoGpu::kAdreno130},
      {"120", AdrenoGpu::kAdreno120},
  };

  for (const auto& v : kMapping) {
    if (device_name.find(v.first) != std::string::npos) {
      return v.second;
    }
  }
  return AdrenoGpu::kUnknown;
}

MaliGPU GetMaliGPUVersion(const std::string& device_name) {
  const std::map<std::string, MaliGPU> kMapping = {
      {"T604", MaliGPU::T604}, {"T622", MaliGPU::T622}, {"T624", MaliGPU::T624},
      {"T628", MaliGPU::T628}, {"T658", MaliGPU::T658}, {"T678", MaliGPU::T678},
      {"T720", MaliGPU::T720}, {"T760", MaliGPU::T760}, {"T820", MaliGPU::T820},
      {"T830", MaliGPU::T830}, {"T860", MaliGPU::T860}, {"T880", MaliGPU::T880},
      {"G31", MaliGPU::G31},   {"G51", MaliGPU::G51},   {"G71", MaliGPU::G71},
      {"G52", MaliGPU::G52},   {"G72", MaliGPU::G72},   {"G76", MaliGPU::G76},
      {"G57", MaliGPU::G57},   {"G77", MaliGPU::G77},   {"G68", MaliGPU::G68},
      {"G78", MaliGPU::G78},
  };
  for (const auto& v : kMapping) {
    if (device_name.find(v.first) != std::string::npos) {
      return v.second;
    }
  }
  return MaliGPU::UNKNOWN;
}

}  // namespace

std::string GpuVendorToString(GpuVendor v) {
  switch (v) {
    case GpuVendor::kApple:
      return "Apple";
    case GpuVendor::kQualcomm:
      return "Qualcomm";
    case GpuVendor::kMali:
      return "Mali";
    case GpuVendor::kPowerVR:
      return "PowerVR";
    case GpuVendor::kNvidia:
      return "NVIDIA";
    case GpuVendor::kAMD:
      return "AMD";
    case GpuVendor::kIntel:
      return "Intel";
    case GpuVendor::kUnknown:
      return "unknown vendor";
  }
}

std::string OpenCLVersionToString(OpenCLVersion version) {
  switch (version) {
    case OpenCLVersion::CL_1_0:
      return "1.0";
    case OpenCLVersion::CL_1_1:
      return "1.1";
    case OpenCLVersion::CL_1_2:
      return "1.2";
    case OpenCLVersion::CL_2_0:
      return "2.0";
    case OpenCLVersion::CL_2_1:
      return "2.1";
    case OpenCLVersion::CL_2_2:
      return "2.2";
    case OpenCLVersion::CL_3_0:
      return "3.0";
  }
}

AdrenoInfo::AdrenoInfo(const std::string& device_version)
    : adreno_gpu(GetAdrenoGpuVersion(device_version)) {}

bool AdrenoInfo::IsAdreno1xx() const {
  return adreno_gpu == AdrenoGpu::kAdreno120 ||
         adreno_gpu == AdrenoGpu::kAdreno130;
}

bool AdrenoInfo::IsAdreno2xx() const {
  return adreno_gpu == AdrenoGpu::kAdreno200 ||
         adreno_gpu == AdrenoGpu::kAdreno203 ||
         adreno_gpu == AdrenoGpu::kAdreno205 ||
         adreno_gpu == AdrenoGpu::kAdreno220 ||
         adreno_gpu == AdrenoGpu::kAdreno225;
}

bool AdrenoInfo::IsAdreno3xx() const {
  return adreno_gpu == AdrenoGpu::kAdreno304 ||
         adreno_gpu == AdrenoGpu::kAdreno305 ||
         adreno_gpu == AdrenoGpu::kAdreno306 ||
         adreno_gpu == AdrenoGpu::kAdreno308 ||
         adreno_gpu == AdrenoGpu::kAdreno320 ||
         adreno_gpu == AdrenoGpu::kAdreno330;
}

bool AdrenoInfo::IsAdreno4xx() const {
  return adreno_gpu == AdrenoGpu::kAdreno405 ||
         adreno_gpu == AdrenoGpu::kAdreno418 ||
         adreno_gpu == AdrenoGpu::kAdreno420 ||
         adreno_gpu == AdrenoGpu::kAdreno430;
}

bool AdrenoInfo::IsAdreno5xx() const {
  return adreno_gpu == AdrenoGpu::kAdreno504 ||
         adreno_gpu == AdrenoGpu::kAdreno505 ||
         adreno_gpu == AdrenoGpu::kAdreno506 ||
         adreno_gpu == AdrenoGpu::kAdreno508 ||
         adreno_gpu == AdrenoGpu::kAdreno509 ||
         adreno_gpu == AdrenoGpu::kAdreno510 ||
         adreno_gpu == AdrenoGpu::kAdreno512 ||
         adreno_gpu == AdrenoGpu::kAdreno530 ||
         adreno_gpu == AdrenoGpu::kAdreno540;
}

bool AdrenoInfo::IsAdreno6xx() const {
  return adreno_gpu == AdrenoGpu::kAdreno605 ||
         adreno_gpu == AdrenoGpu::kAdreno610 ||
         adreno_gpu == AdrenoGpu::kAdreno612 ||
         adreno_gpu == AdrenoGpu::kAdreno615 ||
         adreno_gpu == AdrenoGpu::kAdreno616 ||
         adreno_gpu == AdrenoGpu::kAdreno618 ||
         adreno_gpu == AdrenoGpu::kAdreno620 ||
         adreno_gpu == AdrenoGpu::kAdreno630 ||
         adreno_gpu == AdrenoGpu::kAdreno640 ||
         adreno_gpu == AdrenoGpu::kAdreno650 ||
         adreno_gpu == AdrenoGpu::kAdreno675 ||
         adreno_gpu == AdrenoGpu::kAdreno680 ||
         adreno_gpu == AdrenoGpu::kAdreno685;
}

bool AdrenoInfo::IsAdreno6xxOrHigher() const { return IsAdreno6xx(); }

int AdrenoInfo::GetMaximumWavesCount() const {
  if (IsAdreno6xx()) {
    if (adreno_gpu == AdrenoGpu::kAdreno640) {
      return 30;
    } else {
      return 16;
    }
  } else {
    // all other versions not supported
    return 1;
  }
}

int AdrenoInfo::GetRegisterMemorySizePerComputeUnit() const {
  if (IsAdreno6xx()) {
    if (adreno_gpu == AdrenoGpu::kAdreno640) {
      return 128 * 144 * 16;
    } else if (adreno_gpu == AdrenoGpu::kAdreno650) {
      return 128 * 64 * 16;
    } else {
      return 128 * 96 * 16;
    }
  } else {
    // all other versions not supported
    return 1;
  }
}

int AdrenoInfo::GetMaximumWavesCount(int register_footprint_per_tread,
                                     bool full_wave) const {
  const int register_usage_per_wave =
      GetWaveSize(full_wave) * register_footprint_per_tread;
  const int possible_waves_count =
      GetRegisterMemorySizePerComputeUnit() / register_usage_per_wave;
  return std::min(possible_waves_count, GetMaximumWavesCount());
}

int AdrenoInfo::GetWaveSize(bool full_wave) const {
  if (IsAdreno6xx()) {
    return full_wave ? 128 : 64;
  } else if (IsAdreno5xx() || IsAdreno4xx()) {
    return full_wave ? 64 : 32;
  } else {
    // all other versions not supported
    return 1;
  }
}

MaliInfo::MaliInfo(const std::string& device_name)
    : gpu_version(GetMaliGPUVersion(device_name)) {}

bool MaliInfo::IsMaliT6xx() const {
  return gpu_version == MaliGPU::T604 || gpu_version == MaliGPU::T622 ||
         gpu_version == MaliGPU::T624 || gpu_version == MaliGPU::T628 ||
         gpu_version == MaliGPU::T658 || gpu_version == MaliGPU::T678;
}

bool MaliInfo::IsMaliT7xx() const {
  return gpu_version == MaliGPU::T720 || gpu_version == MaliGPU::T760;
}

bool MaliInfo::IsMaliT8xx() const {
  return gpu_version == MaliGPU::T820 || gpu_version == MaliGPU::T830 ||
         gpu_version == MaliGPU::T860 || gpu_version == MaliGPU::T880;
}

bool MaliInfo::IsMidgard() const {
  return IsMaliT6xx() || IsMaliT7xx() || IsMaliT8xx();
}

bool MaliInfo::IsBifrostGen1() const {
  return gpu_version == MaliGPU::G31 || gpu_version == MaliGPU::G51 ||
         gpu_version == MaliGPU::G71;
}

bool MaliInfo::IsBifrostGen2() const {
  return gpu_version == MaliGPU::G52 || gpu_version == MaliGPU::G72;
}

bool MaliInfo::IsBifrostGen3() const { return gpu_version == MaliGPU::G76; }

bool MaliInfo::IsBifrost() const {
  return IsBifrostGen1() || IsBifrostGen2() || IsBifrostGen3();
}

bool MaliInfo::IsValhall() const {
  return gpu_version == MaliGPU::G57 || gpu_version == MaliGPU::G77 ||
         gpu_version == MaliGPU::G68 || gpu_version == MaliGPU::G78;
}

bool DeviceInfo::SupportsTextureArray() const {
  return cl_version >= OpenCLVersion::CL_1_2;
}

bool DeviceInfo::SupportsImageBuffer() const {
  return cl_version >= OpenCLVersion::CL_1_2;
}

bool DeviceInfo::SupportsImage3D() const {
  if (IsMali() && mali_info.IsMidgard()) {
    // On Mali T880 read_imageh doesn't compile with image3d_t
    return false;
  }
  return supports_image3d_writes;
}

bool DeviceInfo::SupportsFloatImage2D(DataType data_type, int channels) const {
  if (channels == 1) {
    return data_type == DataType::FLOAT32 ? supports_r_f32_tex2d
                                          : supports_r_f16_tex2d;
  } else if (channels == 2) {
    return data_type == DataType::FLOAT32 ? supports_rg_f32_tex2d
                                          : supports_rg_f16_tex2d;
  } else if (channels == 3) {
    return data_type == DataType::FLOAT32 ? supports_rgb_f32_tex2d
                                          : supports_rgb_f16_tex2d;
  } else if (channels == 4) {
    return data_type == DataType::FLOAT32 ? supports_rgba_f32_tex2d
                                          : supports_rgba_f16_tex2d;
  } else {
    return false;
  }
}

bool DeviceInfo::SupportsExtension(const std::string& extension) const {
  for (const auto& ext : extensions) {
    if (ext == extension) {
      return true;
    }
  }
  return false;
}

bool DeviceInfo::IsCL20OrHigher() const {
  return cl_version != OpenCLVersion::CL_1_0 &&
         cl_version != OpenCLVersion::CL_1_1 &&
         cl_version != OpenCLVersion::CL_1_2;
}

bool DeviceInfo::SupportsSubGroupWithSize(int sub_group_size) const {
  for (auto subgroup_size : supported_subgroup_sizes) {
    if (sub_group_size == subgroup_size) {
      return true;
    }
  }
  return false;
}

bool DeviceInfo::IsAdreno() const { return gpu_vendor == GpuVendor::kQualcomm; }

bool DeviceInfo::IsApple() const { return gpu_vendor == GpuVendor::kApple; }

bool DeviceInfo::IsMali() const { return gpu_vendor == GpuVendor::kMali; }

bool DeviceInfo::IsPowerVR() const { return gpu_vendor == GpuVendor::kPowerVR; }

bool DeviceInfo::IsNvidia() const { return gpu_vendor == GpuVendor::kNvidia; }

bool DeviceInfo::IsAMD() const { return gpu_vendor == GpuVendor::kAMD; }

bool DeviceInfo::IsIntel() const { return gpu_vendor == GpuVendor::kIntel; }

}  // namespace cl
}  // namespace gpu
}  // namespace tflite
