#include <cmath>
#include <nvector/nvector_serial.h>
#include <sstream>

const size_t INDEX_C_START = 0;
const size_t INDEX_C_SIZE = 10 - 1 + 1;
const size_t INDEX_G = INDEX_C_START + INDEX_C_SIZE;

#define STATE_SIZE INDEX_G + 1;
double T(double* values);
double cm_to_m(double x, double n, double* values);

double __summation_0(double* values) {
	double sum = 0.0;
	for (size_t i = 1; i < 5; i++) {
		sum += std::pow(values[INDEX_C_START + (i) - 1], 2);
	}
	 return sum;
}

double T(double* values)
{
	return ((1) - (values[INDEX_C_START + (1) - 1]));
}

double cm_to_m(double x, double n, double* values)
{
	return ((((std::exp(((n) - (x)))) / (100))) - (__summation_0(values)));
}

void get_initial_state(N_Vector state) {
    double* values = N_VGetArrayPointer(state);

    for (size_t n = 1; n <= 10; ++n)
    {
        values[INDEX_C_START + (n - 1)] = ((T(values)) + (n));
    }
    values[INDEX_G] = -(cm_to_m((5), (1.9), values));
}

std::string get_state_csv_label() {
	std::stringstream str; 
	str << "t (seconds)";
	for (size_t i = 1; i <= 10; ++i)
	{
		str << ", C[" << i << "]";
	}

	str << ", G";
	return str.str();
}

int system(sunrealtype t, N_Vector y, N_Vector ydot, void *user_data) {
    double* values = N_VGetArrayPointer(y);
    double* derivatives = N_VGetArrayPointer(ydot);

    for (size_t n = 1; n <= 10; ++n)
    {
        derivatives[INDEX_C_START + (n - 1)] = ((-(values[INDEX_G])) + (n));
    }
    derivatives[INDEX_G] = values[INDEX_C_START + (1) - 1];

    derivatives[INDEX_C_START + (size_t)(1 - 1)] = -(values[INDEX_G]);
    return 0;
}