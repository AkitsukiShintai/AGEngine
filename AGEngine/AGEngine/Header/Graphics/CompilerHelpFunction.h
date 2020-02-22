#pragma once

static std::string ProcessingForAttribute(std::string _src, std::vector<VkVertexInputBindingDescription>& inputBinding, std::vector<VkVertexInputAttributeDescription>& inputAttributes)
{
	std::string return_val("");
	uint32_t location_counter = 0;
	//default mode is per vertex input
	VkVertexInputRate _mode = VK_VERTEX_INPUT_RATE_VERTEX;
	VkVertexInputBindingDescription* describer = nullptr;

	bool new_describer = true;
	std::regex vertex_mode_pattern("^\\s*#pragma\\s+pervertex\\s*$");
	std::regex instance_mode_pattern("^\\s*#pragma\\s+perinstance\\s*$");
	std::regex pattern("^\\s*in\\s+(\\S+).*$");

	std::istringstream _stream(_src);
	std::string line;
	std::smatch _match;
	size_t binding = -1;
	while (std::getline(_stream, line))
	{
		if (regex_match(line, vertex_mode_pattern))
		{
			_mode = VK_VERTEX_INPUT_RATE_VERTEX;
			new_describer = true;
		}
		else if (regex_match(line, instance_mode_pattern))
		{
			_mode = VK_VERTEX_INPUT_RATE_INSTANCE;
			new_describer = true;
		}
		else if (regex_match(line, _match, pattern))
		{
			if (new_describer)
			{
				binding++;
				inputBinding.push_back({});
				describer = {&(*inputBinding.rbegin())};
				describer->inputRate = _mode;
				describer->binding = binding;
				new_describer = false;
			}
			uint32_t data_type = 0;
			VkFormat foramt = VK_FORMAT_R32_SFLOAT;
			std::string type_name = _match[1];
			if (type_name == "int")
			{
				data_type = sizeof(int);
				foramt = VK_FORMAT_R32_SINT;
			}
			else if (type_name == "uint")
			{
				data_type = sizeof(unsigned int);
				foramt = VK_FORMAT_R32_UINT;
			}
			else if (type_name == "float")
			{
				data_type = sizeof(float);
				foramt = VK_FORMAT_R32_SFLOAT;
			}
			else if (type_name == "vec2")
			{
				data_type = sizeof(float) * 2;
				foramt = VK_FORMAT_R32G32_SFLOAT;
			}
			else if (type_name == "vec3")
			{
				data_type = sizeof(float) * 3;
				foramt = VK_FORMAT_R32G32B32_SFLOAT;
			}
			else if (type_name == "vec4")
			{
				data_type = sizeof(float) * 4;
				foramt = VK_FORMAT_R32G32B32A32_SFLOAT;
			}
			else if (type_name == "ivec2")
			{
				data_type = sizeof(int) * 2;
				foramt = VK_FORMAT_R32G32_SINT;
			}
			else if (type_name == "ivec3")
			{
				data_type = sizeof(int) * 3;
				foramt = VK_FORMAT_R32G32B32_SINT;

			}
			else if (type_name == "ivec4")
			{
				data_type = sizeof(int) * 4;
				foramt = VK_FORMAT_R32G32B32A32_SINT;
			}
			else if (type_name == "uvec2")
			{
				data_type = sizeof(unsigned int) * 2;
				foramt = VK_FORMAT_R32G32_UINT;
			}
			else if (type_name == "uvec3")
			{
				data_type = sizeof(unsigned int) * 3;
				foramt = VK_FORMAT_R32G32B32_UINT;
			}
			else if (type_name == "uvec4")
			{
				data_type = sizeof(unsigned int) * 4;
				foramt = VK_FORMAT_R32G32B32A32_UINT;
			}
			else if (type_name == "mat4")
			{
				data_type = sizeof(float)*16;
				foramt = VK_FORMAT_R32G32B32A32_UINT;
				std::cerr << "Unsupported attribute type! typename " << type_name.c_str() << std::endl;
			}
			else
			{
				std::cerr << "Unsupported attribute type! typename " << type_name.c_str()<<std::endl;
				//FDebug::reportErr("Unsupported attribute type!");
				return "";
			}

			VkVertexInputAttributeDescription attributeDes = {};
			attributeDes.binding = binding;
			attributeDes.location = location_counter;
			attributeDes.format = foramt;
			attributeDes.offset = describer->stride;
			inputAttributes.push_back(attributeDes);

			describer->stride += data_type;
			
			line = std::string("layout(location = ") + std::to_string(location_counter) + ") " + line;
			++location_counter;
		}
		return_val += "\n" + line;
	}
	return return_val;
}

static void SearchingForUniform(std::string _src, VkShaderStageFlags _shader_type, std::map<uint32_t, VkDescriptorSetLayoutBinding>& bindings)
{
	//bindings.clear();
	std::regex pattern("^\\s*layout\\s*\\(.*(?:set\\s*=\\s*(\\d+)\\s*,)?.*binding\\s*=\\s*(\\d+).*\\)\\s*(\\S+)\\s+(?:coherent\\s+)?(?:volatile\\s+)?(?:readonly\\s+)?(?:writeonly\\s+)?(\\S+).*$");
	std::smatch match_data;
	std::string remain_str = _src;
	while (regex_search(remain_str, match_data, pattern))
	{
		std::string set_name = match_data[1];
		uint32_t set = 0;
		if (set_name != "")
		{
			set = std::stoi(set_name);
		}
		if (set == 0)
		{
			uint32_t binding = static_cast<uint32_t>(std::stoi(match_data[2]));
			std::string _type_name = match_data[3];
			std::string _subtype_name = match_data[4];
			bool isBuffer = true;
			VkDescriptorType _binding_type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			if (_type_name == "uniform")
			{
				if (_subtype_name == "sampler2D" || _subtype_name == "sampler2DMS" || _subtype_name == "sampler3D")
				{
					_binding_type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
					isBuffer = false;
				}
				else if (_subtype_name == "image2D" || _subtype_name == "image3D")
				{
					_binding_type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
					isBuffer = false;
				}
				else//treat as uniform buffer
				{
					_binding_type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				}
			}
			else if (_type_name == "buffer")
			{
				_binding_type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			}
			auto find = bindings.find(binding);
			if (find != bindings.end())
			{
				if (_binding_type)
				{
					std::cerr << "Two different resources are using same binding!"<<std::endl;
					//FDebug::reportErr("Two different resources are using same binding!");
					return;
				}
				find->second.stageFlags |= _shader_type;
			}
			else
			{
				VkDescriptorSetLayoutBinding new_binding = {};
				new_binding.binding = binding;
				new_binding.descriptorCount = 1;
				new_binding.descriptorType = _binding_type;
				new_binding.stageFlags = _shader_type;
				bindings.insert(std::make_pair(binding, new_binding));
			}
		}
		remain_str = match_data.suffix().str();
	}
}


