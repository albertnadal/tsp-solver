/*****************************************************************
[ Travelling Salesman Problem Solver ]

Aquest és un programa per optimitzar TSPs. Ha estat desenvolupat mitjançant
la implementació d'un algorisme genètic. El programa s'ha de compilar mitjançant
un compilador de C++ (g++) en sistemes Unix.

g++ tsp.cpp -o tsp

Per resoldre un mapa TSP cal introduïr
les coordenades del mapa una a una. Per evitar introduïr totes les coordenades
manualment es poden introduir les coordenades en un fitxer i volcar les dades
en el moment d'execució de la següent manera.

./tsp < mapa.txt

Juntament amb el codi font s'adjunten diferents mapes de prova amb extensió txt
que poden sér emprats a mode de prova. Els fitxers tenen el següent format. La
primera línia conté el nombre de coordenades (nodes) del mapa. Les linies restants
del fitxer contenen els valors dels components X i Y de les coordenades. S'empra
una línia per component.

El programa retorna una solució òptima a mida que en va trobant de millors. Cada
solució és una cadena de dígits, on cada dígit identifica un node del mapa (en
l'ordre seqüencial introduït), juntament amb el fitness, és a dir, la distància
total de la solució.


Author: Albert Nadal Garriga
Date: 13-10-2002

*****************************************************************/

#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>

using namespace std;

struct node
{
   int x;
   int y;
};

struct poblacio
{
   int* solucio[30];
   int fitnes[30];
};

struct valors
{
   int numero_nodes;      // indica el nombre de nodes o ciutats del problema
   int tamany_poblacio;   // indica el tamany de la població de solucions
   node *nodes;           // conté les coordenades dels nodes o ciutats
};

// *********************** ALLIBERAR MEMORIA DADES ****************************** //
void alliberar_memoria_dades(valors* dades)
{
  delete dades->nodes;  
}

// ************************ ALLIBERAR MEMORIA SOLUCIONS ************************* //
void alliberar_memoria_solucions(poblacio* solucions)
{
  for(int num_solucio = 0; num_solucio < 30; num_solucio++)
   {
      delete solucions->solucio[num_solucio];
   }
}

// ***************************** ALLIBERAR MEMORIA ****************************** //
void alliberar_memoria(valors* dades, poblacio* solucions, poblacio* fills, poblacio* solucions2, poblacio* fills2)
{
   alliberar_memoria_dades(dades);
   alliberar_memoria_solucions(solucions);
   alliberar_memoria_solucions(fills);
   alliberar_memoria_solucions(solucions2);
   alliberar_memoria_solucions(fills2);
}

// ******************************* ENTRAR DADES ********************************* //
void entrar_dades(valors* dades)
{
  printf("Quantes ciutats s'han de recorrer? ");
  std::cin >> dades->numero_nodes;
  dades->nodes = new node[dades->numero_nodes];
  for(int num_node = 0; num_node < dades->numero_nodes; num_node++)
   {
      std::cout << "\nCoordenada X del node " << num_node << ":  ";  
      std::cin >> dades->nodes[num_node].x;
      std::cout << "Coordenada Y del node " << num_node << ":  ";  
      std::cin >> dades->nodes[num_node].y;
   }
}

// **************************** INICIALITZAR VALORS****************************** //
void inicialitzar_valors(int numero_nodes, poblacio* solucions, poblacio* fills)
{ 

  for(int num_solucio = 0; num_solucio < 30; num_solucio++)
   {
      solucions->solucio[num_solucio] = new int[numero_nodes];
      solucions->fitnes[num_solucio] = 0;
   }

  for(int num_solucio = 0; num_solucio < 30; num_solucio++)
   {
      fills->solucio[num_solucio] = new int[numero_nodes];
      fills->fitnes[num_solucio] = 0;
   }

}

// ********************************* DISTANCIA ******************************** //
int distancia(int nodea, int nodeb, valors* dades)
{
  return int(floor( sqrt(pow((dades->nodes[nodea].x)-(dades->nodes[nodeb].x),2) + pow((dades->nodes[nodea].y)-(dades->nodes[nodeb].y),2)) ));
}

// ********************************* POSICIO ********************************** //
int posicio(int node, int* solucio)
{
  bool trobat = false;
  int i = 0;
  while (!trobat)
  {
     if (solucio[i] == node) {trobat = true; return i;}
     else { i = i + 1;} 
  }

  return 0;
}

// *********************************** POS ************************************ //
int pos(int x, int num_nodes)
{
 if (x < 0) { return (num_nodes + x); }
 else { return (x%num_nodes); }
}

