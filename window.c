/*!\file window.c
 * \brief Utilisation du raster "maison" pour finaliser le pipeline de
 * rendu 3D. Ici on peut voir les géométries disponibles.
 * \author Farès BELHADJ, amsi@up8.edu
 * \date December 4, 2020.
 * \todo pour les étudiant(e)s : changer la variation de l'angle de
 * rotation pour qu'il soit dépendant du temps et non du framerate
 */
#include <assert.h>
#include <math.h>
/* inclusion des entêtes de fonctions de gestion de primitives simples
 * de dessin. La lettre p signifie aussi bien primitive que
 * pédagogique. */
#include <GL4D/gl4dp.h>
/* inclure notre bibliothèque "maison" de rendu */
#include "moteur.h"
#include "pile.h"

/* inclusion des entêtes de fonctions de création et de gestion de
 * fenêtres système ouvrant un contexte favorable à GL4dummies. Cette
 * partie est dépendante de la bibliothèque SDL2 */
#include <GL4D/gl4duw_SDL2.h>

/* protos de fonctions locales (static) */
static void init(void);
static void draw(void);
static void key(int keycode);
static void stop(float angle_1,float angle_2, float angle_3);
static void animation_vue(float x, float y , float z);
static void camera(float * mvMat);
static void rotate_sun(float * m, float angle,float rayon, int x, int y);
static void init_vue(int planete);
static void sortie(void);
/*!\brief un identifiant pour l'écran (de dessin) */
static GLuint _screenId = 0;

/*!\brief une surface représentant un quadrilatère */
static surface_t * _planetes = NULL;
static surface_t * _soleil = NULL;
static surface_t * _mercure = NULL;
static surface_t * _venus = NULL;
static surface_t * _terre = NULL;
static surface_t * _mars = NULL;
static surface_t * _lune = NULL;
static surface_t * _jupiter = NULL;
static surface_t * _saturne = NULL;
static surface_t * _dsaturne = NULL;
static surface_t * _uranus = NULL;
static surface_t * _neptune = NULL;
/*!\brief une surface représentant un cube */
const char * planete_tex[10] = {
  "images/Soleil.bmp",
  "images/Mercure.bmp",
  "images/Venus.bmp",
  "images/Terre.bmp",
  "images/Lune.bmp",
  "images/Mars.bmp",
  "images/Jupiter.bmp",
  "images/Saturne.bmp",
  "images/Uranus.bmp",
  "images/Neptune.bmp",
  
};


/*!\brief on peut bouger la caméra vers le haut et vers le bas avec cette variable */
static float a = 0.0f;
static float angle = 0.0f;
static float angle_inv = 0.0f;
static float angle_lune = 0.0f;

static int vue_soleil = 0;
static int vue_mercure = 0;
static int vue_venus = 0;
static int vue_terre = 0;
static int vue_mars = 0;
static int vue_jupiter = 0;
static int vue_saturne = 0;
static int vue_uranus = 0;
static int vue_neptune = 0;

static int vitesse_r = 0;
static int vitesse_l = 0;
static int vue_x = 3.0f;
static int vue_y = 160.0f;
static int vue_z = 0.0f;

/*!\brief paramètre l'application et lance la boucle infinie. */
int main(int argc, char ** argv) {
  /* tentative de création d'une fenêtre pour GL4Dummies */
  if(!gl4duwCreateWindow(argc, argv, /* args du programme */
			 "Mon moteur de rendu <<Maison>>", /* titre */
			 10, 10, 800, 600, /* x, y, largeur, heuteur */
			 GL4DW_SHOWN) /* état visible */) {
    /* ici si échec de la création souvent lié à un problème d'absence
     * de contexte graphique ou d'impossibilité d'ouverture d'un
     * contexte OpenGL (au moins 3.2) */
    return 1;
  }
  /* Pour forcer la désactivation de la synchronisation verticale */
  SDL_GL_SetSwapInterval(0);
  init();
  /* création d'un screen GL4Dummies (texture dans laquelle nous
   * pouvons dessiner) aux dimensions de la fenêtre */
  _screenId = gl4dpInitScreen();
  /* mettre en place la fonction d'interception clavier */
    gl4duwKeyDownFunc(key);

  /* mettre en place la fonction de display */
  gl4duwDisplayFunc(draw);
  /* boucle infinie pour éviter que le programme ne s'arrête et ferme
   * la fenêtre immédiatement */
  gl4duwMainLoop();
  return 0;
}

