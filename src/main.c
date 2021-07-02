#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <gmp.h>


void factorial(uint8_t x, mpz_t x_fact)
{
    mpz_set_ui(x_fact, 1);
    for (int i = x; x > 0; x--)
    {
        mpz_mul_ui(x_fact, x_fact, i);
    }
}

double binomial_distribution(int n, float p, int k)
{
    float q = 1.0 - p;
    double prob_k;

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
    mpf_t frac_f;
    mpf_init(frac_f);
    mpf_set_z(frac_f, frac);

    // Free memory of fraction of factorials as precise integer
    mpz_clear(frac);

    // Multiply by p^k and q^(n-k)
    mpf_t prob_k_f;
    mpf_init(prob_k_f);
    mpf_mul_ui(prob_k_f, frac_f, pow(p, k));
    mpf_mul_ui(prob_k_f, prob_k_f, pow(q, (n - k)));

    // Convert from precise float to C float
    prob_k = mpf_get_d(prob_k_f);

    // Free memory of precise floats
    mpf_clear(frac_f);
    mpf_clear(prob_k_f);

    return prob_k;
}

int main(void)
{
    int n = 49;
    int p = 0.6;
    int k = 30;
    float bin_dist = binomial_distribution(n, p, k);
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
