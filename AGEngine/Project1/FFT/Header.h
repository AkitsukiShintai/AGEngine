#pragma once
#include <glm/glm.hpp>
#include "Grid.h"
template<class T>
glm::vec2 frequencyVector(const Grid<T>& B, int p, int q) {
	assert(p >= 0 && p <= B.cellWidth());
	assert(q >= 0 && q <= B.cellHeight());

	int gridWidth = B.width();//L
	int gridHeight = B.height();

	int midW = B.cellWidth() / 2;// N/2
	int midH = B.cellHeight() / 2;
	glm::vec2 result;
	if (p> midW)
	{
		result.x = (float)p / gridWidth;
	}
	else
	{
		result.x = (float)(p- B.cellWidth()) / gridWidth;
	}

	if (q > midH)
	{
		result.y = (float)q / gridHeight;
	}
	else
	{
		result.y = (float)(q - B.cellHeight()) / gridHeight;
	}

	return result;
}