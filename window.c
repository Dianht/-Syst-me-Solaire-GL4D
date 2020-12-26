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

/* inclusion des entêtes de fonctions de création et de gestion de
 * fenêtres système ouvrant un contexte favorable à GL4dummies. Cette
 * partie est dépendante de la bibliothèque SDL2 */
#include <GL4D/gl4duw_SDL2.h>

/* protos de fonctions locales (static) */
static void init(void);
static void draw(void);
static void key(int keycode);
static void sortie(void);
static void rotate_sun(float * m, float angle,float rayon);
/*!\brief un identifiant pour l'écran (de dessin) */
static GLuint _screenId = 0;

/*!\brief une surface représentant un quadrilatère */
static surface_t * _soleil = NULL;
static surface_t * _mercure = NULL;
static surface_t * _venus = NULL;
static surface_t * _terre = NULL;
static surface_t * _mars = NULL;
static surface_t * _lune = NULL;
static surface_t * _jupiter = NULL;
static surface_t * _saturne = NULL;
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
static float _ycam = 3.0f;
static float a = 0.0f;
static float angle = 0.0f;
static float angle_inv = 0.0f;
static int vue_soleil = 0;
static int vue_mercure = 0;
static int vue_venus = 0;
static int vue_terre = 0;
static int vue_mars = 0;
static int vue_jupiter = 0;
static int vue_saturne = 0;
static int vue_uranus = 0;
static int vue_neptune = 0;

static int _temps = 0;
static int vue_x = 80;
static int vue_y = 50;

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
  vec4 r = {1, 0, 0, 1}, g = {0, 1, 0, 1}, b = {0, 0, 1, 1},w = {1, 1, 1, 1};
  /* on créé nos trois type de surfaces */
 for(i = 0;i < 10;i++){
   id[i] = getTexFromBMP(planete_tex[i]);
 }
  _soleil = mkSphere(30, 30); 
  _soleil->dcolor = w; 
  setTexId(_soleil, id[0]);
  enableSurfaceOption(_soleil, SO_USE_TEXTURE);
  
  _mercure = mkSphere(30, 30); 
  _mercure->dcolor = w; 
  setTexId(_mercure, id[1]);
  enableSurfaceOption(_mercure, SO_USE_TEXTURE);
  enableSurfaceOption(_mercure, SO_USE_LIGHTING);

  _venus = mkSphere(30, 30); 
  _venus->dcolor = w; 
  setTexId(_venus, id[2]);
  enableSurfaceOption(_venus, SO_USE_TEXTURE);
  enableSurfaceOption(_venus, SO_USE_LIGHTING);


  _terre = mkSphere(30, 30); 
  _terre->dcolor = w; 
  setTexId(_terre, id[3]);
  enableSurfaceOption(_terre, SO_USE_TEXTURE);
  enableSurfaceOption(_terre, SO_USE_LIGHTING);
  
  _lune = mkSphere(30, 30); 
  _lune->dcolor = w; 
  setTexId(_lune, id[4]);
  enableSurfaceOption(_lune, SO_USE_TEXTURE);
  enableSurfaceOption(_lune, SO_USE_LIGHTING);

  _mars = mkSphere(30, 30); 
  _mars->dcolor = w; 
  setTexId(_mars, id[5]);
  enableSurfaceOption(_mars, SO_USE_TEXTURE);
  enableSurfaceOption(_mars, SO_USE_LIGHTING);

  _jupiter = mkSphere(30, 30); 
  _jupiter->dcolor = w; 
  setTexId(_jupiter, id[6]);
  enableSurfaceOption(_jupiter, SO_USE_TEXTURE);
  enableSurfaceOption(_jupiter, SO_USE_LIGHTING);

  _saturne = mkSphere(30, 30); 
  _saturne->dcolor = w; 
  setTexId(_saturne, id[7]);
  enableSurfaceOption(_saturne, SO_USE_TEXTURE);
  enableSurfaceOption(_saturne, SO_USE_LIGHTING);

  _uranus = mkSphere(30, 30); 
  _uranus->dcolor = w; 
  setTexId(_uranus, id[8]);
  enableSurfaceOption(_uranus, SO_USE_TEXTURE);
  enableSurfaceOption(_uranus, SO_USE_LIGHTING);
  
  _neptune = mkSphere(30, 30); 
  _neptune->dcolor = w; 
  setTexId(_neptune, id[9]);
  enableSurfaceOption(_neptune, SO_USE_TEXTURE);
  enableSurfaceOption(_neptune, SO_USE_LIGHTING);

  atexit(sortie);
}

