#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <gmp.h>
#include <time.h>


void factorial(uint8_t x, mpz_t x_fact)
{
    mpz_set_ui(x_fact, 1);
    for (int i = x; i > 0; i--)
    {
        mpz_mul_ui(x_fact, x_fact, i);
    }
}

double binomial_distribution(int n, float p, int k)
{
    clock_t begin = clock();
    double total;

    int scale = 1000000000;
    int scaled_p = p * scale;
    mpf_t p_f;
    mpf_init(p_f);
    mpf_set_d(p_f, scaled_p);
    mpf_div_ui(p_f, p_f, scale);
    
    mpf_t q_f;
    mpf_init(q_f);
    mpf_ui_sub(q_f, 1, p_f);
    
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
    mpf_pow_ui(p_part_f, p_f, k);
    
    mpf_t q_part_f;
    mpf_init(q_part_f);
    mpf_pow_ui(q_part_f, q_f, (n - k));

    mpf_clear(p_f);
    mpf_clear(q_f);

    mpf_t total_f;
    mpf_init(total_f);
    mpf_mul(total_f, p_part_f, q_part_f);
    mpf_mul(total_f, total_f, frac_part_f);

    mpf_clear(frac_part_f);
    mpf_clear(p_part_f);
    mpf_clear(q_part_f);

    // Convert from precise float to C float
    total = mpf_get_d(total_f);

    // Free memory of precise floats
    mpf_clear(total_f);

    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("In %f seconds\n", time_spent);
    return total;
}

int main(void)
{
    int n = 49;
    double p = 0.6;
    int k = 30;
    double bin_dist = binomial_distribution(n, p, k);
    printf("%f\n", bin_dist);

    /*
    float p_indv = 1.0/1.5;
    int init_num_susceptible = 49;
    int * arr_num_susceptible = (int)malloc(sizeof(int) * loop_num);
    int * arr_num_new_infected = (int)malloc(sizeof(int) * loop_num);

    free(arr_num_susceptible);
    free(arr_num_new_infected);
    */
    return 0;
}
