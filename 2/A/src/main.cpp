#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

struct VisitorEvent
{
    int timeInSeconds;
    int delta; 
};

int TimeStringToSeconds(const std::string& timeStr);

int main()
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    size_t visitorCount = 0;
    std::cin >> visitorCount;

    std::vector<VisitorEvent> events(visitorCount * 2);

    for (size_t i = 0; i < visitorCount; ++i)
    {
        std::string arrivalStr, departureStr;
        std::cin >> arrivalStr >> departureStr;

        events[2 * i].timeInSeconds     = TimeStringToSeconds(arrivalStr);
        events[2 * i].delta             = 1;
        events[2 * i + 1].timeInSeconds = TimeStringToSeconds(departureStr);
        events[2 * i + 1].delta         = -1;
    }

    std::sort(events.begin(), events.end(), [](const VisitorEvent& a, const VisitorEvent& b) {
        if (a.timeInSeconds == b.timeInSeconds)
        {
            return a.delta > b.delta;
        }
        return a.timeInSeconds < b.timeInSeconds;
    });

    int currentVisitors = 0;
    int maxVisitors = 0;

    for (const auto& ev : events)
    {
        currentVisitors += ev.delta;
        if (currentVisitors > maxVisitors)
            maxVisitors = currentVisitors;
    }

    std::cout << maxVisitors << "\n";

    return 0;
}

int TimeStringToSeconds(const std::string& timeStr)
{
    int hours = 0, minutes = 0, seconds = 0;
    sscanf(timeStr.c_str(), "%d:%d:%d", &hours, &minutes, &seconds);
    return hours * 3600 + minutes * 60 + seconds;
}
