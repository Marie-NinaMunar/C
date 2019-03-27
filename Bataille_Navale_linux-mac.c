#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

// Dans l'affichage, il y aura deux modes :
    // 0 -> Le mode visee ou le joueur voit ou il a deja frappe et le resultat de ses frappes (pour j1, vue plateauj1)
    // 1 -> Le mode bataillon ou le joueur voit ou se situe ses propres bateaux, et si ils sont touches ou non (pour j1, vue plateau j2)

// Les valeurs du plateau :
    // -1 -> Il n'y a rien
    // 0 -> Bateau non decouvert
    // 1 -> Dans l'eau
    // 2 -> Touche
        // Dans le mode visee, -1 et 0 sont affiches de la meme maniere
        // Dans le mode bataillon, 1 et -1 sont affiches de la meme maniere

typedef struct bataillon{
    //Chaque bateau fait une certaine taille (en cases) pour chacunes de ces cases, nous savons si la partie du bateau est abimee (1 si il est intact, 0 si la partie est touchee), sa position verticale et sa position horizontale
    int b0[5][3]; // Porte-avion
    int b1[4][3]; // Croiseur
    int b2[3][3]; // Contre-torpilleur
    int b3[3][3]; // Sous-marin
    int b4[2][3]; // Torpilleur
    int restants[5];// Chaque case correspond au bateau ci dessus, 1 signifie qu'il flotte, 0 qu'il est coule, quand tout est a 0, c'est perdu pour le joueur
    int type; // 0=ordinateur ; 1=humain (important pour les tours de j2)
}bataillon;

void initialiser(int plateau[11][11]);
// Permet de mettre le plateau en parametre a -1 (rien), sur chaque case et de mettre sur la colonne et la ligne 0 les valeurs correspondates (A-J, 1-10)

void afficher_plateau(int plateau[11][11], int mode);
// Permet d'afficher le plateau en parametre selon le mode (0=visee ; 1=bataillon)

void afficher_bataillon(bataillon bataillon);
// Permet d'afficher le bataillon en parametres et les donnees a son propos

void commencer_joueur(bataillon * bataillon, int plateau[11][11]);
// Permet au joueur humain de placer ses bateaux sur le plateau, en donnant le debut et l'orientation de chacun d'eux (pas de chevauchement ni de sortie de plateau)
// Permet a l'ordinateur de tirer au sort le placement de ses bateaux sur le plateau

void initialiser_positions(int plateau[11][11], bataillon * bataillon);
// Permet de placer les bateaux du bataillon en parametre sur le plateau en parametres

int tour(int touche, int joueur, int plateauj[11][11], int plateaue[11][11], bataillon * allies, bataillon * ennemis);
// Affiche en premier lieu si l'adversaire a touche ou coule un de vos bateaux, si c'est le cas, il affichera directement l'affichage bataillon
// Puis, menu permettant d'acceder au tour en cours et de faire les actions voulues(attaque, affichage visee, affichage bataillon, fin de tour)
// A la fin du tour, le programme clear l'ecran de la console pour que ce que l'autre a fait ne s'affiche pas
// Si le joueur en cours est l'ordinateur, rien ne s'affiche mais l'ordinateur choisi un endroit a viser selon son intelligence artificielle
// Renvoie 0 si le tir arrive dans l'eau, 1 si le tir touche, 2 si le tir coule, 3 si le joueur en cours gagne la partie

int test_touche(int plateau[11][11], int vertical, int horizontal, bataillon * ennemi);
// La fonction teste si l'ennemi est touche
// Renvoie 0 si le tir arrive dans l'eau, 1 si le tir touche, 2 si le tir coule
// Ajuste les valeurs du bataillon en fonction
// Place dans le plateau en parametre le bon marqueur au bon endroit

void intelligence_artificielle(int plateau[11][11], int choix[2]);
// Fonction la plus compliquee ( A FAIRE EN DERNIER )
// Ici, version simplifiee ou l'ordinateur choisit sans tenir compte des bateaux deja touches
// Fait intervenir le hasard
// Modifie le tableau choix pour y placer son choix final, en vertical et en horizontal

void saisie_tir(int plateau[11][11], int choix[2]);
// Fais saisir au joueur les coordonnees de sa cible
// Le previens si il a deja tire a cet endroit
// L'empeche de tirer hors des limites de la carte