// ********************************** IGUALS ********************************** //
bool iguals(int nodea[], int nodeb[], int num_nodes)
{
  bool fi = false;
  int e = nodea[0];
  int direccio;                // 1 = esquerra, 0 = dreta
  int d;
  int i = 0;
  
  d = posicio(e, nodeb);

  if (nodeb[pos(d + 1, num_nodes)] == nodea[pos(1, num_nodes)]) {direccio = 0;}
  else if (nodeb[pos(d - 1, num_nodes)] == nodea[pos(-1, num_nodes)]) {direccio = 1;}
  else {return false;}

  if (direccio == 0)
   {
       while(!fi)
        {
            d++;
            i++;
            if (nodeb[pos(d, num_nodes)] != nodea[pos(i, num_nodes)]) {return false;}
            if (pos(i, num_nodes) == 0) { fi = true;}             
        }
   }
  else
   {
       while(!fi)
        {
            d--;
            i--;
            if (nodeb[pos(d, num_nodes)] != nodea[pos(i, num_nodes)]) {return false;}
            if (pos(i, num_nodes) == 0) { fi = true;}             
        }
   }

  return true;
}

// *************************** INICIALITZAR POBLACIÓ ************************** //
void inicialitzar_poblacio(poblacio* solucions, int num_nodes)
{
  int aux, pos1, pos2;
  for(int num_node = 0; num_node < num_nodes; num_node++)
   {     
      for(int num_solucio = 0; num_solucio < 30; num_solucio++)
       {
            solucions->solucio[num_solucio][num_node] = num_node;
       }  
   }

  for(int num_solucio = 0; num_solucio < 30; num_solucio++)
   {
      for(int num_node = 0; num_node < num_nodes; num_node++)
       {     
            pos1 = rand()%(num_nodes);
            pos2 = rand()%(num_nodes);
            aux = solucions->solucio[num_solucio][pos1];
            solucions->solucio[num_solucio][pos1] = solucions->solucio[num_solucio][pos2];
            solucions->solucio[num_solucio][pos2] = aux;
       }  
   }

}

// ***************************** EVALUAR POBLACIÓ ***************************** //
void evaluar_poblacio(poblacio* solucions, valors* dades)
{
  int d;
  for(int num_solucio = 0; num_solucio < 30; num_solucio++)
   {
      solucions->fitnes[num_solucio] = 0;
      for(int num_node = 1; num_node < dades->numero_nodes; num_node++)
       {     
          d = distancia(solucions->solucio[num_solucio][num_node - 1], solucions->solucio[num_solucio][num_node], dades);
          solucions->fitnes[num_solucio] = solucions->fitnes[num_solucio] + d;
       }  
      d = distancia(solucions->solucio[num_solucio][0], solucions->solucio[num_solucio][dades->numero_nodes - 1], dades);
      solucions->fitnes[num_solucio] = solucions->fitnes[num_solucio] + d;
   }
}

// *************************** INTERCANVI SOLUCIONS *************************** //
void intercanvi_solucions(poblacio* solucions, int i, int num_nodes)
{
 int aux;
 for (int e = 0; e < num_nodes; e++)
   {
      aux = solucions->solucio[i-1][e];
      solucions->solucio[i-1][e] = solucions->solucio[i][e];
      solucions->solucio[i][e] = aux;
   }
}

// ***************************** EFECTE BOMBOLLA ****************************** //
void efecte_bombolla(poblacio* solucions, const int e, int num_nodes)
{
int aux, i;
i = e;
while ((solucions->fitnes[i-1] > solucions->fitnes[i]) && (i>0))
 {
     aux = solucions->fitnes[i-1];
     solucions->fitnes[i-1] = solucions->fitnes[i];
     solucions->fitnes[i] = aux;
     intercanvi_solucions(solucions, i, num_nodes);
     i = i - 1; 
 }

}

// ***************************** ORDENAR POBLACIÓ ***************************** //
void ordenar_poblacio(poblacio* solucions, int num_nodes)
{
  for(int i = 0; i < 29; i++)
   {
      if (solucions->fitnes[i+1] < solucions->fitnes[i]) 
       {
           efecte_bombolla(solucions, i+1, num_nodes);  //fem una ordenació pel mètode de la bombolla
       }
   }
}

// ***************************** INICIALITZAR USATS *************************** //
void inicialitzar_usats(bool usats[], const int i)
{
  for(int n = i; n>=0; n--) { usats[n] = false;}
}

// ****************************** NODES REPETITS ****************************** //
int nodes_repetits(int veinat[])
{
int e = 0;
int i;
while (veinat[e] != -1)
 {
    i = e + 1;
    while (veinat[i] != -1)
     {
        if (veinat[e] == veinat[i]) { return veinat[e]; }
        i++;
     }
    e++;
 }
return -1;
}

