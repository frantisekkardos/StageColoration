#define VERTS   25   /* max number of vertices in a free completion + 1 */
#define DEG     5  /*13    max degree of a vertex in a free completion + 1 */
         /* must be at least 13 because of row 0            */
#define EDGES   62   /* max number of edges in a free completion + 1    */
#define MAXRING 13   /* max ring-size */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


typedef long tp_confmat[VERTS][DEG];
typedef long tp_angle[EDGES][7];
typedef long tp_edgenb[EDGES][EDGES];
typedef struct Couplage Couplage;
typedef struct Liste Liste;

struct Couplage
{
   int premier;
   int deuxieme;
   Couplage *suivant;
};

struct Liste
{
   Couplage *first;
   Liste *second;
};

Liste *vector[MAXRING];
int arete[MAXRING];
long newcode[MAXRING+1];
long codeTest=-1;

int *Couleursext(long, long, int *);
int Nbcouleurs(int *, int, int, long);
void AretesColories(int* , int, int, long);
void Genermodification(long, int *, int, int, long, Couplage *, long[][4][4][17], char *, long *);
bool BonneModif(long, int *, int, int, long, Couplage *, long[][4][4][17], char *, long *);
bool BonCoup(long, int *, int, int, long, long[][4][4][17], char *, long *);
bool BonneColor(long, int *, long, long[][4][4][17], char *, long *);
long Comparer(Couplage *, Couplage *);
Couplage *Inserer(int, int, Couplage *);
long Appartenir(Couplage *, Liste *);
Liste *Ajouter(Couplage *, Liste *);
int *GenerInterval(int, int, long,bool);
void GenerCouplage(int);
void AffichageC(Couplage *);
void AffichageL(Liste *);
void Numerotation(tp_confmat, tp_edgenb);
void Voisinage(tp_confmat, tp_angle);
long Coloration(char *, tp_angle, long[]);
void record(long[], long[], long, char *);
long Lecture(tp_confmat, FILE*, long *);