int verif_win(bataillon bataillon);
// Verifie si le bataillon en parametre est totalement coule, si oui renvoie 1, sinon renvoie 0

int empiete(int bateau1[][3], int bateau2[][3], int largeur1, int largeur2);
// Verifie si le bateau1 et le bateau2 du bataillon en parametre empietent l'un sur l'autre (si ils ont des cases en commun
// Renvoie 1 si les bateaux s'empietent, 0 sinon


int main(){
    srand(time(NULL));
    int plateauj1[11][11]; // La bataille navale se joue sur un tableau de A a J verticalement et de 1 a 10 horizontalement la colonne et la ligne 0 permette d'afficher le nom de la colonne et le numero de la ligne
    int plateauj2[11][11]; // On a un plateau pour le joueur 1, sur lequel seront caches les bateaux du joueur 2, et inversement
    bataillon joueur1;
    bataillon joueur2;
    int type_j2; // 0=Le joueur 2 est l'ordinateur | 1=Le joueur 2 est un humain| On considere que j2 est l'ordinateur dans les parties contre l'ia
    int joueur;
    int attente=0;

    do {
        printf("Voulez-vous jouer a 2 joueurs ou contre l'IA ? (0=ordinateur, 1=humain, 2=regles du jeu) ");
        scanf("%d", &type_j2);
        if ((type_j2 != 0) && (type_j2 != 1) && (type_j2 != 2)) printf("Incorrect !\n");
        else if(type_j2==2){
            printf("\n\n    Pour jouer a la bataille navale, il faut par joueur :\nUne grille de jeu numerotee de 1 a 10 horizontalement et de A a J verticalement\n1 porte avion (5 cases)\n1 croiseur (4 cases)\n1 contre torpilleur (3 cases)\n1 sous-marin (3 cases)\n1 torpilleur (2 cases)\n\n");
            printf("    Commencer une partie de bataille navale :\nAu debut du jeu, chaque joueur place a sa guise tous les bateaux sur sa grille de facon strategique. Le but etant de compliquer au maximum la tache de son adversaire, c'est-a-dire detruire tous vos navires. Bien entendu, le joueur ne voit pas la grille de son adversaire.\nUne fois tous les bateaux en jeu, la partie peut commencer.. Un a un, les joueurs se tire dessus pour detruire les navires ennemis.\n\n");
            printf("    Differents cas possibles : \nSi un joueur tire sur un navire ennemi, l'adversaire doit le signaler en disant \" touche \". Il peut pas jouer deux fois de suite et doit attendre le tour de l'autre joueur.\nSi le joueur ne touche pas de navire, l'adversaire le signale en disant \" rate \" .\nSi le navire est entierement touche l'adversaire doit dire \" touche coule \".\n\n");
            printf("    Comment gagner une partie de bataille navale : \n Une partie de bataille navale se termine lorsque l'un des joueurs n'a plus de navires.\n\n");
            printf("                    // Les regles proviennent du site : https://www.regles-de-jeux.com/regle-de-la-bataille-navale/ \n\n");
        }
    }while((type_j2 != 0) && (type_j2 != 1));

    joueur1.type=1;
    joueur2.type=type_j2;

    printf("Chacun votre tour, vous devez placer vos bateaux.\n");
    printf("Joueur 1, placez vous devant l'ecran.\n");
    commencer_joueur(&joueur1, plateauj2);
    system("clear");
    printf("C'est au tour du Joueur 2 !\n");
    commencer_joueur(&joueur2, plateauj1);
    system("clear");

    joueur=rand()%2+1;
    printf("Le joueur %d commence ! (Saisissez max 1 caractere pour continuer)\n", joueur);
    scanf("%d", &attente);

    int touche=-1;

    do {
        if(joueur==1) touche=tour(touche, joueur, plateauj1, plateauj2, &joueur1, &joueur2);
        else touche=tour(touche, joueur, plateauj2, plateauj1, &joueur2, &joueur1);
        joueur=3-joueur;
    }while(touche!=-1);

    printf("Fin ! \n");
    return 0;
}

void initialiser(int plateau[11][11]) {
// Permet de mettre le plateau en parametre a -1 (rien), sur chaque case et de mettre sur la colonne et la ligne 0 les valeurs correspondates (A-J, 1-10)
    plateau[0][0] = ' ' ;//On met le caractere ' ' sur la premiere case du plateau, elle sera inutile
    for(int alpha=1; alpha<11; alpha++) plateau[0][alpha]=alpha+'A' -1;   //On met les valeurs ascii de A jusqu'a J sur la premiere ligne
    for(int num=1; num<11; num++) plateau[num][0]=num;    //On met les chiffres de 1 a 10 sur la premiere colonne
    for(int i=1; i<11; i++)
        for(int j=1; j<11; j++)
            plateau[i][j]=-1;
}

