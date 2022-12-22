#include <iostream>
#include <utility>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>
#include <queue>
#include <algorithm>
#include <tuple>
#include <chrono>
#include <limits>
#include <random>       
#include <cstdlib>
#include <time.h>
#include <math.h>

#define PHI 1.184692
#define GAMA 1.112434
#define Cr 0.01
#define g 9.81
#define P 1.2041
#define A 3.912
#define Ca 0.7
#define m 650
#define km_to_meter 1000

#define joule_to_watt_hour 1/3600

#define speed_km_hour 40
#define vel speed_km_hour/3.6

#define ENERGY_CONST PHI * GAMA * (Cr * g * m + (P * A * Ca * pow(vel, 2) / 2)) * joule_to_watt_hour
#define ENERGY_CONST_LOAD PHI * GAMA * Cr * g * joule_to_watt_hour

// #define ENERGY_CONST_LOAD 0.129285172
// #define ENERGY_CONST 86.20810869
#define ENERGY_BATTERY 18000