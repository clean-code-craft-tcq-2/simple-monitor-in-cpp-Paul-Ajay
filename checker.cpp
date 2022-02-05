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
template <typename T>
bool IF(T value, vector<T> limits, bool (*function_pointer)(T, vector<T>)) {
  return (*function_pointer)(value, limits);
}
void console_print(string message) {
  cout << message <<std::endl;
}

bool batteryIsOk(float temperature, float soc, float chargeRate) {
  if(IF(temperature, {0, 45}, checkInLimits)) {
    console_print("Temperature out of range!");
    return false;
  } else if(IF(soc, {20, 80}, checkInLimits)) {
    console_print("State of Charge out of range!");
    return false;
  } else if (IF(chargeRate, {0.8}, checkGreaterThanLimit)) {
    console_print("Charge Rate out of range!");
    return false;
  }
  return true;
}

int main() {
  assert(batteryIsOk(25, 70, 0.7) == true);
  assert(batteryIsOk(50, 85, 0) == false);
}
