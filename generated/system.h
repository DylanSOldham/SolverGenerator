#include <cmath>
#include <nvector/nvector_serial.h>
#include <sstream>

const size_t INDEX_C_START = 0;
const size_t INDEX_C_SIZE = (3 - 1 + 1) * (3 - 1 + 1);
const size_t STATE_SIZE =INDEX_C_START + INDEX_C_SIZE;

std::string get_state_csv_label() {
	std::stringstream str; 
	str << "t (seconds)";
	for (size_t n = 1; n <= 3; ++n)
	{
		for (size_t x = 1; x <= 3; ++x)
		{
			str << ", C[" << n << " " << x << "]";
		}
	}
	return str.str();
}

std::string get_csv_line(N_Vector state) {
	std::stringstream str;
	double* values = N_VGetArrayPointer(state);
	for (size_t i = 0; i < STATE_SIZE; ++i) {
		str << ", " << values[i];
	}
	return str.str();
}

void get_initial_state(N_Vector state) {
    double* values = N_VGetArrayPointer(state);

	for (size_t n = 1; n <= 3; ++n)
	{
		for (size_t x = 1; x <= 3; ++x)
		{
			values[INDEX_C_START + ((n) - 1) + (3 - 1 + 1) * (((x) - 1))] = ((x) - (((0.05) * (n))));
		}
	}
}

int derivative(sunrealtype t, N_Vector y, N_Vector ydot, void *user_data) {
    double* values = N_VGetArrayPointer(y);
    double* derivatives = N_VGetArrayPointer(ydot);

	for (size_t n = 1; n <= 3; ++n)
	{
		for (size_t x = 1; x <= 3; ++x)
		{
			derivatives[INDEX_C_START + ((n) - 1) + (3 - 1 + 1) * (((x) - 1))] = 0;
		}
	}

    return 0;
}