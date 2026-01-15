#pragma once

#include "ShaderGroupBase.hpp"
#include "client/app/AppPlatformListener.hpp"

namespace mce
{
    class ShaderGroup : public ShaderGroupBase, public AppPlatformListener
    {
    protected:
        static ShaderGroup* singletonPtr;

    public:
        ShaderGroup();
        ~ShaderGroup();

    protected:
        void _clearShaders();
        void _clearPrograms();
    
    public:
        void onAppSuspended() override;

        ShaderProgram& getShaderProgram(ShaderType shaderType, const std::string& codeOrPath, const std::string& header, const std::vector<std::string>& resourcepacks);
        void processIncludeDirectives(const std::string& path, std::string& code, const std::vector<std::string>& resourcepacks);
        Shader& loadShader(const std::string& header, const std::string& vertexCodeOrPath, const std::string& fragmentCodeOrPath, const std::string& geometryCodeOrPath, const std::vector<std::string>& resourcepacks);

        static ShaderGroup* singleton();
    };
}
