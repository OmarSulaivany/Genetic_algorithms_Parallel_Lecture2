// Genetic_algorithms_Parallel_Lecture2.cpp : This file contains the 'main' function. Program execution begins and ends there.

// ==========================================
// Title:  C++ program to create target string using Genetic Algorithm.
// Author: Omar T. Mohammed
// Date:   24 Dec 2020
// ==========================================

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <ctime>
#include <chrono>
#include <omp.h>
#define GREEN   "\033[32m"
#define RED     "\033[31m"   
#define RESET   "\033[0m"
#define CYAN    "\033[36m"     
#define YELLOW  "\033[33m"
#define BOLDWHITE   "\033[1m\033[37m" 
using namespace std;


/* Number of individuals per each generation. */
#define POPULATION_SIZE 100 


/* Function to generate random numbers in a given range. */  
int random_number(int begin, int end)
{
    int range = (end - begin) + 1;
    int random_int = begin + (rand() % range);
    return random_int;
}


/* Our Target string. */
string Target()
{
    const string TARGET = "Catch Me If You Can";

    return TARGET;
}

/* Possible Genes. */
const string GENES = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ 1234567890, .-;:_!\"#%&/()=?@${[]}";


/* Create random genes for mutation operation. */
char mutate_genes()
{
    unsigned len = GENES.size();
    int rand = random_number(0, len - 1);
    return GENES[rand];
}

/* Create chromosome or string of genes. */
string create_chromosome()
{
    unsigned len = Target().size();
    string chromosome = "";

#pragma omp for
    for (unsigned i = 0; i < len; i++)
        chromosome += mutate_genes();
    return chromosome;
}

/* Class representing individual in population "Equivalent to Solutions". */
struct Individual
{
    Individual(string chromosome);
    Individual mate(Individual other_individual);
    unsigned fitness_function();

    string chromosome;
    int fitness_value;
    
};

/* Constructor. */
Individual::Individual(string chromosome)
{
    this->chromosome = chromosome;
    fitness_value = fitness_function();
};

/* Perform mating and produce new "child" offspring. */
Individual Individual::mate(Individual other_individual)
{
    /* Chromosome for "child" offspring. */
    string child_chromosome = "";

    unsigned len = chromosome.size();
#pragma omp for
    for (unsigned i = 0; i < len; i++)
    {
        /* random probability. */  
        float probability = random_number(0, 100) / 100;

        /* if probability is less than 0.30, insert gene from parent 1 */ 
        if (probability < 0.30)
            child_chromosome += chromosome[i];

        /* if probability is between 0.30 and 0.85 then insert gene from parent 2 */
        else if (probability < 0.85)
            child_chromosome += other_individual.chromosome[i];

        /* Otherwise insert random gene(mutate), for maintaining diversity. */
        else
            child_chromosome += mutate_genes();
    }

    /* Create new child (offspring) using generated chromosome for offspring. */
    return Individual(child_chromosome);
};

/* Calculate fittness score, in this example it is the number of characters in string that differs target string. */
unsigned Individual::fitness_function()
{
    unsigned len = Target().size();
    unsigned fitness_value = 0;

#pragma omp for
    for (unsigned i = 0; i < len; i++)
    {
        if (chromosome[i] != Target()[i])
            fitness_value++;
    }
    return fitness_value;
};

/* Operator Overloading < */ 
bool operator<(const Individual& ind1, const Individual& ind2)
{
    return ind1.fitness_value < ind2.fitness_value;
}


int main()
{   
    /* Capture the starting excution time, this is to later measure execution time for the whole process in seconds. */
    auto starting_time = chrono::high_resolution_clock::now();

    srand((unsigned)(time(0)));

    /* First generation. */
    unsigned generation = 0;


    bool target_found = false;
    vector<Individual> population;
    
#pragma omp for
    /* Create initial population randomly. */
    for (unsigned i = 0; i < POPULATION_SIZE; i++)
    {
        string chromosome = create_chromosome();
        population.push_back(Individual(chromosome));
    }
#pragma omp for
        while (!target_found)
        {
            /*sort the population in an increasing order of the fitness score  */

            sort(population.begin(), population.end());

            /*if the first individual's fitness score in the population was 0, then we know that we have reached the target.
            Thus, we will terminate the loop i.e while (!target_found)*/
            if (population[0].fitness_value <= 0)
            {
                target_found = true;
                break;
            }

            /* Otherwise generate new (children) offsprings for new generation. */
            vector<Individual> new_generation;

            /* Perform Elitism, that mean 20% of fittest population will get passed directly to the next generation. */
            int Elitism = (25 * POPULATION_SIZE) / 100;
            for (int i = 0; i < Elitism; i++)
                new_generation.push_back(population[i]);

            /* From 90% of fittest population, Individuals  will mate to produce offspring */
            Elitism = (75 * POPULATION_SIZE) / 100;
            for (unsigned i = 0; i < Elitism; i++)
            {
                unsigned len = population.size();
                int rand = random_number(0, 25);

                /* Select parent 1. */
                Individual parent1 = population[rand];

                /* Select parent 2. */
                rand = random_number(0, 25);
                Individual parent2 = population[rand];

                Individual child = parent1.mate(parent2);
                new_generation.push_back(child);
            }
            population = new_generation;
            cout << "Generation: "  <<BOLDWHITE<<generation  <<RESET<<"\n\n";
            cout << "Best Solution: " << GREEN << population[0].chromosome << RESET << "\t";
            cout << "Fitness score: " <<CYAN<< population[0].fitness_value <<RESET<< "\n";

            generation++;
            system("CLS");
        }
    
    cout << "Generation: "  <<BOLDWHITE<<generation <<RESET<< "\n";
    cout << "Best Solution: " <<GREEN<< population[0].chromosome <<RESET<< " \n"; 
    cout << "Fitness score: " <<CYAN<< population[0].fitness_value <<RESET <<"\n";

    auto ending_time = chrono::high_resolution_clock::now();

    auto time_taken = chrono::duration_cast<chrono::seconds>(starting_time - ending_time).count();

    cout << "Executation time taken: "<<YELLOW<<abs(time_taken)<< " Seconds\n"<<RESET;
}


// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
