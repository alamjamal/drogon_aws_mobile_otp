#pragma once
#include <drogon/HttpController.h>
#include <drogon/HttpClient.h>
#include "OtpService.h"
#include <DatabaseManager.h>

class OtpController : public drogon::HttpController<OtpController> {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(OtpController::sendOtp, "/send-otp", drogon::Post);
    ADD_METHOD_TO(OtpController::verifyOtp, "/verify-otp", drogon::Post);
    METHOD_LIST_END

    void sendOtp(const drogon::HttpRequestPtr &req, std::function<void(const drogon::HttpResponsePtr &)> &&callback);
    void verifyOtp(const drogon::HttpRequestPtr &req, std::function<void(const drogon::HttpResponsePtr &)> &&callback);
};
