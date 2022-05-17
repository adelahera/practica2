#include "../Comportamientos_Jugador/jugador.hpp"
#include "motorlib/util.h"

#include <iostream>
#include <cmath>
#include <set>
#include <stack>
#include <queue>

// Este es el método principal que se piden en la practica.
// Tiene como entrada la información de los sensores y devuelve la acción a realizar.
// Para ver los distintos sensores mirar fichero "comportamiento.hpp"
Action ComportamientoJugador::think(Sensores sensores)
{

	Action accion = actIDLE;

	actual.fila = sensores.posF;
	actual.columna = sensores.posC;
	actual.orientacion = sensores.sentido;

	cout << "Fila: " << actual.fila << endl;
	cout << "Col : " << actual.columna << endl;
	cout << "Ori : " << actual.orientacion << endl;

	pintarMapa(sensores, actual);

	if(sensores.terreno[0] == 'K') {

		tengoBikini = true;
		tengoZapatillas = false;
	}

	if(sensores.terreno[0] == 'D'){

		tengoBikini = false;
		tengoZapatillas = true;
	}

	if(sensores.nivel < 3) {

		// Capturo los destinos
		cout << "sensores.num_destinos : " << sensores.num_destinos << endl;
		objetivos.clear();
		for (int i = 0; i < sensores.num_destinos; i++){
			estado aux;
			aux.fila = sensores.destino[2 * i];
			aux.columna = sensores.destino[2 * i + 1];
			objetivos.push_back(aux);
		}

		if(!hayPlan){
			hayPlan = pathFinding(sensores.nivel, actual, objetivos, plan);
		}

		if(hayPlan and plan.size() > 0){

			accion = plan.front();
			plan.erase(plan.begin());
		}
		else {
			cout << "No se pudo encontrar un plan " << endl;
			
		}
	}
	else {

		estado destino;

		if(!hayPlan) {

			destino = obtenerDestino();
			cout << "el destino nuevo es " << destino.fila << destino.columna << endl;
			hayPlan = pathFinding_AEstrella(actual, destino, plan);
			
		}

		if(hayPlan and plan.size() > 0){

			// if(puedoAvanzar(sensores, actual) || cambioPlan) {

				accion = plan.front();
				plan.erase(plan.begin());	
				cambioPlan = false;			
			// }
			
			if(!puedoAvanzar(sensores, actual) && accion == actFORWARD) {

				accion = actIDLE;
				hayPlan = false;
				plan.clear();
				cambioPlan = true;
			}

		}
		else {
			cout << "No se pudo encontrar un plan " << endl;
			hayPlan = false;
		}

	}


	return accion;
}

// Llama al algoritmo de busqueda que se usara en cada comportamiento del agente
// Level representa el comportamiento en el que fue iniciado el agente.
bool ComportamientoJugador::pathFinding(int level, const estado &origen, const list<estado> &destino, list<Action> &plan)
{
	switch (level){

	case 0:
		cout << "Demo\n";
		estado un_objetivo;
		un_objetivo = objetivos.front();
		cout << "fila: " << un_objetivo.fila << " col:" << un_objetivo.columna << endl;
		return pathFinding_Profundidad(origen, un_objetivo, plan);
		break;

	case 1:
		cout << "Optimo numero de acciones\n";
		estado obj;
		obj = objetivos.front();
		cout << "fila: " << obj.fila << " col:" << obj.columna << endl;
		return pathFinding_Anchura(origen, obj, plan);
		break;

	case 2:
		cout << "Optimo en coste\n";
		estado otro_obj;
		otro_obj = objetivos.front();
		cout << "fila: " << otro_obj.fila << " col:" << otro_obj.columna << endl;
		return pathFinding_Costeuniforme(origen, otro_obj, plan);
	
		break;

	case 3:
		cout << "Reto 1: Descubrir el mapa\n";
		// Incluir aqui la llamada al algoritmo de busqueda para el Reto 1
		cout << "No implementado aun\n";
		break;

	case 4:
		cout << "Reto 2: Maximizar objetivos\n";
		// Incluir aqui la llamada al algoritmo de busqueda para el Reto 2
		cout << "No implementado aun\n";
		break;
	}
	return false;
}

//---------------------- Implementación de la busqueda en profundidad ---------------------------

// Dado el codigo en caracter de una casilla del mapa dice si se puede
// pasar por ella sin riegos de morir o chocar.
bool EsObstaculo(unsigned char casilla)
{
	if (casilla == 'P' or casilla == 'M')
		return true;
	else
		return false;
}

