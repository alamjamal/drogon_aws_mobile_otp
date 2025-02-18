#ifndef CONFIG_H
#define CONFIG_H

#include <cstdlib>
#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>

class Config {
public:
    // Fetch an environment variable; throw an error if not set
    static std::string getEnvVar(const std::string& varName) {
        const char* value = std::getenv(varName.c_str());
        if (!value) {
            throw std::runtime_error("Environment variable " + varName + " not set");
        }
        return std::string(value);
    }

    // Fetch an environment variable with a default value
    static std::string getEnvVar(const std::string& varName, const std::string& defaultValue) {
        const char* value = std::getenv(varName.c_str());
        return value ? std::string(value) : defaultValue;
    }
    static void loadEnvFile(const std::string& filePath) {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open .env file");
        }

        std::string line;
        while (std::getline(file, line)) {
            // Skip comments and empty lines
            if (line.empty() || line[0] == '#') continue;

            std::istringstream keyValueStream(line);
            std::string key, value;

            // Split key and value
            if (std::getline(keyValueStream, key, '=') &&
                std::getline(keyValueStream, value)) {
                setenv(key.c_str(), value.c_str(), 1); // Set environment variable
            }
        }
    }
};

#endif // CONFIG_H
