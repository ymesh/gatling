//
// Copyright (C) 2019-2022 Pablo Delgado Krämer
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.
//

#include "GlslangShaderCompiler.h"

#include <glslang/Public/ShaderLang.h>
#include <glslang/Public/ResourceLimits.h>
#include <SPIRV/GlslangToSpv.h>

#include <fstream>

namespace detail
{
  class FileIncluder : public glslang::TShader::Includer
  {
  private:
    fs::path m_rootPath;

  public:
    FileIncluder(const fs::path& rootPath)
      : m_rootPath(rootPath)
    {
    }

    IncludeResult* includeSystem(const char* headerName,
                                 const char* includerName,
                                 size_t inclusionDepth) override
    {
      // There's no reason to support this right now.
      return nullptr;
    }

    IncludeResult* includeLocal(const char* headerName,
                                const char* includerName,
                                size_t inclusionDepth) override
    {
      fs::path filePath = m_rootPath / headerName;

      std::ifstream fileStream(filePath.c_str(), std::ios_base::binary | std::ios_base::ate);
      if (!fileStream.is_open())
      {
        std::string pathStr = filePath.string();
        fprintf(stderr, "Failed to find shader include '%s'\n", pathStr.c_str());
        return nullptr;
      }

      size_t textLength = fileStream.tellg();
      char* text = new char[textLength];
      fileStream.seekg(0, std::ios::beg);
      fileStream.read(text, textLength);
      return new IncludeResult(headerName, text, textLength, text);
    }

    void releaseInclude(IncludeResult* result) override
    {
      if (!result)
      {
        return;
      }
      delete[] static_cast<char*>(result->userData);
      delete result;
    }
  };

  using ShaderStage = gi::sg::GlslangShaderCompiler::ShaderStage;

  EShLanguage getGlslangShaderLanguage(ShaderStage stage)
  {
    switch (stage)
    {
    case ShaderStage::AnyHit:     return EShLangAnyHit;
    case ShaderStage::ClosestHit: return EShLangClosestHit;
    case ShaderStage::Compute:    return EShLangCompute;
    case ShaderStage::Miss:       return EShLangMiss;
    case ShaderStage::RayGen:     return EShLangRayGen;
    default:
      assert(false);
      return EShLangCount;
    }
  }
}

namespace gi::sg
{
  static bool s_glslangInitialized = false;

  bool GlslangShaderCompiler::init()
  {
    if (s_glslangInitialized)
    {
      return true;
    }
    return glslang::InitializeProcess();
  }

  void GlslangShaderCompiler::deinit()
  {
    if (!s_glslangInitialized)
    {
      return;
    }
    glslang::FinalizeProcess();
  }

  GlslangShaderCompiler::GlslangShaderCompiler(const fs::path& shaderPath)
    : m_fileIncluder(new detail::FileIncluder(shaderPath))
  {
  }

  GlslangShaderCompiler::~GlslangShaderCompiler()
  {
    delete (detail::FileIncluder*) m_fileIncluder;
  }

  bool GlslangShaderCompiler::compileGlslToSpv(ShaderStage stage,
                                               std::string_view source,
                                               std::vector<uint8_t>& spv)
  {
    EShLanguage language = detail::getGlslangShaderLanguage(stage);

    glslang::TShader shader(language);

    const char* sources[] = { source.data() };
    const int sourceLengths[] = { static_cast<int>(source.length()) };
    shader.setStringsWithLengths(sources, sourceLengths, 1);
    shader.setEntryPoint("main");
    shader.setEnvClient(glslang::EShClientVulkan, glslang::EshTargetClientVersion::EShTargetVulkan_1_1);
    shader.setEnvTarget(glslang::EShTargetLanguage::EShTargetSpv, glslang::EShTargetLanguageVersion::EShTargetSpv_1_4);
    shader.setEnvInput(glslang::EShSourceGlsl, language, glslang::EShClient::EShClientVulkan, 450);

    EShMessages messages = static_cast<EShMessages>(EShMsgVulkanRules | EShMsgSpvRules);
#ifndef NDEBUG
    messages = static_cast<EShMessages>(messages | EShMsgDebugInfo);
#endif

    auto printErrorMessage = [&shader](const char* baseFmtString) {
      fprintf(stderr, baseFmtString, shader.getInfoLog());
#ifndef NDEBUG
      const char* debugInfoLog = shader.getInfoDebugLog();
      if (strlen(debugInfoLog) > 0)
      {
        fprintf(stderr, " (%s)", debugInfoLog);
      }
#endif
      fprintf(stderr, "\n");
    };

    const TBuiltInResource* resourceLimits = GetDefaultResources(); // TODO: do we want to use the actual device limits?
    int defaultVersion = 450; // Will be overriden by #version in source.
    bool forwardCompatible = false;
    auto fileIncluder = *reinterpret_cast<detail::FileIncluder*>(m_fileIncluder);

    bool success = shader.parse(resourceLimits, defaultVersion, forwardCompatible, messages, fileIncluder);
    if (!success)
    {
      printErrorMessage("Failed to compile shader: %s");
      return false;
    }

    glslang::TProgram program;
    program.addShader(&shader);

    success = program.link(messages);
    if (!success)
    {
      printErrorMessage("Failed to link program: %s");
      return false;
    }

    glslang::SpvOptions spvOptions;
#ifdef NDEBUG
    spvOptions.stripDebugInfo = true;
#else
    spvOptions.generateDebugInfo = true;
    spvOptions.validate = true;
#endif

    glslang::TIntermediate* intermediate = program.getIntermediate(language);
    glslang::GlslangToSpv(*intermediate, *reinterpret_cast<std::vector<unsigned int>*>(&spv), &spvOptions);
    return true;
  }
}