// Comprueba si la casilla que hay delante es un obstaculo. Si es un
// obstaculo devuelve true. Si no es un obstaculo, devuelve false y
// modifica st con la posición de la casilla del avance.
bool ComportamientoJugador::HayObstaculoDelante(estado &st)
{
	int fil = st.fila, col = st.columna;

	// calculo cual es la casilla de delante del agente
	switch (st.orientacion)
	{
	case 0:
		fil--;
		break;
	case 1:
		fil--;
		col++;
		break;
	case 2:
		col++;
		break;
	case 3:
		fil++;
		col++;
		break;
	case 4:
		fil++;
		break;
	case 5:
		fil++;
		col--;
		break;
	case 6:
		col--;
		break;
	case 7:
		fil--;
		col--;
		break;
	}

	// Compruebo que no me salgo fuera del rango del mapa
	if (fil < 0 or fil >= mapaResultado.size())
		return true;
	if (col < 0 or col >= mapaResultado[0].size())
		return true;

	// Miro si en esa casilla hay un obstaculo infranqueable
	if (!EsObstaculo(mapaResultado[fil][col]))
	{
		// No hay obstaculo, actualizo el parametro st poniendo la casilla de delante.
		st.fila = fil;
		st.columna = col;
		return false;
	}
	else
	{
		return true;
	}
}

int ComportamientoJugador::calcularCoste(estado &st, Action accion) {

	int coste = 0;
	switch (accion){

		case actFORWARD:

		if(mapaResultado[st.fila][st.columna] == 'A'){

			if(st.bikini)
				coste = 10;
			else
				coste = 200;
		}
		else if(mapaResultado[st.fila][st.columna] == 'B') {

			if(st.zapatillas)
				coste = 15;
			else
				coste = 100;
		}
		else if(mapaResultado[st.fila][st.columna] == 'T'){

			coste = 2;
		}
		else{

			coste = 1;
		}

		break;
		
		case actTURN_R:

		if(mapaResultado[st.fila][st.columna] == 'A'){

			if(st.bikini)
				coste = 5;
			else
				coste = 500;
		}
		else if(mapaResultado[st.fila][st.columna] == 'B') {

			if(st.zapatillas)
				coste = 1;
			else
				coste = 3;
		}
		else if(mapaResultado[st.fila][st.columna] == 'T'){

			coste = 2;
		}
		else{

			coste = 1;
		}

		break;

		case actTURN_L:

		if(mapaResultado[st.fila][st.columna] == 'A'){

			if(st.bikini)
				coste = 5;
			else
				coste = 500;
		}
		else if(mapaResultado[st.fila][st.columna] == 'B') {

			if(st.zapatillas)
				coste = 1;
			else
				coste = 3;
		}
		else if(mapaResultado[st.fila][st.columna] == 'T'){

			coste = 2;
		}
		else{

			coste = 1;
		}

		break;

		case actSEMITURN_R:

		if(mapaResultado[st.fila][st.columna] == 'A'){

			if(st.bikini)
				coste = 2;
			else
				coste = 300;
		}
		else if(mapaResultado[st.fila][st.columna] == 'B') {

			if(st.zapatillas)
				coste = 1;
			else
				coste = 2;
		}
		else if(mapaResultado[st.fila][st.columna] == 'T'){

			coste = 1;
		}
		else{

			coste = 1;
		}

		break;

		case actSEMITURN_L:

		if(mapaResultado[st.fila][st.columna] == 'A'){

			if(st.bikini)
				coste = 2;
			else
				coste = 300;
		}
		else if(mapaResultado[st.fila][st.columna] == 'B') {

			if(st.zapatillas)
				coste = 1;
			else
				coste = 2;
		}
		else if(mapaResultado[st.fila][st.columna] == 'T'){

			coste = 1;
		}
		else{

			coste = 1;
		}

		break;

	}

	return coste;
}

int ComportamientoJugador::heuristica(const estado &orig, const estado &dst) {
	return max(abs(orig.fila - dst.fila), abs(orig.columna - dst.columna));
}

struct nodo
{
	estado st;
	list<Action> secuencia;
};

struct ComparaEstados
{
	bool operator()(const estado &a, const estado &n) const
	{
		if ((a.fila > n.fila) or (a.fila == n.fila and a.columna > n.columna) or
			(a.fila == n.fila and a.columna == n.columna and a.orientacion > n.orientacion) or 
			(a.fila == n.fila and a.columna == n.columna and a.orientacion == n.orientacion and a.bikini > n.bikini) or
			(a.fila == n.fila and a.columna == n.columna and a.orientacion == n.orientacion and a.bikini > n.bikini and a.zapatillas > n.zapatillas))
			return true;
		else
			return false;
	}
};

struct ComparaCoste{

	bool operator()(const nodo &a, const nodo &n) const
	{
		if (a.st.coste > n.st.coste) 
			return true;
		else
			return false;
	}	
};

struct ComparaFn{

	bool operator()(const nodo &a, const nodo &n) const
	{
		if (a.st.fn > n.st.fn) 
			return true;
		else
			return false;
	}	
};

