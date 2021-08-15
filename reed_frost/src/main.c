#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <gmp.h>
#include <time.h>


typedef uint32_t bin_t;
typedef mpf_t prob_t;

void factorial(uint8_t x, mpz_t x_fact)
{
    mpz_set_ui(x_fact, 1);
    for (int i = x; i > 0; i--)
    {
        mpz_mul_ui(x_fact, x_fact, i);
    }
}

int check(bin_t* arr, int num_bins, int sum)
{
    int count = 0;
    for (int i = 0; i <= num_bins; i++)
    {
        count += arr[i];
    }
    return (count == sum);
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

void cumulative_binomial_distribution(prob_t* cum_bin_dist, int n, mpf_t p)
{
    mpf_t probability;
    mpf_init(probability);
    mpf_init(cum_bin_dist[0]);
    mpf_set_ui(cum_bin_dist[0], 0);

    // k     = <0  0  1  2  ...  n-1   n
    // index =  0  1  2  3  ...   n   n+1 

    int k;
    for (int index = 1; index <= n+1; index++)
    {
        k = index - 1;
        binomial_distribution(probability, n, p, k);
        mpf_init(cum_bin_dist[index]);
        mpf_add(cum_bin_dist[index], cum_bin_dist[index-1], probability);
    }

    mpf_clear(probability);
}

void cumulative_uniform_random_float(mpf_t cumulative_probabilty, int n)
{
    uint16_t rand_num = rand() % n + 1;

    mpf_set_d(cumulative_probabilty, n);
    mpf_ui_div(cumulative_probabilty, 1, cumulative_probabilty);
    mpf_mul_ui(cumulative_probabilty, cumulative_probabilty, rand_num);
}

bin_t random_binomial_integer(int n, prob_t* cum_prob_arr)
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

int reed_frost_model_timestep(int susceptibles, int infectives, mpf_t indiv_probability, prob_t* cum_bin_dist)
{
    int n = susceptibles;

    mpf_t p;
    mpf_init(p);
    get_infection_probability(p, infectives, indiv_probability);

    cumulative_binomial_distribution(cum_bin_dist, n, p);
    int new_infectives = random_binomial_integer(n, cum_bin_dist);

    mpf_clear(p);

    if ((n - new_infectives) <= 0)
    {
        return 0;
    }

    return new_infectives;
}

int reed_frost_model(int initial_susceptibles, int initial_infectives, mpf_t indiv_probability, prob_t* cum_bin_dist)
{
    int n = initial_susceptibles;
    int z = initial_infectives;
    while (z != 0)
    {
        z = reed_frost_model_timestep(n,
                                      z,
                                      indiv_probability,
                                      cum_bin_dist);
        n -= z;
    }
    int total_size = initial_susceptibles - n;
    return total_size;
}

bin_t* reed_frost_model_simulate(int iterations, int initial_susceptibles, int initial_infectives, mpf_t indiv_probability)
{
    int num_bins = initial_susceptibles + initial_infectives + 1;
    bin_t* total_size_bins = (bin_t*)malloc(num_bins * sizeof(bin_t));
    prob_t* cum_bin_dist = (prob_t*)malloc( ( ( initial_susceptibles 
                                                + initial_infectives ) 
                                             + 2 ) 
                                             * sizeof(prob_t) ); 

    for (int b = 0; b < num_bins; b++)
    {
        total_size_bins[b] = 0;
    }

    int total_size;

    for (int i = 0; i < iterations; i++)
    {
        total_size = reed_frost_model(initial_susceptibles,
                                      initial_infectives,
                                      indiv_probability,
                                      cum_bin_dist);
        total_size_bins[total_size] += 1;
    }
    
    free(cum_bin_dist);

    for (int b = 0; b < num_bins; b++)
    {
        printf("%02d: %d\n", b, total_size_bins[b]);
    }
    if (!check(total_size_bins, num_bins, iterations))
    {
        printf("Unequal bin contents and iterations set mismatch.\n");
        exit(-1);
    }
    printf("Check complete.\n");
    return total_size_bins;
}

void convert_double_to_mpf(double f, mpf_t accurate_float)
{
    int scale = 100000;
    double scaled_f = scale * f;
    mpf_set_d(accurate_float, scaled_f);
    mpf_div_ui(accurate_float, accurate_float, scale);
}

bin_t max(bin_t* arr, int arr_size)
{
    bin_t max_val = 0;
    for (int i = 0; i < arr_size; i++)
    {
        if (arr[i] > max_val)
        {
            max_val = arr[i];
        }
    }
    return max_val;
}

void draw_histogram(bin_t* bins, int num_bins)
{
    int y_max_intervals = 10;
    int y_interval_size = ceil( (float) y_max_intervals / 
                                (float) max(bins, num_bins));
    for (int i = y_max_intervals; i > 0; i--)
    {
        for (int j = 0; j < num_bins; j++)
        {
            int square = ( bins[j] - ( i * y_interval_size ) );
            if ( ( bins[j] - (i * y_interval_size) ) > (y_max_intervals / 2) )
            {
                printf("*");
            }
            else
            {
                printf(" ");
            }
        }
        printf("\n");
    }
}

int main(void)
{
    clock_t begin = clock();
    time_t t;
    srand((unsigned) time(&t));

    int susceptibles            =   49  ;
    int infectives              =    1  ;
    double indiv_probability_d  =    0.1;
    int iterations              = 1000  ;

    mpf_t indiv_probability;
    mpf_init(indiv_probability);
    convert_double_to_mpf(indiv_probability_d, indiv_probability);

    bin_t* bins = reed_frost_model_simulate(iterations,
                                            susceptibles,
                                            infectives,
                                            indiv_probability);

    int num_bins = susceptibles + infectives + 1;
    // draw_histogram(bins, num_bins);

    mpf_clear(indiv_probability);
    free(bins);

    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("In %f seconds\n", time_spent);

    return 0;
}