int main()
{
   long ring,ring2, n,n2, i,j,k,l,m,h, power[17], edges, ncodes,ncodes2, nbColoration, nbColorationRed, modifcode[4][4][4][17];
   tp_angle angle, angle2;
   tp_confmat A, B;
   tp_edgenb edgenb;
   char *s;
   FILE *fp;
   
   s = "instance.conf";

   fp = fopen(s, "r");
   if(fp == NULL){
      printf("Can't open the file %s\n",s);
      exit(1);
   }


   power[1] = 1;
   for(i = 2; i <= 16; i++){
      power[i] = 4* power[i-1];
   }
   
   for(k = 0; k <= 3; k++){
      for(l = 0; l <= 3; l++){
         for(m = 0; m<= 3; m++){
            for(i = 0; i <= 16; i++){
               modifcode[k][l][m][i] = (-2*m+k+l)*power[i+1];
            }
         }
      }
   }

   printf("Avant de générer les couplages ça va.\n");
   for(i = 0; i <= MAXRING; i++){
      GenerCouplage(i);
   }

   char *live = malloc(power[MAXRING+1] * sizeof(live));
   int *couleur = malloc(MAXRING * sizeof(couleur));
   long *modifies = malloc(MAXRING * sizeof(modifies));
   char *reduites = malloc(power[MAXRING+1]* sizeof(reduites));
   
   // int *couleur = malloc(8 * sizeof(couleur));
   // int *are = malloc( VERTS * sizeof(are));
   // long *newcode = malloc(ncodes * sizeof(newcode));
 
   while(!Lecture(A, fp, NULL)){
      n = A[0][0];
      ring = A[0][1];
      edges = 0;
      for(i = 1; i <= n; i++){
         edges += A[i][0];
      }
      edges = edges/2;
      ncodes = power[ring+1];
      for(i = 0; i < ncodes; i++){
         live[i] = 1;
      }

      Voisinage(A, angle);

      nbColoration = Coloration(live, angle, power);
      int s=0;
      int cmt = 1;
      for (i=0;i<ncodes;i++) 
         {if (live[i]==0) {s++;
         }}
      printf("J'ai trouve %d  de coloration \n", s);
      while(cmt > 0){
         cmt = 0;
         for(i = 0; i < ncodes; i++){
            if(live[i] == 1){
               if(BonneColor(i, couleur, ring, modifcode, live, modifies) == true){
                  live[i] = 0;
                  //printf("nouvelles bonnes colorations %d\n",i);
                  s++;
                  cmt++;
                  //printf("%d desormais bonne coloration\n", i); 
               }
            }
         }
         printf("nb de nouvelles colorations desormais bonnes %d \n", cmt);
      }

      if(!Lecture(B, fp, NULL)){
         // for(i = 0; i<= B[0][0]; i++){
         //    printf("[");
         //    for(j = 0; j <= B[i][0]; j++){
         //       printf(" %d, ", B[i][j]);
         //    }
         //    printf("]\n");
         // }
         n2 = B[0][0];
         ring2 = B[0][1];
         edges = 0;
         for(i = 1; i <= n; i++){
            edges += B[i][0];
         }
         edges = edges/2;
         ncodes2 = power[ring2+1];

         for(i = 0; i < ncodes2; i++){
            reduites[i] = 1;
         }
         int t = 0;
         int u;
         Voisinage(B,angle2);
          //for(u = 1; u <= edges; u++){
              //printf("la valeur de angle2[%d] = [%d][%d][%d][%d][%d][%d][%d]\n",u,angle2[u][0],angle2[u][1],angle2[u][2],angle2[u][3],angle2[u][4],angle2[u][5],angle2[u][6]);
           //}
         nbColorationRed = Coloration(reduites, angle2, power);
         int r = 0;
         bool reductible = true;
         for(i = 0; i < ncodes2; i++){
            if(reduites[i] == 0){
               r++;
               
            }
         }
         printf("J'ai trouve %d  de colorations reduites\n", r);
         //printf(" valeur %d et %d\n", reduites[228], live [228]);
         for(i = 0; i < ncodes2; i++){
            if(reduites[i] != live[i]){
               t++;
               //printf("reduites vs live %d\n", i);
            }
            if(reduites[i] == 0 && live[i] != 0){
               reductible = false;
               //printf("Le code %d ne se reduit pas\n", i);
            }
         }
         if(reductible == true){
            printf("La reduction est bonne et %d inegalites entre %d live et %d reduites\n", t, s, r);
         }else{
            printf("La reduction n'est pas licite\n");
         }  


      }
      printf("--------------------------------------------------------------------------\n");
   }
   free(couleur);
   free(modifies);
   free(live);
   free(reduites);
   return (0);
}


int *Couleursext(ncodes, ring, couleurs)
long ncodes, ring;
int *couleurs;
{
   //int *couleur = malloc(ring * sizeof(couleur));
   int *couleur = couleurs;
   long code;
   int i;
   code = ncodes;
   for(i = 0; i < ring; i++){
      couleur[i] = code % 4;
      code = (code - (code % 4))/4;
   }
   return (couleur);
}

int Nbcouleurs(couleurs, i, j, ring)
int *couleurs;
int i, j;
long ring;
{
   int k, idx;
   idx = 0;
   for(k = 0; k < ring; k++){
      if(couleurs[k] == i || couleurs[k] == j){
         idx++;
      }
   }
   return (idx);
}

void AretesColories(couleurs, i, j, ring)
int *couleurs;
int i;
int j;
long ring;
{
   int k, idx;
   idx = 0;
   for(k = 0; k < MAXRING; k++){
      arete[k] = 0;
   }
   for(k = 0; k < ring; k++){
      if(couleurs[k] == i || couleurs[k] == j){
         arete[idx] = k;
         idx++;
      }
   }
}