// Implementación de la busqueda en profundidad.
// Entran los puntos origen y destino y devuelve la
// secuencia de acciones en plan, una lista de acciones.
bool ComportamientoJugador::pathFinding_Profundidad(const estado &origen, const estado &destino, list<Action> &plan){
	// Borro la lista
	cout << "Calculando plan\n";
	plan.clear();
	set<estado, ComparaEstados> Cerrados; // Lista de Cerrados
	stack<nodo> Abiertos;				  // Lista de Abiertos

	nodo current;
	current.st = origen;
	current.secuencia.empty();

	Abiertos.push(current);

	while (!Abiertos.empty() and (current.st.fila != destino.fila or current.st.columna != destino.columna))
	{

		Abiertos.pop();
		Cerrados.insert(current.st);

		// Generar descendiente de girar a la derecha 90 grados
		nodo hijoTurnR = current;
		hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion + 2) % 8;
		if (Cerrados.find(hijoTurnR.st) == Cerrados.end())
		{
			hijoTurnR.secuencia.push_back(actTURN_R);
			Abiertos.push(hijoTurnR);
		}

		// Generar descendiente de girar a la derecha 45 grados
		nodo hijoSEMITurnR = current;
		hijoSEMITurnR.st.orientacion = (hijoSEMITurnR.st.orientacion + 1) % 8;
		if (Cerrados.find(hijoSEMITurnR.st) == Cerrados.end())
		{
			hijoSEMITurnR.secuencia.push_back(actSEMITURN_R);
			Abiertos.push(hijoSEMITurnR);
		}

		// Generar descendiente de girar a la izquierda 90 grados
		nodo hijoTurnL = current;
		hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion + 6) % 8;
		if (Cerrados.find(hijoTurnL.st) == Cerrados.end())
		{
			hijoTurnL.secuencia.push_back(actTURN_L);
			Abiertos.push(hijoTurnL);
		}

		// Generar descendiente de girar a la izquierda 45 grados
		nodo hijoSEMITurnL = current;
		hijoSEMITurnL.st.orientacion = (hijoSEMITurnL.st.orientacion + 7) % 8;
		if (Cerrados.find(hijoSEMITurnL.st) == Cerrados.end())
		{
			hijoSEMITurnL.secuencia.push_back(actSEMITURN_L);
			Abiertos.push(hijoSEMITurnL);
		}

		// Generar descendiente de avanzar
		nodo hijoForward = current;
		if (!HayObstaculoDelante(hijoForward.st))
		{
			if (Cerrados.find(hijoForward.st) == Cerrados.end())
			{
				hijoForward.secuencia.push_back(actFORWARD);
				Abiertos.push(hijoForward);
			}
		}

		// Tomo el siguiente valor de la Abiertos
		if (!Abiertos.empty())
		{
			current = Abiertos.top();
		}
	}

	cout << "Terminada la busqueda\n";

	if (current.st.fila == destino.fila and current.st.columna == destino.columna)
	{
		cout << "Cargando el plan\n";
		plan = current.secuencia;
		cout << "Longitud del plan: " << plan.size() << endl;
		PintaPlan(plan);
		// ver el plan en el mapa
		VisualizaPlan(origen, plan);
		return true;
	}
	else
	{
		cout << "No encontrado plan\n";
	}

	return false;
}

bool ComportamientoJugador::pathFinding_Anchura(const estado &origen, const estado &destino, list<Action> &plan){
	// Borro la lista
	cout << "Calculando plan\n";
	plan.clear();
	set<estado, ComparaEstados> Cerrados; // Lista de Cerrados
	queue<nodo> Abiertos;				  // Lista de Abiertos

	nodo current;
	current.st = origen;
	current.secuencia.empty();

	Abiertos.push(current);

	while (!Abiertos.empty() and (current.st.fila != destino.fila or current.st.columna != destino.columna))
	{

		Abiertos.pop();
		Cerrados.insert(current.st);

		// Generar descendiente de girar a la derecha 90 grados
		nodo hijoTurnR = current;
		hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion + 2) % 8;
		if (Cerrados.find(hijoTurnR.st) == Cerrados.end())
		{
			hijoTurnR.secuencia.push_back(actTURN_R);
			Abiertos.push(hijoTurnR);
		}

		// Generar descendiente de girar a la derecha 45 grados
		nodo hijoSEMITurnR = current;
		hijoSEMITurnR.st.orientacion = (hijoSEMITurnR.st.orientacion + 1) % 8;
		if (Cerrados.find(hijoSEMITurnR.st) == Cerrados.end())
		{
			hijoSEMITurnR.secuencia.push_back(actSEMITURN_R);
			Abiertos.push(hijoSEMITurnR);
		}

		// Generar descendiente de girar a la izquierda 90 grados
		nodo hijoTurnL = current;
		hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion + 6) % 8;
		if (Cerrados.find(hijoTurnL.st) == Cerrados.end())
		{
			hijoTurnL.secuencia.push_back(actTURN_L);
			Abiertos.push(hijoTurnL);
		}

		// Generar descendiente de girar a la izquierda 45 grados
		nodo hijoSEMITurnL = current;
		hijoSEMITurnL.st.orientacion = (hijoSEMITurnL.st.orientacion + 7) % 8;
		if (Cerrados.find(hijoSEMITurnL.st) == Cerrados.end())
		{
			hijoSEMITurnL.secuencia.push_back(actSEMITURN_L);
			Abiertos.push(hijoSEMITurnL);
		}

		// Generar descendiente de avanzar
		nodo hijoForward = current;
		if (!HayObstaculoDelante(hijoForward.st))
		{
			if (Cerrados.find(hijoForward.st) == Cerrados.end())
			{
				hijoForward.secuencia.push_back(actFORWARD);
				Abiertos.push(hijoForward);

				if(hijoForward.st.fila == destino.fila && hijoForward.st.columna == destino.columna){

					current = hijoForward;
					break;
				}
			}
		}

		// Tomo el siguiente valor de la Abiertos
		if (!Abiertos.empty())
		{
			current = Abiertos.front();
		}
	}

	cout << "Terminada la busqueda\n";

	if (current.st.fila == destino.fila and current.st.columna == destino.columna)
	{
		cout << "Cargando el plan\n";
		plan = current.secuencia;
		cout << "Longitud del plan: " << plan.size() << endl;
		PintaPlan(plan);
		// ver el plan en el mapa
		VisualizaPlan(origen, plan);
		return true;
	}
	else
	{
		cout << "No encontrado plan\n";
	}

	return false;
}

