#ifndef MEDIAN_CUT_H_
#define MEDIAN_CUT_H_

#include <list>
const int NUM_DIMENSIONS = 3;

struct Point
{
    unsigned char x[NUM_DIMENSIONS];
};

class Block
{
public:
    Block(Point* points, int pointsLength);
    Point * getPoints();
    int numPoints() const;
    int longestSideIndex() const;
    int longestSideLength() const;
    bool operator<(const Block& rhs) const;
    void shrink();

private:
	Point minCorner, maxCorner;
    Point* points;
    int pointsLength;

    template <typename T>
    static T min(const T a, const T b)
    {
        if (a < b)
            return a;
        else
            return b;
    }

    template <typename T>
    static T max(const T a, const T b)
    {
        if (a > b)
            return a;
        else
            return b;
    }
};

template <int index>
class CoordinatePointComparator
{
public:
    bool operator()(Point left, Point right)
    {
        return left.x[index] < right.x[index];
    }
};

std::list<Point> medianCut(Point* image, int numPoints, unsigned int desiredSize);

#endif /* #ifndef MEDIAN_CUT_H_ */