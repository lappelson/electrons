/*
    fastsim.cpp  -  Don Cross  -  1 July 2016.
    
    A simulation of mutually repulsive particles trapped on the surface of a sphere.
    This is also known as the Thompson Problem.
    See:  https://en.wikipedia.org/wiki/Thomson_problem
*/

#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <cstdint>
#include <cmath>

namespace Electrons
{
    struct Vector
    {
        const double  x;
        const double  y;
        const double  z;
        
        Vector(): x(0), y(0), z(0) {}
        Vector(double _x, double _y, double _z): x(_x), y(_y), z(_z) {}
        
        double MagSquared() const 
        {
            return (x*x) + (y*y) + (z*z);
        }
    };
    
    typedef std::vector<Vector> VectorList;

    inline Vector operator + (const Vector& a, const Vector& b)
    {
        return Vector(a.x + b.x, a.y + b.y, a.z + b.z);
    }
    
    void Print(std::ostream& output, double t)
    {
        using namespace std;
        output << setprecision(14) << fixed;
        if (t >= 0.0) output << " ";
        output << t;
    }
    
    std::ostream& operator<< (std::ostream& output, const Vector& v)
    {
        output << "{\"x\":"; 
        Print(output, v.x); 
        output << ", \"y\":"; 
        Print(output, v.y);
        output << ", \"z\":";
        Print(output, v.z);
        output << "}";
        return output;
    }
    
    std::ostream& operator<< (std::ostream& output, const VectorList& list)
    {
        using namespace std;
        output << "[\n";
        bool first = true;
        for (const Vector& v : list) 
        {
            cout << (first ? " " : ",") << "   " << v << "\n";
            first = false;
        }
        output << "]\n";
        return output;
    }
    
    double Random(std::ifstream& infile)
    {
        using namespace std;
        
        uint64_t data;
        infile.read((char *)&data, sizeof(data));
        if (!infile) throw "Read failure generating random number.";
        
        // Convert the 64-bit integer to double-precision floating point.
        // Divide by maximum possible value to get a number in the closed range [0, +1].
        double x = static_cast<double>(data) / static_cast<double>(UINT64_MAX);
        
        // Convert to the closed range [-1, +1].
        
        return 1.0 - (2.0 * x);
    }

    Vector RandomSpherePoint(std::ifstream& infile)
    {    
        // Algorithm for picking a random point on a sphere.
        // Avoids any clustering of points.
        // http://mathworld.wolfram.com/SpherePointPicking.html
        // See equations (9), (10), (11) there.
        while (true)
        {
            double a = Random(infile);
            double b = Random(infile);
            double mag = (a*a) + (b*b);
            if (mag < 1.0)
            {
                double root = 2.0 * sqrt(1.0 - mag);
                return Vector(a*root, b*root, 1.0 - (2.0*mag));
            }
        }
    }

    VectorList RandomSpherePoints(int numPoints)
    {
        using namespace std;
        
        if (numPoints < 0) throw "Number of points not allowed to be negative.";        
        ifstream infile("/dev/urandom", ios::in | ios::binary);
        if (!infile) throw "Could not open /dev/urandom to obtain random numbers.";
        VectorList list;
        for (int i=0; i < numPoints; ++i)
        {
            list.push_back(RandomSpherePoint(infile));
        }
        return list;
    }
}


int main(int argc, const char *argv[])
{
    using namespace std;
    using namespace Electrons;
    try 
    {
        cout << RandomSpherePoints(10) << endl;        
        return 0;
    }
    catch (const char *message)
    {
        cerr << "EXCEPTION: " << message << endl;
        return 1;
    }
}