bool ComportamientoJugador::pathFinding_Costeuniforme(const estado &origen, const estado &destino, list<Action> &plan){

	// Borro la lista
	cout << "Calculando plan\n";
	plan.clear();
	set<estado, ComparaEstados> Cerrados; // Lista de Cerrados
	priority_queue<nodo, vector<nodo>, ComparaCoste> Abiertos;				  // Lista de Abiertos

	nodo current;
	current.st = origen;
	current.secuencia.empty();
	current.st.coste = 0;
	current.st.bikini = false;
	current.st.zapatillas = false;

	Abiertos.push(current);

	while (!Abiertos.empty() and (current.st.fila != destino.fila or current.st.columna != destino.columna))
	{

		Abiertos.pop();

		if(Cerrados.find(current.st) == Cerrados.end())
			Cerrados.insert(current.st);

		if(mapaResultado[current.st.fila][current.st.columna] == 'K'){
			current.st.bikini = true;
			current.st.zapatillas = false;
		}

		if(mapaResultado[current.st.fila][current.st.columna] == 'D'){
			current.st.bikini = false;
			current.st.zapatillas = true;
		}

		// Generar descendiente de girar a la derecha 90 grados
		nodo hijoTurnR = current;
		hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion + 2) % 8;
		hijoTurnR.st.coste += calcularCoste(hijoTurnR.st, actTURN_R);
		if (Cerrados.find(hijoTurnR.st) == Cerrados.end())
		{
			hijoTurnR.secuencia.push_back(actTURN_R);
			Abiertos.push(hijoTurnR);
		}

		// Generar descendiente de girar a la derecha 45 grados
		nodo hijoSEMITurnR = current;
		hijoSEMITurnR.st.orientacion = (hijoSEMITurnR.st.orientacion + 1) % 8;
		hijoSEMITurnR.st.coste += calcularCoste(hijoSEMITurnR.st, actSEMITURN_R);
		if (Cerrados.find(hijoSEMITurnR.st) == Cerrados.end())
		{
			hijoSEMITurnR.secuencia.push_back(actSEMITURN_R);
			Abiertos.push(hijoSEMITurnR);
		}

		// Generar descendiente de girar a la izquierda 90 grados
		nodo hijoTurnL = current;
		hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion + 6) % 8;
		hijoTurnL.st.coste += calcularCoste(hijoTurnL.st, actTURN_L);
		if (Cerrados.find(hijoTurnL.st) == Cerrados.end())
		{
			hijoTurnL.secuencia.push_back(actTURN_L);
			Abiertos.push(hijoTurnL);
		}

		// Generar descendiente de girar a la izquierda 45 grados
		nodo hijoSEMITurnL = current;
		hijoSEMITurnL.st.orientacion = (hijoSEMITurnL.st.orientacion + 7) % 8;
		hijoSEMITurnL.st.coste += calcularCoste(hijoSEMITurnL.st, actSEMITURN_L);
		if (Cerrados.find(hijoSEMITurnL.st) == Cerrados.end())
		{
			hijoSEMITurnL.secuencia.push_back(actSEMITURN_L);
			Abiertos.push(hijoSEMITurnL);
		}

		// Generar descendiente de avanzar
		nodo hijoForward = current;
		if (!HayObstaculoDelante(hijoForward.st))
		{
			hijoForward.st.coste += calcularCoste(hijoForward.st, actFORWARD);
			if (Cerrados.find(hijoForward.st) == Cerrados.end())
			{	
				hijoForward.secuencia.push_back(actFORWARD);
				Abiertos.push(hijoForward);

				if(hijoForward.st.fila == destino.fila && hijoForward.st.columna == destino.columna){

					current = hijoForward;
					break;
				}
			}
		}

		// Tomo el siguiente valor de la Abiertos
		if (!Abiertos.empty())
		{
			current = Abiertos.top();
			while(Cerrados.find(current.st) != Cerrados.end()){
				Abiertos.pop();
				current = Abiertos.top();
			}
		}
	}

	cout << "Terminada la busqueda\n";

	if (current.st.fila == destino.fila and current.st.columna == destino.columna)
	{
		cout << "Cargando el plan\n";
		plan = current.secuencia;
		cout << "Longitud del plan: " << plan.size() << endl;
		PintaPlan(plan);
		// ver el plan en el mapa
		VisualizaPlan(origen, plan);
		return true;
	}
	else
	{
		cout << "No encontrado plan\n";
	}

	return false;


}

