import java.util.Random;
// ****************************************************************************

class auxx
{
  static Random genAlea = new Random() ;

  static void dormir_max( int milisecsMax )
  { 
    try
    { 
      Thread.sleep(milisecsMax) ;
    } 
    catch( InterruptedException e )
    {
    	System.err.println("sleep interumpido en 'aux.dormir_max()'");
    }
  }
}
