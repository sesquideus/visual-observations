#include "amos.h"

bool operator < (const Observation& o1, const Observation& o2) {
    return o1.timestamp < o2.timestamp;
}

double toTimestamp(int year, int month, int day, int hour, int minute, double second) {
    time_t rawtime;
    struct tm* timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    timeinfo->tm_year   = year - 1900;
    timeinfo->tm_mon    = month - 1;
    timeinfo->tm_mday   = day;
    timeinfo->tm_hour   = hour;
    timeinfo->tm_min    = minute;
    timeinfo->tm_sec    = (int) floor(second);

    return (double) mktime(timeinfo) + timezone + fmod(second, 1);
}

double toTimestamp(const string what) {
    int year, month, day, hour, minute;
    double second;
    sscanf(what.c_str(), "%d-%d-%dT%d:%d:%lf", &year, &month, &day, &hour, &minute, &second);
    return toTimestamp(year, month, day, hour, minute, second);
}

const string fromTimestamp(double timestamp) {
    const time_t rawtime = (const time_t) timestamp;

    struct tm* dt;
    char timestr[50];

    dt = localtime(&rawtime);
    strftime(timestr, sizeof(timestr), "%Y-%m-%dT%H:%M:%S", dt);
    return string(timestr);
}

Observation::Observation(double _timestamp, double _azimuth, double _elevation, double _magnitude):
    timestamp(_timestamp), azimuth(_azimuth), elevation(_elevation), magnitudeAMOS(_magnitude), magnitudeVisual(0.0), observer(0), amosCount(1), observerCount(0) {}

Observation::Observation(double _timestamp, int _observer, double _magnitude):
    timestamp(_timestamp), magnitudeAMOS(0.0), magnitudeVisual(_magnitude), observer(_observer), amosCount(0), observerCount(1) {}
    
Observation::Observation(const vector<Observation>& visuals):
    timestamp(visuals[0].timestamp), azimuth(0.0), elevation(0.0), magnitudeAMOS(0.0), magnitudeVisual(0.0), observer(0), amosCount(0), observerCount(visuals.size()) {
    
    double mag = 0;
    for (auto i: visuals) {
        mag += i.magnitudeVisual;
        this->observer |= (1 << i.observer);
    }
    this->magnitudeVisual = mag / this->observerCount;

}

Observation::Observation(const Observation& a, const Observation& v):
    timestamp(a.timestamp), azimuth(a.azimuth), elevation(a.elevation), magnitudeAMOS(a.magnitudeAMOS), magnitudeVisual(v.magnitudeVisual), observer(v.observer),
    amosCount(a.amosCount), observerCount(v.observerCount) { 
}

const string Observation::print() const {
    char buf[200];
    snprintf(buf, 200, "%s\t%4.1f\t%4.1f%5d%5d   %02X%7.1f%7.1f\n",
        fromTimestamp(this->timestamp).c_str(), this->magnitudeAMOS, this->magnitudeVisual, this->amosCount, this->observerCount, this->observer, this->azimuth, this->elevation
    );
    return string(buf);
}

vector<Observation> loadFile(const string& filename, bool isAMOS) {
    vector<Observation> o;

    FILE* f = fopen(filename.c_str(), "r");
    if (f == nullptr) {
        printf("Could not load file %s\n", filename.c_str());
        return o;
    }

    int year, month, day, hour, minute, observer;
    double second, azimuth, elevation, magnitude;

    while (!feof(f)) {
        year = month = day = hour = minute = 0;
        second = azimuth = elevation = magnitude = 0;
        if (isAMOS) {
            if (int count = fscanf(f, "%lf\t%d\t%d\t%d\t%d\t%d\t%lf\t%lf\t%lf\n", &magnitude, &year, &month, &day, &hour, &minute, &second, &azimuth, &elevation) != 9) {
                printf("Malformed line, only %d read\n", count);
            }
            o.push_back(Observation(toTimestamp(year, month, day, hour, minute, second), azimuth, elevation, magnitude));
        } else {
            if (int count = fscanf(f, "%d\t%d\t%d\t%d\t%d\t%lf\t%d\t%*c\t%lf\n", &year, &month, &day, &hour, &minute, &second, &observer, &magnitude) != 8) { 
                printf("Malformed line, only %d tokens read\n", count);
            }
            o.push_back(Observation(toTimestamp(year, month, day, hour, minute, second), observer, magnitude / 10));
        }
    }
    fclose(f);

    sort(o.begin(), o.end());
    return o;
}

queue<Observation> mergeVisual(const vector<Observation>& fromVisual) {
    vector<Observation> candidates;
    queue<Observation> result;

    double pivot = 0;
    for (vector<Observation>::const_iterator visual = fromVisual.cbegin(); visual != fromVisual.cend(); ++visual) {
        if (visual->timestamp - pivot > 1.5) {
            if (candidates.size() > 0) {
                result.push(Observation(candidates));
                candidates.clear();
            }
            pivot = visual->timestamp;
        }
        candidates.push_back(*visual);
    }
    result.push(Observation(candidates));
    
    return result;
}

