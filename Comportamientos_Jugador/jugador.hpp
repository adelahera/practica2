#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include "comportamientos/comportamiento.hpp"

#include <list>

struct estado {
  int fila;
  int columna;
  int orientacion;
  bool bikini;
  bool zapatillas;
  int coste;
  int fn;
};

class ComportamientoJugador : public Comportamiento {
  public:
    ComportamientoJugador(unsigned int size) : Comportamiento(size) {
      hayPlan = false;
      cambioPlan = false;
      i = j = 0;
    }
    ComportamientoJugador(std::vector< std::vector< unsigned char> > mapaR) : Comportamiento(mapaR) {
      hayPlan = false;
      cambioPlan  = false;
      i = j = 0;
    }
    ComportamientoJugador(const ComportamientoJugador & comport) : Comportamiento(comport){}
    ~ComportamientoJugador(){}

    Action think(Sensores sensores);
    int interact(Action accion, int valor);
    void VisualizaPlan(const estado &st, const list<Action> &plan);
    ComportamientoJugador * clone(){return new ComportamientoJugador(*this);}

  private:
    // Declarar Variables de Estado
    estado actual;

    list<estado> objetivos;
    list<Action> plan;
    bool hayPlan;
    bool cambioPlan;
    bool tengoBikini;
    bool tengoZapatillas;
    int i, j;
    int contador;

    // Métodos privados de la clase
    bool pathFinding(int level, const estado &origen, const list<estado> &destino, list<Action> &plan);
    bool pathFinding_Profundidad(const estado &origen, const estado &destino, list<Action> &plan);

    bool pathFinding_Anchura(const estado &origen, const estado &destino, list<Action> &plan);
    bool pathFinding_Costeuniforme(const estado &origen, const estado &destino, list<Action> &plan);
    bool pathFinding_AEstrella(const estado &origen, const estado &destino, list<Action> &plan);
    bool pathFinding_AEstrella_migue(const estado &origen, const estado &destino, list<Action> &plan);


    void PintaPlan(list<Action> plan);
    estado obtenerDestino();
    estado obtenerDestinoAleatorio();
    void pintarMapa(Sensores sensores, estado & st);
    bool HayObstaculoDelante(estado &st);
    int calcularCoste(estado &st, Action accion);
    int heuristica(const estado &orig, const estado &dst);
    bool puedoAvanzar(Sensores sensores, estado & st);

};

#endif
