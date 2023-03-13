#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define MASTER 0
#define TAG 0 


int main(int argc, char **argv)
{
    int num_proc, id;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_proc);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    
    if (num_proc == 1){
        printf("\nInvalid number of processes.\n");
        exit(-1);
    }
    const  int arr_size = 15,
           items_per_proc = arr_size / num_proc,
           rest = arr_size % num_proc;
    
    int position;
    if (id == MASTER){
        // initialize arrays send elements to each process
        
        int v1[arr_size], v2[arr_size];
        int sum, data;
        char *outbuff;
        MPI_Status status;

        for (int i = 0; i < arr_size; i++)
            v1[i] = i;
        
        for (int i = 0; i < arr_size; i++)
            v2[i] = i;

        int local_items, capacity;
        for (int i = 1; i < num_proc; i++){
            position = 0;
            // Process with last identifier will handle the rest
            local_items = (i == num_proc-1) ? items_per_proc + rest : items_per_proc;
            capacity = local_items*2*sizeof(int);
            outbuff = (char *) malloc(capacity);

            MPI_Pack(v1+i*items_per_proc, local_items, MPI_INT, outbuff, capacity, &position, MPI_COMM_WORLD);
            MPI_Pack(v2+i*items_per_proc, local_items, MPI_INT, outbuff, capacity, &position, MPI_COMM_WORLD);
            MPI_Send(outbuff, capacity, MPI_PACKED, i, TAG, MPI_COMM_WORLD);
            
            free(outbuff);
        }
        sum = 0;
        for (int i = 0; i < items_per_proc; i++)
            sum += v1[i] * v2[i];
        
        for (int i = 0; i < num_proc-1; i++){
            MPI_Recv(&data, 1, MPI_INT, MPI_ANY_SOURCE, TAG, MPI_COMM_WORLD, &status);
            sum += data;
        }

        printf("\n\nScalar product: %d\n\n", sum);
        
    } else{
        // some stuff
        int count;
        position = 0;
        MPI_Status status;
        MPI_Probe(MASTER, TAG, MPI_COMM_WORLD, &status);
        MPI_Get_count(&status, MPI_INT, &count);
        
        int num_items = count / 2;
        char recv_buff[count*sizeof(int)];
        int sum1[num_items], sum2[num_items];
        
        MPI_Recv(recv_buff, count*sizeof(int), MPI_PACKED, MASTER, TAG, MPI_COMM_WORLD, &status);
        MPI_Unpack(recv_buff, count*sizeof(int), &position, sum1, num_items, MPI_INT, MPI_COMM_WORLD);
        MPI_Unpack(recv_buff, count*sizeof(int), &position, sum2, num_items, MPI_INT, MPI_COMM_WORLD);
    
        // Summ
        
        int sum = 0;
        for (int i = 0; i < num_items; i++)
            sum +=  sum1[i] * sum2[i];
        
        MPI_Send(&sum, 1, MPI_INT, MASTER, TAG, MPI_COMM_WORLD);
        
    }

    MPI_Finalize();
    
    return 0;
}