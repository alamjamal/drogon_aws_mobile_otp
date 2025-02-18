#include <drogon/drogon.h>
#include <json/json.h>

#include <aws_s3.h>

#include <drogon/orm/DbClient.h>
#include <drogon/utils/Utilities.h>
#include <trantor/utils/Logger.h>
#include <fstream>
#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <sstream>
#include <cstdlib>
#include <memory>

#include "OtpService.h"
#include "OtpController.h"

#include "Config.h"

#include "DatabaseManager.h"

using namespace std::chrono_literals;
using namespace drogon::orm;
using namespace drogon;


int main()
{
    try
    {
        // load env file
        Config::loadEnvFile("../.env");
        // create db instance.
        DatabaseManager& dbManager = DatabaseManager::getInstance(); 

        // Schedule periodic cleanup of expired OTPs
        drogon::app().getLoop()->runEvery(60.0, []() {
            OtpService::getInstance().cleanupExpiredOtps();
        });
        
        // Run the Drogon app (this method will start the application)
        app().registerHandler(
        "/",
        [](const HttpRequestPtr &request,
           std::function<void(const HttpResponsePtr &)> &&callback) {
            LOG_INFO << "connected:"
                     << (request->connected() ? "true" : "false");
            auto resp = HttpResponse::newHttpResponse();
            resp->setBody("Hello, World!");
            callback(resp);
        },
        {Get});

        // Load configuration directly from config.json
        std::cout<<"Server is started now..."<<std::endl;
        drogon::app().loadConfigFile("../config.json").run();
        
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}