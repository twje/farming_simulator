#pragma once

#include <vector>
#include <string_view>
#include <memory>
#include <filesystem>
#include <cassert>
#include <type_traits>

namespace fs = std::filesystem;

namespace test
{
	class IJson
	{
	public:

		virtual IJson& operator[](std::string_view key) = 0;
		virtual IJson& At(std::string_view key) = 0;
		virtual IJson& At(size_t pos) = 0;
		/*!
		 * If current json object is an array, this will get all elements of it!
		 * @return An array
		 */
		[[nodiscard]] virtual std::vector<std::unique_ptr<IJson>> Array() = 0;
		[[nodiscard]] virtual std::vector<std::unique_ptr<IJson>>& Array(std::string_view key) = 0;
		/*!
		 * Get the size of an object. This will be equal to the number of
		 * variables an object contains.
		 * @return
		 */
		[[nodiscard]] virtual size_t Size() const = 0;
		[[nodiscard]] virtual bool Parse(const fs::path& path) = 0;
		[[nodiscard]] virtual bool Parse(const void* data, size_t size) = 0;

		template<typename T>
		bool TryGet(const std::string& key, T& outValue)
		{
			if (Count(key) > 0)
			{
				outValue = At(key).Get<T>();
				return true;
			}
			else
			{
				return false;
			}
		}

		template <typename T>
		[[nodiscard]] T Get(std::string_view key);
		template <typename T>
		[[nodiscard]] T Get();
		[[nodiscard]] virtual size_t Count(std::string_view key) const = 0;
		[[nodiscard]] virtual bool Any(std::string_view key) const = 0;
		[[nodiscard]] virtual bool IsArray() const = 0;
		[[nodiscard]] virtual bool IsObject() const = 0;
		[[nodiscard]] virtual bool IsNull() const = 0;

		/*!
		 * Get the directory where the json was loaded.
		 * Only assigned if json is parsed by file.
		 * @return
		 */
		[[nodiscard]] virtual fs::path GetDirectory() const = 0;		
		virtual void SetDirectory(fs::path filepath) = 0;

		/*!
		 * Create a new empty instance using the same engine
		*/
		virtual std::unique_ptr<IJson> Create() = 0;

		/*!
		 * Pure virtual class needs virtual destructor so derived classes can call their own destructors
		*/
		virtual ~IJson() = default;

	protected:
		[[nodiscard]] virtual int32_t GetInt32(std::string_view key) = 0;
		[[nodiscard]] virtual uint32_t GetUInt32(std::string_view key) = 0;
		[[nodiscard]] virtual int64_t GetInt64(std::string_view key) = 0;
		[[nodiscard]] virtual uint64_t GetUInt64(std::string_view key) = 0;
		[[nodiscard]] virtual double GetDouble(std::string_view key) = 0;
		[[nodiscard]] virtual float GetFloat(std::string_view key) = 0;
		[[nodiscard]] virtual std::string GetString(std::string_view key) = 0;
		[[nodiscard]] virtual bool GetBool(std::string_view key) = 0;

		[[nodiscard]] virtual int32_t GetInt32() = 0;
		[[nodiscard]] virtual uint32_t GetUInt32() = 0;
		[[nodiscard]] virtual int64_t GetInt64() = 0;
		[[nodiscard]] virtual uint64_t GetUInt64() = 0;
		[[nodiscard]] virtual double GetDouble() = 0;
		[[nodiscard]] virtual float GetFloat() = 0;
		[[nodiscard]] virtual std::string GetString() = 0;
		[[nodiscard]] virtual bool GetBool() = 0;
	};

	template<typename T>
	T IJson::Get(std::string_view key)
	{
		if constexpr (std::is_same<T, double>::value)
			return GetDouble(key);
		if constexpr (std::is_same<T, float>::value)
			return GetFloat(key);
		else if constexpr (std::is_same<T, int32_t>::value)
			return GetInt32(key);
		else if constexpr (std::is_same<T, uint32_t>::value)
			return GetUInt32(key);
		else if constexpr (std::is_same<T, int64_t>::value)
			return GetInt64(key);
		else if constexpr (std::is_same<T, uint64_t>::value)
			return GetUInt64(key);
		else if constexpr (std::is_same<T, std::string>::value)
			return GetString(key);
		else if constexpr (std::is_same<T, bool>::value)
			return GetBool(key);
		else
			return nullptr;
	}

	template<typename T>
	T IJson::Get()
	{
		if constexpr (std::is_same<T, double>::value)
			return GetDouble();
		if constexpr (std::is_same<T, float>::value)
			return GetFloat();
		else if constexpr (std::is_same<T, int32_t>::value)
			return GetInt32();
		else if constexpr (std::is_same<T, uint32_t>::value)
			return GetUInt32();
		else if constexpr (std::is_same<T, int64_t>::value)
			return GetInt64();
		else if constexpr (std::is_same<T, uint64_t>::value)
			return GetUInt64();
		else if constexpr (std::is_same<T, std::string>::value)
			return GetString();
		else if constexpr (std::is_same<T, bool>::value)
			return GetBool();
		else
			return nullptr;
	};
}