// ******************************** MILLOR VEI ******************************** //
int millor_vei(int node, int veinat[], valors* dades)
{
int millor = -1;
int millor_distancia = 999999999;              //aquesta és una distància impossible
int dist, n;
int i = 0;

if (veinat[0] == -1) { return -1; }

n = nodes_repetits(veinat);

if (n != -1) { return n; }
else
  {
     while (veinat[i] != -1)
      {
          dist = distancia(node, veinat[i], dades);
          if ( dist < millor_distancia) {millor = veinat[i]; millor_distancia = dist;}
          i++;
      }
     return millor;
  }
}

// *********************** CREAR LLISTA NODES RESTANTS ************************ //
void crear_llista_nodes_restants(int veinat[], bool usats[], int num_nodes)
{
  int p = 0;
  for(int i = 0; i < num_nodes; i++)
   {
     if(usats[i] == false) { veinat[p] = i; p++; }
   }
  veinat[p] = -1;
}

// ********************************** VEINS *********************************** //
void veins(int x, int* pare, int* mare, bool usats[], int num_nodes, int veinat[], int e)
{
  int i = 0;
  int p;

  if (usats[  pare[  pos(x - 1, num_nodes)  ]  ] == false) { veinat[i] = pare[  pos(x - 1, num_nodes)  ]; i++; }
  if (usats[  pare[  pos(x + 1, num_nodes)  ]  ] == false) { veinat[i] = pare[  pos(x + 1, num_nodes)  ]; i++; }
  
  p = posicio(pare[x], mare);
         
  if (usats[  mare[  pos(p - 1, num_nodes)  ]  ] == false) { veinat[i] = mare[  pos(p - 1, num_nodes)  ]; i++; }
  if (usats[  mare[  pos(p + 1, num_nodes)  ]  ] == false) { veinat[i] = mare[  pos(p + 1, num_nodes)  ]; i++; }

  veinat[i] = -1;  //aquesta és la marca de final
 
  if ((veinat[0] == -1) && (e < num_nodes)) { crear_llista_nodes_restants(veinat, usats, num_nodes);}
}

// *********************************** MUTAR ********************************** //
void mutar(int* fill, int num_nodes)
{
  int x;
  int y;
  int aux;
  div_t e = div(num_nodes, 10);        //per cada 10 nodes farem una mutació
  
  for(int i = e.quot; i > 0; i--)
   {
     y = rand()%(num_nodes);
     x = rand()%(num_nodes);
     aux = fill[x];
     fill[x] = fill[y];
     fill[y] = aux;
   }
}

// ********************************* CROSSOVER ******************************** //
void crossover(int* pare, int* mare, int* fill, valors* dades)
{
  int x = 0;
  int num_nodes = dades->numero_nodes;
  bool usats[num_nodes];
  bool fi = false;
  int pos = 0;
  int veinat[num_nodes + 1];

  inicialitzar_usats(usats, num_nodes);             //considerem tots els nodes com a no usats

  fill[pos] = pare[x];
  pos++;
  usats[pare[x]] = true;
  while ((pos < num_nodes) && (!fi)) 
   {
       veins(x, pare, mare, usats, num_nodes, veinat, pos);

       if (veinat[0] != -1) 
        { 
            fill[pos] = millor_vei(pare[x], veinat, dades);
            usats[fill[pos]] = true; 
            x = posicio(fill[pos], pare);
            pos++;
        }
       else { fi = true; }
   }

  if (iguals(pare, fill, num_nodes)) { mutar(fill, num_nodes); }  //si surt un fill igual aplico mutació
}

// *************************** CROSSOVER I MUTACIÓ **************************** //
void crossover_i_mutacio(poblacio* solucions, poblacio* fills, valors* dades)
{
 for(int i = 0; i < 29; i++)
   {
      crossover(solucions->solucio[i], solucions->solucio[i+1], fills->solucio[i], dades);
   }
}

// *************************** VISUALITZAR POBLACIO *************************** //
void visualitzar_poblacio(poblacio* solucions, int num_nodes)
{
  for(int i = 0; i < 30; i++)
    {
       for(int e = 0; e < num_nodes; e++)
         {
             std::cout << solucions->solucio[i][e] << " ";
         }
       std::cout << "-> " << solucions->fitnes[i] << "\n";
    }
}

// ********************************** COPIAR ********************************** //
void copiar(int desti[], int origen[], int num_nodes)
{
   for(int e = 0; e < num_nodes; e++)
     {
         desti[e] = origen[e];
     }
}

// **************************** DESPLAÇAR SOLUCIONS *************************** //
void desplacar_solucions(poblacio* pares, int i, int num_nodes)
{
   if (i == 29) { }
   else
    {
       for(int e = 28; e >= i; e--)
         {
             copiar(pares->solucio[e+1], pares->solucio[e], num_nodes);
             pares->fitnes[e+1] = pares->fitnes[e];
         }
    }

}

