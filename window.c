/*!\file window.c
 * \brief Utilisation du raster "maison" pour finaliser le pipeline de
 * rendu 3D. Ici on peut voir les géométries disponibles.
 * \author Farès BELHADJ, amsi@up8.edu
 * \date December 4, 2020.
 * \todo pour les étudiant(e)s : changer la variation de l'angle de
 * rotation pour qu'il soit dépendant du temps et non du framerate
 */
#include <assert.h>

#include <stdio.h>

#include <math.h>

/* inclusion des entêtes de fonctions de gestion de primitives simples
 * de dessin. La lettre p signifie aussi bien primitive que
 * pédagogique. */
#include <GL4D/gl4dp.h>

/* inclure notre bibliothèque "maison" de rendu */
#include "moteur.h"

/* inclure notre pile "maison" */ #include "pile.h"

/* inclusion des entêtes de fonctions de création et de gestion de
 * fenêtres système ouvrant un contexte favorable à GL4dummies. Cette
 * partie est dépendante de la bibliothèque SDL2 */
#include <GL4D/gl4duw_SDL2.h>

//Nombre d'astres dans le Systeme solaire
#define NB_ASTRE 11
/* protos de fonctions locales (static) */
static void init(void);
static void draw(void);
static void key(int keycode);
static void collision(float coord_x[], float coord_y[]);
static void animation_vue(float x, float y, float z);
static void rotate_sun(float * m, float angle, float rayon, float centre_x, float centre_y, float * coord_x, float * coord_y);
static void sortie(void);

/*!\brief un identifiant pour l'écran (de dessin) */
static GLuint _screenId = 0;

/* brief des surfaces représentants une sphere */
static surface_t * _astres = NULL;
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
static surface_t * _orbite = NULL;

// On charge nos textures dans la variable
const char * astres_tex[NB_ASTRE] = {
  "images/Soleil.bmp",
  "images/Mercure.bmp",
  "images/Venus.bmp",
  "images/Terre.bmp",
  "images/Lune.bmp",
  "images/Mars.bmp",
  "images/Jupiter.bmp",
  "images/Saturne.bmp",
  "images/Disque_Saturne.bmp",
  "images/Uranus.bmp",
  "images/Neptune.bmp",

};

//L'angle pour la rotation autour de lui-même,autour du soleil et autour de la lune
static float a = 0.0f;
static float angle = 0.0f;
static float angle_lune = 0.0f;

//Variables qui permettront de placer les orbites à des coordonnées voulu (à savoir ceux des planetes)
//On l'initialise les coordonnées des orbites sur Mercure
static float orbite_r = 14.0f;
static float orbite_x = 3.0f;
static float orbite_y = 0.0f;

static int vue_orbite = 0;
//Variable qui permettra de pouvoir arreter les mouvements des rotations
static int stop = 0;
//Variable qui permettra en cas de collision d'arrêter la simulation
static int ok = 0;
//Variables qui va incrémenter les angles 
static float v_a = 0.1f;
static float v_angle = 0.01f;
static float v_lune = 0.02f;
//Variables qui permettront de changer le point de vue de la caméra
static int vue_x = 120.0f;
static int vue_y = 15.0f;
static int vue_z = 5.0f;
static int v_x = 120.0f;
static int v_y = 15.0f;
static int v_z = 5.0f;
//Tableau qui permet d'obtenir les coordonnée de chaque astres
static float coord_x[NB_ASTRE];
static float coord_y[NB_ASTRE];
//Tableau qui va octroier à chaque astre le rayon correspondant
const float rayon[NB_ASTRE] = {
  10.0f,
  0.8f,
  1.2f,
  1.5f,
  0.4f,
  1.3f,
  6.0f,
  5.0f,
  8.0f,
  1.2f,
  1.5f
};

