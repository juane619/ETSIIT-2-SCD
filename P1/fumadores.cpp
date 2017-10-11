// *****************************************************************************
//
// Prácticas de SCD. Práctica 1.
// Plantilla de código para el ejercicio de los fumadores
//
// *****************************************************************************

#include <iostream>
#include <cassert>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>      // incluye "time(....)"
#include <unistd.h>    // incluye "usleep(...)"
#include <stdlib.h>    // incluye "rand(...)" y "srand"

using namespace std ;

const int FUMADORES = 3;
sem_t fuma[FUMADORES], est;

unsigned int ingrediente;

// ---------------------------------------------------------------------
// introduce un retraso aleatorio de duración comprendida entre
// 'smin' y 'smax' (dados en segundos)

void retraso_aleatorio( const float smin, const float smax )
{
  static bool primera = true;
  if ( primera )        // si es la primera vez:
  {  srand(time(NULL)); //   inicializar la semilla del generador
     primera = false ;  //   no repetir la inicialización
  }
  // calcular un número de segundos aleatorio, entre {\ttbf smin} y {\ttbf smax}
  const float tsec = smin+(smax-smin)*((float)random()/(float)RAND_MAX);
  // dormir la hebra (los segundos se pasan a microsegundos, multiplicándos por 1 millón)
  usleep( (useconds_t) (tsec*1000000.0)  );
}

// ----------------------------------------------------------------------------
// función que simula la acción de fumar, como un retardo aleatorio de la hebra.
// recibe como parámetro el numero de fumador
// el tiempo que tarda en fumar está entre dos y ocho décimas de segundo.

void fumar( int num_fumador )
{
   cout << "Fumador número " << num_fumador << ": comienza a fumar." << endl << flush ;
   if(num_fumador == 2)
    retraso_aleatorio( 10, 10 );
    else
    retraso_aleatorio( 0.1, 0.5 );
   cout << "Fumador número " << num_fumador << ": termina de fumar." << endl << flush ;
}
// ----------------------------------------------------------------------------

void* funcion_fumador(void * num_fum_void){
  long long int num_fum = (long long)num_fum_void;

  while(1){
    sem_wait(&fuma[num_fum]);    //En caso de que la hebra num_fum tenga su semaforo 'en verde', pone a cero el semaforo y entra a fumar. En caso de que todavia la hebra num_fum no disponga de ingredientes, sigue sin poder fumar.
    sem_post(&est);        //desbloquea al estanquero y se pone a fumar
    fumar(num_fum);
  }
}

void* funcion_estanquero(void * p){
  int aleatorio;

  while(1){
    aleatorio = rand()%3;        //producira un ingrediente aleatorio entre los 3
    ingrediente = aleatorio;
    sem_wait(&est);              //se bloquea en cuanto produce el ingrediente y lo pone en el mostrador
    sem_post(&fuma[ingrediente]);   //segun el ingrediente producido(1,2,3), se desbloqueara el fumador correspondiente
  }
}

// ----------------------------------------------------------------------------

int main()
{
  srand( time(NULL) ); // inicializa semilla aleatoria para selección aleatoria de fumador
	int i;

	for(i=0; i<FUMADORES; i++){
		sem_init(&fuma[i], 0, 0);  //ningun fumador puede fumar aun
	}

  sem_init( &est, 0, 1 );   // inicialmente puede producir ingrediente

  pthread_t hebra_fumador[FUMADORES];
  pthread_t hebra_estanquero;

  ///creamos las hebras despues de haber iniciado el valor de los semaforos
	for(i=0; i<FUMADORES; i++){
		pthread_create(&hebra_fumador[i], NULL, funcion_fumador, (void*)i );
	}

  pthread_create( &hebra_estanquero, NULL, funcion_estanquero, NULL );

  for(i=0; i < FUMADORES; i++)
    pthread_join( hebra_fumador[i], NULL ) ;

  pthread_join( hebra_estanquero,   NULL ) ;

  for(i=0; i < FUMADORES; i++)
    sem_destroy( &fuma[i] );

  sem_destroy(&est);

  return 0 ;
}