bool ComportamientoJugador::pathFinding_AEstrella(const estado &origen, const estado &destino, list<Action> &plan) {

	// Borro la lista
	cout << "Calculando plan\n";
	plan.clear();
	
	set<estado, ComparaEstados> Cerrados; // Lista de Cerrados
	priority_queue<nodo, vector<nodo>, ComparaFn> Abiertos;		  // Lista de Abiertos

	nodo current;
	current.st = origen;
	current.secuencia.empty();
	current.st.bikini = tengoBikini;
	current.st.zapatillas = tengoZapatillas;

	current.st.coste = calcularCoste(current.st, actIDLE);
	current.st.fn = current.st.coste + heuristica(origen,destino);

	Abiertos.push(current);

	while (!Abiertos.empty() and (current.st.fila != destino.fila or current.st.columna != destino.columna))
	{
		if(!Abiertos.empty()){
			Abiertos.pop();
		}

		if (Cerrados.find(current.st)== Cerrados.end()){
			Cerrados.insert(current.st);
		}
		if(mapaResultado[current.st.fila][current.st.columna] == 'K'){
			
			current.st.bikini = true;
			current.st.zapatillas = false;

		}

		if(mapaResultado[current.st.fila][current.st.columna] == 'D'){
			
			current.st.zapatillas = true;
			current.st.bikini = false;
		}

		// Generar descendiente de girar a la derecha 90 grados
		nodo hijoTurnR = current;
		hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion + 2) % 8;

		hijoTurnR.st.coste += calcularCoste(hijoTurnR.st,actTURN_R);
		hijoTurnR.st.fn = hijoTurnR.st.coste + heuristica(origen,destino);

		if (Cerrados.find(hijoTurnR.st) == Cerrados.end())
		{
			hijoTurnR.secuencia.push_back(actTURN_R);
			Abiertos.push(hijoTurnR);
		}

		// Generar descendiente de girar a la derecha 45 grados
		nodo hijoSEMITurnR = current;
		hijoSEMITurnR.st.orientacion = (hijoSEMITurnR.st.orientacion + 1) % 8;

		hijoSEMITurnR.st.coste += calcularCoste(hijoSEMITurnR.st, actSEMITURN_R);
		hijoSEMITurnR.st.fn = hijoSEMITurnR.st.coste + heuristica(origen,destino);

		if (Cerrados.find(hijoSEMITurnR.st) == Cerrados.end())
		{
			hijoSEMITurnR.secuencia.push_back(actSEMITURN_R);
			Abiertos.push(hijoSEMITurnR);
		}
		// Generar descendiente de girar a la izquierda 90 grados
		nodo hijoTurnL = current;
		hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion + 6) % 8;

		hijoTurnL.st.coste += calcularCoste(hijoTurnL.st ,actTURN_L);
		hijoTurnL.st.fn = hijoTurnL.st.coste + heuristica(origen,destino);

		if (Cerrados.find(hijoTurnL.st) == Cerrados.end())
		{
			hijoTurnL.secuencia.push_back(actTURN_L);
			Abiertos.push(hijoTurnL);
		}

		// Generar descendiente de girar a la izquierda 45 grados
		nodo hijoSEMITurnL = current;
		hijoSEMITurnL.st.orientacion = (hijoSEMITurnL.st.orientacion + 7) % 8;

		hijoSEMITurnL.st.coste += calcularCoste(hijoSEMITurnL.st ,actSEMITURN_L);
		hijoSEMITurnL.st.fn = hijoSEMITurnL.st.coste + heuristica(origen,destino);

		if (Cerrados.find(hijoSEMITurnL.st) == Cerrados.end())
		{
			hijoSEMITurnL.secuencia.push_back(actSEMITURN_L);
			Abiertos.push(hijoSEMITurnL);
		}

		// Generar descendiente de avanzar
		nodo hijoForward = current;

		hijoForward.st.coste += calcularCoste(hijoForward.st, actFORWARD);
		hijoForward.st.fn = hijoForward.st.coste + heuristica(origen,destino);

		if (!HayObstaculoDelante(hijoForward.st))
		{
			if (Cerrados.find(hijoForward.st) == Cerrados.end())
			{
				hijoForward.secuencia.push_back(actFORWARD);
				Abiertos.push(hijoForward);
			}
		}

		// Tomo el siguiente valor de la Abiertos
		if (!Abiertos.empty())
		{
			current = Abiertos.top();
			while(Cerrados.find(current.st) != Cerrados.end() && !Abiertos.empty()){
				Abiertos.pop();
				if(!Abiertos.empty())
					current = Abiertos.top();
			}
		}
	}

	cout << "Terminada la busqueda\n";

	if (current.st.fila == destino.fila and current.st.columna == destino.columna)
	{
		cout << "Cargando el plan\n";
		plan = current.secuencia;
		cout << "Longitud del plan: " << plan.size() << endl;
		PintaPlan(plan);
		// ver el plan en el mapa
		VisualizaPlan(origen, plan);
		return true;
	}
	else
	{
		cout << "No encontrado plan\n";
	}

	return false;


}


