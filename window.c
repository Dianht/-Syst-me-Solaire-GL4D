/*!\file window.c
 * \brief Utilisation du raster "maison" pour finaliser le pipeline de
 * rendu 3D. Ici on peut voir les géométries disponibles.
 * \author Farès BELHADJ, amsi@up8.edu
 * \date December 4, 2020.
 * \todo pour les étudiant(e)s : changer la variation de l'angle de
 * rotation pour qu'il soit dépendant du temps et non du framerate
 */
#include <assert.h>
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
static void sortie(void);

/*!\brief un identifiant pour l'écran (de dessin) */
static GLuint _screenId = 0;

/*!\brief une surface représentant un quadrilatère */
static surface_t * _soleil = NULL;
static surface_t * _mercure = NULL;
static surface_t * _venus = NULL;
static surface_t * _terre = NULL;
static surface_t * _mars = NULL;
static surface_t * _jupiter = NULL;
static surface_t * _saturne = NULL;
static surface_t * _uranus = NULL;
static surface_t * _neptune = NULL;
/*!\brief une surface représentant un cube */
const char * planete_tex[9] = {
  "images/Soleil.bmp",
  "images/Mercure.bmp",
  "images/Venus.bmp",
  "images/Terre.bmp",
  "images/Mars.bmp",
  "images/Jupiter.bmp",
  "images/Saturne.bmp",
  "images/Uranus.bmp",
  "images/Neptune.bmp",
};

/* des variable d'états pour activer/désactiver des options de rendu */
static int _use_tex = 1, _use_color = 1, _use_lighting = 1;

/*!\brief on peut bouger la caméra vers le haut et vers le bas avec cette variable */
static float _ycam = 0.0f;

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
 for(i = 0;i < 9;i++){
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

  _mars = mkSphere(30, 30); 
  _mars->dcolor = w; 
  setTexId(_mars, id[4]);
  enableSurfaceOption(_mars, SO_USE_TEXTURE);
  enableSurfaceOption(_mars, SO_USE_LIGHTING);

  _jupiter = mkSphere(30, 30); 
  _jupiter->dcolor = w; 
  setTexId(_jupiter, id[5]);
  enableSurfaceOption(_jupiter, SO_USE_TEXTURE);
  enableSurfaceOption(_jupiter, SO_USE_LIGHTING);

  _saturne = mkSphere(30, 30); 
  _saturne->dcolor = w; 
  setTexId(_saturne, id[6]);
  enableSurfaceOption(_saturne, SO_USE_TEXTURE);
  enableSurfaceOption(_saturne, SO_USE_LIGHTING);

  _uranus = mkSphere(30, 30); 
  _uranus->dcolor = w; 
  setTexId(_uranus, id[7]);
  enableSurfaceOption(_uranus, SO_USE_TEXTURE);
  enableSurfaceOption(_uranus, SO_USE_LIGHTING);
  
  _neptune = mkSphere(30, 30); 
  _neptune->dcolor = w; 
  setTexId(_neptune, id[8]);
  enableSurfaceOption(_neptune, SO_USE_TEXTURE);
  enableSurfaceOption(_neptune, SO_USE_LIGHTING);

  atexit(sortie);
}

/*!\brief la fonction appelée à chaque display. */
void draw(void) {
  static float a = 0.0f;
  float mvMat[16], projMat[16], nmv[16];
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
  lookAt(mvMat, 0, _ycam, 100, 10, 0, 0, 0, 1, 0);
  /* le quadrilatère est mis à gauche et tourne autour de son axe x */
  /* la sphère est laissée au centre et tourne autour de son axe y */
  memcpy(nmv, mvMat, sizeof nmv); 
  scale(nmv,5.0f,6.0f,5.0f);
  translate(nmv,0.0f,0.0f,0.0f);
  rotate(nmv, a, 0.0f, 5.0f, 0.0f);
  transform_n_raster(_soleil, nmv, projMat);
    /* le quadrilatère est mis à gauche et tourne autour de son axe x */
  /* la sphère est laissée au centre et tourne autour de son axe y */
  memcpy(nmv, mvMat, sizeof nmv); 
  translate(nmv,8.0f,0.0f,0.0f);
  rotate(nmv, a, 5.0f, 5.0f, 5.0f);
  transform_n_raster(_mercure, nmv, projMat);  /* le quadrilatère est mis à gauche et tourne autour de son axe x */
  /* la sphère est laissée au centre et tourne autour de son axe y */
  memcpy(nmv, mvMat, sizeof nmv); 
  scale(nmv,1.2f,1.5f,1.2f);
  translate(nmv,10.0f,0.0f,0.0f);
  rotate(nmv, a, 5.0f, 5.0f, 5.0f);
  transform_n_raster(_venus, nmv, projMat);  /* le quadrilatère est mis à gauche et tourne autour de son axe x */
  /* la sphère est laissée au centre et tourne autour de son axe y */
  memcpy(nmv, mvMat, sizeof nmv);
  scale(nmv,1.5f,1.8f,1.5f);
  translate(nmv,12.0f,0.0f,0.0f); 
  rotate(nmv, a, 5.0f, 5.0f, 5.0f);
  transform_n_raster(_terre, nmv, projMat);
    /* le quadrilatère est mis à gauche et tourne autour de son axe x */
  /* la sphère est laissée au centre et tourne autour de son axe y */
  memcpy(nmv, mvMat, sizeof nmv);
  scale(nmv,1.3f,1.6f,1.3f);
  translate(nmv,19.0f,0.0f,0.0f); 
  rotate(nmv, a, 5.0f, 5.0f, 5.0f);
  transform_n_raster(_mars, nmv, projMat);

  memcpy(nmv, mvMat, sizeof nmv);
  scale(nmv,3.0f,3.8f,3.0f);
  translate(nmv,11.0f,0.0f,0.0f); 
  rotate(nmv, a, 5.0f, 5.0f, 5.0f);
  transform_n_raster(_jupiter, nmv, projMat);

  memcpy(nmv, mvMat, sizeof nmv);
  scale(nmv,2.0f,2.8f,2.0f);
  translate(nmv,20.0f,0.0f,0.0f); 
  rotate(nmv, a, 5.0f, 5.0f, 5.0f);
  transform_n_raster(_saturne, nmv, projMat);

  memcpy(nmv, mvMat, sizeof nmv);
  scale(nmv,1.2f,1.5f,1.2f);
  translate(nmv,38.0f,0.0f,0.0f); 
  rotate(nmv, a, 5.0f, 5.0f, 5.0f);
  transform_n_raster(_uranus, nmv, projMat);

  memcpy(nmv, mvMat, sizeof nmv);
  scale(nmv,1.5f,1.8f,1.5f);
  translate(nmv,34.0f,0.0f,0.0f); 
  rotate(nmv, a, 5.0f, 5.0f, 5.0f);
  transform_n_raster(_neptune, nmv, projMat);

  /* déclarer qu'on a changé (en bas niveau) des pixels du screen  */
  gl4dpScreenHasChanged();
  /* fonction permettant de raffraîchir l'ensemble de la fenêtre*/
  gl4dpUpdateScreen(NULL);
  a += 0.1f;
}

/*!\brief à appeler à la sortie du programme. */
void sortie(void) {
  /* on libère nos trois surfaces */

  if(_soleil) {
    freeSurface(_soleil);
    _soleil = NULL;
  }
  /* libère tous les objets produits par GL4Dummies, ici
   * principalement les screen */
  gl4duClean(GL4DU_ALL);
}