/*!\brief init de nos données, spécialement les trois surfaces
 * utilisées dans ce code */
void init(void) {
  int i;
  GLuint id[10];
  vec4 w = {1, 1, 1, 1};

  //On utilice ici une pile afin d'eviter d'avoir 1000 lignes de codes
  for(i = 0;i < 10;i++){
    id[i] = getTexFromBMP(planete_tex[i]);
    _planetes = mkSphere(30, 30); 
    _planetes->dcolor = w; 
    setTexId(_planetes, id[i]);
    enableSurfaceOption(_planetes, SO_USE_TEXTURE);
    enableSurfaceOption(_planetes, SO_USE_LIGHTING);
    push(_planetes);
 }
  _planete = malloc(_nb_planete * sizeof * _planete);

  _neptune = pop(); 
  _uranus = pop(); 
  _saturne = pop(); 
  _jupiter = pop(); 
  _mars = pop(); 
  _lune = pop(); 
  _terre = pop(); 
  _venus = pop(); 
  _mercure = pop(); 
  _soleil = pop(); 

  _dsaturne = mkDisk(30, 30); 
  _dsaturne->dcolor = w; 
  setTexId(_dsaturne, id[7]);
  enableSurfaceOption(_dsaturne, SO_USE_TEXTURE);

  atexit(sortie);
}
void init_vue(int planete){
  vue_soleil = 0;
  vue_mercure = 0;
  vue_venus = 0;
  vue_terre = 0;
  vue_mars = 0;
  vue_jupiter = 0;
  vue_saturne = 0;
  vue_uranus = 0;
  vue_neptune = 0;
  switch(planete) {
    case 0:
      vue_soleil = 1;
      break;
    case 1:
      vue_mercure = 1;
      break;
    case 2:
      vue_venus = 1;
      break;
    case 3:
      vue_terre = 1;
      break;
    case 4:
      vue_mars = 1;
      break;
    case 5:
      vue_jupiter = 1;
      break;
    case 6:
      vue_saturne = 1;
      break;
    case 7:
      vue_uranus = 1;
      break;
    case 8:
      vue_neptune = 1;
      break;
  default: 
  break;
  }
   
}
void stop(float angle_1,float angle_2, float angle_3){
    angle = 0.0;
    angle_inv = 10.0;
}
static void animation_vue(float x, float y , float z){
  if (vue_y != y){
    if (vue_y > y){
      vue_y -= 1.0f;
    }
    else{
      vue_y += 1.0f;
    }
  }
  if (vue_x != x){
   if (vue_x > x){
      vue_x -= 1.0f;
    }
    else{
      vue_x += 1.0f;
    }
  }
  if (vue_z != z){
   if (vue_z > z){
      vue_z -= 1.0f;
    }
    else{
      vue_z += 1.0f;
    }
  }

}
void camera(float * mvMat){
  if((vue_soleil == 0) && (vue_mercure == 0) && (vue_venus == 0) && (vue_terre == 0) && (vue_mars == 0) && (vue_jupiter == 0) && (vue_saturne == 0) && (vue_uranus == 0)&& (vue_neptune == 0)){
    vue_x = 3.0f;
    vue_y = 190.0f;
    vue_z = 0.0f;
    lookAt(mvMat, vue_x, vue_y,vue_z, 0, 0, 0, 0, 1,0);
  }
  else if (vue_soleil == 1){
    init_vue(0);
    stop(0,10,0);
    lookAt(mvMat, vue_x, vue_y, vue_z, 0, 0, 0, 0, 1, 0);
    animation_vue(30.0f,0.0f,0.0f);
  }
  else if (vue_mercure  == 1 ){
    init_vue(1);
    stop(0,10,0);
    lookAt(mvMat, vue_x, vue_y, vue_z, 0, 0, 0, 0, 1, 0);
    animation_vue(13.0f,0.0f,-15.0f);
  }
  else if (vue_venus  == 1){
    init_vue(2);
    stop(0,10,0);
    lookAt(mvMat, vue_x, vue_y, vue_z, 0, 0, 0, 0, 1, 0);
    animation_vue(20.0f,0.0f,-12.0f);
  }
  else if (vue_terre == 1){
    init_vue(3);
    stop(0,10,0);
    lookAt(mvMat, vue_x, vue_y, vue_z, 0, 0, 0, 0, 1,0);
    animation_vue(-20.0f,0.0f,-31.0f);
  }
  else if (vue_mars == 1){
    init_vue(4);
    stop(0,10,0);
    lookAt(mvMat, vue_x, vue_y, vue_z, 0, 0, 0, 0, 1,0);
    animation_vue(29.0f,0.0f,26.0f);
  }
  else if (vue_jupiter == 1){
    init_vue(5);
    stop(0,10,0);
    lookAt(mvMat, vue_x, vue_y, vue_z, 0, 0, 0, 0, 1,0);
    animation_vue(0.0f,0.0f,70.0f);
  }
  else if (vue_saturne == 1){
    init_vue(6);
    stop(0,10,0);
    lookAt(mvMat, vue_x, vue_y, vue_z, 0, 0, 0, 0, 1,0);
    animation_vue(65.0f,1.0f,75.0f);
  }
  else if (vue_uranus == 1){
    init_vue(7);
    stop(0,10,0);
    lookAt(mvMat, vue_x, vue_y, vue_z, 0, 0, 0, 0, 1,0);
    animation_vue(34.0f,0.0f,-75.0f);
  }
  else if (vue_neptune == 1){
    init_vue(8);
    stop(0,10,0);
    lookAt(mvMat, vue_x, vue_y, vue_z, 0, 0, 0, 0, 1,0);
    animation_vue(-70.0f,0.0f,-59.0f);
  }
}
void rotate_sun(float * m, float angle, float rayon, int x_0, int y_0) {
    float x, y;
    x = x_0 + cos(angle) * rayon;
    y = y_0 + sin(angle) * rayon;
    
    translate(m,y,0.0f,x);
}