// Sacar por la consola la secuencia del plan obtenido
void ComportamientoJugador::PintaPlan(list<Action> plan){
	auto it = plan.begin();
	while (it != plan.end())
	{
		if (*it == actFORWARD)
		{
			cout << "A ";
		}
		else if (*it == actTURN_R)
		{
			cout << "D ";
		}
		else if (*it == actSEMITURN_R)
		{
			cout << "d ";
		}
		else if (*it == actTURN_L)
		{
			cout << "I ";
		}
		else if (*it == actSEMITURN_L)
		{
			cout << "i ";
		}
		else
		{
			cout << "- ";
		}
		it++;
	}
	cout << endl;
}

void ComportamientoJugador::pintarMapa(Sensores sensores, estado &st){

	mapaResultado[st.fila][st.columna] = sensores.terreno[0];

	switch(st.orientacion){

		case 0: 
			mapaResultado[st.fila-1][st.columna-1] = sensores.terreno[1];
			mapaResultado[st.fila-1][st.columna] = sensores.terreno[2];
			mapaResultado[st.fila-1][st.columna+1] = sensores.terreno[3];
			mapaResultado[st.fila-2][st.columna-2] = sensores.terreno[4];
			mapaResultado[st.fila-2][st.columna-1] = sensores.terreno[5];
			mapaResultado[st.fila-2][st.columna] = sensores.terreno[6];
			mapaResultado[st.fila-2][st.columna+1] = sensores.terreno[7];
			mapaResultado[st.fila-2][st.columna+2] = sensores.terreno[8];
			mapaResultado[st.fila-3][st.columna-3] = sensores.terreno[9];
			mapaResultado[st.fila-3][st.columna-2] = sensores.terreno[10];
			mapaResultado[st.fila-3][st.columna-1] = sensores.terreno[11];
			mapaResultado[st.fila-3][st.columna] = sensores.terreno[12];
			mapaResultado[st.fila-3][st.columna+1] = sensores.terreno[13];
			mapaResultado[st.fila-3][st.columna+2] = sensores.terreno[14];
			mapaResultado[st.fila-3][st.columna+3] = sensores.terreno[15];
		break;

		case 1: 
			mapaResultado[st.fila-1][st.columna] = sensores.terreno[1];
			mapaResultado[st.fila-1][st.columna+1] = sensores.terreno[2];
			mapaResultado[st.fila][st.columna+1] = sensores.terreno[3];
			mapaResultado[st.fila-2][st.columna] = sensores.terreno[4];
			mapaResultado[st.fila-2][st.columna+1] = sensores.terreno[5];
			mapaResultado[st.fila-2][st.columna+2] = sensores.terreno[6];
			mapaResultado[st.fila-1][st.columna+2] = sensores.terreno[7];
			mapaResultado[st.fila][st.columna+2] = sensores.terreno[8];
			mapaResultado[st.fila-3][st.columna] = sensores.terreno[9];
			mapaResultado[st.fila-3][st.columna+1] = sensores.terreno[10];
			mapaResultado[st.fila-3][st.columna+2] = sensores.terreno[11];
			mapaResultado[st.fila-3][st.columna+3] = sensores.terreno[12];
			mapaResultado[st.fila-2][st.columna+3] = sensores.terreno[13];
			mapaResultado[st.fila-1][st.columna+3] = sensores.terreno[14];
			mapaResultado[st.fila][st.columna+3] = sensores.terreno[15];
		break;

		case 2: 
			mapaResultado[st.fila-1][st.columna+1] = sensores.terreno[1];
			mapaResultado[st.fila][st.columna+1] = sensores.terreno[2];
			mapaResultado[st.fila+1][st.columna+1] = sensores.terreno[3];
			mapaResultado[st.fila-2][st.columna+2] = sensores.terreno[4];
			mapaResultado[st.fila-1][st.columna+2] = sensores.terreno[5];
			mapaResultado[st.fila][st.columna+2] = sensores.terreno[6];
			mapaResultado[st.fila+1][st.columna+2] = sensores.terreno[7];
			mapaResultado[st.fila+2][st.columna+2] = sensores.terreno[8];
			mapaResultado[st.fila-3][st.columna+3] = sensores.terreno[9];
			mapaResultado[st.fila-2][st.columna+3] = sensores.terreno[10];
			mapaResultado[st.fila-1][st.columna+3] = sensores.terreno[11];
			mapaResultado[st.fila][st.columna+3] = sensores.terreno[12];
			mapaResultado[st.fila+1][st.columna+3] = sensores.terreno[13];
			mapaResultado[st.fila+2][st.columna+3] = sensores.terreno[14];
			mapaResultado[st.fila+3][st.columna+3] = sensores.terreno[15];
		break;
			
		case 3: 
			mapaResultado[st.fila][st.columna+1] = sensores.terreno[1];
			mapaResultado[st.fila+1][st.columna+1] = sensores.terreno[2];
			mapaResultado[st.fila+1][st.columna] = sensores.terreno[3];
			mapaResultado[st.fila][st.columna+2] = sensores.terreno[4];
			mapaResultado[st.fila+1][st.columna+2] = sensores.terreno[5];
			mapaResultado[st.fila+2][st.columna+2] = sensores.terreno[6];
			mapaResultado[st.fila+2][st.columna+1] = sensores.terreno[7];
			mapaResultado[st.fila+2][st.columna] = sensores.terreno[8];
			mapaResultado[st.fila][st.columna+3] = sensores.terreno[9];
			mapaResultado[st.fila+1][st.columna+3] = sensores.terreno[10];
			mapaResultado[st.fila+2][st.columna+3] = sensores.terreno[11];
			mapaResultado[st.fila+3][st.columna+3] = sensores.terreno[12];
			mapaResultado[st.fila+3][st.columna+2] = sensores.terreno[13];
			mapaResultado[st.fila+3][st.columna+1] = sensores.terreno[14];
			mapaResultado[st.fila+3][st.columna] = sensores.terreno[15];
		break;

		case 4: 
			mapaResultado[st.fila+1][st.columna+1] = sensores.terreno[1];
			mapaResultado[st.fila+1][st.columna] = sensores.terreno[2];
			mapaResultado[st.fila+1][st.columna-1] = sensores.terreno[3];
			mapaResultado[st.fila+2][st.columna+2] = sensores.terreno[4];
			mapaResultado[st.fila+2][st.columna+1] = sensores.terreno[5];
			mapaResultado[st.fila+2][st.columna] = sensores.terreno[6];
			mapaResultado[st.fila+2][st.columna-1] = sensores.terreno[7];
			mapaResultado[st.fila+2][st.columna-2] = sensores.terreno[8];
			mapaResultado[st.fila+3][st.columna+3] = sensores.terreno[9];
			mapaResultado[st.fila+3][st.columna+2] = sensores.terreno[10];
			mapaResultado[st.fila+3][st.columna+1] = sensores.terreno[11];
			mapaResultado[st.fila+3][st.columna] = sensores.terreno[12];
			mapaResultado[st.fila+3][st.columna-1] = sensores.terreno[13];
			mapaResultado[st.fila+3][st.columna-2] = sensores.terreno[14];
			mapaResultado[st.fila+3][st.columna-3] = sensores.terreno[15];
		break;

		case 5: 
			mapaResultado[st.fila+1][st.columna] = sensores.terreno[1];
			mapaResultado[st.fila+1][st.columna-1] = sensores.terreno[2];
			mapaResultado[st.fila][st.columna-1] = sensores.terreno[3];
			mapaResultado[st.fila+2][st.columna] = sensores.terreno[4];
			mapaResultado[st.fila+2][st.columna-1] = sensores.terreno[5];
			mapaResultado[st.fila+2][st.columna-2] = sensores.terreno[6];
			mapaResultado[st.fila+1][st.columna-2] = sensores.terreno[7];
			mapaResultado[st.fila][st.columna-2] = sensores.terreno[8];
			mapaResultado[st.fila+3][st.columna] = sensores.terreno[9];
			mapaResultado[st.fila+3][st.columna-1] = sensores.terreno[10];
			mapaResultado[st.fila+3][st.columna-2] = sensores.terreno[11];
			mapaResultado[st.fila+3][st.columna-3] = sensores.terreno[12];
			mapaResultado[st.fila+2][st.columna-3] = sensores.terreno[13];
			mapaResultado[st.fila+1][st.columna-3] = sensores.terreno[14];
			mapaResultado[st.fila][st.columna-3] = sensores.terreno[15];
		break;

		case 6: 
			mapaResultado[st.fila+1][st.columna-1] = sensores.terreno[1];
			mapaResultado[st.fila][st.columna-1] = sensores.terreno[2];
			mapaResultado[st.fila-1][st.columna-1] = sensores.terreno[3];
			mapaResultado[st.fila+2][st.columna-2] = sensores.terreno[4];
			mapaResultado[st.fila+1][st.columna-2] = sensores.terreno[5];
			mapaResultado[st.fila][st.columna-2] = sensores.terreno[6];
			mapaResultado[st.fila-1][st.columna-2] = sensores.terreno[7];
			mapaResultado[st.fila-2][st.columna-2] = sensores.terreno[8];
			mapaResultado[st.fila+3][st.columna-3] = sensores.terreno[9];
			mapaResultado[st.fila+2][st.columna-3] = sensores.terreno[10];
			mapaResultado[st.fila+1][st.columna-3] = sensores.terreno[11];
			mapaResultado[st.fila][st.columna-3] = sensores.terreno[12];
			mapaResultado[st.fila-1][st.columna-3] = sensores.terreno[13];
			mapaResultado[st.fila-2][st.columna-3] = sensores.terreno[14];
			mapaResultado[st.fila-3][st.columna-3] = sensores.terreno[15];
		break;

		case 7: 
			mapaResultado[st.fila][st.columna-1] = sensores.terreno[1];
			mapaResultado[st.fila-1][st.columna-1] = sensores.terreno[2];
			mapaResultado[st.fila-1][st.columna] = sensores.terreno[3];
			mapaResultado[st.fila][st.columna-2] = sensores.terreno[4];
			mapaResultado[st.fila-1][st.columna-2] = sensores.terreno[5];
			mapaResultado[st.fila-2][st.columna-2] = sensores.terreno[6];
			mapaResultado[st.fila-2][st.columna-1] = sensores.terreno[7];
			mapaResultado[st.fila-2][st.columna] = sensores.terreno[8];
			mapaResultado[st.fila][st.columna-3] = sensores.terreno[9];
			mapaResultado[st.fila-1][st.columna-3] = sensores.terreno[10];
			mapaResultado[st.fila-2][st.columna-3] = sensores.terreno[11];
			mapaResultado[st.fila-3][st.columna-3] = sensores.terreno[12];
			mapaResultado[st.fila-3][st.columna-2] = sensores.terreno[13];
			mapaResultado[st.fila-3][st.columna-1] = sensores.terreno[14];
			mapaResultado[st.fila-3][st.columna] = sensores.terreno[15];
		break;


	}
}