void Genermodification(ncodes, couleur, k, l, ring, couplage, modifcode, live, modifies)
int *couleur;
int k, l;
long ncodes, ring;
Couplage *couplage;
long modifcode[][4][4][17];
char *live;
long *modifies;
//long *newcode;
{
   Couplage *C;
   //long *modifies = malloc(ring * sizeof(modifies));
   int i, idx, nb;

   idx = 1;
   C = couplage;
   nb = Nbcouleurs(couleur, k, l, ring);
   AretesColories(couleur, k, l, ring);
   for(i = 0; i <= MAXRING; i++){
      newcode[i] = 0;
   }
   for(i = 0; i < nb; i++){
      modifies[i] = 0;
   }
   for(i = 0; i < nb; i++){
      if(modifies[i] != 1){
         if(C == NULL){
            newcode[idx] = ncodes + modifcode[k][l][couleur[arete[i]]][arete[i]];
         }else{
            if(i == C->premier){
               newcode[idx] = ncodes + modifcode[k][l][couleur[arete[C->premier]]][arete[C->premier]] + modifcode[k][l][couleur[arete[C->deuxieme]]][arete[C->deuxieme]];
               modifies[C->premier] = 1;
               modifies[C->deuxieme] = 1;
               C = C->suivant;
            }else{
               newcode[idx] = ncodes + modifcode[k][l][couleur[arete[i]]][arete[i]];
            }
         }
         idx++;
      }
   }
   newcode[0] = idx -1;
   // printf("%d code et newcode: [", ncodes);
   // for(i = 0; i < idx; i++){
   //    printf( "%d, ", newcode[i]);
   // }
   // printf(" ]\n");
}

bool BonneModif(code, couleur, k, l, ring, C, modifcode, live, modifies)
int *couleur;
int k, l;
long code, ring;
Couplage *C;
long modifcode[][4][4][17];
char *live;
long *modifies;
{
   int i;
   Genermodification(code, couleur, k, l, ring, C, modifcode, live, modifies);
   for(i = 1; i <= newcode[0]; i++){
      if(live[newcode[i]] == 0){
         if (code==codeTest){
            printf("Le code %d se réduit en %d\n",code, newcode[i]);
         }
         return true;
      }
   }
   return false;
}
/*
returne true/false, étant donné un code ou un vecteur de couleurs et un pair de couleurs, on teste sur tout les couplages possibles
s'il existe une modification telle que la nouvelle coloration marche
*/
bool BonCoup(code, couleur, k, l, ring, modifcode, live, modifies)
long code, ring;
int *couleur;
int k, l;
long modifcode[][4][4][17];
char *live;
long *modifies;
{
   //int *couleur; = malloc(ring * sizeof(couleur));
   int nb, i, j;
   Liste *L; //= malloc(sizeof(*L));
   couleur = Couleursext(code, ring, couleur);
   nb = Nbcouleurs(couleur, k, l, ring);
   L = vector[nb];
   while(L != NULL){
      if(BonneModif(code, couleur, k, l, ring, L->first, modifcode, live, modifies) == false){
         if (code==codeTest) {
            printf("le code %d ne correspond pas a une bonne coloration pour le pair de couleur %d-%d\n", code, k,l);
            AffichageC(L->first);
         }   
         return false;
      }
      L = L->second;
   }
   return true;
}

bool BonneColor(code, couleur, ring, modifcode, live, modifies)
long code, ring;
int *couleur;
long modifcode[][4][4][17];
char *live;
long *modifies;
{
   int i, j;
   for(i = 0; i < 3; i++){
      for(j = i+1; j <= 3; j++){
         //printf("Je vais tester les couleurs %d et %d\n", i, j);
         if(BonCoup(code, couleur, i, j, ring, modifcode, live, modifies) == true){
            //printf("C'est bon pour le couleur %d et %d \n", i, j);
            return true;
         }
      }
   }
   return false;
}


long Comparer(c, d)
Couplage *c;
Couplage *d;
{
   if((c == NULL && d != NULL) || (c != NULL && d == NULL)){
      return (0);
   }else if(c == NULL && d == NULL){
      return (1);
   }else if(c != NULL && d != NULL){
      if(c->premier != d->premier){
         return (0);
      }else{
         if(c->deuxieme != d->deuxieme){
            return (0);
         }else{
            return(Comparer(c->suivant,d->suivant));
         }
      }
   }
}

