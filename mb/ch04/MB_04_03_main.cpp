#include "MB_04_03.hpp"
#include <algorithm>
#include <iostream>

int main()
{
    std::cout << "=== Exercise 03 from Chapter 04 ===" << std::endl;

    std::vector<double> distances = collectDistances();

    std::cout << "Total distance: " << getTotalDistance(distances) << std::endl;
    std::cout << "Smallest distance: " << getSmallestDistance(distances)
              << std::endl;
    std::cout << "Largest distance: " << getLargestDistance(distances)
              << std::endl;
    std::cout << "Mean distance: " << getMeanDistance(distances) << std::endl;

    return 0;
}