void afficher_plateau(int plateau[11][11], int mode) {
// Permet d'afficher le plateau en parametre selon le mode (0=visee ; 1=bataillon)
    printf(" %c ", plateau[0][0]);
    for(int alpha=1; alpha<11; alpha++) printf(" %c ", plateau[0][alpha]);
    printf("\n");
    for(int i=1; i<11; i++){
        for(int j=0; j<11; j++){
            if (j){
                if (mode){
                switch(plateau[i][j]){
                    case -1 : printf(" : "); break; //De l'eau
                    case 1 : printf(" * "); break; //Un tir dans l'eau
                    case 0 : printf(" o "); break; //Un bateau intact
                    case 2 : printf(" x "); break; //Un bateau endommage
                    default : printf(" e "); //Uner erreur
                    }
                }
            else{
                switch(plateau[i][j]){
                    case -1 :
                    case 0 : printf(" : "); break; //De l'eau
                    case 1 : printf(" * "); break; //Un tir echoue
                    case 2 : printf(" x "); break; //Un tir reussi
                    default : printf(" e "); //Une erreur
                    }
                }
            }
            else{
                if (plateau[i][j]!=10) printf(" ");
                printf("%d ", plateau[i][j]);
            }
        }
        printf("\n");
    }
}

void afficher_bataillon(bataillon bataillon) {
// Permet d'afficher le bataillon en parametres et les donnees a son propos
    int b0_etat =0;
    int b1_etat =0;
    int b2_etat =0;
    int b3_etat =0;
    int b4_etat =0;

    printf("Porte-avion :\n");
    for(int i0=0; i0<5; i0++){
        if (bataillon.b0[i0][0]){
            printf(" O ");
            b0_etat++;
        }
        else printf(" X ");
    }
    if(b0_etat) printf("      %d parties touchees !\n", 5-b0_etat);
    else printf("     COULE !\n");
    printf("\n");

    printf("Croiseur :\n");
    for(int i1=0; i1<4; i1++){
        if (bataillon.b1[i1][0]){
            printf(" O ");
            b1_etat++;
        }
        else printf(" X ");
    }
    if(b1_etat) printf("         %d parties touchees !\n", 4-b1_etat);
    else printf("        COULE !\n");
    printf("\n");

    printf("Contre-torpilleur :\n");
    for(int i2=0; i2<3; i2++){
        if (bataillon.b2[i2][0]){
            printf(" O ");
            b2_etat++;
        }
        else printf(" X ");
    }
    if(b2_etat) printf("            %d parties touchees !\n", 3-b2_etat);
    else printf("           COULE !\n");
    printf("\n");

    printf("Sous-marin :\n");
    for(int i3=0; i3<3; i3++){
        if (bataillon.b3[i3][0]){
            printf(" O ");
            b3_etat++;
        }
        else printf(" X ");
    }
    if(b3_etat) printf("            %d parties touchees !\n", 3-b3_etat);
    else printf("           COULE !\n");
    printf("\n");

    printf("Torpilleur :\n");
    for(int i4=0; i4<2; i4++){
        if (bataillon.b4[i4][0]){
            printf(" O ");
            b4_etat++;
        }
        else printf(" X ");
    }
    if(b4_etat) printf("               %d parties touchees !\n", 2-b4_etat);
    else printf("               COULE !\n");
    printf("\n");
    printf("\n");
}