Couplage *Inserer(a, b, c)
int a;
int b;
Couplage *c;
{
   if(c == NULL){
      Couplage *d = malloc(sizeof(*d));
      d->premier = a;
      d->deuxieme = b;
      d->suivant = NULL;
      return (d);
   }else{
      if(a < c->premier){
         Couplage *d = malloc(sizeof(*d));
         d->premier = a;
         d->deuxieme = b;
         d->suivant = c;
         return (d);
      }else if(a > c->premier){
         c->suivant = Inserer(a, b, c->suivant);
      }
   }
   
}

long Appartenir(c, d)
Couplage *c;
Liste *d;
{
   if(d == NULL){
      return (0);
   }else{
      if(Comparer(d->first, c) == 1){
         return (1);
      }else{
         Appartenir(c, d->second);
      }
   }
}

Liste *Ajouter(c, d)
Couplage *c;
Liste *d;
{
   Liste *g = malloc(sizeof(*g));
   g->second = d;
   g-> first = c;
   return (g);
}

int *GenerInterval(int a, int b, long ring, bool FLAG)
{
   int *interval = malloc(ring * sizeof(interval));
   int idx = -1;
   int idx2 = 0;
   bool parse_flag = FLAG; 

   while (++idx < ring)
   {
     if (!FLAG)
      {
         if (idx == a)
            parse_flag = true;
         else if (idx == b)
            parse_flag = false;
         else if (parse_flag)
            interval[idx2++] = idx;
      }
      if (FLAG)
      {
         if (idx == a)
            parse_flag = false;
         else if (idx == b)
            parse_flag = true;
         else if (parse_flag)
            interval[idx2++] = idx;
      }
   }
   return (interval);
}

// int *GenerInterval(a, b, ring)
// int a;
// int b;
// int ring;
// {
//    int *interval;
//    int i, cmt;
//    if(a < b){
//       if(b-a-1 == 0){
//          interval[0] = 0;
//          return (interval);
//       }else{
//          for(i = 0; i < b-a-1; i++){
//             interval[i] = a + 1 + i;
//          }
//       }
//    }else{
//       cmt = 0;
//       for(i = 0; i < b-1; i++){
//          interval[i] = i + 1;
//          cmt++;
//       }
//       for(i = 0; i < ring - a; i++){
//          interval[cmt+i] = a+i+1;
//       }
//    }
//    return (interval);
// }

void GenerCouplage(k)
int k;
{
   Couplage *C1 = malloc(sizeof(*C1));
   Couplage *C2;
   Couplage *C3;
   Liste *L;
   int *interval;
   int i, j, l,h,h2, u,w;
   if(k == 0 || k == 1){
      C1 = NULL;
      L = Ajouter(C1, NULL);
      vector[k] = L;
      //AffichageL(vector[k]);
   }else if(k == 2){
      L = Ajouter(NULL, NULL);
      C1->premier = 0;
      C1->deuxieme = 1;
      C1->suivant = NULL;
      L = Ajouter(C1, L);
      vector[2] = L;
      //AffichageL(vector[2]);
   }else{
      for(u = k; u <= k; u++){
         L = Ajouter(NULL, NULL);
         for(i = 0; i < u-1; i++){
            for(j = i+1; j < u; j++){
               C1->premier = i;
               C1->deuxieme = j;
               C1->suivant = NULL;
               if(Appartenir(C1,L) == 0){
                  L = Ajouter(C1, L);
               }
               interval = GenerInterval(i, j, u,false);
               //printf("%d-%d ieme interval:[", i,j);
               //for(l = 0; l < j-i-1; l++){
               //   printf(" %d ",interval[l]);
               //}
               //printf("]\n");
               h = j-i-1;
               Liste *g;
               g = vector[h];
               //printf("vector de %d\n",h);
               //AffichageL(vector[h]);
               while(g != NULL){
                  C3 = NULL;
                  C2 = g->first;
                  //AffichageC(C2);
                  while(C2 != NULL){
                     C3 = Inserer(interval[C2->premier], interval[C2->deuxieme], C3);
                     //printf("C3\n");
                     //AffichageC(C3);
                     C2 = C2->suivant;
                  }
                  C1 = Inserer(i, j, C3);
                  if(Appartenir(C1, L) == 0){
                     L = Ajouter(C1, L);
                  }
                  g = g->second;
               }
               //AffichageL(L);
               
               interval = GenerInterval(i, j, u,true);
               //printf("%d-%d ieme interval:[", i,j);
               //for(l = 0; l < u-j+i-1; l++){
               //   printf(" %d ",interval[l]);
               //}
               //printf("]\n");
               h2 = u-j+i-1;
               g = vector[h2];
               //printf("vector de %d\n",h2);
               //AffichageL(vector[h2]);
               while(g != NULL){
                  C2 = g->first;
                  //AffichageC(C2);
                  C3 = NULL;
                  while(C2 != NULL){
                     C3 = Inserer(interval[C2->premier], interval[C2->deuxieme], C3);
                     C2 = C2->suivant;
                  }
                  C1 = Inserer(i, j, C3);
                  if(Appartenir(C1, L) == 0){
                     L = Ajouter(C1, L);
                  }

                  
                  g = g->second;
               }
            }

         }
         vector[u] = L;
         //AffichageL(L);
         //printf("ça y est%d \n",u);
      }
   }
   //free(C1);
}

