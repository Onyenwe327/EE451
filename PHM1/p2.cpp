#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <omp.h>
#include <float.h>

#define h 800
#define w 800
#define input_file "input.raw"
#define output_file "output.raw"
#define K 4
#define MAX_ITERATIONS 30

int main(int argc, char** argv) {
    int i, j, k, iteration;
    FILE *fp;
    unsigned char *a = (unsigned char*) malloc(sizeof(unsigned char) * h * w);
    int *cluster = (int*) malloc(sizeof(int) * h * w);
    float means[K] = {0, 85, 170, 255};
    int counts[K];
    float new_means[K];

    // Read input file
    if (!(fp = fopen(input_file, "rb"))) {
        printf("Cannot open input file\n");
        return 1;
    }
    fread(a, sizeof(unsigned char), w * h, fp);
    fclose(fp);

    // Measure the start time
    double start_time = omp_get_wtime();

    // K-Means algorithm
    for (iteration = 0; iteration < MAX_ITERATIONS; iteration++) {
        // Reset counts and new means
        for (k = 0; k < K; k++) {
            counts[k] = 0;
            new_means[k] = 0;
        }

        // Assign each pixel to the closest cluster
        #pragma omp parallel for private(j, k) reduction(+:counts[:K], new_means[:K])
        for (i = 0; i < h * w; i++) {
            float min_distance = FLT_MAX;
            int closest_cluster = 0;

            for (k = 0; k < K; k++) {
                float distance = fabs(a[i] - means[k]);
                if (distance < min_distance) {
                    min_distance = distance;
                    closest_cluster = k;
                }
            }

            cluster[i] = closest_cluster;
            counts[closest_cluster]++;
            new_means[closest_cluster] += a[i];
        }

        // Update means
        for (k = 0; k < K; k++) {
            if (counts[k] > 0) {
                means[k] = new_means[k] / counts[k];
            }
        }
    }

    // Assign final cluster means to pixels
    #pragma omp parallel for
    for (i = 0; i < h * w; i++) {
        a[i] = (unsigned char)round(means[cluster[i]]);
    }

    // Measure the end time
    double end_time = omp_get_wtime();

    // Print execution time
    printf("Execution time: %.4f seconds\n", end_time - start_time);

    // Write output file
    if (!(fp = fopen(output_file, "wb"))) {
        printf("Cannot open output file\n");
        return 1;
    }
    fwrite(a, sizeof(unsigned char), w * h, fp);
    fclose(fp);

    // Free allocated memory
    free(a);
    free(cluster);

    return 0;
}