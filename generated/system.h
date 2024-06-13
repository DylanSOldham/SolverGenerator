#include <cmath>
#include <nvector/nvector_serial.h>
#include <sstream>

const double end_time = std::pow(10, 1);
const double sample_interval = 1;
const double absolute_tolerance = std::pow(10, -(1));
const double relative_tolerance = std::pow(10, -(6));
const size_t initial_step_size = 1e-10;
const double maximum_step_size = std::pow(10, 5);
const double minimum_step_size = std::pow(10, -(30));
const double maximum_num_steps = 500;
const bool use_direct_solver = 0;


const size_t INDEX_C_START = 0;
const size_t INDEX_C_SIZE = (3 - 1 + 1);
const size_t STATE_SIZE =INDEX_C_START + INDEX_C_SIZE;


std::string get_state_csv_label() {
	std::stringstream str; 
	str << "t (seconds)";
	for (size_t n = 1; n <= 3; ++n)
	{
		str << ", C[" << n << "]";
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
		values[INDEX_C_START + ((n) - 1)] = n;
	}
}

int derivative(sunrealtype t, N_Vector y, N_Vector ydot, void *user_data) {
    double* values = N_VGetArrayPointer(y);
    double* derivatives = N_VGetArrayPointer(ydot);

	for (size_t n = 1; n <= 3; ++n)
	{
		derivatives[INDEX_C_START + ((n) - 1)] = ((3) * (n));
	}

    return 0;
}