void commencer_joueur(bataillon * bataillon, int plateau[11][11]) {
// Permet au joueur humain de placer ses bateaux sur le plateau, en donnant le debut et l'orientation de chacun d'eux (pas de chevauchement ni de sortie de plateau)
// Permet a l'ordinateur de tirer au sort le placement de ses bateaux sur le plateau
    int validation=1;
    int empietement;
    int verifligne=1;
    int verifcolonne=1;
    int bonsens=1;
    int ligne=0;
    char colonne=0;
    int sens=0;
    int vertical;
    int horizontal;
    do{
        initialiser(plateau);
        if(bataillon->type){
            afficher_plateau(plateau, 1);
            printf("Les prochaines reponses seront saisies sous la forme ColonneLigne:Orientation avec Colonne une lettre entre A et J, Ligne un nombre entre 1 et 10, et Orientation une valeur (0 pour  horizontale et 1 pour verticale) \n Les valeurs ligne et colonne correspondent aux emplacement de la premiere case du bateau sur la carte, l'orientation determine si le bateau sera dirige vers la droite ou le bas.\n\n");
        }
        else printf("L'ordinateur choisit ou placer ses bateaux...\n");
            // 1er bateau :
        do{
                if(bataillon->type){
                    printf("Premier bateau : Porte-Avion (5 cases) ");
                    scanf(" \n%c%d:%d", &colonne, &ligne, &sens);


                    if (colonne>='A' && colonne<='J') colonne -='A'-1;
                    else if (colonne>='a' && colonne<='j') colonne -='a'-1;
                    else verifcolonne=0;

                    if (ligne<1 || ligne >10) verifligne=0;

                    if (sens && ligne>6) bonsens=0;
                    if(!sens && colonne>6) bonsens=0;


                    if (!verifligne || !verifcolonne || !bonsens) printf("Placement impossible !\n");
                }
                else{
                    do{
                        ligne=rand()%10 +1;
                        colonne=rand()%10 +1;
                    }while (ligne>6 && colonne>6);
                    if(ligne>6) sens=0;
                    else if (colonne>6) sens=1;
                    else sens=rand()%2;
                }

        }while (!verifligne || !verifcolonne || !bonsens);

        if(!sens){
            for(horizontal=0; horizontal<5; horizontal++){
                bataillon->b0[horizontal][0]=1;
                bataillon->b0[horizontal][1]=ligne;
                bataillon->b0[horizontal][2]=colonne+horizontal;
            }
        }
        else{
            for(vertical=0; vertical<5; vertical++){
                bataillon->b0[vertical][0]=1;
                bataillon->b0[vertical][1]=ligne+vertical;
                bataillon->b0[vertical][2]=colonne;
            }
        }

        // 2e bateau:
        do{
            do{
                    empietement=0;
                    verifligne=1;
                    verifcolonne=1;
                    bonsens=1;
                    if(bataillon->type){
                    printf("Deuxieme bateau : Croiseur (4 cases) ");
                    scanf("\n%c%d:%d", &colonne, &ligne, &sens);

                    if (colonne>='A' && colonne<='J') colonne-='A'-1;
                    else if (colonne>='a' && colonne<='j') colonne -='a'-1;
                    else verifcolonne=0;

                    if (ligne<1 || ligne >10) verifligne=0;


                    if (sens && ligne>7) bonsens=0;
                    if(!sens && colonne>7) bonsens=0;

                    if (!verifligne || !verifcolonne || !bonsens) printf("Placement impossible !\n");
                    }
                    else{
                        do{
                            ligne=rand()%10 +1;
                            colonne=rand()%10 +1;
                        }while (ligne>7 && colonne>7);
                        if(ligne>7) sens=0;
                        else if (colonne>7) sens=1;
                        else sens=rand()%2;
                    }
                }while (!verifligne || !verifcolonne || !bonsens);

        if(!sens){
            for(horizontal=0; horizontal<4; horizontal++){
                bataillon->b1[horizontal][0]=1;
                bataillon->b1[horizontal][1]=ligne;
                bataillon->b1[horizontal][2]=colonne+horizontal;
            }
        }
        else{
            for(vertical=0; vertical<4; vertical++){
                bataillon->b1[vertical][0]=1;
                bataillon->b1[vertical][1]=ligne+vertical;
                bataillon->b1[vertical][2]=colonne;
            }
        }

            if(!empietement) empietement=empiete(bataillon->b0, bataillon->b1, 5, 4);
            if(empietement && bataillon->type) printf("Ce bateau est place au meme endroit qu'un autre !\n");
        }while (empietement);


        // 3e Bateau
        do{
             do{
                    empietement=0;
                    verifligne=1;
                    verifcolonne=1;
                    bonsens=1;
                    if(bataillon->type){
                    printf("Troisieme bateau : Contre-torpilleur (3 cases) ");
                    scanf("\n%c%d:%d", &colonne, &ligne, &sens);

                    if (colonne>='A' && colonne<='J') colonne-='A'-1;
                    else if (colonne>='a' && colonne<='j') colonne -='a'-1;
                    else verifcolonne=0;

                    if (ligne<1 || ligne >10) verifligne=0;


                    if (sens && ligne>8) bonsens=0;
                    if(!sens && colonne>8) bonsens=0;

                    if (!verifligne || !verifcolonne || !bonsens) printf("Placement impossible !\n");
                    }

                    else{
                        do{
                            ligne=rand()%10 +1;
                            colonne=rand()%10 +1;
                        }while (ligne>8 && colonne>8);
                        if(ligne>8) sens=0;
                        else if (colonne>8) sens=1;
                        else sens=rand()%2;
                    }
            }while (!verifligne || !verifcolonne || !bonsens);


        if(!sens){
            for(horizontal=0; horizontal<3; horizontal++){
                bataillon->b2[horizontal][0]=1;
                bataillon->b2[horizontal][1]=ligne;
                bataillon->b2[horizontal][2]=colonne+horizontal;
            }
        }
        else{
            for(vertical=0; vertical<3; vertical++){
                bataillon->b2[vertical][0]=1;
                bataillon->b2[vertical][1]=ligne+vertical;
                bataillon->b2[vertical][2]=colonne;
            }
        }


            if(!empietement) empietement=empiete(bataillon->b0, bataillon->b2, 5, 3);

            if(!empietement) empietement=empiete(bataillon->b1, bataillon->b2, 4, 3);


            if(empietement && bataillon->type) printf("Ce bateau est place au meme endroit qu'un autre !\n");
        }while(empietement);


        // 4e Bateau
        do{
            do{
                    empietement=0;
                    verifligne=1;
                    verifcolonne=1;
                    bonsens=1;
                    if(bataillon->type){
                    printf("Quatrieme bateau : Sous-marin (3 cases) ");
                    scanf("\n%c%d:%d", &colonne, &ligne, &sens);

                    if (colonne>='A' && colonne<='J') colonne-='A'-1;
                    else if (colonne>='a' && colonne<='j') colonne -='a'-1;
                    else verifcolonne=0;

                    if (ligne<1 || ligne >10) verifligne=0;

                    if (sens && ligne>8) bonsens=0;
                    if(!sens && colonne>8) bonsens=0;

                    if (!verifligne || !verifcolonne || !bonsens) printf("Placement impossible !\n");
                    }

                    else{
                        do{
                            ligne=rand()%10 +1;
                            colonne=rand()%10 +1;
                        }while (ligne>8 && colonne>8);
                        if(ligne>8) sens=0;
                        else if (colonne>8) sens=1;
                        else sens=rand()%2;
                    }
            }while (!verifligne || !verifcolonne || !bonsens);

         if(!sens){
            for(horizontal=0; horizontal<3; horizontal++){
                bataillon->b3[horizontal][0]=1;
                bataillon->b3[horizontal][1]=ligne;
                bataillon->b3[horizontal][2]=colonne+horizontal;
            }
        }
        else{
            for(vertical=0; vertical<3; vertical++){
                bataillon->b3[vertical][0]=1;
                bataillon->b3[vertical][1]=ligne+vertical;
                bataillon->b3[vertical][2]=colonne;
            }
        }
        if(!empietement) empietement=empiete(bataillon->b0, bataillon->b3, 5, 3);

            if(!empietement) empietement=empiete(bataillon->b1, bataillon->b3, 4, 3);

            if(!empietement) empietement=empiete(bataillon->b2, bataillon->b3, 3, 3);




            if(empietement && bataillon->type) printf("Ce bateau est place au meme endroit qu'un autre !\n");
        }while(empietement);

        // 5e Bateau
        do{
            do{
                    empietement=0;
                    verifligne=1;
                    verifcolonne=1;
                    bonsens=1;
                    if(bataillon->type){
                    printf("Cinquieme bateau : Torpilleur (2 cases) ");
                    scanf("\n%c%d:%d", &colonne, &ligne, &sens);

                    if (colonne>='A' && colonne<='J') colonne-='A'-1;
                    else if (colonne>='a' && colonne<='j') colonne -='a'-1;
                    else verifcolonne=0;

                    if (ligne<1 || ligne >10) verifligne=0;


                    if (sens && ligne>9) bonsens=0;
                    if(!sens && colonne>9) bonsens=0;

                    if (!verifligne || !verifcolonne || !bonsens) printf("Placement impossible !\n");
                    }

                    else{
                        do{
                            ligne=rand()%10 +1;
                            colonne=rand()%10 +1;
                        }while (ligne>9 && colonne>9);
                        if(ligne>9) sens=0;
                        else if (colonne>9) sens=1;
                        else sens=rand()%2;
                    }
            }while (!verifligne || !verifcolonne || !bonsens);


        if(!sens){
            for(horizontal=0; horizontal<2; horizontal++){
                bataillon->b4[horizontal][0]=1;
                bataillon->b4[horizontal][1]=ligne;
                bataillon->b4[horizontal][2]=colonne+horizontal;
            }
        }
        else{
            for(vertical=0; vertical<2; vertical++){
                bataillon->b4[vertical][0]=1;
                bataillon->b4[vertical][1]=ligne+vertical;
                bataillon->b4[vertical][2]=colonne;
            }
        }
         if(!empietement) empietement=empiete(bataillon->b0, bataillon->b4, 5, 2);

            if(!empietement) empietement=empiete(bataillon->b1, bataillon->b4, 4, 2);

            if(!empietement) empietement=empiete(bataillon->b2, bataillon->b4, 3, 2);

            if(!empietement) empietement=empiete(bataillon->b3, bataillon->b4, 3, 2);


            if(empietement && bataillon->type) printf("Ce bateau est place au meme endroit qu'un autre !\n");
        }while(empietement);

        initialiser_positions(plateau, bataillon);
        if(bataillon->type){
            printf("Vos bateaux seront places ainsi : \n\n");
            afficher_plateau(plateau, 1);
            printf("Est ce correct  ? (oui = 1, non = 0) ");
            scanf("%d", &validation);
        }
    }while(!validation);
    for(int i=0; i<4; i++) bataillon->restants[i]=1; // Fais en sorte que tout les bateaux soit consideres non coules
}

