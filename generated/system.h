#include <cmath>
#include <nvector/nvector_serial.h>
#include <sstream>

#define INDEX_C_START 0
#define INDEX_C_SIZE 10
#define INDEX_G 10

#define NUM_DEPS 11

double T(double* values);
double cm_to_m(double x, double n);

double T(double* values)
{
	return ((1) - (values[INDEX_C_START + (1) - 1]));
}

double cm_to_m(double x, double n)
{
	return ((std::exp(((n) - (x)))) / (100));
}

void get_initial_state(N_Vector state) {
    double* values = N_VGetArrayPointer(state);

    for (size_t n = 1; n <= 10; ++n)
    {
        values[INDEX_C_START + (n - 1)] = ((T(values)) + (n));
    }
    values[INDEX_G] = -(cm_to_m((5), (1.9)));
}

std::string get_state_csv_label() {
	std::stringstream str; 
	str << "t (seconds)";
	for (size_t i = 0; i < 10; ++i)
	{
		str << ", C[" << i + 1 << "]";
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