/*!\brief la fonction appelée à chaque display. */
void draw(void) {
  float mvMat[16], projMat[16], nmv[16];
  static double t0 = 0, t, dt;
  static GLfloat av= 0;

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

  if(vue_soleil == 0){
    vue_x = 80;
    vue_y = 50;
    lookAt(mvMat, vue_x, vue_y, 0, 0, 0, 0, 0, 1, 0);
  }
   else{
    lookAt(mvMat, vue_x, vue_y, 0, 0, 0, 0, 0, 1, 0);
    if (vue_y != 0){
      vue_x -= 1;
      vue_y -= 1;
    }
  }
  /*
  if (vue_mercure  == 0){
    vue_x = 80;
    vue_y = 50;
    lookAt(mvMat, vue_x, vue_y, 0, 0, 0, 0, 0, 1, 0);
  }
  else{
    
    lookAt(mvMat, vue_x, 20, 10, 0, 0, 0, 0, 1, 0);
    }
  if(vue_venus  == 0){

  }else{

  }
  if(vue_terre == 0){

  }else{

  }
  if(vue_mars  == 0){

  }else{

  }
  if(vue_jupiter  == 0){
  
  }
  else{

  }
  if(vue_saturne  == 0){

  } 
    if(vue_soleil == 0 || vue_mercure || vue_venus || vue_terre || vue_mars || vue_jupiter || vue_saturne || vue_uranus || vue_neptune) {
    vue_x = 80;
    vue_y = 50;
    lookAt(mvMat, vue_x, vue_y, 0, 0, 0, 0, 0, 1, 0);
  } else if (vue_soleil == 1){
    lookAt(mvMat, vue_x, 50, 0, 0, 0, 0, 0, 1, 0);
    if (vue_y != 0){
      vue_x -= 1;
      vue_y -= 1;
    }
  */
  //Soleil//
  memcpy(nmv, mvMat, sizeof nmv); 
  scale(nmv,9.0f,11.0f,9.0f);
  rotate(nmv, a, 5.0f, 5.0f, 5.0f);
  transform_n_raster(_soleil, nmv, projMat);

  //Mercure//
  memcpy(nmv, mvMat, sizeof nmv); 
  rotate_sun(nmv,angle_inv,13.0f);
  rotate(nmv, a, 5.0f, 5.0f, 5.0f);
  transform_n_raster(_mercure, nmv, projMat); 

  //Venus//
  memcpy(nmv, mvMat, sizeof nmv); 
  scale(nmv,1.2f,1.5f,1.2f);
  rotate_sun(nmv,angle + 2,15.0f);
  rotate(nmv, a, 5.0f, 5.0f, 5.0f);
  transform_n_raster(_venus, nmv, projMat); 

  //Terre//
  memcpy(nmv, mvMat, sizeof nmv);
  scale(nmv,1.5f,1.8f,1.5f);
  rotate_sun(nmv,angle_inv - 1.5,18.0f);
  rotate(nmv, a, 5.0f, 5.0f, 5.0f);
  transform_n_raster(_terre, nmv, projMat);

  //Mars//
  memcpy(nmv, mvMat, sizeof nmv);
  scale(nmv,1.3f,1.6f,1.3f);
  rotate_sun(nmv,angle + 1,28.0f);
  rotate(nmv, a, 5.0f, 5.0f, 5.0f);
  transform_n_raster(_mars, nmv, projMat);


  //Jupiter//
  memcpy(nmv, mvMat, sizeof nmv);
  scale(nmv,6.0f,6.8f,6.0f);
  rotate_sun(nmv,angle_inv,8.0f);
  rotate(nmv, a, 5.0f, 5.0f, 5.0f);
  transform_n_raster(_jupiter, nmv, projMat);


  //Saturne//
  memcpy(nmv, mvMat, sizeof nmv);
  scale(nmv,5.0f,5.8f,5.0f);
  rotate_sun(nmv,angle + 7,15.0f);
  rotate(nmv, a, 5.0f, 5.0f, 5.0f);
  transform_n_raster(_saturne, nmv, projMat);


  //Uranus//
  memcpy(nmv, mvMat, sizeof nmv);
  scale(nmv,1.2f,1.5f,1.2f);
  rotate_sun(nmv,angle_inv - 1,50.0f);
  rotate(nmv, a, 5.0f, 5.0f, 5.0f);
  transform_n_raster(_uranus, nmv, projMat);

  //Neptune//
  memcpy(nmv, mvMat, sizeof nmv);
  scale(nmv,1.5f,1.8f,1.5f);
  rotate_sun(nmv,angle_inv + 0.3,70.0f);
  rotate(nmv, a, 5.0f, 5.0f, 5.0f);
  transform_n_raster(_neptune, nmv, projMat);

  /* déclarer qu'on a changé (en bas niveau) des pixels du screen  */
  gl4dpScreenHasChanged();
  /* fonction permettant de raffraîchir l'ensemble de la fenêtre*/
  gl4dpUpdateScreen(NULL);
  if (_temps == 0){
    a += 0.1f;
    angle += 0.01;
    angle_inv -= 0.01;
  } else{
    a += 1.0;
    angle += 0.10;
    angle_inv -= 0.10;
  }


}
void rotate_sun(float * m, float angle, float rayon) {
    float x, y;
    x = cos(angle) * rayon;
    y = sin(angle) * rayon;
    
    translate(m,y,0.0f,x);
}

/*!\brief intercepte l'événement clavier pour modifier les options. */
void key(int keycode) {
  switch(keycode) {
  case GL4DK_UP: /* 'v' changer de vue */
    _temps = !_temps;
  break;
  case GL4DK_v: /* 'v' changer de vue */
    vue_soleil = !vue_soleil;
    break;
    case GL4DK_b: /* 'v' changer de vue */
    vue_mercure = !vue_mercure;
    break;  
  default: break;
  }
  
}

/*!\brief à appeler à la sortie du programme. */
void sortie(void) {
  /* on libère nos trois surfaces */

  if(_soleil) {
    freeSurface(_soleil);
    freeSurface(_mercure);
    freeSurface(_venus);
    freeSurface(_terre);
    freeSurface(_mars);
    freeSurface(_jupiter);
    freeSurface(_saturne);
    freeSurface(_uranus);
    freeSurface(_neptune);
    freeSurface(_lune);
    _soleil = NULL;
    _mercure = NULL;
    _venus = NULL;
    _terre = NULL;
    _mars = NULL;
    _jupiter = NULL;
    _saturne = NULL;
    _uranus = NULL;
    _neptune = NULL;
    _lune = NULL;
  }
  /* libère tous les objets produits par GL4Dummies, ici
   * principalement les screen */
  gl4duClean(GL4DU_ALL);
}
