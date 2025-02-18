cmake_minimum_required(VERSION 3.15)

project(InstaXeroxCpp)

find_package(Drogon REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(mariadb-connector-c REQUIRED)
find_package(AWSSDK REQUIRED COMPONENTS s3)

include_directories("${CMAKE_SOURCE_DIR}/include")

file(GLOB SRC_FILES "src/*.cpp")
file(GLOB AWS_FILES "aws_cloud/*.cpp")
file(GLOB DB_FILES "db_access/*.cpp")
file(GLOB OTP_FILES "otp/*.cpp")

list(APPEND SOURCES ${SRC_FILES} ${AWS_FILES} ${DB_FILES} ${OTP_FILES})

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} ZLIB::ZLIB 
                                      Drogon::Drogon 
                                      nlohmann_json::nlohmann_json 
                                      openssl::openssl
                                      mariadb-connector-c::mariadb-connector-c
                                      AWS::aws-sdk-cpp-s3
                                    )