void AffichageC(C)
Couplage *C;
{
   if(C == NULL){
      printf("FIN DE COUPLAGE\n");
   }else{
      printf(" %d %d \n", C->premier, C->deuxieme);
      AffichageC(C->suivant);
   }
}

void AffichageL(L)
Liste *L;
{
   if(L == NULL){
      printf("FIN DE LA LISTE \n");
   }else{
      Couplage *C;
      C=L->first;
      //printf(" %d %d \n", C->premier, C->deuxieme);
      AffichageC(C);
      AffichageL(L->second);
   }
}

/* Enregistrement des voisins de chaque arête */
// void Voisinage(A,angle)
// tp_confmat A;
// tp_angle angle;
// {
//    long   u, v, n, ring, w, count, arete, edges;
//    tp_edgenb edgenb;
//    //printf("coucou1\n");
//    (void) Numerotation(A,edgenb);
//    n = A[0][0];
//    ring = A[0][1];
//    edges = 0;

//    for(u = 1; u <= n; u++){
//       edges += A[u][0];
//    }
//    edges = edges/2;

//    angle[0][0] = n;
//    angle[0][1] = ring;
//    angle[0][2] = edges;
//    for(u = 1; u <= edges; u++){
//       angle[u][0] = angle[u][1] = angle[u][2] = angle[u][3] = angle[u][4] = angle[u][5] = 0;
//    }
//    for (u = 1; u <= n; u++){
//       for(v = 1; v <= n; v++){
//          if(edgenb[u][v] != 0){
//             arete = edgenb[u][v];
//             //printf("arete %d = edgenb[%d][%d] = %d\n",arete,u,v,edgenb[u][v]);
//             angle[arete][0] = A[u][0] + A[v][0] - 2;
//             count = 1;
//             for(w = 1; w <= A[u][0]; w++){
//                if(A[u][w] != v){
//                   angle[arete][count] = edgenb[u][A[u][w]];
//                   count++;
//                }
//             }
//             for(w = 1; w <= A[v][0]; w++){
//                if(A[v][w] != u){
//                   angle[arete][count] = edgenb[v][A[v][w]];
//                   count++;
//                }
//             }
//             if(count > 6){
//                printf("Error reading le voisinage de l'arete %d\n", arete);
//                exit(5);
//             }
//          }
//       }
//    }
//    // for(u = 1; u <= edges; u++){
//    //    printf("la valeur de angle[%d] = [%d][%d][%d][%d][%d][%d][%d]\n",u,angle[u][0],angle[u][1],angle[u][2],angle[u][3],angle[u][4],angle[u][5],angle[u][6]);
//    // }
// }


