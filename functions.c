#include "functions.h"

void functions(float* inputs,  float* outputs, float* context) {
  float net_0 = 0.0f;
  float net_1 = 0.0f;
  float net_2 = 0.0f;
  float net_3 = 0.0f;
  float net_4 = 0.0f;

  // input, circuit input (0)
  net_0 = inputs[functions_I_0];

  // unit, function (0)
  context[functions_unit_0 + 0] = net_0;
  context[functions_unit_0 + 1] = context[functions_unit_0 + 2];
  context[functions_unit_0 + 2] = 0.0;
  context[functions_unit_0 + 2] += 1.000000 * context[functions_unit_0 + 0];
  context[functions_unit_0 + 2] -= -1.000000 * context[functions_unit_0 + 1];
  context[functions_unit_0 + 2] /= 1.000000;
  net_1 = context[functions_unit_0 + 2];

  // unit, function (1)
  context[functions_unit_1 + 0] = context[functions_unit_1 + 1];
  context[functions_unit_1 + 1] = net_0;
  context[functions_unit_1 + 2] = 0.0;
  context[functions_unit_1 + 2] += 1.000000 * context[functions_unit_1 + 1];
  context[functions_unit_1 + 2] += -1.000000 * context[functions_unit_1 + 0];
  context[functions_unit_1 + 2] /= 1.000000;
  net_2 = context[functions_unit_1 + 2];

  // unit, function (2)
  context[functions_unit_2 + 0] = context[functions_unit_2 + 1];
  context[functions_unit_2 + 1] = net_0;
  context[functions_unit_2 + 2] = 0.0;
  context[functions_unit_2 + 2] += 0.000000 * context[functions_unit_2 + 1];
  context[functions_unit_2 + 2] += 1.000000 * context[functions_unit_2 + 0];
  context[functions_unit_2 + 2] /= 1.000000;
  net_3 = context[functions_unit_2 + 2];

  // unit, function (3)
  context[functions_unit_3 + 0] = net_0;
  context[functions_unit_3 + 1] = 0.0;
  context[functions_unit_3 + 1] += 2.000000 * context[functions_unit_3 + 0];
  context[functions_unit_3 + 1] /= 1.000000;
  net_4 = context[functions_unit_3 + 1];

  // output, circuit output (0)
  outputs[functions_O_0] = net_1;

  // output, circuit output (1)
  outputs[functions_O_1] = net_2;

  // output, circuit output (2)
  outputs[functions_O_2] = net_3;

  // output, circuit output (3)
  outputs[functions_O_3] = net_4;
}
