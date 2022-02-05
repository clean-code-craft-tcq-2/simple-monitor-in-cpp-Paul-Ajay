#include <assert.h>
#include <iostream>
#include <vector>
using namespace std;

// bool batteryIsOk(float temperature, float soc, float chargeRate) {
//   if(temperature < 0 || temperature > 45) {
//     cout << "Temperature out of range!\n";
//     return false;
//   } else if(soc < 20 || soc > 80) {
//     cout << "State of Charge out of range!\n";
//     return false;
//   } else if(chargeRate > 0.8) {
//     cout << "Charge Rate out of range!\n";
//     return false;
//   }
//   return true;
// }

template <typename T>
bool checkInLimits(T value, vector<T> limits) {
  return (value < limits.at(0) || value > limits.at(1)); 
}
template <typename T>
bool checkGreaterThanLimit(T value, vector<T> limits) {
  return (value > limits.at(0));
}

class BatteryStatusChecker {
public:
  BatteryStatusChecker(float lower_temperature, float upper_temperature, float lower_soc, float upper_soc, float minimum_charge_rate) {
    this->temperature_lower_limit = lower_temperature;
    this->temperature_upper_limit = upper_temperature;
    this->soc_lower_limit = lower_soc;
    this->soc_upper_limit = upper_soc;
    this->minimum_charge_rate = minimum_charge_rate;
  }
  template <typename T>
  bool IF(T value, vector<T> limits, bool (*function_pointer)(T, vector<T>), string property) {
    if ((*function_pointer)(value, limits)) {
      console_print(property);
      return false;
    }
    return true;
  }
  bool batteryIsOk(float temperature, float soc, float charge_rate) {
    temperature_ok = IF(temperature, {temperature_lower_limit, temperature_upper_limit}, checkInLimits, "Temperature");
    soc_ok = IF(soc, {soc_lower_limit, soc_upper_limit}, checkInLimits, "SOC");
    charge_rate_ok = IF(charge_rate, {minimum_charge_rate}, checkGreaterThanLimit, "Charge Rate");
    return temperature_ok && soc_ok && charge_rate_ok;
  }

private:
  float temperature_lower_limit, temperature_upper_limit, soc_lower_limit, soc_upper_limit, minimum_charge_rate;
  bool temperature_ok, soc_ok, charge_rate_ok;
  void console_print(string property) {
    cout << property << " out of range! " <<std::endl;
  }
};

int main() {
  BatteryStatusChecker battery_status_object(0, 45, 20, 80, 0.8);
  assert(battery_status_object.batteryIsOk(25, 70, 0.7) == true);
  assert(battery_status_object.batteryIsOk(50, 85, 0) == false);
}
