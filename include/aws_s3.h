#ifndef H_AWSS3
#define H_AWSS3

#include <memory>

#include <aws/core/Aws.h>
#include <aws/core/client/AWSError.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/S3Errors.h>

#include <nlohmann/json.hpp>
#include <time.h>
#include <aws_auth.h>
#include <boost/date_time/posix_time/posix_time.hpp>

class AWSS3 {
 public:
  AWSS3(const bool& init, const std::string& service_account_conf_filepath);
  AWSS3(const bool& init, const nlohmann::json& service_account_conf);
  ~AWSS3();
  Aws::S3::S3Client GetS3Client();
  bool PutObject(const Aws::String& bucket_name, const Aws::String& object_name,
                 const std::string& object_content);
  bool GetObject(const Aws::String& bucket_name, const Aws::String& object_name,
                 std::string& object_content);
  bool GetObject(const Aws::String& bucket_name, const Aws::String& object_name,
                 nlohmann::json& object_content);
  bool GetObjectModificationTime(const Aws::String& bucket_name, const Aws::String& object_name,
                 time_t& object_modification_time);
  bool DeleteObject(const Aws::String& bucket_name, const Aws::String& object_name);
  void LogError(const std::string& message, const Aws::Client::AWSError<Aws::S3::S3Errors>& error);
private:
  bool m_init;
  std::shared_ptr<AWSAuth> m_aws_auth;
  Aws::SDKOptions m_options;
};

#endif
