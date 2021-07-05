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

void binomial_distribution(mpf_t probability, int n, float p, int k)
{
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

float* cumulative_binomial_distribution(int n, double p)
{
    prob_t* cum_prob_arr = (prob_t*)malloc((n + 2) * sizeof(prob_t));
    float* float_arr = (float*)malloc((n + 2) * sizeof(float));

    mpf_t probability;
    mpf_init(probability);
    mpf_init(cum_prob_arr[0]);
    mpf_set_ui(cum_prob_arr[0], 0);

    for (int k = 0; k <= n; k++)
    {
        binomial_distribution(probability, n, p, k);
        mpf_init(cum_prob_arr[k + 1]);
        mpf_add(cum_prob_arr[k + 1], cum_prob_arr[k], probability);
        float_arr[k + 1] = mpf_get_d(cum_prob_arr[k]) + mpf_get_d(probability);
    }

    mpf_clear(probability);
    free(cum_prob_arr);

    return float_arr;
}

void cumulative_uniform_random_float(mpf_t cumulative_probabilty, int n)
{
    uint16_t rand_num = rand() % n + 1;

    mpf_set_d(cumulative_probabilty, n);
    mpf_ui_div(cumulative_probabilty, 1, cumulative_probabilty);
    mpf_mul_ui(cumulative_probabilty, cumulative_probabilty, rand_num);
}

uint16_t random_binomial_integer(int n, double p)
{
    mpf_t cum_uni_prob;
    mpf_init(cum_uni_prob);
    cumulative_uniform_random_float(cum_uni_prob, n);

    float* cum_prob_arr = cumulative_binomial_distribution(n, p);

    float t = mpf_get_d(cum_uni_prob);
    printf("Uni:\n%f\n", t);
    int k = 0;
    for (int i = 0; mpf_cmp_d(cum_uni_prob, cum_prob_arr[i]) > 0; i++)
    {
        k = i+1;
    }
    printf("%d\n", k);
    mpf_clear(cum_uni_prob);
    free(cum_prob_arr);
    return k;
}

bin_t* simulate(int iterations, int n, float p)
{
    time_t t;
    srand((unsigned) time(&t));
    bin_t* bins = (bin_t*)malloc((n + 1) * sizeof(bin_t));
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
    int iterations = 1000;
    
    bin_t* bins;

    bins = simulate(iterations, n, p);

    int sum = 0;
    for (int i = 0; i <= n; i++)
    {
        printf("%02u: %u\n", i, bins[i]);
        sum += bins[i];
    }
    printf("sum = %u\n", sum);

    free(bins);
    
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("In %f seconds\n", time_spent);

    return 0;
}
