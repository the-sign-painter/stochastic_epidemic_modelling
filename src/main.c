#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <gmp.h>
#include <time.h>


typedef uint16_t bin_t;
typedef mpf_t prob_t;

void factorial(uint8_t x, mpz_t x_fact)
{
    mpz_set_ui(x_fact, 1);
    for (int i = x; i > 0; i--)
    {
        mpz_mul_ui(x_fact, x_fact, i);
    }
}

void binomial_distribution(mpf_t probability, int n, mpf_t p, int k)
{
    mpf_t q;
    mpf_init(q);
    mpf_ui_sub(q, 1, p);
    
    // Get n! k! and (n-k)!
    mpz_t fact_n;
    mpz_init(fact_n);
    factorial(n, fact_n);

    mpz_t fact_k;
    mpz_init(fact_k);
    factorial(k, fact_k);

    mpz_t fact_nk;
    mpz_init(fact_nk);
    factorial(n-k, fact_nk);

    // Multiply and divide
    //      n!
    // -----------
    // k! (n - k)!
    mpz_t frac;
    mpz_init(frac);
    mpz_mul(frac, fact_k, fact_nk);
    mpz_cdiv_q(frac, fact_n, frac); // This is a ceiling divide for the quotient

    // Free memory of large, unused integers
    mpz_clear(fact_n);
    mpz_clear(fact_k);
    mpz_clear(fact_nk);
    
    // Convert the fraction of factorials to a precise float
    mpf_t frac_part_f;
    mpf_init(frac_part_f);
    mpf_set_z(frac_part_f, frac);

    // Free memory of fraction of factorials as precise integer
    mpz_clear(frac);

    // Multiply by p^k and q^(n-k)
    mpf_t p_part_f;
    mpf_init(p_part_f);
    mpf_pow_ui(p_part_f, p, k);
    
    mpf_t q_part_f;
    mpf_init(q_part_f);
    mpf_pow_ui(q_part_f, q, (n - k));

    mpf_clear(q);

    mpf_mul(probability, p_part_f, q_part_f);
    mpf_mul(probability, probability, frac_part_f);

    mpf_clear(frac_part_f);
    mpf_clear(p_part_f);
    mpf_clear(q_part_f);
}

prob_t* cumulative_binomial_distribution(int n, mpf_t p)
{
    prob_t* cum_prob_arr = (prob_t*)malloc((n + 2) * sizeof(prob_t));

    mpf_t probability;
    mpf_init(probability);
    mpf_init(cum_prob_arr[0]);
    mpf_set_ui(cum_prob_arr[0], 0);

    // k     = <0  0  1  2  ... n-1   n
    // index =  0  1  2  3  ...  n   n+1 

    int k;
    for (int index = 1; index <= n+1; index++)
    {
        k = index - 1;
        binomial_distribution(probability, n, p, k);
        mpf_init(cum_prob_arr[index]);
        mpf_add(cum_prob_arr[index], cum_prob_arr[index-1], probability);
    }

    mpf_clear(probability);

    return cum_prob_arr;
}

void cumulative_uniform_random_float(mpf_t cumulative_probabilty, int n)
{
    uint16_t rand_num = rand() % n + 1;

    mpf_set_d(cumulative_probabilty, n);
    mpf_ui_div(cumulative_probabilty, 1, cumulative_probabilty);
    mpf_mul_ui(cumulative_probabilty, cumulative_probabilty, rand_num);
}

uint16_t random_binomial_integer(int n, prob_t* cum_prob_arr)
{
    mpf_t cum_uni_prob;
    mpf_init(cum_uni_prob);
    cumulative_uniform_random_float(cum_uni_prob, n);

    int k = 0;
    for (int i = 0; mpf_cmp(cum_uni_prob, cum_prob_arr[i]) > 0; i++)
    {
        k = i+1;
    }
    k -= 1;
    mpf_clear(cum_uni_prob);
    return k;
}

void get_infection_probability(mpf_t infection_probability, int infectives, mpf_t indiv_probability)
{
    // p_i = 1 - ( 1 - p ) ^ I
    mpf_ui_sub(infection_probability, 1, indiv_probability);
    mpf_pow_ui(infection_probability, infection_probability, infectives);
    mpf_ui_sub(infection_probability, 1, infection_probability);
}

int reed_frost_model_timestep(int susceptibles, int infectives, mpf_t indiv_probability)
{
    int n = susceptibles;

    mpf_t p;
    mpf_init(p);
    get_infection_probability(p, infectives, indiv_probability);

    prob_t* cum_prob_arr = cumulative_binomial_distribution(n, p);
    int new_infectives = random_binomial_integer(n, cum_prob_arr);

    free(cum_prob_arr);
    mpf_clear(p);

    if ((n - new_infectives) <= 0)
    {
        return 0;
    }

    return new_infectives;
}

int reed_frost_model(int initial_susceptibles, int initial_infectives, mpf_t indiv_probability)
{
    int n = initial_susceptibles;
    int z = initial_infectives;
    while (z != 0)
    {
        z = reed_frost_model_timestep(n, z, indiv_probability);
        n -= z;
    }
    int total_size = initial_susceptibles - n;
    return total_size;
}

bin_t* reed_frost_model_simulate(int iterations, int initial_susceptibles, int initial_infectives, mpf_t indiv_probability)
{
    int num_bins = initial_susceptibles + initial_infectives + 1;
    bin_t* total_size_bins = (bin_t*)malloc(num_bins * sizeof(bin_t));
    
    for (int b = 0; b < num_bins; b++)
    {
        total_size_bins[b] = 0;
    }

    int total_size;

    for (int i = 0; i < iterations; i++)
    {
        total_size = reed_frost_model(initial_susceptibles, initial_infectives, indiv_probability);
        total_size_bins[total_size] += 1;
    }
    
    for (int b = 0; b < num_bins; b++)
    {
        printf("%02d: %d\n", b, total_size_bins[b]);
    }

    return total_size_bins;
}

void convert_double_to_mpf(double f, mpf_t accurate_float)
{
    int scale = 100000;
    double scaled_f = scale * f;
    mpf_set_d(accurate_float, scaled_f);
    mpf_div_ui(accurate_float, accurate_float, scale);
}

int main(void)
{
    clock_t begin = clock();
    srand((unsigned) time(0));

    int susceptibles = 49;
    int infectives = 1;
    double indiv_probability_d = 0.1;
    int iterations = 100;

    mpf_t indiv_probability;
    mpf_init(indiv_probability);
    convert_double_to_mpf(indiv_probability_d, indiv_probability);

    bin_t* out = reed_frost_model_simulate(iterations, susceptibles, infectives, indiv_probability);

    mpf_clear(indiv_probability);
    free(out);

    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("In %f seconds\n", time_spent);

    return 0;
}

