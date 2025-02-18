#pragma once
#include <string>
#include <iostream>
#include <unordered_map>
#include <mutex>
#include <chrono>
#include <json/json.h>

class OtpService {
public:
    struct OtpInfo {
        std::string otp;
        std::chrono::steady_clock::time_point expireTime;
    };

    static OtpService &getInstance();

    void storeOtp(const std::string &mobile, const std::string &otp, int validityInSeconds = 600);
    bool verifyOtp(const std::string &mobile, const std::string &otp);
    void cleanupExpiredOtps();

private:
    OtpService() = default;
    OtpService(const OtpService &) = delete;
    OtpService &operator=(const OtpService &) = delete;

    std::unordered_map<std::string, OtpInfo> otpMap_;
    std::mutex mapMutex_;
};
