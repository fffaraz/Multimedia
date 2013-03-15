/* Copyright (c) 2013 the authors listed at the following URL, and/or
the authors of referenced articles or incorporated external code:
http://en.literateprograms.org/Median_cut_algorithm_(C_Plus_Plus)?action=history&offset=20080309133934

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Retrieved from: http://en.literateprograms.org/Median_cut_algorithm_(C_Plus_Plus)?oldid=12754
*/

#include <iostream>
#include "median_cut.h"

int main(int argc, char* argv[]) {
    FILE * raw_in;
    int numPoints = atoi(argv[2]) * atoi(argv[3]);
    Point* points = (Point*)malloc(sizeof(Point) * numPoints);

    raw_in = fopen(argv[1], "rb");
    for(int i = 0; i < numPoints; i++)
    {
        fread(&points[i], 3, 1, raw_in);
    }
    fclose(raw_in);

    std::list<Point> palette =
        medianCut(points, numPoints, atoi(argv[4]));

    std::list<Point>::iterator iter;
    for (iter = palette.begin() ; iter != palette.end(); iter++)
    {
        std::cout << (int)iter->x[0] << " "
                  << (int)iter->x[1] << " "
                  << (int)iter->x[2] << std::endl;
    }

    return 0;
}
