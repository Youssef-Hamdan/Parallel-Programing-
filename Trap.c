%%sh
cat > a.c << EOF
#include <mpi.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
// Function to evaluate the curve (y = f(x))
float f(float x) {
    return x * x ; // Example: y = x^2
}

// Function to compute the area of a trapezoid
float trapezoid_area(float a, float b, float d) { 
    float area = 0;
    for (float x = a; x < b; x+=d) {
        area += f(x) + f(x+d);
    }
    
    return area * d / 2.0f;
}

int main(int argc, char** argv) {
    int rank, size;
    float a = 0.0f, b = 10.0f;  // Limits of integration
    int n;
    float start, end, local_area, total_area;
    double start_time,end_time;
    
    MPI_Init(&argc, &argv); // Initialize MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Get rank of the process
    MPI_Comm_size(MPI_COMM_WORLD, &size); // Get number of processes

    if (rank == 0) {
        // Get the number of intervals from the user
        printf("Enter the number of intervals: ");
        n=1;
    }
    
    start_time=MPI_Wtime();
    sleep(0.5);
    // Broadcast the number of intervals to all processes
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    // Calculate the interval size for each process
    float d = (b - a) / n; // delta
    float region = (b - a)/ size;
    
    // Calculate local bounds for each process
    start = a + rank * region;
    end = start + region;
    
    // Each process calculates the area of its subinterval
    local_area = trapezoid_area(start, end, d);
    
    // Reduce all local areas to the total area on the root process
    MPI_Reduce(&local_area, &total_area, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);

    end_time=MPI_Wtime();

    if (rank == 0) {
        printf("The total number of intervals is: 10000\n");
        printf("The total area under the curve is: %f\n", total_area);
        printf("The total time of execution is: %f sec\n",end_time - start_time );
    }
    
    MPI_Finalize(); // Finalize MPI
    return 0;
}
EOF
