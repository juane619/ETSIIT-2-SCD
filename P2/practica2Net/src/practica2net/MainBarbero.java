/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

import monitor.*;
/**
 *
 * @author juane
 */

class Barberia extends AbstractMonitor
{
    private Condition sala_espera= makeCondition();
    private Condition duerme_barbero= makeCondition();
    private Condition silla_barberia= makeCondition();


    // invcado por los clientes para cortarse el pelo
    public void cortarPelo (String cliente)
    {
        enter();
        //System.out.println(cliente+": Quiero pelarme.");
        if(!silla_barberia.isEmpty()){
            System.out.println(cliente+": No puedo pelarme, esperaré..");
            sala_espera.await();
        }
System.out.println(cliente+": Despertare al barbero y que me pele...");
            duerme_barbero.signal();
            silla_barberia.await();

        leave();
    }
    // invocado por el barbero para esperar (si procede) a un nuevo cliente y sentarlo para el corte
    public void siguienteCliente ()
    {
        enter();
        System.out.println("Alguien se quiere pelar!?");
        if(sala_espera.isEmpty()){
            System.out.println("Voi a dormir, no se tiene que pelar nadie.");
            duerme_barbero.await();
        }
        sala_espera.signal();
        System.out.println("Venga, empiezo a pelar!.");
        leave();
    }
    // invocado por el barbero para indicar que ha terminado de cortar el pelo
    public void finCliente ()
    {
        enter();
        System.out.println("Termino de pelar!.");
        silla_barberia.signal();
        leave();
    }
}

class Cliente implements Runnable
{
    public Thread thr ;
    Barberia barberia;

    public Cliente(Barberia p_barb, int p_numCli){
        thr= new Thread(this, "Cliente "+p_numCli);
        barberia= p_barb;
    }

    public void run ()
    {
        while (true) {
            barberia.cortarPelo (thr.getName()); // el cliente espera (si procede) y se corta el pelo
            System.out.println(thr.getName()+": pelado! salgo de la barberia y espero..");
            auxx.dormir_max( 60000 ); // el cliente está fuera de la barberia un tiempo
        }
    }
}

class Barbero implements Runnable
{
    private Barberia barberia;
    public Thread thr;

    public Barbero(Barberia p_barb){
        thr= new Thread(this, "barbero");
        barberia= p_barb;
    }

    public void run ()
    {
        while (true) {
            barberia.siguienteCliente ();
            auxx.dormir_max( 10000 ); // el barbero está cortando el pelo
            barberia.finCliente ();
        }
    }
}

public class MainBarbero {
    public static void main(String[] args) {
        if(args.length != 1){
            System.err.println("Necesita 1 argumento: núm. clientes.");
        }

        int n_clientes= Integer.parseInt(args[0]);
        Cliente[] clientes= new Cliente[n_clientes];
        Barberia barberia= new Barberia();

        Barbero barbero= new Barbero(barberia);

        barbero.thr.start();

        for(int i=0; i<clientes.length;i++)
            clientes[i] = new Cliente(barberia, i);


        for(int i=0; i<clientes.length;i++)
            clientes[i].thr.start();


    }

}