void initialiser_positions(int plateau[11][11], bataillon * bataillon) {
// Permet de placer les bateaux du bataillon en parametre sur le plateau en parametres
    int emplacement;
    //1er bateau
    for(emplacement=0; emplacement<5; emplacement++)
        plateau[bataillon->b0[emplacement][1]][bataillon->b0[emplacement][2]]=0;
    //2e bateau
    for(emplacement=0; emplacement<4; emplacement++)
        plateau[bataillon->b1[emplacement][1]][bataillon->b1[emplacement][2]]=0;
    //3e bateau
    for(emplacement=0; emplacement<3; emplacement++)
        plateau[bataillon->b2[emplacement][1]][bataillon->b2[emplacement][2]]=0;
    //4e bateau
    for(emplacement=0; emplacement<3; emplacement++)
        plateau[bataillon->b3[emplacement][1]][bataillon->b3[emplacement][2]]=0;
    //5e bateau
    for(emplacement=0; emplacement<2; emplacement++)
        plateau[bataillon->b4[emplacement][1]][bataillon->b4[emplacement][2]]=0;
}

int tour(int touche, int joueur, int plateauj[11][11], int plateaue[11][11], bataillon * allies, bataillon * ennemis) {
// Si touche vaut 3, le programme annonce au joueur qu'il a perdu, tous ses bateaux sont coules
// Affiche en premier lieu si l'adversaire a touche ou coule un de vos bateau, si c'est le cas, il affichera directement l'affichage bataillon
// Puis, menu permettant d'acceder au tour en cours et de faire les actions voulues(attaque, affichage visee, affichage bataillon, fin de tour)
// A la fin du tour, le programme clear l'ecran de la console pour que ce que l'autre a fait ne s'affiche pas
// Si le joueur en cours est l'ordinateur, rien ne s'affiche mais l'ordinateur choisi un endroit a viser selon son intelligence artificielle
// Renvoie 0 si le tir arrive dans l'eau, 1 si le tir touche, 2 si le tir coule, 3 si le joueur en cours gagne la partie
    int ennemi=3-joueur;
    int tir[2]; //tir[0]=vertical | tir[1]=horizontal
    tir[0]=0;
    tir[1]=0;
    int resultat=-1;
    int sortir=0;
    int menu=0;
    int attente=0;
    printf("-----------------------TOUR DU JOUEUR %d-----------------------\n", joueur);
    if(allies->type){
        switch(touche){
            case 0: printf("LE JOUEUR %d  A RATE !\n\n\n", ennemi); break;
            case 1: printf("JOUEUR %d TOUCHE !\n\n\n", ennemi); afficher_bataillon(*allies); break;
            case 2: printf("JOUEUR %d TOUCHE ET COULE !\n\n\n", ennemi); afficher_bataillon(*allies); break;
            case 3: printf("JOUEUR %d A GAGNE !!!\n\n\n", ennemi); return resultat; break;
        }
        do{
            printf("\n\n\n\n\n                      Menu du tour : \n\n");
            printf("Affichage pour la visee.....................1\n\n");
            printf("Affichage pour du plateau allie........2\n\n");
            printf("Affichage des bateaux allies.............3\n\n");
            printf("Tirer...................................................4\n\n");
            printf("Fin du tour.........................................5\n\n");
            scanf("\n%d", &menu);
            switch(menu){
                case 1 : afficher_plateau(plateauj, 0); break;
                case 2 : afficher_plateau(plateaue, 1); break;
                case 3 : afficher_bataillon(*allies); break;
                case 4 : saisie_tir(plateauj, tir); break;
                default : printf("Voulez-vous vraiment terminer le tour ? (oui=1, non=0) "); scanf("\n%d", &sortir); if(!tir[0] && !tir[1]){ sortir=0; printf("Vous n'avez pas rentre de cible !\n");}; break;
            }
        }while(!sortir);
    }
    else{
        if(touche==3) return resultat;
        else{
            intelligence_artificielle(plateauj, tir);
        }
    }
    resultat=test_touche(plateauj, tir[1], tir[0], ennemis);
    resultat+=verif_win(*ennemis);
    if(allies->type){
        switch(resultat){
                case 0: printf("\n\n\nRATE !\n\n\n"); break;
                case 1: printf("\n\n\nTOUCHE !\n\n\n"); break;
                case 2: printf("\n\n\nTOUCHE ET COULE !\n\n\n"); break;
                case 3: printf("\n\n\nVOUS AVEZ GAGNE !!!\n\n\n"); break;
        }
    }
    printf("Tour termine. (Saisir 1 caractere max pour passer au tour de l'adversaire)\n");
    scanf("%d", &attente);
    system("clear");
    return resultat;
}

