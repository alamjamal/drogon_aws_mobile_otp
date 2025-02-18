#include <aws_s3.h>

#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#include <aws/core/Aws.h>
#include <aws/core/auth/AWSCredentials.h>
#include <aws/core/client/AWSError.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/S3Errors.h>
#include <aws/s3/model/PutObjectRequest.h>
#include <aws/s3/model/GetObjectRequest.h>
#include <nlohmann/json.hpp>
#include <aws/s3/model/HeadObjectRequest.h>
#include <aws/s3/model/DeleteObjectRequest.h>
#include <aws_auth.h>

using namespace std;

using namespace Aws::Auth;
using namespace Aws::Client;
using namespace Aws::S3;
using namespace Aws::S3::Model;


using json = nlohmann::json;

AWSS3::AWSS3(const bool& init, const std::string& service_account_conf_filepath):
    m_init(init) {
  if (m_init) {
    // Initialize the API: This step is mandatory to use the API
    m_options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Info;
    Aws::InitAPI(m_options);
  }
  m_aws_auth = make_shared<AWSAuth>(service_account_conf_filepath);
}

AWSS3::AWSS3(const bool& init, const nlohmann::json& service_account_conf):
    m_init(init) {
  if (m_init) {
    // Initialize the API: This step is mandatory to use the API
    m_options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Info;
    Aws::InitAPI(m_options);
  }
  m_aws_auth = make_shared<AWSAuth>(service_account_conf);
}

AWSS3::~AWSS3() {
  if (m_init) {
    Aws::ShutdownAPI(m_options);
  }
}

S3Client AWSS3::GetS3Client() {
  json creds;
  if (m_aws_auth->Empty() || m_aws_auth->TokenExpired()) {
    creds = m_aws_auth->FetchCreds();
  } else {
    creds = m_aws_auth->GetCreds();
  }
  AWSCredentials credentials(creds["awsAccessKey"], creds["awsSecretKey"], creds["awsSessionToken"]);
  S3Client s3_client(credentials);
  return s3_client;
}

void AWSS3::LogError(const string& message, const AWSError<S3Errors>& e) {
    std::cout << message << "; "
                 << "HTTP response code: " << static_cast<int>(e.GetResponseCode()) << "; "
                 << "exception name: " << e.GetExceptionName() << "; "
                 << "error message: " << e.GetMessage() << endl;
}

bool AWSS3::PutObject(const Aws::String& bucket_name, const Aws::String& object_name,
                      const string& object_content) {
    const auto key = bucket_name + "/" + object_name;
    
    auto s3_client = GetS3Client();

    PutObjectRequest request;
    request.SetBucket(bucket_name);
    request.SetKey(object_name);

    const shared_ptr<Aws::IOStream> object_data = Aws::MakeShared<Aws::StringStream>("");
    *object_data << object_content.c_str();
    request.SetBody(object_data);

    //Timer put_timer("put object - " + key);
    PutObjectOutcome outcome = s3_client.PutObject(request);
    //std::cout<< put_timer.label() << " took " << to_string(put_timer.duration()) << " microseconds" << endl;
    if (outcome.IsSuccess()) {
        std::cout << "Uploaded object: " << key << endl;
        return true;
    } else {
        std::cout<<"Unable to upload object: " + key<<
                 outcome.GetError()<<std::endl;
        return false;
    }
}

bool AWSS3::GetObject(const Aws::String& bucket_name, const Aws::String& object_name,
                      string& object_content) {
    const auto key = bucket_name + "/" + object_name;

    auto s3_client = GetS3Client();

    GetObjectRequest request;
    request.SetBucket(bucket_name);
    request.SetKey(object_name);

    GetObjectOutcome outcome = s3_client.GetObject(request);

    if (outcome.IsSuccess()) {
        std::cout << "Downloaded object: " << key << endl;
        auto& object_file = outcome.GetResultWithOwnership().GetBody();
        stringstream object_stream;
        object_stream << object_file.rdbuf();
        object_content = object_stream.str();
        return true;
    } else {
        std::cout<<"Unable to download object: " + key<<
                 outcome.GetError()<<std::endl;
        return false;
    }
}

bool AWSS3::GetObject(const Aws::String& bucket_name, const Aws::String& object_name,
                      json& object_content) {
    const auto key = bucket_name + "/" + object_name;

    auto s3_client = GetS3Client();

    GetObjectRequest request;
    request.SetBucket(bucket_name);
    request.SetKey(object_name);

    GetObjectOutcome outcome = s3_client.GetObject(request);

    if (outcome.IsSuccess()) {
        std::cout << "Downloaded object: " << key << endl;
        outcome.GetResultWithOwnership().GetBody() >> object_content;
        return true;
    } else {
        std::cout<<"Unable to download object: " + key <<
                 outcome.GetError()<<std::endl;
        return false;
    }
}
bool AWSS3::GetObjectModificationTime(const Aws::String& bucket_name, const Aws::String& object_name,
                 time_t& object_modification_time){
    const auto key = bucket_name + "/" + object_name;
    auto s3_client = GetS3Client();
    HeadObjectRequest request;
    request.SetBucket(bucket_name);
    request.SetKey(object_name);
    auto outcome = s3_client.HeadObject(request);
    if (outcome.IsSuccess()) {
        std::cout << "Downloaded object modification time: " << key << endl;
        Aws::Utils::DateTime lastModifiedTime = outcome.GetResult().GetLastModified();    
        // Now transform DateTime of mili second to time_t in seconds
        object_modification_time = lastModifiedTime.Millis() / 1000;
        return true;
    } else {
        std::cout<<"Unable to download object modification time: " + key<<
                 outcome.GetError()<<std::endl;
        return false;
    }
}
bool AWSS3::DeleteObject(const Aws::String& bucket_name, const Aws::String& object_name)
{
    const auto key = bucket_name + "/" + object_name;
    auto s3_client = GetS3Client();
    DeleteObjectRequest  request;
    request.SetBucket(bucket_name);
    request.SetKey(object_name);
    auto outcome = s3_client.DeleteObject(request);
    if(!outcome.IsSuccess()){
      std::cout<<"Unable to delete object : " + key<<
                 outcome.GetError()<<std::endl;
      return false;
    }
    else{
      std::cout << "object: " << key << " has been deleted."<< endl;
      return true;
    }
}