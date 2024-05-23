#include <cmath>
#include <nvector/nvector_serial.h>
#include <sstream>


double PI = 3.14159;
double BOLTZMAN_CONSTANT = ((8.6173) * (std::pow(10, -(5))));
double MAX_CLUSTER_SIZE = 50;
double i_migration = 0.45;
double v_migration = 1.35;
double i_diffusion_0 = ((1) * (std::pow(10, -(3))));
double v_diffusion_0 = 0.6;
double i_formation = 4.1;
double v_formation = 1.7;
double i_binding = 0.6;
double v_binding = 0.5;
double recombination_radius = ((0.7) * (std::pow(10, -(7))));
double i_loop_bias = 63;
double i_dislocation_bias = 0.8;
double i_dislocation_bias_param = 1.1;
double v_loop_bias = 33;
double v_dislocation_bias = 0.65;
double v_dislocation_bias_param = 1;
double dislocation_density_0 = ((10) * (std::pow(10, -(12))));
double grain_size = ((4) * (std::pow(10, -(3))));
double lattice_param = ((3.6) * (std::pow(10, -(8))));
double burgers_vector = ((lattice_param) / (std::sqrt(2)));
double atomic_volume = ((std::pow(lattice_param, 3)) / (4));
double i_diffusion = ((i_diffusion_0) * (std::exp(-(((i_migration) / (BOLTZMAN_CONSTANT))))));
double v_diffusion = ((v_diffusion_0) * (std::exp(-(((v_migration) / (BOLTZMAN_CONSTANT))))));
double flux = ((2.9) * (std::pow(10, -(7))));
double temperature = ((330) + (273.15));
double recombination = 3;
double i_bi = 5;
double i_tri = 2;
double i_quad = 6;
double v_bi = 6;
double v_tri = 3;
double v_quad = 2;
double dislocation_density_evolution = 300;
double dislocation_gain = 0;
double i1_cluster_abs = 0;
double i1_gb_abs = 0;
double i1_em = 0;
double Riv = ((((((4) * (PI))) * (((i_diffusion) + (v_diffusion))))) * (recombination_radius));
double v1_d_abs = 0;
double v1_gb_abs = 0;
double v1_cluster_abs = 0;
double v1_em = 0;

const size_t INDEX_Ci_START = 0;
const size_t INDEX_Ci_SIZE = MAX_CLUSTER_SIZE - 1 + 1;
const size_t INDEX_Cv_START = INDEX_Ci_START + INDEX_Ci_SIZE;
const size_t INDEX_Cv_SIZE = MAX_CLUSTER_SIZE - 1 + 1;

const size_t INDEX_Rho = INDEX_Cv_START + INDEX_Cv_SIZE;
#define STATE_SIZE INDEX_Rho + 1;

double Gi(double n);
double Gv(double n);
double a_i(double n, double* values);
double b_i(double n, double* values);
double c_i(double n, double* values);
double a_v(double n, double* values);
double b_v(double n, double* values);
double c_v(double n, double* values);
double i1_d_abs(double* values);
double alpha_ii(double n);
double alpha_vv(double n);
double beta_ii(double n);
double beta_iv(double n);
double beta_vi(double n);
double beta_vv(double n);

double __summation_0(double* values) {
	double sum = 0.0;
	for (size_t i = 3; i < MAX_CLUSTER_SIZE; i++) {
		sum += ((alpha_ii((i))) * (values[INDEX_Ci_START + (i) - 1]));
	}
	 return sum;
}

double Gi(double n)
{
	if (n == (1)) {
		 return ((((((flux) * (recombination))) * (((((((1) - (i_bi))) - (i_tri))) - (i_quad))))) / (atomic_volume));
	}
	if (n == (2)) {
		 return ((((((flux) * (recombination))) * (i_bi))) / (atomic_volume));
	}
	if (n == (3)) {
		 return ((((((flux) * (recombination))) * (i_tri))) / (atomic_volume));
	}
	if (n == (4)) {
		 return ((((((flux) * (recombination))) * (i_quad))) / (atomic_volume));
	}
	return 0;
}

double Gv(double n)
{
	if (n == (1)) {
		 return ((((((flux) * (recombination))) * (((((((1) - (v_bi))) - (v_tri))) - (v_quad))))) / (atomic_volume));
	}
	if (n == (2)) {
		 return ((((((flux) * (recombination))) * (v_bi))) / (atomic_volume));
	}
	if (n == (3)) {
		 return ((((((flux) * (recombination))) * (v_tri))) / (atomic_volume));
	}
	if (n == (4)) {
		 return ((((((flux) * (recombination))) * (v_quad))) / (atomic_volume));
	}
	return 0;
}

double a_i(double n, double* values)
{
	return ((((beta_iv((n))) * (values[INDEX_Cv_START + (1) - 1]))) + (alpha_ii((n))));
}

