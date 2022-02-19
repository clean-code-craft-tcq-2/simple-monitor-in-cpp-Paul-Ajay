#include <assert.h>
#include <iostream>
#include <vector>
#include <map>
#include <memory>
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
  return (value < limits.at(0) || value > limits.at(1)); 
}
template <typename T>
bool checkGreaterThanLimit(T value, vector<T> limits) {
  return (value > limits.at(0));
}
template <typename T>
std::vector<T> generateBoundaryLimits(T lower_limit, T higher_limit, T tolerance, T maximum_value) {
  return {lower_limit, lower_limit + tolerance * maximum_value / 100, higher_limit - tolerance * maximum_value / 100, higher_limit, maximum_value};
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

std::string getStatusMessage(LanguagesSupported language, BoundaryRangeWithTolerance status) {
  return status_message_list[language][status];
}

void consolePrint(string property, string message) {
  cout <<property <<" : "<< message <<std::endl;
}

class TemperatureProperty {
public:
  TemperatureProperty(float lower_temperature, 
                      float upper_temperature,
                      float temperature_tolerance = 0,
                      float max_temperature = 0)
    {
      this->temperature_lower_limit = lower_temperature;
      this->temperature_upper_limit = upper_temperature;
      this->temperature_tolerance = temperature_tolerance;
      this->max_temperature = max_temperature;
    }
  BoundaryRangeWithTolerance temperatureStatus(float temperature) {
    BoundaryRangeWithTolerance range_label_value;
    range_label_value = getPropertyStatus(
                                        temperature, 
                                        {temperature_lower_limit, temperature_upper_limit, temperature_tolerance}, 
                                        max_temperature, checkInLimits);
    return range_label_value;
  }
private:
  float temperature_lower_limit, temperature_upper_limit, temperature_tolerance, max_temperature;
};

class SocProperty {
public:
  SocProperty(float lower_soc,
              float upper_soc, 
              float soc_tolerance = 0) 
    {
      this->lower_soc = lower_soc;
      this->upper_soc = upper_soc;
      this->soc_tolerance = soc_tolerance;

    }
  BoundaryRangeWithTolerance socStatus(float soc) {
    BoundaryRangeWithTolerance range_label_value;
    range_label_value = getPropertyStatus(
                                        soc, 
                                        {lower_soc, upper_soc, soc_tolerance}, 
                                        static_cast<float>(100.0), checkInLimits);
    return range_label_value;
  }
private:
  float lower_soc, upper_soc, soc_tolerance;
};

class ChargeRateProperty {
public:
  ChargeRateProperty(float minimum_charge_rate,
                    float charge_rate_tolerance = 0)
    {
      this->minimum_charge_rate = minimum_charge_rate;
      this->charge_rate_tolerance = charge_rate_tolerance;
    }
  BoundaryRangeWithTolerance chargeRateStatus(float charge_rate) {
    BoundaryRangeWithTolerance range_label_value;
    range_label_value = getPropertyStatus(
                                        charge_rate, 
                                        {0, minimum_charge_rate, charge_rate_tolerance}, 
                                        static_cast<float>(1.0), checkInLimits);
    return range_label_value;
  }
private:
  float minimum_charge_rate, charge_rate_tolerance;
};

class BatteryStatusChecker {
public:
  BatteryStatusChecker(shared_ptr<TemperatureProperty> temperature_object_ptr,
                        shared_ptr<SocProperty> soc_object_ptr,
                        shared_ptr<ChargeRateProperty> charge_rate_object_ptr) 
      : temperature_object(temperature_object_ptr),
        soc_object(soc_object_ptr),
        charge_rate_object(charge_rate_object_ptr) 
  {}

  static bool isPropertyOk(BoundaryRangeWithTolerance status) {
    return status == BoundaryRangeWithTolerance::NORMAL;
  }

  bool batteryIsOk(
                  float temperature, 
                  float soc, 
                  float charge_rate, 
                  LanguagesSupported language, 
                  void (*print_function_pointer)(std::string ,std::string)) {
    BoundaryRangeWithTolerance temperature_stauts, soc_status, charge_rate_status;
    temperature_stauts = temperature_object->temperatureStatus(temperature);
    (*print_function_pointer)("Temperature", getStatusMessage(language, temperature_stauts));
    soc_status = soc_object->socStatus(soc);
    (*print_function_pointer)("SOC", getStatusMessage(language, temperature_stauts));
    charge_rate_status = charge_rate_object->chargeRateStatus(charge_rate);
    (*print_function_pointer)("Charge Rate", getStatusMessage(language, temperature_stauts));
    return isPropertyOk(temperature_stauts) && isPropertyOk(soc_status) && isPropertyOk(charge_rate_status);
  }

private:
  std::shared_ptr<TemperatureProperty> temperature_object;
  std::shared_ptr<SocProperty> soc_object;
  std::shared_ptr<ChargeRateProperty> charge_rate_object;

  LanguagesSupported selected_language;
};

int main() {
  shared_ptr<TemperatureProperty> temperature_object(new TemperatureProperty(0.0, 45.0, 5.0, 100.0));
  shared_ptr<SocProperty> soc_object(new SocProperty(20, 80, 5));
  shared_ptr<ChargeRateProperty> charge_rate_object(new ChargeRateProperty(0.8, 5));

  BatteryStatusChecker battery_status_object(temperature_object, soc_object, charge_rate_object);
  assert(battery_status_object.batteryIsOk(25, 70, 0.7, LanguagesSupported::ENGLISH, consolePrint) == true);
  cout << "-----------------------------------" << endl;
  assert(battery_status_object.batteryIsOk(50, 85, 0.8, LanguagesSupported::GERMAN, consolePrint) == false);
  cout << "-----------------------------------" << endl;
  assert(battery_status_object.batteryIsOk(42, 21, 0.7, LanguagesSupported::ENGLISH, consolePrint) == false);
  cout << "-----------------------------------" << endl;

  shared_ptr<TemperatureProperty> temperature_object1(new TemperatureProperty(0, 45, 0, 100));
  shared_ptr<SocProperty> soc_object1(new SocProperty(20, 80, 0));
  shared_ptr<ChargeRateProperty> charge_rate_object1(new ChargeRateProperty(0.8, 0));
  BatteryStatusChecker battery_status_object1(temperature_object1, soc_object1, charge_rate_object1);
  assert(battery_status_object1.batteryIsOk(50, 85, 0.8, LanguagesSupported::GERMAN, consolePrint) == false);
  cout << "-----------------------------------" << endl;
  assert(battery_status_object1.batteryIsOk(42, 21, 0.7, LanguagesSupported::ENGLISH, consolePrint) == true);
  cout << "-----------------------------------" << endl;
}