vector<Observation> mergeAll(queue<Observation> visual, queue<Observation> amos) {
    vector<Observation> result;

    while (!(visual.empty() && amos.empty())) {
        if (visual.empty() || (amos.front().timestamp < visual.front().timestamp - 1)) {
            result.push_back(amos.front());
            amos.pop();
        } else {
            if (amos.empty() || (visual.front().timestamp < amos.front().timestamp - 1)) {
                result.push_back(visual.front());
                visual.pop();
            } else {
                result.push_back(Observation(amos.front(), visual.front()));
                amos.pop();
                visual.pop();
            }
        }
    }    

    return result;    
}

void printAll(const string filename, const vector<Observation>& all) {
    FILE* fout = fopen((filename + ".corr").c_str(), "w");
    fprintf(fout, "#              Time AMOS     Vis   O#   A#  Obs   azim   elev\n");

    for (auto &i: all) {
        fprintf(fout, "%s", i.print().c_str());
    }
    fclose(fout);
    printf("All observations saved to file %s\n", (filename + ".corr").c_str());
}

void printHeatmap(const string filename, const vector<Observation>& all) {
    FILE* fout = fopen((filename + ".heatmap").c_str(), "w");

    int heat[10][10] = { 0 };
    for (auto &i: all) {
        if ((i.amosCount == 0) || (i.observerCount == 0)) continue;
        heat[(int) floor(i.magnitudeAMOS) + 5][(int) floor(i.magnitudeVisual) + 5]++;
    }

    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 10; ++j) {
            fprintf(fout, "%2d ", heat[i][j]);
        }
        fprintf(fout, "\n");
    }
    fclose(fout);

    cout << "Heatmap saved to file " << filename << ".heatmap" << endl;
}

void printBins(const string filename, const vector<Observation>& all, const double width) {
    FILE* fout = fopen((filename + ".bins").c_str(), "w");
    int count = ceil(12.0 / width + 1);
    int counted = 0;
    vector<int> visual(count, 0), amos(count, 0), visualCommon(count, 0), amosCommon(count, 0), amosCalibrated(count, 0), amosCalibratedCommon(count, 0);

    for (auto &i: all) {
        if (i.amosCount * i.observerCount > 0) {
            visualCommon            [(int) floor((i.magnitudeVisual + 6.0) / width)]++;
            amosCommon              [(int) floor((i.magnitudeAMOS + 6.0) / width)]++;
            amosCalibratedCommon    [(int) floor((i.magnitudeAMOS * 0.757057 + 6.0 + 1.7442) / width)]++;
            ++counted;
        }
        if (i.amosCount > 0) {
            amos[(int) floor((i.magnitudeAMOS + 6.0) / width)]++;
            amosCalibrated[(int) floor((i.magnitudeAMOS * 0.757 + 1.7442 + 5.0) / width)]++;
        }
        if (i.observerCount > 0) {
            visual[(int) floor((i.magnitudeVisual + 6.0) / width)]++;
        }
    }

    for (double i = -6.0; i < 5.9; i += width) {
        int u = (int) floor((i + 6.0) / width);
        fprintf(fout, "%4.1f %4d %4d %4d %4d %4d %4d\n", i, visual[u], amos[u], amosCalibrated[u], visualCommon[u], amosCommon[u], amosCalibratedCommon[u]);
    }
    fclose(fout);
    printf("Bins saved to file %s, statistics size %d\n", (filename + ".bins").c_str(), counted);
}

void printEfficiency(const string filename, const vector<Observation>& all) {
    vector<Observation> filtered;
    for (auto &i: all) {
        if (((i.timestamp >= toTimestamp("2016-08-11T20:00:00")) && (i.timestamp <= toTimestamp("2016-08-11T21:00:00"))) ||
            ((i.timestamp >= toTimestamp("2016-08-11T21:15:00")) && (i.timestamp <= toTimestamp("2016-08-11T22:15:00"))) ||
            ((i.timestamp >= toTimestamp("2016-08-11T22:30:00")) && (i.timestamp <= toTimestamp("2016-08-12T00:03:00"))) ||
            ((i.timestamp >= toTimestamp("2016-08-12T00:20:00")) && (i.timestamp <= toTimestamp("2016-08-12T01:45:00"))) ||
            ((i.timestamp >= toTimestamp("2016-08-12T20:35:00")) && (i.timestamp <= toTimestamp("2016-08-12T23:30:00"))) ||
            ((i.timestamp >= toTimestamp("2016-08-12T23:48:00")) && (i.timestamp <= toTimestamp("2016-08-13T00:46:00"))) ||
            ((i.timestamp >= toTimestamp("2016-08-13T00:55:00")) && (i.timestamp <= toTimestamp("2016-08-13T01:55:00")))) {
            filtered.push_back(i);
        }
    }
    printf("Filtered observations by timestamps: %lu remained\n", filtered.size());

    printBins(filename, filtered, 0.5);
}

int main(int argc, char** argv) {
    string filename(argv[1]);

    vector<Observation> fromAMOS, fromVisual, all;
    queue<Observation> q;

    if (argc < 2) return 1;

    fromAMOS = loadFile(filename + "-amos.in", true);
    fromVisual = loadFile(filename + "-visual.in", false);

    for (auto &i: fromAMOS) {
        q.push(i);
    }
    all = mergeAll(mergeVisual(fromVisual), q);

    printf("Total processed: %lu events\n", all.size());

    printAll(filename, all);
    printHeatmap(filename, all);
    printEfficiency(filename, all);

    return 0;
}