bool ComportamientoJugador::puedoAvanzar(Sensores sensores, estado &st) {
    if(sensores.terreno[2] == 'T' or sensores.terreno[2] == 'S' or sensores.terreno[2] == 'K' or
        sensores.terreno[2] == 'X' or sensores.terreno[2] == 'D' or (sensores.terreno[2] == 'B' and tengoZapatillas)
        or (sensores.terreno[2] == 'A' and tengoBikini)){

            return true;
    }
    else {

        return false;
    }
	// return (sensores.terreno[2] != 'P' and sensores.terreno[2] != 'M' and (sensores.terreno[2] != 'B' or tengoZapatillas) and (sensores.terreno[2] != 'A' or tengoBikini));
}

estado ComportamientoJugador::obtenerDestinoAleatorio(){

	estado salida;

		salida.fila = rand() % (mapaResultado.size() -1);
		salida.columna = rand() % (mapaResultado[0].size()-1);	

	return salida;
}

estado ComportamientoJugador::obtenerDestino() {

	estado salida;
	bool encontrado = false;

	for (i = 3; i < mapaResultado.size() - 3 && !encontrado; i++) {
		for (j = 3; j < mapaResultado[0].size() - 3 && !encontrado; j++) {

			if(mapaResultado[i][j] == '?') {

				salida.fila = i;
				salida.columna = j;
				encontrado = true;
			}
		}
	}

	if(!encontrado) 
			salida = obtenerDestinoAleatorio();

	cout << "mi destino es fila " << salida.fila << " columna " << salida.columna << endl;

	return salida;
}