int test_touche(int plateau[11][11], int vertical, int horizontal, bataillon * ennemi) {
// La fonction teste si l'ennemi est touche
// Renvoie 0 si le tir arrive dans l'eau, 1 si le tir touche, 2 si le tir coule
// Ajuste les valeurs du bataillon en fonction
// Place dans le plateau en parametre le bon marqueur au bon endroit
    int touche=0;
    int verifcoule=1;
    int bateau=0;
    int continue_recherche=1;
    int emplacement;
    //Pour chaque bateaux, on verifie si il est touche
    //Bateau 1
    for(emplacement =0; emplacement<5 && continue_recherche; emplacement++){
        if(ennemi->b0[emplacement][1]==vertical){
            if(ennemi->b0[emplacement][2]==horizontal){
                continue_recherche=0;
                touche=1;
                ennemi->b0[emplacement][0]=0;
            }
        }
    }
    //Bateau 2
    if(continue_recherche){
        bateau++;
        for(emplacement =0; emplacement<4 && continue_recherche; emplacement++){
        if(ennemi->b1[emplacement][1]==vertical){
            if(ennemi->b1[emplacement][2]==horizontal){
                continue_recherche=0;
                touche=1;
                ennemi->b1[emplacement][0]=0;
                }
            }
        }
    }
    //Bateau 3
    if(continue_recherche){
        bateau++;
        for(emplacement =0; emplacement<3 && continue_recherche; emplacement++){
        if(ennemi->b2[emplacement][1]==vertical){
            if(ennemi->b2[emplacement][2]==horizontal){
                continue_recherche=0;
                touche=1;
                ennemi->b2[emplacement][0]=0;
                }
            }
        }
    }
    //Bateau 4
    if(continue_recherche){
        bateau++;
        for(emplacement =0; emplacement<3 && continue_recherche; emplacement++){
        if(ennemi->b3[emplacement][1]==vertical){
            if(ennemi->b3[emplacement][2]==horizontal){
                continue_recherche=0;
                touche=1;
                ennemi->b3[emplacement][0]=0;
                }
            }
        }
    }
    //Bateau 5
    if(continue_recherche){
        bateau++;
        for(emplacement =0; emplacement<2 && continue_recherche; emplacement++){
        if(ennemi->b4[emplacement][1]==vertical){
            if(ennemi->b4[emplacement][2]==horizontal){
                continue_recherche=0;
                touche=1;
                ennemi->b4[emplacement][0]=0;
                }
            }
        }
    }
    // Si un des bateaux est touche, on verifie si il est coule
    if (touche){
        switch(bateau){
            case 0: for(int b0_coule=0; b0_coule<5; b0_coule++) if(ennemi->b0[b0_coule][0]) verifcoule=0; break;
            case 1: for(int b1_coule=0; b1_coule<4; b1_coule++) if(ennemi->b1[b1_coule][0]) verifcoule=0; break;
            case 2: for(int b2_coule=0; b2_coule<3; b2_coule++) if(ennemi->b2[b2_coule][0]) verifcoule=0; break;
            case 3: for(int b3_coule=0; b3_coule<3; b3_coule++) if(ennemi->b3[b3_coule][0]) verifcoule=0; break;
            case 4: for(int b4_coule=0; b4_coule<2; b4_coule++) if(ennemi->b4[b4_coule][0]) verifcoule=0; break;
        }
        if (verifcoule) {
            touche=2;
            ennemi->restants[bateau]=0;
        }

    }
    //Ici, on modifie le plateau en fonction du resultat
    switch (touche) {
    case 2 :
    case 1 : plateau[vertical][horizontal]=2; break;
    case 0 : plateau[vertical][horizontal]=1; break;
    }

    return touche;
}

