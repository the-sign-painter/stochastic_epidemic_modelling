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

float binomial_distribution(int n, float p, int k)
{
    mpf_t sum_prob_k;
    mpf_init(sum_prob_k);
    mpf_set_ui(sum_prob_k, 0);
    for (int k = 0 ; k <= n ; k++)
    {
        mpz_t fact_n;
        mpz_init(fact_n);
        factorial(n, fact_n);

        mpz_t fact_k;
        mpz_init(fact_k);
        factorial(k, fact_k);

        mpz_t fact_nk;
        mpz_init(fact_nk);
        factorial(n-k, fact_nk);

        mpz_t frac;
        mpz_init(frac);
        mpz_mul(frac, fact_k, fact_nk);
        mpz_cdiv_q(frac, fact_n, frac);
        
        mpf_t frac_f;
        mpf_init(frac_f);
        mpf_set_z(frac_f, frac);
        mpf_t prob_k_m;
        mpf_init(prob_k_m);
        mpf_mul_ui(prob_k_m, frac_f, pow(p, k));
        mpf_mul_ui(prob_k_m, prob_k_m, pow((1 - p), (n - k)));
        mpf_add(sum_prob_k, sum_prob_k, prob_k_m);
    }
    double prob_k = mpf_get_d(sum_prob_k);
    return prob_k;
}

int main(void)
{
    int n = 49;
    int p = 0.6;
    float bin_dist = binomial_distribution(n, p);
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
