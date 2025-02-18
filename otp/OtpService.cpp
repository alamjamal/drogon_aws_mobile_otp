#include "OtpService.h"
#include <chrono>

OtpService &OtpService::getInstance() {
    static OtpService instance;
    return instance;
}

void OtpService::storeOtp(const std::string &mobile, const std::string &otp, int validityInSeconds) {
    std::lock_guard<std::mutex> lock(mapMutex_);
    OtpInfo otpInfo = {otp, std::chrono::steady_clock::now() + std::chrono::seconds(validityInSeconds)};
    otpMap_[mobile] = otpInfo;
}

bool OtpService::verifyOtp(const std::string &mobile, const std::string &otp) {
    std::lock_guard<std::mutex> lock(mapMutex_);
    auto it = otpMap_.find(mobile);
    if (it == otpMap_.end()) {
        return false;
    }
    auto &otpInfo = it->second;
    if (std::chrono::steady_clock::now() > otpInfo.expireTime) {
        otpMap_.erase(it); // Remove expired OTP
        return false;
    }
    if (otpInfo.otp == otp) {
        otpMap_.erase(it); // Remove OTP after successful verification
        return true;
    }
    return false;
}

void OtpService::cleanupExpiredOtps() {
    std::lock_guard<std::mutex> lock(mapMutex_);
    for (auto it = otpMap_.begin(); it != otpMap_.end();) {
        if (std::chrono::steady_clock::now() > it->second.expireTime) {
            it = otpMap_.erase(it);
        } else {
            ++it;
        }
    }
}
