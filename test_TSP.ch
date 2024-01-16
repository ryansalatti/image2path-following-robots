#!/bin/ch
#include <math.h>
#include <cv.h>
#include <highgui.h>
#include "point_def.h"

typedef struct {
    Point* points;
    int totalPoints;
    double fitness;
} Tour;

double distance(Point p1, Point p2) {
    return sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
}

double calculateFitness(Tour* tour) {
    double totalDistance = 0.0;
    int i;
    for (i = 0; i < tour->totalPoints - 1; i++) {
        totalDistance += distance(tour->points[i], tour->points[i + 1]);
    }
    totalDistance += distance(tour->points[tour->totalPoints - 1], tour->points[0]); // Return to start
    return 1 / totalDistance;
}

Tour* initializePopulation(Point* allPoints, int totalPoints, int populationSize) {
    Tour* population = malloc(populationSize * sizeof(Tour));
    int i;
    for (i = 0; i < populationSize; i++) {
        population[i].points = malloc(totalPoints * sizeof(Point));
        memcpy(population[i].points, allPoints, totalPoints * sizeof(Point));
        // Shuffle the points to create a random tour
        shuffle(population[i].points, totalPoints);
        population[i].totalPoints = totalPoints;
        population[i].fitness = 0.0;
    }
    return population;
}

void mutatePopulation(Tour* population, int populationSize, double mutationRate) {
    int i;
    for (i = 0; i < populationSize; i++) {
        mutate(&population[i], mutationRate);
    }
}

int mutate(Tour* tour, double mutationRate) {
    int i;
    int j;
    for (i = 0; i < tour->totalPoints; i++) {
        if ((double)rand() / RAND_MAX < mutationRate) {
            j = rand() % tour->totalPoints;
            // Swap points i and j
            Point temp = tour->points[i];
            tour->points[i] = tour->points[j];
            tour->points[j] = temp;
        }
    }
    return 0;
}

Tour findBestTour(Tour* population, int populationSize) {
    Tour bestTour = population[0];
    int i;
    for (i = 1; i < populationSize; i++) {
        if (population[i].fitness > bestTour.fitness) {
            bestTour = population[i];
        }
    }
    return bestTour;
}

void freePopulation(Tour* population, int populationSize) {
    int i;
    for (i = 0; i < populationSize; i++) {
        free(population[i].points);
    }
    free(population);
}

int shuffle(Point* arrayn, int n) {
    if (n > 1) {
        int i;
        int j;
        for (i = 0; i < n - 1; i++) {
            j = i + rand() / (RAND_MAX / (n - i) + 1);
            Point temp = arrayn[j];
            arrayn[j] = arrayn[i];
            arrayn[i] = temp;
        }
    }
    return 0;
}

void onePointCrossover(Point* parent1, Point* parent2, Point* offspring, int totalPoints) {
    int crossoverPoint = rand() % totalPoints;
    int i;
    for (i = 0; i < crossoverPoint; i++) {
        offspring[i] = parent1[i];
    }
    for (i = crossoverPoint; i < totalPoints; i++) {
        offspring[i] = parent2[i];
    }
}

Tour* selectAndBreed(Tour* population, int populationSize, int totalPoints) {
    Tour* newPopulation = malloc(populationSize * sizeof(Tour));
    int i;
    for (i = 0; i < populationSize; i++) {
        int parent1Index = rand() % populationSize;
        int parent2Index = rand() % populationSize;

        newPopulation[i].points = malloc(totalPoints * sizeof(Point));
        newPopulation[i].totalPoints = totalPoints;
        newPopulation[i].fitness = 0.0;

        onePointCrossover(population[parent1Index].points, 
                          population[parent2Index].points, 
                          newPopulation[i].points, 
                          totalPoints);
    }

    return newPopulation;
}


void geneticAlgorithm(Point* allPoints, int totalPoints, int populationSize, int maxGenerations, double mutationRate) {
    // Initialize population
    Tour* population = initializePopulation(allPoints, totalPoints, populationSize);
    int generation;
    int i;
    for (generation = 0; generation < maxGenerations; generation++) {
        // Calculate fitness for each tour
        for (i = 0; i < populationSize; i++) {
            population[i].fitness = calculateFitness(&population[i]);
        }

        // Selection, Crossover, and Mutation
        Tour* newPopulation = selectAndBreed(population, populationSize, totalPoints);
        mutatePopulation(newPopulation, populationSize, mutationRate);

        // Replace old population
        freePopulation(population, populationSize);
        population = newPopulation;
    }

    // Find the best tour in the final population
    Tour bestTour = findBestTour(population, populationSize);

    // Copy the best tour to allPoints
    for (i = 0; i < totalPoints; i++) {
        allPoints[i] = bestTour.points[i];
    }

    freePopulation(population, populationSize);
}

int main() {
    
    
    FILE *file = fopen("points.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }
    
    int totalCorners;
    if (fscanf(file, "%d", &totalCorners) != 1) {
        perror("Error reading totalCorners");
        return 1;
    }

    // initialize pathPoints (based on totalCorners)
    Point* pathPoints = (Point*)malloc(totalCorners * sizeof(Point));
    if (pathPoints == NULL) {
        perror("Error allocating memory");
        return 1;
    }

    size_t numPoints = 0;
    char line[256]; // lines in the file shouldn't exceed this length

    // read remaining lines to store pathPoints
    while (fgets(line, sizeof(line), file)) {
        double x, y;
        if (sscanf(line, "%lf, %lf", &x, &y) == 2) {
            if (numPoints < totalCorners) {
                pathPoints[numPoints].x = x;
                pathPoints[numPoints].y = y;
                numPoints++;
            } else {
                // if more lines than totalCorners
                printf("Warning: More lines in the file than specified by totalCorners.\n");
                break;
            }
        }
    }

    fclose(file);
    
    int populationSize = 50; // Example population size
    int maxGenerations = 1000; // Example number of generations
    double mutationRate = 0.05; // Example mutation rate

    geneticAlgorithm(pathPoints, totalCorners, populationSize, maxGenerations, mutationRate);
    
    FILE* file_new = fopen("points_new.txt", "w");
    if (!file_new) {
        perror("Error opening file for writing");
        free(pathPoints);
        return 0;
    }
    fprintf(file_new, "%d\n", totalCorners);
    
    int i;
    for (i = 0; i < totalCorners; i++) {
        fprintf(file_new, "%f, %f\n", pathPoints[i].x, pathPoints[i].y);
    }

    fclose(file_new);
    
}
