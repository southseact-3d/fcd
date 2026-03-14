#include <FuzzyHelper.h>

using namespace Part;

namespace
{
double BooleanFuzzy = 1.0;
}

double FuzzyHelper::getBooleanFuzzy()
{
    return BooleanFuzzy;
}

void FuzzyHelper::setBooleanFuzzy(const double base)
{
    BooleanFuzzy = base;
}

void FuzzyHelper::withBooleanFuzzy(double base, std::function<void()> func)
{
    double oldValue = getBooleanFuzzy();
    setBooleanFuzzy(base);
    func();
    setBooleanFuzzy(oldValue);
}
