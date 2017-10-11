// *****************************************************************************
//
// Prácticas de SCD. Práctica 1.
// Plantilla de código para el ejercicio del productor-consumidor con
// buffer intermedio.
//
// *****************************************************************************

#include <iostream>
#include <cassert>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h> // necesario para {\ttbf usleep()}
#include <stdlib.h> // necesario para {\ttbf random()}, {\ttbf srandom()}
#include <time.h>   // necesario para {\ttbf time()}

using namespace std ;

//Semaforos utilizados
sem_t puede_producir, puede_consumir, mutex, printf;

// ---------------------------------------------------------------------
// constantes configurables:
int sem;

const unsigned
  num_items  = 40 ,    // numero total de items que se producen o consumen
  tam_buffer = 10 ;    // tamaño del vector, debe ser menor que el número de items

unsigned buffer[tam_buffer];
unsigned p_libre=0;

// ---------------------------------------------------------------------
// introduce un retraso aleatorio de duración comprendida entre
// 'smin' y 'smax' (dados en segundos)

void retraso_aleatorio( const float smin, const float smax )
{
  static bool primera = true ;
  if ( primera )        // si es la primera vez:
  {  srand(time(NULL)); //   inicializar la semilla del generador
     primera = false ;  //   no repetir la inicialización
  }
  // calcular un número de segundos aleatorio, entre {\ttbf smin} y {\ttbf smax}
  const float tsec = smin+(smax-smin)*((float)random()/(float)RAND_MAX);
  // dormir la hebra (los segundos se pasan a microsegundos, multiplicándos por 1 millón)
  usleep( (useconds_t) (tsec*1000000.0)  );
}


// ---------------------------------------------------------------------
// función que simula la producción de un dato
unsigned producir_dato()
{
  static int contador = 0 ;
  contador = contador + 1 ;
  retraso_aleatorio(0.1, 0.5);
  cout << "Productor : dato producido: " << contador << endl << flush ;
  return contador ;
}


// ---------------------------------------------------------------------
// función que simula la consumición de un dato
void consumir_dato( int dato )
{
   retraso_aleatorio(0.1, 1.5);
   cout << "Consumidor:                              dato consumido: " << dato << endl << flush ;
}


// ---------------------------------------------------------------------
// función que ejecuta la hebra del productor
void * funcion_productor( void * )
{
   for(unsigned i = 0 ; i < num_items ; i++ ){

      int dato = producir_dato();

      sem_wait(&puede_producir);

      sem_wait(&mutex);
      buffer[p_libre]= dato;
      cout << "Productor: dato insertado: " << dato << endl << flush ;
      p_libre++;
      sem_post(&mutex);

      //sem_wait(&printf);

      //sem_post(&printf);

      sem_post(&puede_consumir);

  }
  return NULL ;
}
// ---------------------------------------------------------------------
// función que ejecuta la hebra del consumidor

void * funcion_consumidor( void * )
{
   for( unsigned i = 0 ; i < num_items ; i++ ){
      int dato;

      sem_wait(&puede_consumir);

      sem_wait(&mutex);
      dato=buffer[p_libre-1];
      cout << "Consumidor:                              dato extraído : " << dato << endl << flush;
      p_libre--;
      sem_post(&mutex);

      //sem_wait(&printf);
      consumir_dato(dato);
      //sem_post(&printf);
      sem_post(&puede_producir);
   }

   return NULL ;
}
//----------------------------------------------------------------------

int main()
{
   pthread_t hebra_productora, hebra_consumidora;

   sem_init( &mutex,          0, 1); // EM de seccion critica
   sem_init( &puede_producir, 0, tam_buffer); // inicialmente se podran producir hasta tam_buffer elementos(en caso de que no se consuma ninguno)
   sem_init( &puede_consumir, 0, 0 ); // inicialmente no se puede consumir(hasta que no se lea)
   sem_init( &printf, 0, 1); // EM de escritura en pantalla

   pthread_create( &hebra_productora, NULL, funcion_productor, NULL );
   pthread_create( &hebra_consumidora, NULL, funcion_consumidor, NULL );

   pthread_join( hebra_productora, NULL ) ;
   pthread_join( hebra_consumidora, NULL ) ;

   cout << "\n\nFIN\n\n" << endl << flush;

   sem_destroy(&puede_producir );
   sem_destroy(&puede_consumir );
   sem_destroy(&mutex );
   sem_destroy(&printf );

   return 0 ;
}