void Voisinage(A,angle)
tp_confmat A;
tp_angle angle;
{
   long   u, v, n, ring, w, count, arete, edges;
   tp_edgenb edgenb;
   //printf("coucou1\n");
   (void) Numerotation(A,edgenb);
   n = A[0][0];
   ring = A[0][1];
   edges = 0;

   for(u = 1; u <= n; u++){
      edges += A[u][0];
   }
   edges = edges/2;

   angle[0][0] = n;
   angle[0][1] = ring;
   angle[0][2] = edges;
   for(u = 1; u <= edges; u++){
      angle[u][0] = angle[u][1] = angle[u][2] = angle[u][3] = angle[u][4] = angle[u][5] = 0;
   }
   for (u = 1; u <= n; u++){
      for(v = 1; v <= n; v++){
         if(edgenb[u][v] != 0){
            arete = edgenb[u][v];
            //printf("arete %d = edgenb[%d][%d] = %d\n",arete,u,v,edgenb[u][v]);            
            count = 1;
            for(w = 1; w <= A[u][0]; w++){
               if(A[u][w] != v){
                  if(edgenb[u][A[u][w]] > arete){
                     angle[arete][count] = edgenb[u][A[u][w]];
                     count++;
                  }
               }
            }
            for(w = 1; w <= A[v][0]; w++){
               if(A[v][w] != u){
                  if(edgenb[v][A[v][w]] > arete){
                     angle[arete][count] = edgenb[v][A[v][w]];
                     count++;
                  }
               }
            }
            angle[arete][0] = count-1;
            if(count > 6){
               printf("Error reading le voisinage de l'arete %d\n", arete);
               exit(5);
            }
         }
      }
   }
   // for(u = 1; u <= edges; u++){
   //     printf("la valeur de angle[%d] = [%d][%d][%d][%d][%d][%d][%d]\n",u,angle[u][0],angle[u][1],angle[u][2],angle[u][3],angle[u][4],angle[u][5],angle[u][6]);
   //  }
}


void Numerotation(A, edgenb)
tp_confmat A;
tp_edgenb edgenb;
{
   long i, j, n, ring,h, count;

   n = A[0][0];
   ring = A[0][1];
   for(i = 1; i <= n; i++){
      for(j = 1; j <= n; j++){
         edgenb[i][j] = 0;
      }
   }
   count = 1;
   /*On numerote premierement les arêtes de l'exterieur */
   for(i = 1; i <= ring; i++){
      if(A[i][0] != 1){
         printf("Error, pas bien défini la configuration \n");
         exit(4);
      }
      edgenb[i][A[i][1]] = count;
      count++;
   }
   /* Pour les arêtes qui ne sont pas dans l'exterieur */
   //count = ring + 1;
   for(i = ring + 1; i <= n; i++){
      for(j = 1; j <= A[i][0]; j++){
         h = A[i][j];
         //printf("A[%d][%d] = %d\n", i,j,A[i][j]);
         if(edgenb[i][h] == 0 && edgenb[h][i] == 0){
            edgenb[i][h] = count;
            count++;
         }else if(edgenb[h][i] != 0){
            edgenb[i][h] = edgenb[h][i];
         }

      }
   }
   // for(i = 1; i <= n; i++){
   //    for(j = 1; j <= n; j++){
   //       printf("la valeur de edgenb[%d][%d] = %d\n",i,j,edgenb[i][j]);
   //    }
   //  }
}

long Coloration(live,  angle, power)
long power[];
tp_angle angle;
char *live;
{
   long ring, edges, j, extent, forbidden[EDGES], c[EDGES], *am,u,i,c1;

   ring = angle[0][1];
   edges = angle[0][2];
   //printf("edges %d\n", edges);
   j = edges + 1;
   c[j] = 1;
   extent = 0;
   while(1){
         //printf("forbidden[%d]= %d et couleur: c=%d\n", j, forbidden[j], c[j]);
         while(forbidden[j] & c[j]){
            c[j] <<= 1;
             //printf("1 c[%d] = %d\n",j,c[j]);
//             scanf("%d",c1);
            while( c[j] & 16){
               if ( j >= edges){
                  return (extent);
               }
               c[++j] <<= 1;
               //printf("2 c[%d] = %d\n",j,c[j]);
//               scanf("%d",c1);
            }
         }
         if (j == 1) {
            record(c, power, ring, live);
            extent++;
            //printf("%d nb de colorations\n",extent);
            c[j] <<= 1;
            //printf("4 c[%d] = %d\n",j,c[j]);
              //scanf("%d",c1);
            while (c[j] & 16) {
               if (j >= edges) {
                  return (extent);
               }
               c[++j] <<= 1;
            }
         } else {
            am = angle[--j];
            c[j] = 1;
            //printf("3 c[%d] = %d\n",j,c[j]);
//            scanf("%d",c1);
            for (u = 0, i = 1; i <= am[0]; i++)
               u |= c[am[i]];
            forbidden[j] = u;
            //printf("forbidden[%d] : %d\n",j, forbidden[j]);
         }
   }
}

