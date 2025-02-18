#include "OtpController.h"
#include <drogon/drogon.h>
#include <random>
std::string OTP_URL = "https://www.fast2sms.com";
std::string OTP_ENDPOINT = "/dev/bulkV2";
std::string OTP_AUTH_KEY ="";
namespace {
    std::string generateOtp(int length = 6) {
        static std::mt19937 rng{std::random_device{}()};
        std::uniform_int_distribution<int> dist(0, 9);
        std::string otp;
        for (int i = 0; i < length; ++i) {
            otp += std::to_string(dist(rng));
        }
        return otp;
    }
}

void OtpController::sendOtp(const drogon::HttpRequestPtr &req, std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
    std::cout << "Received request: " << req->methodString() << " " << req->path()<<std::endl;
    auto response = drogon::HttpResponse::newHttpResponse();
    auto json = req->getJsonObject();
    if (!json || !json->isMember("mobile")) {
        response->setStatusCode(drogon::k400BadRequest);
        response->setBody("Mobile number is required.");
        callback(response);
        return;
    }

    std::string mobile = (*json)["mobile"].asString();
    std::string otp = generateOtp();
    OtpService::getInstance().storeOtp(mobile, otp);
    LOG_INFO << "OTP for " << mobile << ": " << otp;

    try
    {
        // Prepare the JSON body
        Json::Value body;
        body["route"] = "otp";
        body["variables_values"] = otp;
        body["numbers"] = mobile;
        // http client
        auto client = drogon::HttpClient::newHttpClient(OTP_URL); 
        // make a request
        auto req = drogon::HttpRequest::newHttpRequest();
        req->setPath(OTP_ENDPOINT);
        req->setMethod(drogon::Post);
        req->addHeader("Content-Type", "application/json");
        req->addHeader("authorization",OTP_AUTH_KEY);
        req->setBody(body.toStyledString());
        client->sendRequest(req,
        [&](drogon::ReqResult result, const drogon::HttpResponsePtr &res) {
            if (result == drogon::ReqResult::Ok) {
                // store mobile number into db.
                DatabaseManager& dbManager = DatabaseManager::getInstance(); 
                dbManager.executeQuery("INSERT INTO general_user (mobile_no, user_name) VALUES ('" + mobile + "', '" + otp + "')");

                // Handle successful response
                response->setStatusCode(drogon::k200OK);
                response->setBody("OTP sent successfully");
                callback(response);
            } else {
                // Handle errors
                response->setStatusCode(drogon::k400BadRequest);
                response->setBody("Sorry not able to send OTP , please try after sometime..");
                callback(response);
            }
        });
    }
    catch(...)
    {
        LOG_ERROR << "not able to send otp.."<<otp<<":"<<mobile;
    }
}

void OtpController::verifyOtp(const drogon::HttpRequestPtr &req, std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
    std::cout << "Received request: " << req->methodString() << " " << req->path()<<std::endl;
    auto json = req->getJsonObject();
    if (!json || !json->isMember("mobile") || !json->isMember("otp")) {
        auto response = drogon::HttpResponse::newHttpResponse();
        response->setStatusCode(drogon::k400BadRequest);
        response->setBody("Mobile number and OTP are required.");
        callback(response);
        return;
    }

    std::string mobile = (*json)["mobile"].asString();
    std::string otp = (*json)["otp"].asString();

    bool success = OtpService::getInstance().verifyOtp(mobile, otp);

    auto response = drogon::HttpResponse::newHttpResponse();
    if (success) {
        response->setBody("OTP verified successfully");
    } else {
        response->setStatusCode(drogon::k400BadRequest);
        response->setBody("Invalid or expired OTP");
    }
    callback(response);
}