double b_i(double n, double* values)
{
	return ((((((beta_iv((n))) * (values[INDEX_Cv_START + (1) - 1]))) + (((beta_ii((n))) * (values[INDEX_Ci_START + (1) - 1]))))) + (alpha_ii((n))));
}

double c_i(double n, double* values)
{
	return ((beta_ii((n))) * (values[INDEX_Ci_START + (1) - 1]));
}

double a_v(double n, double* values)
{
	return ((((beta_vi((n))) * (values[INDEX_Ci_START + (1) - 1]))) + (alpha_vv((n))));
}

double b_v(double n, double* values)
{
	return ((((((beta_vi((n))) * (values[INDEX_Cv_START + (1) - 1]))) + (((beta_vv((n))) * (values[INDEX_Cv_START + (1) - 1]))))) + (alpha_vv((n))));
}

double c_v(double n, double* values)
{
	return ((beta_vv((n))) * (values[INDEX_Cv_START + (1) - 1]));
}

double i1_d_abs(double* values)
{
	return ((((__summation_0(values)) + (((((2) * (alpha_ii((2))))) * (values[INDEX_Ci_START + (2) - 1]))))) + (((((beta_iv((2))) * (values[INDEX_Cv_START + (1) - 1]))) * (values[INDEX_Ci_START + (2) - 1]))));
}

double alpha_ii(double n)
{
	return 0;
}

double alpha_vv(double n)
{
	return 0;
}

double beta_ii(double n)
{
	return 0;
}

double beta_iv(double n)
{
	return 0;
}

double beta_vi(double n)
{
	return 0;
}

double beta_vv(double n)
{
	return 0;
}

void get_initial_state(N_Vector state) {
    double* values = N_VGetArrayPointer(state);

    for (size_t n = 1; n <= MAX_CLUSTER_SIZE; ++n)
    {
        values[INDEX_Ci_START + (n - 1)] = ((1) * (std::pow(10, -(6))));
    }

    for (size_t n = 1; n <= MAX_CLUSTER_SIZE; ++n)
    {
        values[INDEX_Cv_START + (n - 1)] = ((1) * (std::pow(10, -(6))));
    }
    values[INDEX_Rho] = dislocation_density_0;
}

std::string get_state_csv_label() {
	std::stringstream str; 
	str << "t (seconds)";
	for (size_t i = 1; i <= MAX_CLUSTER_SIZE; ++i)
	{
		str << ", Ci[" << i << "]";
	}

	for (size_t i = 1; i <= MAX_CLUSTER_SIZE; ++i)
	{
		str << ", Cv[" << i << "]";
	}

	str << ", Rho";
	return str.str();
}

int system(sunrealtype t, N_Vector y, N_Vector ydot, void *user_data) {
    double* values = N_VGetArrayPointer(y);
    double* derivatives = N_VGetArrayPointer(ydot);

    for (size_t n = 1; n <= MAX_CLUSTER_SIZE; ++n)
    {
        derivatives[INDEX_Ci_START + (n - 1)] = ((((Gv((n))) + (((((a_i((((n) + (1))), values)) * (-(b_i((n), values))))) * (values[INDEX_Ci_START + (n) - 1]))))) + (((c_i((((n) - (1))), values)) * (values[INDEX_Ci_START + (((n) - (1))) - 1]))));
    }

    for (size_t n = 1; n <= MAX_CLUSTER_SIZE; ++n)
    {
        derivatives[INDEX_Cv_START + (n - 1)] = ((((Gv((n))) + (((((a_v((((n) + (1))), values)) * (-(b_v((n), values))))) * (values[INDEX_Cv_START + (n) - 1]))))) + (((c_v((((n) - (1))), values)) * (values[INDEX_Cv_START + (((n) - (1))) - 1]))));
    }
    derivatives[INDEX_Rho] = ((dislocation_gain) - (((((dislocation_density_evolution) * (std::pow(burgers_vector, 2)))) * (std::pow(values[INDEX_Rho], ((3) / (2)))))));

    derivatives[INDEX_Ci_START + (size_t)(1 - 1)] = ((((((((Gi((1))) - (((((Riv) * (values[INDEX_Ci_START + (1) - 1]))) * (-(i1_d_abs(values))))))) - (i1_gb_abs))) - (i1_cluster_abs))) + (i1_em));
    derivatives[INDEX_Cv_START + (size_t)(1 - 1)] = ((((((((Gv((1))) - (((((Riv) * (values[INDEX_Ci_START + (1) - 1]))) * (-(v1_d_abs)))))) - (v1_gb_abs))) - (v1_cluster_abs))) + (v1_em));
    derivatives[INDEX_Ci_START + (size_t)(MAX_CLUSTER_SIZE - 1)] = 0;
    derivatives[INDEX_Cv_START + (size_t)(MAX_CLUSTER_SIZE - 1)] = 0;
    return 0;
}