void record(col, power, ring, live)
long col[], power[], ring;
char *live;
{
   long i, somme;

   somme = 0;
   for(i = 1; i <= ring; i++){
      switch(col[i]){
         case 1:
         break;
         case 2:
         somme +=  power[i];
         break;
         case 4:
         somme += 2*power[i];
         break;
         case 8:
         somme += 3*power[i];

      }
   }
   // printf("code de coloration %d %d %d est %d\n",col[1],col[2],col[3],somme);
   live[somme] = 0;
}

// void
// record(col, power, ring, angle, live, p, bigno)
// long col[], power[], ring, angle[][5], *p, bigno;
// char *live;

// /* Given a colouring specified by a 1,2,4-valued function "col", it computes
//  * the corresponding number, checks if it is in live, and if so removes it. */

// {
//    long weight[9], colno, sum, i, min, max, w;

//    for (i = 1; i < 9; i++)
//       weight[i] = 0;
//    for (i = 1; i <= ring; i++) {
//       sum = 15 - col[angle[i][1]] - col[angle[i][2]] - col[angle[i][3]] - col[angle[i][4]] - col[angle[i][5]] - col[angle[i][6]];
//       weight[sum] += power[i];
//    }
//    min = max = weight[8];
//    for (i = 1; i <= 2; i++) {
//       w = weight[i];
//       if (w < min)
//          min = w;
//       else if (w > max)
//          max = w;
//    }
//    colno = bigno - 2 * min - max;
//    if (live[colno]) {
//       (*p)++;
//       live[colno] = 0;
//    }
// }

long Lecture(A, F, C)
tp_confmat A;
FILE *F;
long *C;
{
   char *t, name[256], S[256];
   long i, n, ring, j;

   name[0]='\0';
   t=name;
   while (*t == '\0' || *t == '\n') {
      if (fgets(name, sizeof(name), F) == NULL)
        return ((long) 1);
      for (t = name; *t == ' ' || *t == '\t'; t++); /* On saute tous les espaces */
   }
   /*ligne que nous donne le nombre de sommets, le nombre de sommets exterieures
    et le nombre des aretes contractes*/ 
   (void) fgets(S, sizeof(S), F);
   sscanf(S, "%ld%ld%ld", &A[0][0], &A[0][1], &A[0][2]);
   n=A[0][0];
   ring=A[0][1];
   //contract=A[0][2];
   (void) fgets(S, sizeof(S), F);
   sscanf(S, "%ld%ld%ld%ld%ld%ld%ld%ld%ld", &A[0][4], &A[0][5], &A[0][6], &A[0][7], &A[0][8], &A[0][9], &A[0][10], &A[0][11], &A[0][12]);

   //On se demande juste si on depasse les limites
   if(n > VERTS){
      printf("%s depasse le nomnbre de sommets max\n", name);
      exit(2);
   }

   /* On lit la liste d'adjacence */
   for(i = 1; i <= n; i++){
      fgets(S, sizeof(S), F);
      sscanf(S, "%ld%ld", &j, &A[i][0]);
      if(A[i][0] > DEG){
         printf("%s depasse le degre maximum\n", name);
         exit(3);
      }
      for (t = S; *t < '0' || *t > '9'; t++);
      for (; *t >= '0' && *t <= '9'; t++);
      for (; *t < '0' || *t > '9'; t++);
      for (; *t >= '0' && *t <= '9'; t++);
      for (j = 1; j <= A[i][0]; j++) {
         sscanf(t, "%ld", &A[i][j]);
         for (; *t < '0' || *t > '9'; t++);
         for (; *t >= '0' && *t <= '9'; t++);
      }
   }
   return (0);
}/*Fin de la lecture */ 

