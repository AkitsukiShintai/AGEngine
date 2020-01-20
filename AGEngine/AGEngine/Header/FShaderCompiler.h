#pragma once

class File {
public:
	static std::string loadTextFile(::std::string const& name);
	static std::vector<char> loadByteFile(const char* name);
	//suffix should include '.'
	static bool validateSuffix(::std::string file_name, ::std::string suffix);
	static std::string getSuffix(::std::string file_name);
	static void getFiles(::std::string _path, ::std::vector<::std::string>& _files);
	static std::string getAbsoluteDirectory(::std::string _path);
};


enum class ECompileShaderType : uint32_t
{
	COMPILE_SHADER_TYPE_VERTEX = 0,
	COMPILE_SHADER_TYPE_TESSCTR,
	COMPILE_SHADER_TYPE_TESSEVL,
	COMPILE_SHADER_TYPE_GEOMETRY,
	COMPILE_SHADER_TYPE_FRAGMENT,
	COMPILE_SHADER_TYPE_COMPUTE,
	COMPILE_SHADER_TYPE_MAX,
};
class ShaderCompiler :public Singleton<ShaderCompiler>
{
public:
	void InitCompiler();
	void ReleaseCompiler();
	void AddIncludePath(std::string const& _path);
	std::string PreprocessGLSLShader(const std::string& _source_name, VkShaderStageFlagBits _shader_type, const std::string& _raw_source);
	std::vector<uint32_t> CompileGLSLShader(const std::string& _source_name, VkShaderStageFlagBits _shader_type, const std::string& _raw_source, bool optimize = true);
	void AddMacro(const std::string& name, const std::string& value);

private:
	friend Singleton<ShaderCompiler>;
	ShaderCompiler() {
		InitCompiler();
		AddMacro("BUILT_IN_MAX_BONE_PER_VERTEX", std::to_string(4));
		AddMacro("SUBPASS_INPUT_BINDPOINT( SET , ID )", "layout(set = SET, input_attachment_index = ID , binding = ID ) uniform subpassInput ");
		//AddMacro("TEXTURE_INPUTS( SET )", "layout(set = SET, binding = 0 ) uniform sampler2D Textures[" + std::to_string(TEXTURE_NUMBER) + "]");
		//AddMacro("DIRECTIONAL_SHADOW_MAP( SET )", "layout(set = SET, binding = 0 ) uniform sampler2DArray DirectionalShadowMaps[" + std::to_string(MAX_DIRECTIONAL_LIGHT_SHADOW_NUM) + "]");
		AddIncludePath("../Resource/shader/include/");	
	};

};