/*!\brief la fonction appelée à chaque display. */
void draw(void) {
  float mvMat[16], projMat[16], nmv[16];
  static float x,x_i,y_i, y;
    x = cos(angle_inv) * 29;
    y = sin(angle_inv) * 29;
    x_i = x  + cos(angle_lune) * 3 ;
    y_i = y  + sin(angle_lune) * 3 ;
  /* effacer l'écran et le buffer de profondeur */
  gl4dpClearScreen();
  clearDepth();
  /* des macros facilitant le travail avec des matrices et des
   * vecteurs se trouvent dans la bibliothèque GL4Dummies, dans le
   * fichier gl4dm.h */
  /* charger un frustum dans projMat */
  MFRUSTUM(projMat, -0.05f, 0.05f, -0.05f, 0.05f, 0.1f, 1000.0f);
  /* charger la matrice identité dans model-view */
  MIDENTITY(mvMat);
  /* on place la caméra en arrière-haut, elle regarde le centre de la scène */

  camera(mvMat);
  //Soleil//
  memcpy(nmv, mvMat, sizeof nmv); 
  scale(nmv,10.0f,12.0f,10.0f);
  rotate(nmv, a, 3.0f, 4.0f, 5.0f);
  transform_n_raster(_soleil, nmv, projMat);

  //Mercure//
  memcpy(nmv, mvMat, sizeof nmv); 
  rotate_sun(nmv,angle_inv - 20,15.0f,0,3);
  scale(nmv,0.8f,1.0f,0.8f);
  rotate(nmv, a, 5.0f, 5.0f, 5.0f);
  transform_n_raster(_mercure, nmv, projMat); 

  //Venus//
  memcpy(nmv, mvMat, sizeof nmv); 
  rotate_sun(nmv,angle + 2,20.0f,-2,-1);
  scale(nmv,1.2f,1.5f,1.2f);
  rotate(nmv, a, 5.0f, 1.0f, 3.0f);
  transform_n_raster(_venus, nmv, projMat); 

  //Terre//
  memcpy(nmv, mvMat, sizeof nmv);
  translate(nmv,y,0.0f,x);
  scale(nmv,1.5f,1.8f,1.5f);
  rotate(nmv, a, 5.0f, 6.0f, 2.0f);
  transform_n_raster(_terre, nmv, projMat);

  //Lune//
  memcpy(nmv, mvMat, sizeof nmv);
  translate(nmv,y_i,0.0f,x_i);
  scale(nmv,0.4f,0.5f,0.4f);
  rotate(nmv, a, 1.0f, 5.0f, 3.0f);
  transform_n_raster(_lune, nmv, projMat);


  //Mars//
  memcpy(nmv, mvMat, sizeof nmv);
  rotate_sun(nmv,angle + 1,35.0f,4,-4);
  scale(nmv,1.3f,1.5f,1.3f);
  rotate(nmv, a, 2.0f, 2.0f, 5.0f);
  transform_n_raster(_mars, nmv, projMat);


  //Jupiter//
  memcpy(nmv, mvMat, sizeof nmv);
  rotate_sun(nmv,angle_inv - 10,49.0f,0,0);
  scale(nmv,6.0f,6.8f,6.0f);
  rotate(nmv, a, 5.0f, 4.0f, 4.0f);
  transform_n_raster(_jupiter, nmv, projMat);


  //Saturne//
  memcpy(nmv, mvMat, sizeof nmv);
  rotate_sun(nmv,angle + 7,66.0f,0,0);
  scale(nmv,5.0f,5.9f,5.0f);
  rotate(nmv, a, 5.0f, 0.0f, 5.0f);
  transform_n_raster(_saturne, nmv, projMat);
  
  //Disque de Saturne
  memcpy(nmv, mvMat, sizeof nmv);
  rotate_sun(nmv,angle + 7,66.0f,0,0);
  scale(nmv,8.0f,0.0f,8.0f);
  rotate(nmv, a, 0.0f, 5.0f, 0.0f);
  transform_n_raster(_dsaturne, nmv, projMat);

  //Uranus//
  memcpy(nmv, mvMat, sizeof nmv);
  rotate_sun(nmv,angle_inv - 1,78.0f,0,0);
  scale(nmv,1.2f,1.5f,1.2f);
  rotate(nmv, a, 4.0f, 5.0f, 7.0f);
  transform_n_raster(_uranus, nmv, projMat);

  //Neptune//
  memcpy(nmv, mvMat, sizeof nmv);
  rotate_sun(nmv,angle_inv + 0.3,86.0f,0,0);
  scale(nmv,1.5f,1.8f,1.5f);
  rotate(nmv, a, 5.0f, 5.0f, 5.0f);
  transform_n_raster(_neptune, nmv, projMat);

  /* déclarer qu'on a changé (en bas niveau) des pixels du screen  */
  gl4dpScreenHasChanged();
  /* fonction permettant de raffraîchir l'ensemble de la fenêtre*/
  gl4dpUpdateScreen(NULL);
  if ((vitesse_r == 0) && (vitesse_l == 0)){
    a += 0.1f;
    angle += 0.01;
    angle_inv -= 0.01;
    angle_lune-= 0.02;
  } else if (vitesse_r == 1){
    a += 1.0;
    angle += 0.10;
    angle_inv -= 0.10;
    angle_lune-= 0.20;
  } else{
    a += 0.001;
    angle += 0.001;
    angle_inv -= 0.001;
    angle_lune-= 0.002;
  }


}

