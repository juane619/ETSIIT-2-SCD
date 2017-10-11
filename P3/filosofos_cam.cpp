#include <iostream>
#include <time.h>      // incluye "time"
#include <unistd.h>    // incluye "usleep"
#include <stdlib.h>    // incluye "rand" y "srand"
#include <mpi.h>

using namespace std;

#define TPEDIR 0
#define TSOLTAR 1
#define TSENTARSE 2
#define TLEVANTARSE 3
#define IDcamarero 10

void Filosofo(int id, int nprocesos);
void Tenedor (int id, int nprocesos);
void Camarero();

// ---------------------------------------------------------------------

int main( int argc, char** argv )
{
   int rank, size;
   
   srand(time(0));
   MPI_Init( &argc, &argv );
   MPI_Comm_rank( MPI_COMM_WORLD, &rank );
   MPI_Comm_size( MPI_COMM_WORLD, &size );
   
   if( size!=11)    //Para mostrar el mensaje solo una vez
   {
      if( rank == 0) 
         cout<<"El numero de procesos debe ser 11" << endl << flush ;
      MPI_Finalize( ); 
      return 0; 
   }
   
   if (rank == 10)
        Camarero(); // El proceso 10 es el camarero
    else if (rank%2 == 0)
        Filosofo(rank,size); // los pares son filosofos
    else
        Tenedor(rank,size); // los impares son tenedores


    // Finalización de open-mpi
   MPI_Finalize( );
   return 0;
}  
// ---------------------------------------------------------------------

void Filosofo( int id, int nprocesos )
{
    //AHora tenemos un proceso mas
    int izq = (id+1) % 10;
    int der = ((id+10)-1) % 10; 
    
   while(1)
   {
      // Piensa (espera bloqueada aleatorio del proceso)
      cout << "Filosofo " << id << " PENSANDO.." << endl << flush;
      
      // espera bloqueado durante un intervalo de tiempo aleatorio 
      // (entre una décima de segundo y un segundo)
      usleep( 1000U * (100U+(rand()%900U)) );
      
      //Pedir permiso para sentarse
      cout << "Filosofo "<< id << " ESPERANDO sentarse ..." << endl << flush;
      MPI_Ssend(NULL, 0, MPI_INT, IDcamarero, TSENTARSE, MPI_COMM_WORLD);
       // El filosofo espera a que le digan que puede sentarse
      cout << "Filosofo "<< id << " SE SIENTA." << endl << flush;
       
        // Solicita tenedor izquierdo
      cout << "Filosofo "<< id << " pide tenedor izq ..." << izq << endl << flush;
        MPI_Ssend(NULL, 0, MPI_INT, izq, TPEDIR, MPI_COMM_WORLD);
        cout << "Filosofo "<< id << " coge tenedor izq ..." << izq << endl << flush;

        // Solicita tenedor derecho
        cout << "Filosofo "<< id << " pide tenedor der ..." << der << endl << flush;
        MPI_Ssend(NULL, 0, MPI_INT, der, TPEDIR, MPI_COMM_WORLD);
        cout <<"Filosofo "<<id<< " coge tenedor der ..." << der << endl << flush;
       
       
      cout<<"Filosofo "<<id<< " COMIENDO.."<<endl<<flush;
      sleep((rand() % 3)+1);  //comiendo
      
      // Suelta el tenedor izquierdo
      MPI_Ssend(NULL, 0, MPI_INT, izq, TSOLTAR, MPI_COMM_WORLD);
      cout <<"Filosofo "<<id<< " suelta tenedor izq ..." << izq << endl << flush;
      
      // Suelta el tenedor derecho
      MPI_Ssend(NULL, 0, MPI_INT, der, TSOLTAR, MPI_COMM_WORLD);
      cout <<"Filosofo "<<id<< " suelta tenedor der ..." << der << endl << flush;
      
      //Pedimos levantarnos al camarero
      cout << "Filosofo "<< id << " pide levantarse ..." << endl << flush;
      MPI_Ssend(NULL, 0, MPI_INT, IDcamarero, TLEVANTARSE, MPI_COMM_WORLD);
      cout << "Filosofo "<< id << " SE LEVANTA." << endl << flush;
      
 }
}
// ---------------------------------------------------------------------

void Tenedor(int id, int nprocesos)
{
  int buf; 
  MPI_Status status; 
  int Filo;
  
  while(true){
    // Espera un peticion desde cualquier filosofo vecino ...
    MPI_Probe(MPI_ANY_SOURCE, TPEDIR, MPI_COMM_WORLD, &status);
    
    Filo= status.MPI_SOURCE;
    // Recibe la peticion del filosofo ...
    MPI_Recv(&buf, 1, MPI_INT, Filo, TPEDIR, MPI_COMM_WORLD, &status);
    
    //cout << "Ten. " << id << " recibe petic. de " << Filo << endl << flush;
    
    // Espera a que el filosofo suelte el tenedor...
    //MPI_Probe(Filo, TSOLTAR, MPI_COMM_WORLD, &status);
    MPI_Recv(&buf, 1, MPI_INT, Filo, TSOLTAR, MPI_COMM_WORLD, &status);
    
    //cout << "Ten. " << id << " recibe liberac. de " << Filo << endl << flush; 
  }
}
  
  void Camarero(){
      int fil_sen= 0;
      int filo, accion;
      MPI_Status status;
      
      while (true){
        if (fil_sen < 4) // El maximo de filosofos comiendo es 4
            MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status); // Puede sentarse o levantarse
        else
            MPI_Probe(MPI_ANY_SOURCE, TLEVANTARSE, MPI_COMM_WORLD, &status);  // Solo puede levantarse

        filo= status.MPI_SOURCE;
        accion= status.MPI_TAG;

        if (accion == TSENTARSE) // se le deja sentarse
        {
            MPI_Recv( NULL, 0, MPI_INT, filo, TSENTARSE, MPI_COMM_WORLD,&status);
            fil_sen++;
            
        }
        if (accion == TLEVANTARSE) // Se le deja levantarse
        {
            MPI_Recv( NULL, 0, MPI_INT, filo, TLEVANTARSE, MPI_COMM_WORLD,&status);
            fil_sen--;
        }
    }
  }

// ---------------------------------------------------------------------