void intelligence_artificielle(int plateau[11][11], int choix[2]) {
// Fonction la plus compliquee ( A FAIRE EN DERNIER )
// Ici, version simplifiee ou l'ordinateur choisit sans tenir compte des bateaux deja touches
// Fait intervenir le hasard
// Modifie le tableau choix pour y placer son choix final, en vertical et en horizontal
    int colonne=0;
    int ligne=0;
    int verifdejafait=1;
        printf("L'ordinateur choisit sa cible...\n");
        do{
        verifdejafait=1;
        colonne=rand()%10+1;
        ligne=rand()%10+1;
        if(plateau[colonne][ligne] == 1 || plateau[colonne][ligne] == 2) {
                verifdejafait=0;
        }
    }while(!verifdejafait);
    choix[0]=colonne;
    choix[1]=ligne;
}

void saisie_tir(int plateau[11][11], int choix[2]){
// Fais saisir au joueur les coordonnees de sa cible
// Le previens si il a deja tire a cet endroit
// L'empeche de tirer hors des limites de la carte
    char colonne=0;
    int ligne=0;
    int verifligne=1;
    int verifcolonne=1;
    int verifdejafait=1;
        do{
        verifligne=1;
        verifcolonne=1;
        verifdejafait=1;
        printf("Ou voulez-vous tirer ? (format ColonneLigne) ");
        scanf("\n%c%d", &colonne, &ligne);
        if (colonne>='A' && colonne<='J') colonne -='A'-1;
        else if (colonne>='a' && colonne<='j') colonne -='a'-1;
        else {
            verifcolonne=0;
            printf("Colonne non dans le plateau !\n");
        }
        if (ligne<1 || ligne >10) {
                verifligne=0;
                printf("Ligne non dans le plateau !\n");
        }
        if(plateau[ligne][colonne] == 1 || plateau[ligne][colonne] == 2) {
                printf("Vous avez deja tire ici, voulez vous vraiment recommencer ? (oui=1, non=0) ");
                scanf("\n%d", &verifdejafait);
        }
    }while(!verifligne || !verifcolonne || !verifdejafait);
    choix[0]=colonne;
    choix[1]=ligne;
}

int verif_win(bataillon bataillon) {
// Verifie si le bataillon en parametre est totalement coule, si oui renvoie 1, sinon renvoie 0
    for(int i=0; i<5; i++)
        if(bataillon.restants[i]) return 0;
    return 1;
}

int empiete(int bateau1[][3], int bateau2[][3], int largeur1, int largeur2){
// Verifie si le bateau1 et le bateau2 du bataillon en parametre empietent l'un sur l'autre (si ils ont des cases en commun)
// Renvoie 1 si les bateaux s'empietent, 0 sinon
    for(int i=0; i<largeur1; i++){
        for(int j=0; j<largeur2; j++){
            if(bateau1[i][1]==bateau2[j][1] && bateau1[i][2]==bateau2[j][2]) return 1;
        }
    }
    return 0;
}
