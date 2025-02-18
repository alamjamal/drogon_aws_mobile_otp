#ifndef H_AWSAUTH
#define H_AWSAUTH

#include <chrono>
#include <string>

#include <nlohmann/json.hpp>


class AWSAuth {
 public:
  AWSAuth(const std::string& service_account_conf_filepath);
  AWSAuth(const nlohmann::json& service_account_conf);
  nlohmann::json FetchCreds();
  
  nlohmann::json GetCreds();
  bool TokenExpired();
  bool WithinRefreshFrequency();
  bool Empty();

 private:
  nlohmann::json LoadServiceAccountConf();

  std::string m_service_account_conf_filepath;
  nlohmann::json m_service_account_conf;

  nlohmann::json m_creds;
  std::chrono::high_resolution_clock::time_point m_last_cred_update;
};

#endif