// Funcion auxiliar para poner a 0 todas las casillas de una matriz
void AnularMatriz(vector<vector<unsigned char>> &m)
{
	for (int i = 0; i < m[0].size(); i++)
	{
		for (int j = 0; j < m.size(); j++)
		{
			m[i][j] = 0;
		}
	}
}

// Pinta sobre el mapa del juego el plan obtenido
void ComportamientoJugador::VisualizaPlan(const estado &st, const list<Action> &plan)
{
	AnularMatriz(mapaConPlan);
	estado cst = st;

	auto it = plan.begin();
	while (it != plan.end())
	{
		if (*it == actFORWARD)
		{
			switch (cst.orientacion)
			{
			case 0:
				cst.fila--;
				break;
			case 1:
				cst.fila--;
				cst.columna++;
				break;
			case 2:
				cst.columna++;
				break;
			case 3:
				cst.fila++;
				cst.columna++;
				break;
			case 4:
				cst.fila++;
				break;
			case 5:
				cst.fila++;
				cst.columna--;
				break;
			case 6:
				cst.columna--;
				break;
			case 7:
				cst.fila--;
				cst.columna--;
				break;
			}
			mapaConPlan[cst.fila][cst.columna] = 1;
		}
		else if (*it == actTURN_R)
		{
			cst.orientacion = (cst.orientacion + 2) % 8;
		}
		else if (*it == actSEMITURN_R)
		{
			cst.orientacion = (cst.orientacion + 1) % 8;
		}
		else if (*it == actTURN_L)
		{
			cst.orientacion = (cst.orientacion + 6) % 8;
		}
		else if (*it == actSEMITURN_L)
		{
			cst.orientacion = (cst.orientacion + 7) % 8;
		}
		it++;
	}
}

int ComportamientoJugador::interact(Action accion, int valor)
{
	return false;
}
