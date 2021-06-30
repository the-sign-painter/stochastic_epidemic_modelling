#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <gmp.h>

void factorial(char * mem, int x)
{
    mpz_init(mpz_t x_fact);
    mpz_add_ui( x_fact, 1 );
    for (uint8_t i = x ; i > 0 ; i--)
    {
        mpz_mul_ui(x_fact, x_fact, i);
    }
    mpz_out_str(char * x_fact_str, 10, x_fact);
    mem = &x_fact_str;
}

int fraction(int n, int k)
{
    factorial(char * fact_n ,     n);
    factorial(char * fact_k ,     k);
    factorial(char * fact_nk, n - k);
    long int out = mpz_cdiv_q_ui(mpz_in_str(fafact_n), );
    return mpz_cdiv_q_ui(fact_n, mpz_mul(fact_k, fact_nk));
}

int binomial(int n, float p)
{
    uint64_t bin;
    float prob_k;
    for (int k = 0 ; k <= n ; k++)
    {
        bin = fraction(n, k);
        prob_k = bin * pow(p, k) * pow((1 - p), (n - k));
        printf("%d\n", prob_k);
    }
    return prob_k;
}

int main(void)
{
    float p_indv = 1.0/1.5;
    int init_num_susceptible = 49;
//    printf("%d", binomial(init_num_susceptible, p_indv));
    int n = 49;
    int k = 3;
    printf("When:\nn = %d\bk = %d\nThen:\nfraction = %d",n, k, fraction(n, k));

    /*
    int * arr_num_susceptible = (int)malloc(sizeof(int) * loop_num);
    int * arr_num_new_infected = (int)malloc(sizeof(int) * loop_num);

    

    free(arr_num_susceptible);
    free(arr_num_new_infected);
    */
    return 0;
}
