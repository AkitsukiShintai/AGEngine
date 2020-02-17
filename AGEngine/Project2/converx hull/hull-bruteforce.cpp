#include "hull-bruteforce.h"
#include <algorithm>
#include <iostream>

bool Point::operator==( Point const& arg2 ) const {
    return ( (x==arg2.x) && (y==arg2.y) );
}

std::ostream& operator<< (std::ostream& os, Point const& p) {
	os << "(" << p.x << " , " << p.y << ") ";
	return os;
}

std::istream& operator>> (std::istream& os, Point & p) {
	os >> p.x >> p.y;
	return os;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//return value is (on left, on right)
std::pair<bool,bool> get_location(
		float const& p1x, //check which side of the line (p1x,p1y)-->(p2x,p2y) 
		float const& p1y, //point (qx,qy) is on
		float const& p2x,
		float const& p2y,
		float const& qx,
		float const& qy
		) 
{
	Point dir   = {p2x-p1x,p2y-p1y};
	Point norm  = {dir.y, -dir.x};
	Point point = {qx-p1x,qy-p1y};
	//scalar product is positive if on right side
	float scal_prod = norm.x*point.x + norm.y*point.y;
	return std::make_pair( (scal_prod<0), (scal_prod>0));
}

//returns a set of indices of points that form convex hull
std::set<int> hullBruteForce ( std::vector< Point > const& points ) {
	int num_points = points.size();
	//std::cout << "number of points " << num_points << std::endl;
	if ( num_points < 3 ) throw "bad number of points";

	std::set<int> hull_indices;

	for (int i = 0; i < num_points; i++)
	{
		for (int j = 0; j < num_points; j++)
		{
			if (j == i)
			{
				continue;
			}
			//line is p1 -> p2
			std::pair<bool, bool> result{ false,false };
			bool getall = false;
			for (int k = 0; k < num_points; k++)
			{
				if (k == i || k == j)
				{
					if (k == num_points - 1)
					{
						getall = true;
					}
					continue;
				}
				std::pair<bool, bool> r = get_location(points[i].x, points[i].y, points[j].x, points[j].y, points[k].x, points[k].y);
				result.first = result.first || r.first;
				result.second = result.second || r.second;
				if (result.first && result.second)
				{
					break;
				}
				if (k== num_points - 1)
				{
					getall = true;
				}
			}
			if (getall)
			{
				hull_indices.insert(i);
				hull_indices.insert(j);
			}

		}
	}


		
	return hull_indices;
}

std::vector<int> hullBruteForce2 ( std::vector< Point > const& points ) {
	int num_points = points.size();
	if ( num_points < 3 ) throw "bad number of points";

	std::vector<int> hull_indices;

	//find largest x point
	int largestIndex = 0;
	for (int i = 0; i < num_points; i++)
	{
		if (points[i].x < points[largestIndex].x)
		{
			largestIndex = i;
		}
	}
	hull_indices.push_back(largestIndex);
	int startPoint = largestIndex;
	bool ifBreak = false;

	while (!ifBreak)
	{
		for (int j = 0; j < num_points; j++)
		{
			if (j == startPoint)
			{
				continue;
			}
			//line start-> points[j]
			std::pair<bool, bool> result{ false,false };
			bool getall = false;
	
			for (int k = 0; k < num_points; k++)
			{
				if (k == startPoint)
				{
					if (k == num_points - 1)
					{
						getall = true;
					}
					continue;
				}

				if (k == j)
				{
					if (k == num_points - 1)
					{
						getall = true;
					}
					continue;
				}
				std::pair<bool, bool> r = get_location(points[startPoint].x, points[startPoint].y, points[j].x, points[j].y, points[k].x, points[k].y);
				result.first = result.first || r.first;
				result.second = result.second || r.second;
				if (result.first && result.second)
				{
					break;
				}
				if (k == num_points - 1)
				{
					getall = true;
				}
			}
			if (getall && result.second == false)
			{
				if (hull_indices[0] == j)
				{					
					ifBreak = true;
					break;
				}
				hull_indices.push_back(j);
				startPoint = j;
				break;
			}
			
		}
	}

	

	return hull_indices;
}