/*!\brief intercepte l'événement clavier pour modifier les options. */
void key(int keycode) {
  switch(keycode) {
  case GL4DK_UP:
   vitesse_r = !vitesse_r;
  break;
  case GL4DK_DOWN:
   vitesse_l = !vitesse_l;
  break;
  case GL4DK_a: 
    vue_soleil = !vue_soleil;
    break;
  case GL4DK_z: 
    vue_mercure = !vue_mercure;
    break;
  case GL4DK_e: 
    vue_venus = !vue_venus;
    break; 
  case GL4DK_r: 
    vue_terre = !vue_terre;
    break;  
  case GL4DK_t: 
    vue_mars = !vue_mars;
    break;
  case GL4DK_y: 
    vue_jupiter = !vue_jupiter;
    break;  
  case GL4DK_q: 
    vue_saturne = !vue_saturne;
    break;  
  case GL4DK_s: 
    vue_uranus = !vue_uranus;
    break;
  case GL4DK_d: 
    vue_neptune = !vue_neptune;
    break;  
  default: break;
  }
  
}

/*!\brief à appeler à la sortie du programme. */
void sortie(void) {
  /* on libère nos surfaces */

  if(_soleil) {
    freeSurface(_soleil);
    freeSurface(_mercure);
    freeSurface(_venus);
    freeSurface(_terre);
    freeSurface(_lune);
    freeSurface(_mars);
    freeSurface(_jupiter);
    freeSurface(_saturne);
    freeSurface(_dsaturne);
    freeSurface(_uranus);
    freeSurface(_neptune);
    _soleil = NULL;
    _mercure = NULL;
    _venus = NULL;
    _terre = NULL;
    _lune = NULL;
    _mars = NULL;
    _jupiter = NULL;
    _saturne = NULL;
    _dsaturne = NULL;
    _uranus = NULL;
    _neptune = NULL;
  }
  /* libère tous les objets produits par GL4Dummies, ici
   * principalement les screen */
  gl4duClean(GL4DU_ALL);
}
