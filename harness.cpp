extern "C" {
void functions(float* inputs, float* outputs, float* context);
void harness(float* inputs, float* outputs, float* context);
}

void harness(float* inputs, float* outputs, float* context)
{ functions(inputs, outputs, context); }
