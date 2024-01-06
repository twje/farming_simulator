#pragma once

// Includes
//------------------------------------------------------------------------------
// Core
#include "Core/AssetManager.h"
#include "Core/Support.h"

// Third party
#include "SFML/Graphics.hpp"

// System
#include <string>
#include <fstream>

//------------------------------------------------------------------------------
class Shader : public Asset
{
public:
    Shader(const std::string& filepath)
    {
        std::string source = ReadFile(filepath);        
        auto shaderSources = PreProcess(source);
                                        
        bool hasFragmentShader = shaderSources.find(sf::Shader::Fragment) != shaderSources.end();
        bool hasVertexShader = shaderSources.find(sf::Shader::Vertex) != shaderSources.end();

        // Throw an exception if neither shader type is present
        if (!hasFragmentShader && !hasVertexShader) 
        {
            throw std::runtime_error("No valid shader found in file: " + filepath);
        }

        if (hasFragmentShader && hasVertexShader) 
        {
            if (!mInternalShader.loadFromMemory(shaderSources[sf::Shader::Vertex], shaderSources[sf::Shader::Fragment]))
            {
                throw std::runtime_error("Failed to compile both fragment and vertex shaders: " + filepath);
            }
            return;
        }

        if (hasFragmentShader) 
        {
            if (!mInternalShader.loadFromMemory(shaderSources[sf::Shader::Fragment], sf::Shader::Fragment))
            {
                throw std::runtime_error("Failed to compile fragment shader: " + filepath);
            }
            return;
        }

        if (hasVertexShader) 
        {
            if (!mInternalShader.loadFromMemory(shaderSources[sf::Shader::Vertex], sf::Shader::Vertex))
            {
                throw std::runtime_error("Failed to compile vertex shader: " + filepath);
            }
        }
    }

    const sf::Shader& GetInternalShader() const { return mInternalShader; }

private:    
    std::unordered_map<sf::Shader::Type, std::string> PreProcess(const std::string& source)
    {
        std::unordered_map<sf::Shader::Type, std::string> shaderSources;

        const char* typeToken = "#type";
        size_t pos = source.find(typeToken, 0);

        while (pos != std::string::npos)
        {
            size_t eol = source.find_first_of("\r\n", pos);
            if (eol == std::string::npos) 
            {
                throw std::runtime_error("PreProcess error: Syntax error, end of line not found.");
            }

            size_t begin = pos + strlen(typeToken) + 1;
            std::string type = source.substr(begin, eol - begin);
            Trim(type);

            size_t nextLinePos = FindNextLine(source, eol);
            pos = source.find(typeToken, nextLinePos);

            if (pos == std::string::npos) 
            {
                // Store the remaining string as shader code for the current type.
                shaderSources[ShaderTypeFromString(type)] = source.substr(nextLinePos);
            }
            else 
            {
                // Store the string up to the next shader type as shader code for the current type.
                shaderSources[ShaderTypeFromString(type)] = source.substr(nextLinePos, pos - nextLinePos);
            }            
        }

        return shaderSources;
    }

    sf::Shader::Type ShaderTypeFromString(const std::string& shaderType)
    {
        if (shaderType == "vertex")
        {
            return sf::Shader::Type::Vertex;
        }

        if (shaderType == "fragment")
        {
            return sf::Shader::Type::Fragment;
        }

        throw std::runtime_error("Invalid shader type: " + shaderType);
    }

    sf::Shader mInternalShader;
};

//------------------------------------------------------------------------------
class ShaderLoader : public AssetLoader<Shader>
{
public:
    virtual std::unique_ptr<Asset> Load(AssetFileDescriptor<Shader> descriptor) override
    {
        return std::make_unique<Shader>(descriptor.GetFilePath());
    }
};