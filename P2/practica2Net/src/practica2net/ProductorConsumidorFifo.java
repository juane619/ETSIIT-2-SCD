import monitor.*;
// ****************************************************************************

class Buffer extends AbstractMonitor
{
  private int      numSlots = 0    ,
                   p_li     = 0   , 
                    p_oc=    0,
                    cont=0;
  private double[] buffer   = null ;
  private Condition produce= makeCondition();
  private Condition consume= makeCondition();

  public Buffer( int p_numSlots )
  {
    numSlots = p_numSlots ;
    buffer = new double[numSlots] ;
  }
  
  public void depositar( double valor ) throws InterruptedException
  {
    enter();
    //System.out.println("DEPOSITAR: cont = " + cont);
    if( cont == numSlots ){
        //System.out.println(produce.count());
        //System.out.println("ESPERANDO. cont = " + cont);
        produce.await();
    }
    System.out.println("Insertando en: " + p_li);
    buffer[p_li] = valor ;
    p_li=(p_li+1)%numSlots;
    cont++;
    //System.out.println("cont = " + cont);
    consume.signal();
    leave();
  }
public double extraer() throws InterruptedException{
    enter();
    double valor ;
    if( cont == 0 ){
     //System.out.println("EXTRAER: cont = " + cont);
        consume.await() ;
    }
    valor = buffer[p_oc] ;
    p_oc=(p_oc+1)%numSlots;
    cont--;
    produce.signal();
    leave();
    return valor;
  }
}

// ****************************************************************************

class Productor implements Runnable
{
  private Buffer bb    ;
  private int    veces ,
                 numP  ;
  public  Thread thr   ;

  public Productor( Buffer pbb, int pveces, int pnumP )
  {
    bb    = pbb;
    veces = pveces;
    numP  = pnumP ;
    thr   = new Thread(this,"productor "+numP);
  }

  public void run()
  {
    try
    {
      double item = 100*numP ;

      for( int i=0 ; i < veces ; i++ )
      {
        System.out.println(thr.getName()+", produciendo " + item);
        bb.depositar( item++ );
        //System.out.println("DEPOSITADO");
      }
    }
    catch( Exception e )
    {
      System.err.println("Excepcion en main: " + e);
    }
  }
}

// ****************************************************************************

class Consumidor implements Runnable
{
  private Buffer  bb    ;
  private int     veces ,
                  numC  ;
  public  Thread  thr   ;

  public Consumidor( Buffer pbb, int pveces, int pnumC )
  {
    bb    = pbb;
    veces = pveces;
    numC  = pnumC ;
    thr   = new Thread(this,"consumidor "+numC);
  }
  
  public void run()
  {
    try
    {
      for( int i=0 ; i<veces ; i++ )
      {
          try
        {
            
            Thread.sleep(5000);
        }catch(InterruptedException e){} 
          
        double item = bb.extraer();
        System.out.println(thr.getName()+", consumiendo..  " + item);
      }
    }
    catch( Exception e )
    {
      System.err.println("Excepcion en main: " + e);
    }
  }
}

// ****************************************************************************

class MainProductorConsumidorFifo
{
  public static void main( String[] args )
  {
    if ( args.length != 5 )
    {
      System.err.println("Necesita 5 argumentos: (1) núm.prod.  (2) núm.iter.prod.  (3) núm.cons.  (4) núm.iter.cons.  (5) tam.buf.");
      return ;
    }

    // leer parametros, crear vectores y buffer intermedio
    Productor[]  prod      = new Productor[Integer.parseInt(args[0])] ;
    int          iter_prod = Integer.parseInt(args[1]);
    Consumidor[] cons      = new Consumidor[Integer.parseInt(args[2])] ;
    int          iter_cons = Integer.parseInt(args[3]);
    Buffer       buffer    = new Buffer(Integer.parseInt(args[4]));

    if ( prod.length*iter_prod != cons.length*iter_cons )
    {
      System.err.println("no coinciden número de items a producir con a cosumir");
      return ;
    }

    // crear hebras de consumidores
    for(int i = 0; i < cons.length; i++)
      cons[i] = new Consumidor(buffer,iter_cons,i) ;

    // crear hebras de productores
    for(int i = 0; i < prod.length; i++)
      prod[i] = new Productor(buffer,iter_prod,i) ;

    // poner en marcha las hebras
    for(int i = 0; i < prod.length; i++)
      prod[i].thr.start();
    for(int i = 0; i < cons.length; i++)
      cons[i].thr.start();
  }
}
