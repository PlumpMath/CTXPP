#ifndef CTX_UTILS_H
#define CTX_UTILS_H

#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <sstream>
#include <map>

// using namespace std;

// Forward declarations 
struct ctx_event;
struct ctx_task_base;
struct ctx_object;
struct ctx_component;
struct ctx_signal;
struct ctx_value;
struct ctx_kernel;

// Standard time units
#define CTX_SIM_FS   1.0e-15
#define CTX_SIM_PS   1.0e-12
#define CTX_SIM_AS   1.0e-10
#define CTX_SIM_NS   1.0e-9
#define CTX_SIM_US   1.0e-6
#define CTX_SIM_MS   1.0e-3
#define CTX_SIM_S    1.0
#define CTX_SIM_MIN  60.0
#define CTX_SIM_H    3600.0

#endif
