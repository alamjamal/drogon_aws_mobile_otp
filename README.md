# Drogon AWS S3 & Fast2SMS OTP Service

This project is a backend service built with **C++17** and the **Drogon framework**, designed to handle **file uploads to AWS S3** and **send OTPs using Fast2SMS** for mobile authentication. The service provides RESTful APIs for secure mobile authentication and file storage.

## Features

- **Secure OTP Generation**: Generates and validates OTPs for user authentication.
- **Fast2SMS Integration**: Sends OTPs to mobile numbers via Fast2SMS API.
- **AWS S3 File Uploads**: Allows users to securely upload files to AWS S3.
- **Drogon Framework**: High-performance C++ framework for web applications.
- **REST API Support**: Endpoints for OTP handling and file management.
- **JSON-based Communication**: Uses `nlohmann_json` for handling API requests.

## Prerequisites

Before setting up the project, ensure you have the following:

- **AWS Account**: To configure AWS S3 for file uploads.
- **Fast2SMS API Key**: To send OTPs via Fast2SMS.
- **AWS CLI**: Installed and configured for S3 operations.
- **C++17 Compiler**: Compatible with Drogon framework.
- **CMake**: For building the project.
- **Conan**: Package manager for C++ dependencies.

## Setup Instructions

### 1. Clone the Repository
```bash
git clone https://github.com/alamjamal/drogon_aws_mobile_otp.git
cd drogon_aws_mobile_otp