// ********************************* INSERIR ********************************** //
void inserir(int fill[], int fitnes, poblacio* pares, int num_nodes)
{
  bool inserit = false;
  int i = 0;

  while ((!inserit) && (i < 30))
   {
       if (pares->fitnes[i] > fitnes) 
        { 
            desplacar_solucions(pares, i, num_nodes);
            copiar(pares->solucio[i], fill, num_nodes);              
            pares->fitnes[i] = fitnes;
            inserit = true;
        }
       else if (pares->fitnes[i] == fitnes)
        {
            if (iguals(pares->solucio[i], fill, num_nodes)) { inserit = true; }
            else { i++; }
        } 
       else{ i++; }
   }

}

// ********************************* SELECCIO ********************************* //
void seleccio(poblacio* pares, poblacio* fills, int num_nodes)
{
for(int i=1; i < 30; i++)
 {
    inserir(fills->solucio[i], fills->fitnes[i], pares, num_nodes);
 }
}

// **************************** RESTAURAR POBLACIÓ ************************** //
void restaurar_poblacio(poblacio* solucions, int num_nodes)
{
  int aux, pos1, pos2;

  for(int num_solucio = 0; num_solucio < 30; num_solucio++)
   {
      for(int num_node = 0; num_node < num_nodes; num_node++)
       {     
            pos1 = rand()%(num_nodes);
            pos2 = rand()%(num_nodes);
            aux = solucions->solucio[num_solucio][pos1];
            solucions->solucio[num_solucio][pos1] = solucions->solucio[num_solucio][pos2];
            solucions->solucio[num_solucio][pos2] = aux;
       }  
   }

}

// *********************************** MAIN *********************************** //
int main()
{
  valors dades;
  poblacio solucions, solucions2;
  poblacio fills, fills2;
  int millor = 99999999;

  system("clear");
  printf("TRAVELLING SALESMAN PROBLEM Solver\n");

  srand((unsigned)time( NULL ));                           // generem la llavor d'aleatorització
  entrar_dades(&dades);
  unsigned int temps_inici = (unsigned)time( NULL );


  inicialitzar_valors(dades.numero_nodes, &solucions, &fills);         // entrem les dades dels nodes o ciutats
  inicialitzar_poblacio(&solucions, dades.numero_nodes);   // generem solucions aleatories  
  evaluar_poblacio(&solucions, &dades);                    // evaluem la població de solucions
  ordenar_poblacio(&solucions, dades.numero_nodes);        // ordenem les solucions segons el fitnes

  inicialitzar_valors(dades.numero_nodes, &solucions2, &fills2);         // entrem les dades dels nodes o ciutats
  inicialitzar_poblacio(&solucions2, dades.numero_nodes);   // generem solucions aleatories  
  evaluar_poblacio(&solucions2, &dades);                    // evaluem la població de solucions
  ordenar_poblacio(&solucions2, dades.numero_nodes);        // ordenem les solucions segons el fitnes

  for(int g = 0; g < 50000; g++)
   {
        crossover_i_mutacio(&solucions, &fills, &dades);   // generem una població de fills via crossover   
        evaluar_poblacio(&fills, &dades);                  // evaluem la població de fills generada
        ordenar_poblacio(&fills, dades.numero_nodes);      // ordenem la població de fills segons el fitnes
        seleccio(&solucions, &fills, dades.numero_nodes);  // incorporem els millors fills a la població

        crossover_i_mutacio(&solucions2, &fills2, &dades);   // generem una població de fills via crossover   
        evaluar_poblacio(&fills2, &dades);                  // evaluem la població de fills generada
        ordenar_poblacio(&fills2, dades.numero_nodes);      // ordenem la població de fills segons el fitnes
        seleccio(&solucions2, &fills2, dades.numero_nodes);  // incorporem els millors fills a la població

        if (g%501 == 500) 
          { 
              seleccio(&solucions, &solucions2, dades.numero_nodes);  // incorporem els millors fills a la població
              restaurar_poblacio(&solucions2, dades.numero_nodes);
              evaluar_poblacio(&solucions2, &dades);                  // evaluem la població de fills generada
              ordenar_poblacio(&solucions2, dades.numero_nodes);      // ordenem la població de fills segons el fitnes
          }

        if (solucions.fitnes[0] < millor) 
          {
             millor = solucions.fitnes[0];
             std::cout << "\nTSP Solver [ GENERACIO: " << g << "/50000 ] [ MILLOR LONGITUD: " << millor << " ] [ TEMPS: " << (unsigned)time( NULL ) - temps_inici << " Seg. ]\n";
             visualitzar_poblacio(&solucions, dades.numero_nodes);
          }
   }

   alliberar_memoria(&dades, &solucions, &fills, &solucions2, &fills2);
   return 0;
}



