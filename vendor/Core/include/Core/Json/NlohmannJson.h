#pragma once

// Includes
//------------------------------------------------------------------------------
// Core
#include "Core/Json/IJson.h"

// Third party
#include <nlohmann/json.hpp> 

// System
#include <string>
#include <string_view>
#include <map>
#include <vector>
#include <memory>
#include <filesystem>
#include <fstream>
#include <cassert>
#include <iostream>

namespace test
{
    class NlohmannJson : public test::IJson
    {
    public:
        inline NlohmannJson() = default;
        inline explicit NlohmannJson(nlohmann::json* json)
            : mJson{ json }
        { }

        IJson& operator[](std::string_view key) override
        {
            if (mArrayCache.count(key.data()) == 0)
            {
                mArrayCache[key.data()] = std::make_unique<NlohmannJson>(&mJson->operator[](key.data()));
            }
            return *mArrayCache[key.data()].get();
        }

        IJson& At(std::string_view key) override
        {
            return operator[](key.data());
        }

        IJson& At(size_t pos) override
        {
            assert(false && "At(size_t) not implemented");
            static NlohmannJson dummy;
            return dummy; // Returning a dummy object
        }

        std::vector<std::unique_ptr<IJson>> Array() override
        {
            assert(false && "Array() not implemented");
            return {}; // Returning an empty vector
        }

        std::vector<std::unique_ptr<IJson>>& Array(std::string_view key) override
        {
            if (mArrayListDataCache.count(key.data()) == 0)
            {
                if (mJson->count(key.data()) > 0 && mJson->operator[](key.data()).is_array())
                {
                    std::for_each(mJson->operator[](key.data()).begin(), mJson->operator[](key.data()).end(), [&](nlohmann::json& item)
                    {
                        nlohmann::json* ptr = &item;
                        mArrayListDataCache[key.data()].emplace_back(std::make_unique<NlohmannJson>(ptr));
                    });
                }
            }

            return mArrayListDataCache[key.data()];
        }

        size_t Size() const override
        {
            assert(false && "Size not implemented");
            return 0; // Returning a default size
        }

        bool Parse(const std::filesystem::path& filepath) override
        {
            ClearCache();
            mData = nullptr;
            mJson = nullptr; // check if okay

            if (fs::exists(filepath) && fs::is_regular_file(filepath))
            {
                mDirectory = filepath.parent_path();
                mData = std::make_unique<nlohmann::json>();

                std::ifstream inputFileStream(filepath.generic_string());
                try
                {
                    inputFileStream >> *mData;
                    mJson = mData.get();
                }
                catch (const nlohmann::json::parse_error& error)
                {
                    std::string message = "Parse error: ";
                    message += std::string(error.what());
                    message += std::string("\n");
                    std::cerr << message;
                    return false;
                }
                return true;                
            }
            return false;                     
        }

        bool Parse(const void* data, size_t size) override
        {
            ClearCache();
            assert(false && "Parse(data, size) not implemented");
            return false; // Indicating failure to parse
        }

        size_t Count(std::string_view key) const override
        {
            return mJson->count(key);
        }

        bool Any(std::string_view key) const override
        {
            assert(false && "Any not implemented");
            return false; // Returning a default boolean
        }

        bool IsArray() const override
        {
            return mJson->is_array();
        }

        bool IsObject() const override
        {
            assert(false && "IsObject not implemented");
            return false; // Returning a default boolean
        }

        bool IsNull() const override
        {
            assert(false && "IsNull not implemented");
            return true; // Returning a default boolean
        }

        fs::path GetDirectory() const override
        {
            return mDirectory;
        }

        void SetDirectory(fs::path filepath) override
        {
            mDirectory = filepath;
        }

        std::unique_ptr<IJson> Create() override
        {
            assert(false && "Create not implemented");
            return std::make_unique<NlohmannJson>(); // Returning a new instance
        }

    protected:
        int32_t GetInt32(std::string_view key) override
        {
            assert(false && "GetInt32 not implemented");
            return 0; // Returning a default valu
        }

        uint32_t GetUInt32(std::string_view key) override
        {
            assert(false && "GetUInt32 not implemented");
            return 0; // Returning a default value
        }

        int64_t GetInt64(std::string_view key) override
        {
            assert(false && "GetInt64 not implemented");
            return 0; // Returning a default value
        }

        uint64_t GetUInt64(std::string_view key) override
        {
            assert(false && "GetUInt64 not implemented");
            return 0; // Returning a default value
        }

        double GetDouble(std::string_view key) override
        {
            assert(false && "GetDouble not implemented");
            return 0.0; // Returning a default value
        }

        float GetFloat(std::string_view key) override
        {
            assert(false && "GetFloat not implemented");
            return 0.0f; // Returning a default value
        }

        std::string GetString(std::string_view key) override
        {
            assert(false && "GetString not implemented");
            return ""; // Returning a default value
        }

        bool GetBool(std::string_view key) override
        {
            assert(false && "GetBool not implemented");
            return false; // Returning a default value
        }

        int32_t GetInt32() override
        {
            assert(false && "GetInt32() without parameter not implemented");
            return 0; // Returning a default value
        }

        uint32_t GetUInt32() override
        {
            return mJson->get<uint32_t>();
        }

        int64_t GetInt64() override
        {
            assert(false && "GetInt64() without parameter not implemented");
            return 0; // Returning a default value
        }

        uint64_t GetUInt64() override
        {
            assert(false && "GetUInt64() without parameter not implemented");
            return 0; // Returning a default value
        }

        double GetDouble() override
        {
            assert(false && "GetDouble() without parameter not implemented");
            return 0.0; // Returning a default value
        }

        float GetFloat() override
        {
            assert(false && "GetFloat() without parameter not implemented");
            return 0.0f; // Returning a default value
        }

        std::string GetString() override
        {
            return mJson->get<std::string>();
        }

        bool GetBool() override
        {
            assert(false && "GetBool() without parameter not implemented");
            return false; // Returning a default value
        }

    private:
        inline void ClearCache()
        {
            mArrayCache.clear();
            //m_arrayPosCache.clear();
            mArrayListDataCache.clear();
        }


        // Cache
        std::map<std::string, std::unique_ptr<test::NlohmannJson>> mArrayCache;
        std::map<std::string, std::vector<std::unique_ptr<IJson>>> mArrayListDataCache;

        std::unique_ptr<nlohmann::json> mData = nullptr; // only used if this is the owner json
        nlohmann::json* mJson = nullptr;
        fs::path mDirectory;
    };
}
