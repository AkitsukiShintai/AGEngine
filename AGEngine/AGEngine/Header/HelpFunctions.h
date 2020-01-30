#pragma once
#include <glm.hpp>
#include <iostream>
#include <string>
static void Print(const glm::vec2& value, std::string name="") {
	std::cout << name+ ": " << value.x << " " << value.y  << std::endl;
}

static void Print(const glm::vec3& value, std::string name = "") {
	std::cout << name + ": " << value.x << " " << value.y <<" " << value.z << std::endl;
}