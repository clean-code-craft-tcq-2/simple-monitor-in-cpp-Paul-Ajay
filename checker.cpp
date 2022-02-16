#include <assert.h>
#include <iostream>
#include <vector>
#include <map>
using namespace std;

enum class BoundaryRangeWithTolerance {
  LOW_BREACH = 0,
  LOW_WARNING,
  NORMAL,
  HIGH_WARNING,
  HIGH_BREACH
};
BoundaryRangeWithTolerance getBoundaryRangeLabelWithTolerance(int position) {
  return static_cast<BoundaryRangeWithTolerance>(position);
}

enum class LanguagesSupported {
  ENGLISH,
  GERMAN
};

template <typename T>
bool checkInLimits(T value, vector<T> limits) {
  return (value < limits.at(0) || value >= limits.at(1)); 
}
template <typename T>
bool checkGreaterThanLimit(T value, vector<T> limits) {
  return (value > limits.at(0));
}

map<BoundaryRangeWithTolerance, string> english_status_message {
  {BoundaryRangeWithTolerance::LOW_BREACH, "below lower limit"},
  {BoundaryRangeWithTolerance::LOW_WARNING, "low warning"},
  {BoundaryRangeWithTolerance::NORMAL, "normal"},
  {BoundaryRangeWithTolerance::HIGH_WARNING, "high warning"},
  {BoundaryRangeWithTolerance::HIGH_BREACH, "above upper limit"}
};

map<BoundaryRangeWithTolerance, string> german_status_message {
  {BoundaryRangeWithTolerance::LOW_BREACH, "unter unterer Grenze"},
  {BoundaryRangeWithTolerance::LOW_WARNING, "niedrige Warnung"},
  {BoundaryRangeWithTolerance::NORMAL, "Normal"},
  {BoundaryRangeWithTolerance::HIGH_WARNING, "hohe Warnung"},
  {BoundaryRangeWithTolerance::HIGH_BREACH, "über Obergrenze"}
};

map<LanguagesSupported, map<BoundaryRangeWithTolerance, string>> status_message_list {
  {LanguagesSupported::ENGLISH, english_status_message},
  {LanguagesSupported::GERMAN, german_status_message}
};

class BatteryStatusChecker {
public:
  BatteryStatusChecker(float lower_temperature, 
                        float upper_temperature, 
                        float lower_soc,
                        float upper_soc, 
                        float minimum_charge_rate, 
                        float temperature_tolerance = 0,
                        float soc_tolerance = 0,
                        float charge_rate_tolerance = 0,
                        float max_temperature = 0,
                        LanguagesSupported language = LanguagesSupported::ENGLISH) {
    this->temperature_lower_limit = lower_temperature;
    this->temperature_upper_limit = upper_temperature;
    this->soc_lower_limit = lower_soc;
    this->soc_upper_limit = upper_soc;
    this->minimum_charge_rate = minimum_charge_rate;
    this->temperature_tolerance = temperature_tolerance;
    this->soc_tolerance = soc_tolerance;
    this->charge_rate_tolerance = charge_rate_tolerance;
    this->max_temperature = max_temperature;
    this->selected_language = language;
  }
  template <typename T>
  BoundaryRangeWithTolerance getPropertyStatus(T value, vector<T> limits, T maximum_value, bool (*function_pointer)(T, vector<T>)) {
    std::vector<T> boundary_limits = generateBoundaryLimits(limits.at(0), limits.at(1), limits.at(2), maximum_value);
    int position = 0;
    for (auto itr = boundary_limits.begin(); itr != boundary_limits.end()-1; itr++) {
      if (!(*function_pointer)(value, {*itr, *(itr + 1)})) {
        position = itr - boundary_limits.begin() + 1;
        break;
      }
    }
    return getBoundaryRangeLabelWithTolerance(position);
  }

  template <typename T>
  std::vector<T> generateBoundaryLimits(T lower_limit, T higher_limit, T tolerance, T maximum_value) {
    return {lower_limit, lower_limit + tolerance * maximum_value / 100, higher_limit - tolerance * maximum_value / 100, higher_limit, maximum_value};
  }

  bool isTemperatureOK(float temperature) {
    BoundaryRangeWithTolerance range_label_value;
    range_label_value = getPropertyStatus(temperature, {temperature_lower_limit, temperature_upper_limit, temperature_tolerance}, max_temperature, checkInLimits);
    consolePrint("Temperature",status_message_list[selected_language][range_label_value]);
    if(range_label_value != BoundaryRangeWithTolerance::NORMAL)
      return false;
    else
      return true;
  }

  bool isSocOk(float soc){
    BoundaryRangeWithTolerance range_label_value;
    range_label_value = getPropertyStatus(soc, {soc_lower_limit, soc_upper_limit, soc_tolerance}, static_cast<float>(100.0), checkInLimits);
    consolePrint("SOC", status_message_list[selected_language][range_label_value]);
    if(range_label_value != BoundaryRangeWithTolerance::NORMAL)
      return false;
    else
      return true;
  }
  bool isChargeRateOk(float charge_rate) {
    BoundaryRangeWithTolerance range_label_value;
    range_label_value = getPropertyStatus(charge_rate, {0, minimum_charge_rate, charge_rate_tolerance}, static_cast<float>(1.0), checkInLimits);
    consolePrint("Charge Rate", status_message_list[selected_language][range_label_value]);
    if(range_label_value != BoundaryRangeWithTolerance::NORMAL)
      return false;
    else
      return true;    
  }

  bool batteryIsOk(float temperature, float soc, float charge_rate) {
    temperature_ok = isTemperatureOK(temperature);
    soc_ok = isSocOk(soc);
    charge_rate_ok = isChargeRateOk(charge_rate);
    return temperature_ok && soc_ok && charge_rate_ok;
  }

private:
  float temperature_lower_limit, temperature_upper_limit, soc_lower_limit, soc_upper_limit, minimum_charge_rate;
  float temperature_tolerance, soc_tolerance, charge_rate_tolerance;
  float max_temperature;
  bool temperature_ok, soc_ok, charge_rate_ok;
  void consolePrint(string property, string message) {
    cout <<property <<" : "<< message <<std::endl;
  }
  LanguagesSupported selected_language;
};

int main() {
  BatteryStatusChecker battery_status_object(0, 45, 20, 80, 0.8, 5, 5, 5, 100, LanguagesSupported::ENGLISH);
  assert(battery_status_object.batteryIsOk(25, 70, 0.7) == true);
  assert(battery_status_object.batteryIsOk(50, 85, 0) == false);
}
