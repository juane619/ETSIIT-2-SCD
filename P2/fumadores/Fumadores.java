import monitor.*;
// ****************************************************************************

class Estanco extends AbstractMonitor
{
  private int n_ingrediente=-1;
  private Condition[] fumar= new Condition[3];
  private Condition seguir_produciendo= makeCondition();
  
  public Estanco(){
      for(int i=0; i<3;i++)
          fumar[i]= makeCondition();
  }
  
  public void obtenerIngrediente(int miIngrediente){
      enter();
      if(n_ingrediente!=miIngrediente){
          System.out.println("Fumador "+miIngrediente+" ESPERANDO.");
          fumar[miIngrediente].await();
      }
	
	System.out.println("Fumador número "+miIngrediente+ ": comienza a fumar.");
      	 n_ingrediente=-1;
         seguir_produciendo.signal();
         
      
      
      leave();
  }
  
  public void ponerIngrediente(int ingrediente){
      enter();
      n_ingrediente= ingrediente;
	System.out.println("Creando ingrediente..("+ingrediente+")");
      fumar[ingrediente].signal();

      leave();
  }
  
  public void esperarRecogidaIngrediente(){
      enter();
      if(n_ingrediente!=-1)
        seguir_produciendo.await();
      
	System.out.println("A seguir produciendo ingredientes!");
      leave();
  }
}

// ****************************************************************************

class Fumador implements Runnable
{
  private Estanco est;
  int miIngrediente;
  public  Thread thr   ;

  public Fumador(Estanco p_est, int p_ing, int p_numf)
  {
    est    = p_est;
    miIngrediente = p_ing;
    thr   = new Thread(this,"fumador "+p_numf);
  }

  public void fumar()
{
   
   if(miIngrediente == 2 || miIngrediente == 1)
    auxx.dormir_max(15000);
    else
    auxx.dormir_max(2500);
   
   System.out.println("Fumador número "+miIngrediente+ ": termina de fumar.");
}
  
  public void run()
  {
    try
    {
      while(true){
          est.obtenerIngrediente(miIngrediente);
          fumar();
      }
    }
    catch( Exception e )
    {
      System.err.println("Excepcion en main: " + e);
    }
  }
}

// ****************************************************************************

class Estanquero implements Runnable
{
  private Estanco est    ;
  public  Thread  thr   ;

  public Estanquero(Estanco p_est)
  {
    est= p_est;
    thr   = new Thread(this,"estanquero");
  }
  
  public void run()
  {
    try
    {
      int ingrediente;
      while(true){
          ingrediente= (int) (Math.random()*3.0);
          est.ponerIngrediente(ingrediente);
          est.esperarRecogidaIngrediente();
      }
    }
    catch( Exception e )
    {
      System.err.println("Excepcion en main: " + e);
    }
  }
}

// ****************************************************************************

class MainFumadores
{
  public static void main( String[] args )
  {
    try {
      Fumador[]  fumadores      = new Fumador[3];
    Estanco estanco= new Estanco();

    // crear hebras de fumadores
    for(int i = 0; i < 3; i++)
      fumadores[i] = new Fumador(estanco, i, i) ;

    // crear hebra de estanquero
    Estanquero estanquero = new Estanquero(estanco);

    // poner en marcha las hebras
    estanquero.thr.start();

    for(int i = 0; i < 3; i++)
        fumadores[i].thr.start();
    
    for (int i=0;i<3;i++)
        fumadores[i].thr.join();

    estanquero.thr.join();
    } catch (InterruptedException e) {
			System.out.println("Exceptions happen");
		}
  }
}
