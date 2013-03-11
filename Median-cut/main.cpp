#include <iostream>
#include "median_cut.h"

int main(int argc, char* argv[])
{
	if(argc!=5)
	{
		std::cout << "Bad arguments" << std::endl;
		return -1;
	}

    int numPoints = atoi(argv[2]) * atoi(argv[3]);
    Point* points = (Point*)malloc(sizeof(Point) * numPoints);

    FILE * raw_in = fopen(argv[1], "rb");
    for(int i = 0; i < numPoints; i++)
        fread(&points[i], 3, 1, raw_in);
    fclose(raw_in);

    std::list<Point> palette = medianCut(points, numPoints, atoi(argv[4]));

    std::list<Point>::iterator iter;
    for (iter = palette.begin() ; iter != palette.end(); iter++)
    {
        std::cout << (int)iter->x[0] << " "
                  << (int)iter->x[1] << " "
                  << (int)iter->x[2] << std::endl;
    }

    return 0;
}
