#pragma once

#include <cmath>

struct IdPair
{
    int vertexId0;
    int vertexId1;
};

struct Double3
{
    double x, y, z;

    static double Distance(Double3 left, Double3 right);
};

inline
double Double3::Distance(Double3 left, Double3 right)
{
    //TODO:
    double x_distance = (left.x - right.x);
    double y_distance = (left.y - right.y);
    double z_distance = (left.z - right.z);
    return sqrt((double)x_distance*(double)x_distance + (double)y_distance*(double)y_distance + (double)z_distance*(double)z_distance);
}