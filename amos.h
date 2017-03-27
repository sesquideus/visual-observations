#include <cstdio>
#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <algorithm>
#include <ctime>

using namespace std;

class Observation {
public:
    double timestamp, azimuth, elevation, magnitudeAMOS, magnitudeVisual;
    int observer, amosCount, observerCount;

    Observation(double _timestamp, double _azimuth, double _elevation, double _magnitude);
    Observation(double _timestamp, int _observer, double _magnitude);
    Observation(const vector<Observation>& visuals);
    Observation(const Observation& a, const Observation& v);
    const string print() const;
};

vector<Observation> loadFile(const string& filename, bool isAMOS);
queue<Observation> mergeVisual(const vector<Observation>& fromVisual);
vector<Observation> mergeAll(queue<Observation> visual, queue<Observation> amos);

double toTimestamp(const string what);
double toTimestamp(int year, int month, int day, int hour, int minute, double second);
const string fromTimestamp(double timestamp);
bool operator < (const Observation& o1, const Observation& o2);
void printAll(const string filename, const vector<Observation>& all);
void printHeatmap(const string filename, const vector<Observation>& all);
void printBinsAMOS(const string filename, const vector<Observation>& all, const double width);
void printBinsVisual(const string filename, const vector<Observation>& all, const double width);
void printEfficiency(const string filename, const vector<Observation>& all);
