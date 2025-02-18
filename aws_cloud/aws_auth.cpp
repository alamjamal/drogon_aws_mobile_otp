#include <aws_auth.h>

/*#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Exception.hpp>
#include <curlpp/Infos.hpp>
#include <curlpp/Options.hpp>*/
#include <nlohmann/json.hpp>

#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <iostream>

using namespace std;
using namespace std::chrono;

using json = nlohmann::json;


namespace {
/*const curlpp::options::Url AWS_LOGIN_API("https://aws-login.fdscloud.io/creds");
const curlpp::options::HttpHeader HEADERS({"Content-Type: application/json"});
const curlpp::options::Timeout TIMEOUT(29);  // in seconds
const curlpp::options::NoSignal NO_SIGNAL(1);*/

const string AWS_AUTH_ERROR = "unable to fetch creds - aws login API failure";

const double MAX_CREDS_REFRESH_INTERVAL = 3000;  // in seconds - 50 minutes
    
const int MAX_RETRY_ATTEMPTS = 3;
const int SLEEP_BASE = 3;  // in seconds

json LoadJsonFile(const string& filepath) {
    ifstream file_stream(filepath);
    json content;
    try {
        file_stream >> content;
    } catch (const json::parse_error& e) {
        std::cout<<"unable to load - " + string(e.what())<<std::endl;
        // throw AWSAuth::AWSAuthError("unable to load - " + string(e.what()));
    }
    return content;
}
}

AWSAuth::AWSAuth(const string& service_account_conf_filepath):
    m_service_account_conf_filepath(service_account_conf_filepath),
    m_service_account_conf({}),
    m_creds({}) {
}

AWSAuth::AWSAuth(const json& service_account_conf):
    m_service_account_conf_filepath(""),
    m_service_account_conf(service_account_conf),
    m_creds({}) {
}

json AWSAuth::LoadServiceAccountConf() {
    if (m_service_account_conf_filepath.empty() && m_service_account_conf.empty()) {
        std::cout<<"unable to load - empty service account conf"<<std::endl;
        // throw AWSAuthError("unable to load - empty service account conf");
    }

    if (!m_service_account_conf.empty()) {
        return m_service_account_conf;
    }

    return LoadJsonFile(m_service_account_conf_filepath);
}

json AWSAuth::FetchCreds() {
    /*try {
        for (int i = 1; i <=  MAX_RETRY_ATTEMPTS; ++i) {
            curlpp::Cleanup clean;
            curlpp::Easy request;
            request.setOpt(AWS_LOGIN_API);
            request.setOpt(HEADERS);
            const string request_body = LoadServiceAccountConf().dump();
            request.setOpt(new curlpp::options::PostFields(request_body));
            request.setOpt(new curlpp::options::PostFieldSize(request_body.size()));

            ostringstream response_stream;
            request.setOpt(new curlpp::options::WriteStream(&response_stream));
            request.setOpt(TIMEOUT);
            request.setOpt(NO_SIGNAL);
            request.perform();
            long status_code = curlpp::infos::ResponseCode::get(request);
            if (status_code == 200) {
                m_creds = json::parse(response_stream.str());
                m_last_cred_update = high_resolution_clock::now();
                break;
            } else {
                std::cout<<"aws login API failure - " <<  status_code << endl;
                if (i < MAX_RETRY_ATTEMPTS) {
                    sleep(pow(SLEEP_BASE, i));
                }
                if (i == MAX_RETRY_ATTEMPTS) {
                    std::cout<<"aws AWS_AUTH_ERROR" << endl;
                }
            }
        }
    } catch (curlpp::LogicError& e) {
        std::cout << e.what() << std::endl;
        std::cout<<"aws AWS_AUTH_ERROR" << endl;
    } catch ( curlpp::RuntimeError & e ) {
        std::cout<< e.what() << std::endl;
    }*/
    return m_creds;
}

json AWSAuth::GetCreds() {
    return m_creds;
}

bool AWSAuth::TokenExpired() {
    if (m_creds.empty()) {
        std::cout << "AWS creds not fetched yet" << endl;
        return true;
    }
    tm expiry_time_tm = {};
    istringstream ss(m_creds["awsExpires"].get<string>());
    ss >> get_time(&expiry_time_tm, "%Y-%m-%dT%H:%M:%SZ");
    time_t expiry_time_utc = mktime(&expiry_time_tm);
    time_t now = time(nullptr);
    time_t current_time_utc = std::mktime(gmtime(&now));
    return (difftime(expiry_time_utc, current_time_utc) <= 0);
}

bool AWSAuth::WithinRefreshFrequency() {
    if (m_creds.empty()){
        std::cout << "AWS creds not fetched yet" << std::endl;
        return false;
    }
    duration<double> elapsed = high_resolution_clock::now() - m_last_cred_update;
    return (elapsed.count() < MAX_CREDS_REFRESH_INTERVAL);
}

bool AWSAuth::Empty() {
    return m_creds.empty();  
}
