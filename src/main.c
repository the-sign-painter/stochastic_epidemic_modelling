#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
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

void binomial_distribution(mpf_t probability, int n, float p, int k)
{
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

    mpf_mul(probability, p_part_f, q_part_f);
    mpf_mul(probability, probability, frac_part_f);

    mpf_clear(frac_part_f);
    mpf_clear(p_part_f);
    mpf_clear(q_part_f);
}

void cumulative_binomial_distribution(mpf_t cumulative_probability, int n, double p, int k)
{
    mpf_t probability;
    mpf_init(probability);
    mpf_set_ui(cumulative_probability, 0);

    for (int i = 0; i <= k; i++)
    {
        binomial_distribution(probability, n, p, i);
        mpf_add(cumulative_probability, cumulative_probability, probability);
    }
    mpf_clear(probability);
}

void cumulative_uniform_random_float(mpf_t cumulative_probabilty, int n)
{
    double random_float;
    time_t t;
    srand((unsigned) time(&t));
    uint16_t rand_num = rand() % (n + 1);

    mpf_set_d(cumulative_probabilty, n);
    mpf_ui_div(cumulative_probabilty, 1, cumulative_probabilty);
    mpf_mul_ui(cumulative_probabilty, cumulative_probabilty, rand_num);
}

uint16_t random_binomial_integer(int n, double p)
{
    mpf_t cum_uni_prob;
    mpf_init(cum_uni_prob);
    cumulative_uniform_random_float(cum_uni_prob, n);

    mpf_t bin_prob;
    mpf_init(bin_prob);

    mpf_t cum_bin_prob;
    mpf_init(cum_bin_prob);

    mpf_t uni_step_size;
    mpf_init(uni_step_size);
    mpf_set_ui(uni_step_size, 1);
    mpf_ui_div(uni_step_size, 1, uni_step_size);

    uint16_t k = 0;
    while (mpf_cmp(cum_uni_prob, cum_bin_prob) > 0)
    {
        binomial_distribution(bin_prob, n, p, k);
        mpf_add(cum_bin_prob, cum_bin_prob, bin_prob);
        k = k + 1;
    }
    k = k - 1;

    mpf_clear(bin_prob);
    mpf_clear(cum_uni_prob);
    mpf_clear(cum_bin_prob);
    return k;
}

int* simulate(int iterations, int n, float p)
{
    int* bins = (int*)malloc((n + 1) * sizeof(uint16_t));
    for (int b = 0; b <= n; b++)
    {
        bins[b] = 0;
    }

    uint16_t rand_bin_int;
    for (int i = 0; i < iterations; i++)
    {
        rand_bin_int = random_binomial_integer(n, p);
        bins[rand_bin_int] = bins[rand_bin_int] + 1;
    }
    return bins;
}

int main(void)
{
    clock_t begin = clock();
    
    int n = 49;
    double p = 0.6;
    int iterations = 10;
    int* bins;
    
    bins = simulate(iterations, n, p);
    
    for (int i = 0; i < n; i++)
    {
        printf("%u: %u\n", i, bins[i]);
    }

    free(bins);

    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("In %f seconds\n", time_spent);

    return 0;
}
