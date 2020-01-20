#include "PCH.h"
#include "FShaderCompiler.h"
#include <shaderc.hpp>
#include <io.h>
using namespace shaderc;

	std::string File::loadTextFile(std::string const& name)
	{
		std::string return_val = "", line;
		std::ifstream file(name);
		if (!file)
		{
			std::cerr <<"Cannot Load Text File!:\t" + std::string(name)<<std::endl;
		}
		while (getline(file, line))
		{
			return_val += line + "\n";
		};
		file.close();
		return return_val;
	}

	std::vector<char> File::loadByteFile(const char* name)
	{
		std::ifstream file(name, std::ios::ate | std::ios::binary);
		if (!file.is_open()) {
			std::cerr <<"Cannot Load Binary File!:\t" + std::string(name)<<std::endl;
		}
		size_t file_size = (size_t)file.tellg();
		std::vector<char> return_val(file_size);

		file.seekg(0);
		file.read(return_val.data(), file_size);

		file.close();
		return return_val;
	}

	bool File::validateSuffix(std::string file_name, std::string suffix)
	{
		size_t pos = file_name.find_last_of('.');
		if (pos < 0)
		{
			return false;
		}
		return file_name.substr(pos) == suffix;
	}

	std::string File::getSuffix(std::string file_name)
	{
		size_t pos = file_name.find_last_of('.');
		if (pos < 0)
		{
			return "";
		}
		return file_name.substr(pos);
	}


	std::string& replace_all(std::string& str, const   std::string& old_value, const   std::string& new_value)
	{
		while (true) {
			std::string::size_type   pos(0);
			if ((pos = str.find(old_value)) != std::string::npos)
				str.replace(pos, old_value.length(), new_value);
			else   break;
		}
		return   str;
	}

	void File::getFiles(std::string _path, std::vector<std::string>& _files)
	{
		//file handle
		intptr_t hFile = 0;
		//file data
		struct _finddata_t fileinfo;
		std::string p;
		(hFile = _findfirst(p.assign(_path).append("*.*").c_str(), &fileinfo));
		if (hFile != -1L)
		{
			do {
				//recursive find if needed
				if ((fileinfo.attrib & _A_SUBDIR))
				{
					if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
						getFiles(p.assign(_path).append(fileinfo.name).append("/"), _files);
				}
				else
				{
					_files.push_back(
						p.assign(_path).append(fileinfo.name));
				}
			} while (_findnext(hFile, &fileinfo) == 0);
			_findclose(hFile);
		}
	}

	std::string File::getAbsoluteDirectory(::std::string _path)
	{
		std::string tmp;
		std::vector<std::string>splits;
		for (size_t i = 0; i < _path.size(); ++i)
		{
			if (_path[i] == '/' || _path[i] == '\\')
			{
				if (tmp == ".." && splits.size() > 0 && (*splits.rbegin()) != "..")
				{
					splits.pop_back();
				}
				else
				{
					splits.push_back(tmp);
				}
				tmp = "";
			}
		}
		tmp = "";
		if (splits.size() > 0) {
			tmp = splits[0];
			for (int i = 1; i < splits.size(); ++i)
			{
				tmp += splits[i];
			}
		}
		return tmp;
	}


	class CShaderIncluder
	{
	public:
		std::set<std::string> mIncludeSrcs;
		const std::string VoidVal = "";
		bool inited = false;
		void AddPath(std::string const& _str)
		{
			inited = false;
			if (*(_str.rbegin()) != '/')
			{
				mIncludeSrcs.insert(_str + '/');
				return;
			}
			mIncludeSrcs.insert(_str);
		}
		struct data_struct
		{
			shaderc_include_result _result;
			std::string data;
			data_struct() : _result(), data("") {}
		};
		std::vector<std::map<std::string, data_struct>> mLibrary;
		std::map<std::string, data_struct> mRLibrary;

		void InitLibrary()
		{
			if (!inited) 
			{
				mLibrary.resize(mIncludeSrcs.size());
				size_t id = 0;
				for (std::string const& _src : mIncludeSrcs)
				{
					mLibrary[id].clear();
					std::vector<std::string> files;
					File::getFiles(_src, files);
					for (std::string const& _path : files)
					{
						mLibrary[id].insert(std::make_pair(_path, data_struct()));
					}
					++id;
				}
				inited = true;
			}
		}
		shaderc_include_result* getRelativeData(std::string const& _request_path, std::string _name)
		{
			std::string file = _request_path + "/" + _name;
			file = File::getAbsoluteDirectory(file);
			auto findr = mRLibrary.find(file);
			if (findr != mRLibrary.end())
			{
				return &findr->second._result;
			}
			std::string src = File::loadTextFile(file);
			if (src != "")
			{
				data_struct new_data;
				new_data.data = src;
				new_data._result.content = src.c_str();
				new_data._result.content_length = src.size();
				new_data._result.source_name = file.c_str();
				new_data._result.source_name_length = file.length();
				mRLibrary.insert(std::make_pair(file, new_data));
				return &mRLibrary[file]._result;
			}
			return nullptr;
		}

		shaderc_include_result *getData(std::string _name)
		{
			InitLibrary();
			size_t id = 0;
			for (std::string const& _src : mIncludeSrcs)
			{
				std::string name = _src  + _name;
				auto itr = mLibrary[id].find(name);
				if (itr != mLibrary[id].end())
				{
					if (itr->second.data == "")
					{
						itr->second.data = File::loadTextFile(name);
						itr->second._result.content = itr->second.data.c_str();
						itr->second._result.content_length = itr->second.data.size();
						itr->second._result.source_name = itr->first.c_str();
						itr->second._result.source_name_length = itr->first.length();
					}
					return &itr->second._result;
				}
				++id;
			}
			return nullptr;
		}
	};

	static shaderc::CompileOptions* pOptions;
	static CShaderIncluder* pShaderIncluder;
	class Includer : public shaderc::CompileOptions::IncluderInterface
	{
		public:
			// Handles shaderc_include_resolver_fn callbacks.
			virtual shaderc_include_result* GetInclude(const char* requested_source,
				shaderc_include_type type,
				const char* requesting_source,
				size_t include_depth) 
			{
				std::string requesting_path(requesting_source);
				if (include_depth == 1)
				{
					requesting_path = requesting_path.substr(0, requesting_path.find_last_of('/'));
				}
				if (type == shaderc_include_type_standard)
				{
					return pShaderIncluder->getData(requested_source);
				}
				else if(type == shaderc_include_type_relative)
				{
					return pShaderIncluder->getRelativeData(requesting_path, requested_source);
				}
				return nullptr;
			}

			// Handles shaderc_include_result_release_fn callbacks.
			virtual void ReleaseInclude(shaderc_include_result* data)
			{

			}
	};

	std::string preprocess_shader(const std::string& source_name,
		shaderc_shader_kind kind,
		const std::string& source) {
		shaderc::Compiler compiler;
		
		shaderc::PreprocessedSourceCompilationResult result =
			compiler.PreprocessGlsl(source, kind, source_name.c_str(), *pOptions);

		if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
			std::cerr << result.GetErrorMessage();
			return "";
		}

		return { result.cbegin(), result.cend() };
	}

	std::vector<uint32_t> compile_file(const std::string& source_name,
		shaderc_shader_kind kind,
		const std::string& source,
		bool optimize = false) {
		shaderc::Compiler compiler;
		//shaderc::CompileOptions options;

		if (optimize) pOptions->SetOptimizationLevel(shaderc_optimization_level_size);

		shaderc::SpvCompilationResult module =
			compiler.CompileGlslToSpv(source, kind, source_name.c_str(), *pOptions);

		if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
			std::cerr << module.GetErrorMessage();
			return std::vector<uint32_t>();
		}

		return { module.cbegin(), module.cend() };
	}

	std::map<VkShaderStageFlagBits, shaderc_shader_kind> shader_kind_table =
	{
		{VK_SHADER_STAGE_VERTEX_BIT,shaderc_vertex_shader},
		{VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT,shaderc_tess_control_shader},
		{VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT,shaderc_tess_evaluation_shader},
		{VK_SHADER_STAGE_GEOMETRY_BIT,shaderc_geometry_shader},
		{VK_SHADER_STAGE_FRAGMENT_BIT,shaderc_fragment_shader},
		{VK_SHADER_STAGE_COMPUTE_BIT,shaderc_compute_shader}
	};

	void ShaderCompiler::InitCompiler()
	{
		pOptions = new shaderc::CompileOptions();
		pShaderIncluder = new CShaderIncluder();
		pOptions->SetIncluder(std::unique_ptr<shaderc::CompileOptions::IncluderInterface>(new Includer()));
	}
	void ShaderCompiler::ReleaseCompiler()
	{
		delete pShaderIncluder;
		delete pOptions;
	}
	void ShaderCompiler::AddIncludePath(std::string const&_path)
	{
		pShaderIncluder->AddPath(_path);
	}
	std::string ShaderCompiler::PreprocessGLSLShader(const std::string& _source_name, VkShaderStageFlagBits _shader_type, const std::string& _raw_source)
	{
		return preprocess_shader(_source_name, shader_kind_table[_shader_type], _raw_source);
	}

	std::vector<uint32_t> ShaderCompiler::CompileGLSLShader(const std::string& _source_name, VkShaderStageFlagBits _shader_type, const std::string& _raw_source, bool optimize)
	{
		//std::string processed = preprocess_shader(_source_name, shader_kind_table[_shader_type], _raw_source);
		//std::cout << processed << std::endl;
		return compile_file(_source_name, shader_kind_table[_shader_type], _raw_source, optimize);
	}

	void ShaderCompiler::AddMacro(const std::string& name, const std::string& value)
	{
		if (value == "") {
			pOptions->AddMacroDefinition(name);
		}
		else
		{
			pOptions->AddMacroDefinition(name, value);
		}
	}