/*!\brief paramètre l'application et lance la boucle infinie. */
int main(int argc, char ** argv) {
  /* tentative de création d'une fenêtre pour GL4Dummies */
  if (!gl4duwCreateWindow(argc, argv, /* args du programme */
      "Systeme Solaire par Dinath", /* titre */
      10, 10, 800, 600, /* x, y, largeur, heuteur */
      GL4DW_SHOWN) /* état visible */ ) {
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
  GLuint id[11];
  vec4 w = {
    1,
    1,
    1,
    1
  };

  //On utilice ici une pile afin d'eviter d'avoir 1000 lignes de codes
  for (i = 0; i < NB_ASTRE; i++) {
    //On recupere notre image d'astre
    id[i] = getTexFromBMP(astres_tex[i]);
    //On crée une sphere ou un disque pour saturne
    if (i == 8) _astres = mkDisk(30, 30);
    else _astres = mkSphere(30, 30);
    //On lui ajoute une couleur blanche pour que l'image ne soit pas teinté de couleur
    _astres -> dcolor = w;
    //On lui envoie notre image
    setTexId(_astres, id[i]);
    //On lui fait activer notre texture de planete et on active l'ombrage
    enableSurfaceOption(_astres, SO_USE_TEXTURE);
    enableSurfaceOption(_astres, SO_USE_LIGHTING);
    //On l'empile
    push(_astres);
  }
  _orbite = mkOrbite(30, 30);
  _orbite -> dcolor = w;

  //On dépile notre pile
  _neptune = pop();
  _uranus = pop();
  _dsaturne = pop();
  _saturne = pop();
  _jupiter = pop();
  _mars = pop();
  _lune = pop();
  _terre = pop();
  _venus = pop();
  _mercure = pop();
  _soleil = pop();

  //On libere notre surface
  atexit(sortie);
}

/*!\Arrete le programme lors d'une collision entre les astres*/
static void animation_vue(float x, float y, float z) {
  //On a là une animation de caméra
  //Lorsqu'une caméra va se placer devant une planete
  //En fonction de la position initial, la caméra va baisser ou augmenter 
  //petit à petit, la valeur de ses positions afin de crée une belle animation
  if (vue_y != y) {
    if (vue_y > y) vue_y -= 1.0f;
    else vue_y += 1.0f;
  }

  if (vue_x != x) {
    if (vue_x > x) vue_x -= 1.0f;
    else vue_x += 1.0f;
  }

  if (vue_z != z) {
    if (vue_z > z) vue_z -= 1.0f;
    else vue_z += 1.0f;
  }
}

/*!\Arrete le programme lors d'une collision entre les astres*/
void collision(float coord_x[], float coord_y[]) {
  float dx, dy, distance, somme_rayon;
  int i;
  for (i = 0; i < NB_ASTRE - 1; i++) {
    //On ignore Saturne, car c'est le disque de Saturne qui peut être en contact avec d'autre astres
    if (i == 6) i += 1;
    else {
      //Pour savoir si il y a une collision entre deux astres
      //On utilise la formule ci dessous
      dx = coord_x[i + 1] - coord_x[i];
      dy = coord_y[i + 1] - coord_y[i];
      distance = sqrtf(dx * dx + dy * dy);
      somme_rayon = rayon[i] + rayon[i + 1];
      //Si la distance entre les deux astres est inférieur à la somme des rayons des 
      //deux cercles, il y a donc collision
      if (distance < somme_rayon) {
        //On arrete le programme
        ok = 1;
      }
    }
  }
}

/*!\Permet de faire tourner les planetes autour du Soleil */
void rotate_sun(float * m, float angle, float rayon, float centre_x, float centre_y, float * coord_x, float * coord_y) {
  //En fonction de du rayon, l'angle et de la position voulu
  //Cette fonction va grâce à la formule trigonométrique nous donner les coordonnée d'un cercle
  //Et grâce aux coordonnées , on pourra faire translater les astres à ces coordonnées pour que les astres tournent autour du soleil ou autour de la Terre
  * coord_x = centre_x + cos(angle) * rayon;
  * coord_y = centre_y + sin(angle) * rayon;

  translate(m, * coord_y, 0.0f, * coord_x);
}

/*!\brief la fonction appelée à chaque display. */
void draw(void) {
  //On initialise nos variables
  float mvMat[16], projMat[16], nmv[16];
  coord_x[0] = 0;
  coord_y[0] = 0;
  if (ok == 0) {
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
    /* On initalise notre caméra */
    animation_vue(v_x, v_y, v_z);
    lookAt(mvMat, vue_x, vue_y, vue_z, 0, 0, 0, 0, 1, 0);
    //Lorsque l'utilisateur voudra voir les planete d'en haut, on affichera l'orbite d'un astre
    if (vue_orbite == 1) {
      //Orbite//
      memcpy(nmv, mvMat, sizeof nmv);
      translate(nmv, orbite_x, 0.0f, orbite_y);
      scale(nmv, orbite_r, 0.0f, orbite_r);
      transform_n_raster(_orbite, nmv, projMat);
    }

    /* On place nos planetes notre Soleil et notre Lune */
    //Soleil//
    memcpy(nmv, mvMat, sizeof nmv);
    scale(nmv, 10.0f, 12.0f, rayon[0]);
    rotate(nmv, a, 3.0f, 4.0f, 5.0f);
    transform_n_raster(_soleil, nmv, projMat);

    //Mercure//
    memcpy(nmv, mvMat, sizeof nmv);
    rotate_sun(nmv, angle + 20, 14.0f, 0.0f, 3.0f, & coord_x[1], & coord_y[1]);
    scale(nmv, 0.8f, 1.0f, rayon[1]);
    rotate(nmv, a, 0.0f, 0.1f, 0.0f);
    transform_n_raster(_mercure, nmv, projMat);

    //Venus//
    memcpy(nmv, mvMat, sizeof nmv);
    rotate_sun(nmv, angle + 2, 20.0f, -2.0f, -1.0f, & coord_x[2], & coord_y[2]);
    scale(nmv, 1.2f, 1.5f, rayon[2]);
    rotate(nmv, a, 5.0f, 1.0f, 3.0f);
    transform_n_raster(_venus, nmv, projMat);

    //Terre//
    memcpy(nmv, mvMat, sizeof nmv);
    rotate_sun(nmv, angle, 29.0f, 0, 0, & coord_x[3], & coord_y[3]);
    scale(nmv, 1.5f, 1.8f, rayon[3]);
    rotate(nmv, a, 2.0f, 6.0f, 2.0f);
    transform_n_raster(_terre, nmv, projMat);

    //Lune//
    memcpy(nmv, mvMat, sizeof nmv);
    rotate_sun(nmv, angle_lune, 3.0f, coord_x[3], coord_y[3], & coord_x[4], & coord_y[4]);
    scale(nmv, 0.4f, 0.5f, rayon[4]);
    rotate(nmv, a, 1.0f, 5.0f, 3.0f);
    transform_n_raster(_lune, nmv, projMat);

    //Mars//
    memcpy(nmv, mvMat, sizeof nmv);
    rotate_sun(nmv, angle + 1, 39.0f, 3, -3, & coord_x[5], & coord_y[5]);
    scale(nmv, 1.3f, 1.5f, rayon[5]);
    rotate(nmv, a, 2.0f, 2.0f, 5.0f);
    transform_n_raster(_mars, nmv, projMat);

    //Jupiter//
    memcpy(nmv, mvMat, sizeof nmv);
    rotate_sun(nmv, angle - 10, 49.0f, 0, 0, & coord_x[6], & coord_y[6]);
    scale(nmv, 6.0f, 6.8f, rayon[6]);
    rotate(nmv, a, 0.0f, 4.0f, 0.0f);
    transform_n_raster(_jupiter, nmv, projMat);

    //Saturne//
    memcpy(nmv, mvMat, sizeof nmv);
    rotate_sun(nmv, angle + 7, 66.0f, 0, 0, & coord_x[7], & coord_y[7]);
    scale(nmv, 5.0f, 5.9f, rayon[7]);
    rotate(nmv, a, 5.0f, 0.0f, 5.0f);
    transform_n_raster(_saturne, nmv, projMat);

    //Disque de Saturne
    memcpy(nmv, mvMat, sizeof nmv);
    rotate_sun(nmv, angle + 7, 66.0f, 0, 0, & coord_x[8], & coord_y[8]);
    scale(nmv, 8.0f, 0.0f, rayon[8]);
    rotate(nmv, a, 0.0f, 5.0f, 0.0f);
    transform_n_raster(_dsaturne, nmv, projMat);

    //Uranus//
    memcpy(nmv, mvMat, sizeof nmv);
    rotate_sun(nmv, angle - 1, 78.0f, 0, 0, & coord_x[9], & coord_y[9]);
    scale(nmv, 1.2f, 1.5f, rayon[9]);
    rotate(nmv, a, 4.0f, 5.0f, 7.0f);
    transform_n_raster(_uranus, nmv, projMat);

    //Neptune//
    memcpy(nmv, mvMat, sizeof nmv);
    rotate_sun(nmv, angle + 15, 86.0f, 0, 0, & coord_x[10], & coord_y[10]);
    scale(nmv, 1.5f, 1.8f, rayon[10]);
    rotate(nmv, a, 5.0f, 5.0f, 5.0f);
    transform_n_raster(_neptune, nmv, projMat);

    collision(coord_x, coord_y);

    //Lorsqu'on voudra placer la caméra devant un astre, on devra stopper les mouvements autour du soleil des
    //astres
    if (stop == 0) {
      a += v_a;
      angle += v_angle;
      angle_lune += v_lune;
    } else {
      a += v_a;
      angle = 0;
      angle_lune += v_lune;
    }
    /* déclarer qu'on a changé (en bas niveau) des pixels du screen  */
    gl4dpScreenHasChanged();
    /* fonction permettant de raffraîchir l'ensemble de la fenêtre*/
    gl4dpUpdateScreen(NULL);

  }
}

/*!\brief intercepte l'événement clavier pour modifier les options. */
void key(int keycode) {
  switch (keycode) {
    //Accelere le temps
  case GL4DK_KP_PLUS:
    v_a += 0.01f;
    v_angle += 0.001f;
    v_lune += 0.002f;
    break;
    //Decelere le temps
  case GL4DK_KP_MINUS:
    v_a -= 0.01f;
    v_angle -= 0.001f;
    v_lune -= 0.002f;
    break;
    //Pour pouvoir se rapprocher de 0 sur l'axe x
  case GL4DK_UP:
    v_x -= 1.0f;
    break;
    //Pour pouvoir se s'éloigner de 0 sur l'axe x
  case GL4DK_DOWN:
    v_x += 1.0f;
    break;
    //Pour pouvoir se déplacer à gauche en visant le Soleil
  case GL4DK_LEFT:
    v_z += 1.0f;
    break;
    //Pour pouvoir se déplacer à droite en visant le Soleil
  case GL4DK_RIGHT:
    v_z -= 1.0f;
    break;
    //Place la caméra devant le Soleil
  case GL4DK_a:
    stop = 1;
    vue_orbite = 0;
    v_x = 30.0f;
    v_y = 0.0f;
    v_z = 0.0f;
    break;
    //Place la caméra devant Mercure
  case GL4DK_z:
    stop = 1;
    vue_orbite = 0;
    v_x = 21.0f;
    v_y = 0.0f;
    v_z = 8.0f;
    break;
    //Place la caméra devant Venus
  case GL4DK_e:
    stop = 1;
    vue_orbite = 0;
    v_x = 20.0f;
    v_y = 0.0f;
    v_z = -12.0f;
    break;
    //Place la caméra devant la Terre et la Lune
  case GL4DK_r:
    stop = 1;
    vue_orbite = 0;
    v_x = -0.0f;
    v_y = 0.0f;
    v_z = 35.0f;
    break;
    //Place la caméra devant Mars
  case GL4DK_t:
    stop = 1;
    vue_orbite = 0;
    v_x = 33.0f;
    v_y = 0.0f;
    v_z = 27.0f;
    break;
    //Place la caméra devant Jupiter
  case GL4DK_q:
    stop = 1;
    vue_orbite = 0;
    v_x = 50.0f;
    v_y = 0.0f;
    v_z = -75.0f;
    break;
    //Place la caméra Saturne et le Disque de Saturne
  case GL4DK_s:
    stop = 1;
    vue_orbite = 0;
    v_x = 65.0f;
    v_y = 1.0f;
    v_z = 75.0f;
    break;
    //Place la caméra devant Uranus
  case GL4DK_d:
    stop = 1;
    vue_orbite = 0;
    v_x = -70.0f;
    v_y = 0.0f;
    v_z = 45.0f;
    break;
    //Place la caméra devant Neptune
  case GL4DK_f:
    stop = 1;
    vue_orbite = 0;
    v_x = 60.0f;
    v_y = 0.0f;
    v_z = -70.0f;
    break;
    //Pour pouvoir voir les orbites des Astres sur un plan orthogonal
  case GL4DK_v:
    stop = 0;
    vue_orbite = !vue_orbite;
    v_x = 3.0f;
    v_y = 190.0f;
    v_z = 0.0f;
    break;
    //Place la caméra sur un point de vue 
  case GL4DK_c:
    stop = 0;
    vue_orbite = 0;
    v_x = 120.0f;
    v_y = 15.0f;
    v_z = 5.0f;
    break;
    //Montre l'orbite de Mercure
  case GL4DK_KP_0:
    orbite_r = 14.0f;
    orbite_x = 3.0f;
    orbite_y = 0.0f;
    break;
    //Montre l'orbite de Venus
  case GL4DK_KP_1:
    orbite_r = 20.0f;
    orbite_x = -1.0f;
    orbite_y = -2.0f;
    break;
    //Montre l'orbite de la Terre
  case GL4DK_KP_2:
    orbite_r = 29.0f;
    orbite_x = 0.0f;
    orbite_y = 0.0f;
    break;
    //Montre l'orbite de Mars
  case GL4DK_KP_3:
    orbite_r = 39.0f;
    orbite_x = -3;
    orbite_y = 3;
    break;
    //Montre l'orbite de Jupiter
  case GL4DK_KP_4:
    orbite_r = 49.0f;
    orbite_x = 0;
    orbite_y = 0;
    break;
    //Montre l'orbite de Saturne
  case GL4DK_KP_5:
    orbite_r = 66.0f;
    orbite_x = 0;
    orbite_y = 0;
    break;
    //Montre l'orbite d'Uranus
  case GL4DK_KP_6:
    orbite_r = 78.0f;
    orbite_x = 0;
    orbite_y = 0;
    break;
    //Montre l'orbite de Neptune
  case GL4DK_KP_7:
    orbite_r = 86.0f;
    orbite_x = 0;
    orbite_y = 0;
    break;
  default:
    break;
  }
}
/*!\brief à appeler à la sortie du programme. */
void sortie(void) {
  /* on libère nos surfaces */

  if (_soleil) {
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
    freeSurface(_orbite);
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
    _orbite = NULL;
  }
  /* libère tous les objets produits par GL4Dummies, ici
   * principalement les screen */
  gl4duClean(GL4DU_ALL);
}
