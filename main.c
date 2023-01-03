#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#define TAILLE_DES_CHAR 15
#define ModeDeveloppeur true
#include <windows.h>

///Declaration des structures ----------------------------------------------------

struct TransfertInfoEtVisuel
{
    SDL_Window* fenetre;
    SDL_Renderer* rendu;
    SDL_Texture* textureimage;
};

struct Joueur
{
    char nom[TAILLE_DES_CHAR];
    int vote;
    bool couple ;
    int vie;

    char rolejoueur[TAILLE_DES_CHAR];
    bool maire;
    bool charmed;
    bool protected;

};

struct sorciere
{
    bool potion;
    bool poison;
    int mort;
};

///-------------------------------------------------------------------------------

///Fonctions impliquant la fenetre SDL et la console------------------------------

void SDL_FatalError(const char *messagederreur) //Fonction de Gestion d'Erreur SDL
{
    SDL_Log("ERREUR : <%s> %s \n", messagederreur, SDL_GetError());
    SDL_Quit;
    exit(EXIT_FAILURE);
}

SDL_Window* CreationDeFenetre() //Fonction de Création de la feunetre SDL
{
    SDL_Window *fenetre = NULL;
    #define largeur_fenetre 400
    #define hauteur_fenetre 400
    fenetre = SDL_CreateWindow("Loup Garou | Joan & Khiem |",
                               1000, 200, largeur_fenetre, hauteur_fenetre, SDL_WINDOW_BORDERLESS);


    if(fenetre == NULL)
        SDL_FatalError("FAIL a la création de la fenetre d'affichage des images");

    return fenetre;
}

SDL_Renderer* CreationRendu(SDL_Window* fenetre) //Fonction de Création du rendu de la feunetre SDL
{
   SDL_Renderer *rendu = NULL;
    rendu = SDL_CreateRenderer(fenetre, -1,
                               SDL_RENDERER_SOFTWARE);

     if(rendu == NULL)
        SDL_FatalError("FAIL à la création du rendu");
 return rendu;
}

SDL_Texture* CreationEtAffichageTexture(SDL_Window* fenetre, SDL_Renderer* rendu, char *emplacementimage) //Fonction de création de la Texture de la feunetre SDL
{
    SDL_RenderPresent(rendu);
    //Création de la surface du fond de la fenetre
    SDL_Surface *image_fond_fenetre = NULL;

    image_fond_fenetre = SDL_LoadBMP(emplacementimage);
    if(image_fond_fenetre == NULL)
    {
        SDL_DestroyRenderer(rendu);
        SDL_DestroyWindow(fenetre);
        SDL_FatalError("Fail a la creation de la surface de l'image ");
    }

   //Création de la Texture du fond
    SDL_Texture *texture_fond = NULL;
    texture_fond = SDL_CreateTextureFromSurface(rendu, image_fond_fenetre);
    SDL_FreeSurface(image_fond_fenetre);
    if(texture_fond == NULL)
    {
        SDL_DestroyRenderer(rendu);
        SDL_DestroyWindow(fenetre);
        SDL_FatalError("Fail a la creation de la texture de l'image ");
    }

    // Mise dans la RAM de la texture

    SDL_Rect rectangle;
    if(SDL_QueryTexture(texture_fond, NULL, NULL, &rectangle.w, &rectangle.h)!= 0)
    {
        SDL_DestroyRenderer(rendu);
        SDL_DestroyWindow(fenetre);
        SDL_FatalError("Fail a la mise en memoire RAM de la texture de l'image ");
    }

    rectangle.x = (largeur_fenetre - rectangle.w)/2;
    rectangle.y = (hauteur_fenetre  - rectangle.h)/2;

    // Affichage de l'image
    SDL_RenderCopy(rendu, texture_fond, NULL, &rectangle) ;

    SDL_RenderPresent(rendu);

    return texture_fond;
}

void ChangementImage(struct TransfertInfoEtVisuel *transfert, char *emplacementimage) //Fonction pour modifier l'image de la feunetre SDL
{
        SDL_DestroyTexture((*transfert).textureimage);
        SDL_DestroyRenderer((*transfert).rendu);
        SDL_DestroyWindow((*transfert).fenetre);
        (*transfert).fenetre = CreationDeFenetre() ;
        (*transfert).rendu = CreationRendu((*transfert).fenetre);
        (*transfert).textureimage = CreationEtAffichageTexture((*transfert).fenetre, (*transfert).rendu, emplacementimage);
}

HANDLE ModifConsole() //fonction de recuperation et de modification des informations de la console
{
    HANDLE console;
    console = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTitle("Loup Garou | Joan & Khiem |");
    SetConsoleTextAttribute(console, 15);
    return console;
}

///------------------------------------------------------------------------------

///Fonctions tierces utilitaires-------------------------------------------------

void ChronoPause(int temps, HANDLE console, struct TransfertInfoEtVisuel *transfert) //Fonction pour afficher un chronometre
{
    for (int i=temps; i>-1 ; i--)
    {
        printf(". ");
        SDL_Delay(383);
        printf(". ");
        SDL_Delay(383);
        printf("%d \n", i);
        SDL_Delay(383);
    }
}

int Shuffle(int *Tab, int max) //fonction de tri aleatoire d'un tableau
{
    int i ; int j ; int tmp ;
    srand(time(NULL));
    for(int i=0; i < max ; i++)
    {
      j=(int)(rand()%(max));
      tmp = Tab[i];
      Tab[i]=Tab[j];
      Tab[j]=tmp;
    }
}

bool VerifScanfEntier(char chainescanner[TAILLE_DES_CHAR]) //Fonction qui verifie si un char entre est bien un entier
{
    int taille=strlen(chainescanner);
    bool CestOkay = true ;
    for(int i= 0; i< taille ;i++)
    {
        if ((int)*(chainescanner+i)< 48 || ((int)*(chainescanner+i) > 57 ))
        {
            CestOkay = false;
        }
    }
    return CestOkay;
}

int IntCharToInt(char chainescanner[TAILLE_DES_CHAR]) //Fonction qui transforme un entier en char , en un entier en Int
{
    int taille=strlen(chainescanner);
    int L_Entier=0;
    for(int i=0; i<taille ; i++)
    {
        int P_dix=1;
        for(int j = 0 ; j < i ; j++)
            {P_dix=P_dix*10;}
        int Asciicode=((int)*(chainescanner+(taille-i-1))-48);
        //printf("%d\n",Asciicode);
        L_Entier=L_Entier + (Asciicode*P_dix);
    }
    return L_Entier;
}

int Actualisation_cmp_LG(struct Joueur *TableauJoueurs, int nombredejoueur) //Fonction pour actualiser le nombre de loup garou encore en vie au cours de la partie
{
    int cmp_loupgarou = 0;
    char c_LoupGarou[TAILLE_DES_CHAR] = "LoupGarou";
    for(int i = 0 ; i<nombredejoueur ; i++)
    {
        if ( (strcmp(TableauJoueurs[i].rolejoueur , c_LoupGarou)== 0) && (TableauJoueurs[i].vie != 0) )
            cmp_loupgarou++ ;
    }
    return cmp_loupgarou;
}

///------------------------------------------------------------------------------

///Fonctions d'affichage---------------------------------------------------------

void AfficherTableauJoueur(struct Joueur *TableauJoueurs, int nombredejoueur)
{

    system("cls");
    printf("Voulez vous afficher le Tableau ? \n Oui : 1 \n Non: Autres \n");
    int rep;
    scanf("%d", &rep);

    if(rep==1)
    {
        printf("Entrez le temps d'affichage\n =>");
        int tmp = 5 ;
        scanf("%d", &tmp);
        for(int i = 0 ; i< nombredejoueur; i++)
            printf("\nJoueur %d => nom : %s\n vote : %d\n maire : %d\n nombredevie : %d\n couple : %d\n nom role : %s\n charmed : %d\n protected : %d\n", i+1 , TableauJoueurs[i].nom , TableauJoueurs[i].vote, TableauJoueurs[i].maire, TableauJoueurs[i].vie , TableauJoueurs[i].couple , TableauJoueurs[i].rolejoueur , TableauJoueurs[i].charmed, TableauJoueurs[i].protected);
         SDL_Delay(tmp*1000);
    }
}
void AffichageRegles(HANDLE console) //fonction d'affichage des regles si l'utilisateur le souhaite
{
     char afficheRules[TAILLE_DES_CHAR] ;
     int TrueAfficheRules;
     bool Valeurcorrect = false ;
     while (Valeurcorrect== false)
    {
            printf("Voulez vous connaitre avant de commencer, les specificites de chaque roles ?\n entrez la reponse correspondantes :\n 0: Non\n 1: Oui\n =>");
            scanf("%s", afficheRules);
            SetConsoleTextAttribute(console , 7);
            if(VerifScanfEntier(afficheRules) == true)
            {
                TrueAfficheRules = IntCharToInt(afficheRules);
                if (TrueAfficheRules == 0 || TrueAfficheRules == 1)
                {
                    if (TrueAfficheRules == 1)
                    {
                        //lecture et affichage des regles depuis un fichier Rules.txt
                        FILE * FichierRegles = NULL ;
                        FichierRegles = fopen("Images/Rules.txt" ,"r");
                        if (FichierRegles == NULL)
                            printf("impossible d'ouvrir le fichier Rules.txt");
                        char LesRegles[10000]="";
                        while (fgets(LesRegles, 10000, FichierRegles) != NULL)
                        {
                            printf("%s\n",LesRegles);
                        }
                        fclose(FichierRegles);
                        Valeurcorrect = true ;
                    }
                    else if (TrueAfficheRules == 0 )
                        Valeurcorrect = true ;
                }
                else
                    printf("invalid answer restart\n");
            }
            else
                printf("invalid answer restart\n");
    }
    SetConsoleTextAttribute(console , 15);
}

void AffichageCharme(struct Joueur *TableauJoueurs, int nombredejoueur,HANDLE console, struct TransfertInfoEtVisuel *transfert) //fonction d'affichage des gens charmer lors du jour
{
    SetConsoleTextAttribute(console, 14);
    printf("Liste des Joueurs charmes :\n");
    for (int i =0;i<nombredejoueur;i++)
    {
        if(TableauJoueurs[i].charmed==true )
        {
            if (TableauJoueurs[i].vie>=1)
            {
                printf("%s est charme par le joueur de flute\n",TableauJoueurs[i].nom);
            }
        }
    }

}

void AffichageDesMorts(struct Joueur *TableauJoueurs, int nombredejoueur, int mortLG, int mortLGB, int mortsorciere, HANDLE console, struct TransfertInfoEtVisuel *transfert) //fonction d'affichage des gens mort lors du jour
{
    char c_Chasseur[TAILLE_DES_CHAR]="Chasseur";
    int tmp = -1;
    bool cpldouble_aff = false ;
    int mortchasseur = -1;
    printf("Cette nuit des joueurs sont mort: \n");

    if( mortLG != -1 && TableauJoueurs[mortLG].vie == 0) //verif du mort par les loup garou
    {
        printf("%s est mort, il etait %s \n", TableauJoueurs[mortLG].nom, TableauJoueurs[mortLG].rolejoueur);
        if (TableauJoueurs[mortLG].couple == true)
        {
             TableauJoueurs[mortLG].couple = false ;
             for (int i = 0; i<nombredejoueur; i++)
             {
                if (TableauJoueurs[i].couple == true)
                {
                    tmp = i;
                    i=nombredejoueur;
                }
             }
             TableauJoueurs[mortLG].couple = true ;
             if (cpldouble_aff == false)
             {
                printf("et comme %s est mort et qu'il/elle etait en couple avec %s,\nles deux sont donc mort, ils etaient respectivement %s et %s \n", TableauJoueurs[mortLG].nom , TableauJoueurs[tmp].nom , TableauJoueurs[mortLG].rolejoueur, TableauJoueurs[tmp].rolejoueur);
                cpldouble_aff = true ;
             }
        }
         if (TableauJoueurs[mortLG].maire == true || (tmp != -1 && TableauJoueurs[tmp].maire == true));
        {

            if (TableauJoueurs[mortLG].maire == true)
                SuccessionMaire(TableauJoueurs, nombredejoueur, console, transfert, mortLG);
            else if (tmp != -1 && TableauJoueurs[tmp].maire == true)
                SuccessionMaire(TableauJoueurs, nombredejoueur, console, transfert, tmp);
        }

        if (strcmp(c_Chasseur, TableauJoueurs[mortLG].rolejoueur )==0)
        {
            mortchasseur= MortChasseur(TableauJoueurs, nombredejoueur, mortLG, console, transfert);
        }
    }

    if( mortLGB != -1 && TableauJoueurs[mortLGB].vie == 0) //verif du mort par le Loup Garou Blanc
    {
        printf("%s est mort, il etait %s \n", TableauJoueurs[mortLGB].nom, TableauJoueurs[mortLGB].rolejoueur);
        if (TableauJoueurs[mortLGB].couple == true)
        {
             TableauJoueurs[mortLGB].couple = false ;
             for (int i = 0; i<nombredejoueur; i++)
             {
                if (TableauJoueurs[i].couple == true)
                {
                    tmp = i;
                    i=nombredejoueur;
                }
             }
             TableauJoueurs[mortLGB].couple = true ;
             if (cpldouble_aff == false)
             {
                printf("et comme %s est mort , ils etaient en couple avec %s,\nles deux sont donc, ils etaient respectivement %s et %s \n", TableauJoueurs[mortLGB].nom , TableauJoueurs[tmp].nom , TableauJoueurs[mortLGB].rolejoueur, TableauJoueurs[tmp].rolejoueur);
                cpldouble_aff = true ;
             }
        }
         if (TableauJoueurs[mortLGB].maire == true || (tmp != -1 && TableauJoueurs[tmp].maire == true));
        {
            if (TableauJoueurs[mortLGB].maire == true)
                SuccessionMaire(TableauJoueurs, nombredejoueur, console, transfert, mortLGB);
            else if (tmp != -1 && TableauJoueurs[tmp].maire == true)
                SuccessionMaire(TableauJoueurs, nombredejoueur, console, transfert, tmp);
        }

        if (strcmp(c_Chasseur, TableauJoueurs[mortLGB].rolejoueur )==0)
        {
            mortchasseur= MortChasseur(TableauJoueurs, nombredejoueur, mortLGB, console, transfert);
        }
    }

    if( mortsorciere != -1 && TableauJoueurs[mortsorciere].vie == 0) //verif du mort par la sorciere
    {
        printf("%s est mort, il etait %s \n", TableauJoueurs[mortsorciere].nom, TableauJoueurs[mortsorciere].rolejoueur);
         if (TableauJoueurs[mortsorciere].couple == true)
        {
             TableauJoueurs[mortsorciere].couple = false ;
             for (int i = 0; i<nombredejoueur; i++)
             {
                if (TableauJoueurs[i].couple == true)
                {
                    tmp = i;
                    i=nombredejoueur;
                }
             }
             TableauJoueurs[mortsorciere].couple = true ;
             if (cpldouble_aff == false)
             {
                printf("et comme %s est mort , ils etaient en couple avec %s,\nles deux sont donc, ils etaient respectivement %s et %s \n", TableauJoueurs[mortsorciere].nom , TableauJoueurs[tmp].nom , TableauJoueurs[mortsorciere].rolejoueur, TableauJoueurs[tmp].rolejoueur);
                cpldouble_aff = true ;
             }
        }
         if (TableauJoueurs[mortsorciere].maire == true || (tmp != -1 && TableauJoueurs[tmp].maire == true));
        {
            if (TableauJoueurs[mortsorciere].maire == true)
                SuccessionMaire(TableauJoueurs, nombredejoueur, console, transfert, mortsorciere);
            else if (tmp != -1 && TableauJoueurs[tmp].maire == true)
                SuccessionMaire(TableauJoueurs, nombredejoueur, console, transfert, tmp);
        }

        if (strcmp(c_Chasseur, TableauJoueurs[mortsorciere].rolejoueur )==0)
        {
            mortchasseur= MortChasseur(TableauJoueurs, nombredejoueur, mortsorciere, console, transfert);
        }
    }

    if( mortchasseur != -1 && TableauJoueurs[mortchasseur].vie == 0) // verif du mort par le chasseur
    {
        printf("%s est mort, il etait %s \n", TableauJoueurs[mortchasseur].nom, TableauJoueurs[mortchasseur].rolejoueur);
         if (TableauJoueurs[mortchasseur].couple == true)
        {
             TableauJoueurs[mortchasseur].couple = false ;
             for (int i = 0; i<nombredejoueur; i++)
             {
                if (TableauJoueurs[i].couple == true)
                {
                    tmp = i;
                    i=nombredejoueur;
                }
             }
             TableauJoueurs[mortchasseur].couple = true ;
             if (cpldouble_aff == false)
             {
                printf("et comme %s est mort , ils etaient en couple avec %s,\nles deux sont donc, ils etaient respectivement %s et %s \n", TableauJoueurs[mortchasseur].nom , TableauJoueurs[tmp].nom , TableauJoueurs[mortchasseur].rolejoueur, TableauJoueurs[tmp].rolejoueur);
                cpldouble_aff = true ;
             }
        }
         if (TableauJoueurs[mortchasseur].maire == true || (tmp != -1 && TableauJoueurs[tmp].maire == true));
        {
            if (TableauJoueurs[mortchasseur].maire == true)
                SuccessionMaire(TableauJoueurs, nombredejoueur, console, transfert, mortchasseur);
            else if (tmp != -1 && TableauJoueurs[tmp].maire == true)
                SuccessionMaire(TableauJoueurs, nombredejoueur, console, transfert, tmp);
        }
    }
    SetConsoleTextAttribute(console , 15);
}

void AppelNextPlayer(struct Joueur *TableauJoueurs, int nombredejoueur, HANDLE console, int mode, char nom_ou_role[TAILLE_DES_CHAR*3], struct TransfertInfoEtVisuel *transfert) //Fonction pour appeler le joueur suivant qui doit se rendre devant l'ordinateur + Chrono pour le temps qu'il vienne
{
    //mode 1 appel avec les noms
    //mode 2 appel avec les roles
    if (mode == 1)
    {
         system("cls");
        printf("veuillez appeler %s devant l'ordinateur, TOUT(E) SEUL(E)\n", nom_ou_role);
        if (ModeDeveloppeur == false)
            ChronoPause(9, console, transfert);
        else
            ChronoPause(2, console, transfert);
    }
    if (mode == 2)
    {
        system("cls");
        printf("%s est appele devant l'ordinateur, TOUT(E) SEUL(E)\n", nom_ou_role);
        if (ModeDeveloppeur == false)
            ChronoPause(9, console, transfert);
        else
            ChronoPause(2, console, transfert);
    }
}

void ConnaissanceRoles(struct Joueur *TableauJoueurs, int nombredejoueur, HANDLE console, struct TransfertInfoEtVisuel *transfert) //fonction de pour la prise de connaissance des roles de chaque joueur
{
    char c_LoupGarouBlanc[TAILLE_DES_CHAR] = "LoupGarouBlanc";
    char c_LoupGarou[TAILLE_DES_CHAR] = "LoupGarou";
    char c_JoueurDeFlute[TAILLE_DES_CHAR] = "JoueurDeFlute" ;
    char c_Sorciere[TAILLE_DES_CHAR] = "Sorciere" ;
    char c_Voyante[TAILLE_DES_CHAR] = "Voyante";
    char c_Cupidon[TAILLE_DES_CHAR] = "Cupidon";
    char c_Corbeau[TAILLE_DES_CHAR] = "Corbeau";
    char c_Garde [TAILLE_DES_CHAR] = "Garde";
    char c_Voleur[TAILLE_DES_CHAR] = "Voleur";
    char c_BC[TAILLE_DES_CHAR] = "BoucEmissaire";
    char c_Ancien[TAILLE_DES_CHAR] = "Ancien";
    char c_Villageois[TAILLE_DES_CHAR] = "Villageois";
    char c_PF[TAILLE_DES_CHAR] = "PetiteFille";
    char c_Ange[TAILLE_DES_CHAR] = "Ange";
    char c_Chasseur[TAILLE_DES_CHAR] = "Chasseur";
    char c_IDV[TAILLE_DES_CHAR] = "IdiotDuVillage";


    for (int i=0; i<nombredejoueur; i++)
    {
        int pause = 1;
        if (ModeDeveloppeur == false)
            pause = 5;
        else
            pause = 2;

        AppelNextPlayer(TableauJoueurs, nombredejoueur, console, 1, TableauJoueurs[i].nom, transfert);
        system("cls");
        if (strcmp(TableauJoueurs[i].rolejoueur, c_LoupGarouBlanc)==0)
            {
                printf("%s, tu es %s\n", TableauJoueurs[i].nom, TableauJoueurs[i].rolejoueur);
                ChangementImage(transfert, "Images/loup-garou-blanc.bmp");
                ChronoPause(pause, console, transfert);
                ChangementImage(transfert, "Images/Jour.bmp");
            }
        else if (strcmp(TableauJoueurs[i].rolejoueur, c_LoupGarou)==0)
            {
                printf("%s, tu es %s\n", TableauJoueurs[i].nom, TableauJoueurs[i].rolejoueur);
                ChangementImage(transfert, "Images/loup-garou.bmp");
                ChronoPause(pause, console, transfert);
                ChangementImage(transfert, "Images/Jour.bmp");
            }
        else if (strcmp(TableauJoueurs[i].rolejoueur, c_JoueurDeFlute)==0)
            {
                printf("%s, tu es %s\n", TableauJoueurs[i].nom, TableauJoueurs[i].rolejoueur);
                ChangementImage(transfert, "Images/joueur-de-flute.bmp");
                ChronoPause(pause, console, transfert);
                ChangementImage(transfert, "Images/Jour.bmp");
            }
         else if (strcmp(TableauJoueurs[i].rolejoueur, c_Sorciere)==0)
            {
                printf("%s, tu es %s\n", TableauJoueurs[i].nom, TableauJoueurs[i].rolejoueur);
                ChangementImage(transfert, "Images/sorciere.bmp");
                ChronoPause(pause, console, transfert);
                ChangementImage(transfert, "Images/Jour.bmp");
            }
         else if (strcmp(TableauJoueurs[i].rolejoueur, c_Voyante)==0)
            {
                printf("%s, tu es %s\n", TableauJoueurs[i].nom, TableauJoueurs[i].rolejoueur);
                ChangementImage(transfert, "Images/voyante.bmp");
                ChronoPause(pause, console, transfert);
                ChangementImage(transfert, "Images/Jour.bmp");
            }
         else if (strcmp(TableauJoueurs[i].rolejoueur, c_Cupidon)==0)
            {
                printf("%s, tu es %s\n", TableauJoueurs[i].nom, TableauJoueurs[i].rolejoueur);
                ChangementImage(transfert, "Images/cupidon.bmp");
                ChronoPause(pause, console, transfert);
                ChangementImage(transfert, "Images/Jour.bmp");
            }
         else if (strcmp(TableauJoueurs[i].rolejoueur, c_Corbeau)==0)
            {
                printf("%s, tu es %s\n", TableauJoueurs[i].nom, TableauJoueurs[i].rolejoueur);
                ChangementImage(transfert, "Images/corbeau.bmp");
                ChronoPause(pause, console, transfert);
                ChangementImage(transfert, "Images/Jour.bmp");
            }
        else if (strcmp(TableauJoueurs[i].rolejoueur, c_Garde)==0)
            {
                printf("%s, tu es %s\n", TableauJoueurs[i].nom, TableauJoueurs[i].rolejoueur);
                ChangementImage(transfert, "Images/Garde.bmp");
                ChronoPause(pause, console, transfert);
                ChangementImage(transfert, "Images/Jour.bmp");
            }
        else if (strcmp(TableauJoueurs[i].rolejoueur, c_Voleur)==0)
            {
                printf("%s, tu es %s\n", TableauJoueurs[i].nom, TableauJoueurs[i].rolejoueur);
                ChangementImage(transfert, "Images/voleur.bmp");
                ChronoPause(pause, console, transfert);
                ChangementImage(transfert, "Images/Jour.bmp");
            }
        else if (strcmp(TableauJoueurs[i].rolejoueur, c_BC)==0)
            {
                printf("%s, tu es %s\n", TableauJoueurs[i].nom, TableauJoueurs[i].rolejoueur);
                ChangementImage(transfert, "Images/bouc-emissaire.bmp");
                ChronoPause(pause, console, transfert);
                ChangementImage(transfert, "Images/Jour.bmp");
            }
        else if (strcmp(TableauJoueurs[i].rolejoueur, c_Ancien)==0)
            {
                printf("%s, tu es %s\n", TableauJoueurs[i].nom, TableauJoueurs[i].rolejoueur);
                ChangementImage(transfert, "Images/ancien.bmp");
                ChronoPause(pause, console, transfert);
                ChangementImage(transfert, "Images/Jour.bmp");
            }
        else if (strcmp(TableauJoueurs[i].rolejoueur, c_Villageois)==0)
            {
                printf("%s, tu es %s\n", TableauJoueurs[i].nom, TableauJoueurs[i].rolejoueur);
                ChangementImage(transfert, "Images/villageois.bmp");
                ChronoPause(pause, console, transfert);
                ChangementImage(transfert, "Images/Jour.bmp");
            }
        else if (strcmp(TableauJoueurs[i].rolejoueur, c_PF)==0)
            {
                printf("%s, tu es %s\n", TableauJoueurs[i].nom, TableauJoueurs[i].rolejoueur);
                ChangementImage(transfert, "Images/petite-fille.bmp");
                ChronoPause(pause, console, transfert);
                ChangementImage(transfert, "Images/Jour.bmp");
            }
        else if (strcmp(TableauJoueurs[i].rolejoueur, c_Ange)==0)
            {
                printf("%s, tu es %s\n", TableauJoueurs[i].nom, TableauJoueurs[i].rolejoueur);
                ChangementImage(transfert, "Images/ange.bmp");
                ChronoPause(pause, console, transfert);
                ChangementImage(transfert, "Images/Jour.bmp");
            }
        else if (strcmp(TableauJoueurs[i].rolejoueur, c_Chasseur)==0)
            {
                printf("%s, tu es %s\n", TableauJoueurs[i].nom, TableauJoueurs[i].rolejoueur);
                ChangementImage(transfert, "Images/chasseur.bmp");
                ChronoPause(pause, console, transfert);
                ChangementImage(transfert, "Images/Jour.bmp");
            }
        else if (strcmp(TableauJoueurs[i].rolejoueur, c_IDV)==0)
            {
                printf("%s, tu es %s\n", TableauJoueurs[i].nom, TableauJoueurs[i].rolejoueur);
                ChangementImage(transfert, "Images/idiot-du-village.bmp");
                ChronoPause(pause, console, transfert);
                ChangementImage(transfert, "Images/Jour.bmp");
            }
    }
}

///------------------------------------------------------------------------------

///Fonctions d'initialisation de partie------------------------------------------

int ChoixExtension(HANDLE console)  //fonction du choix de l'extension par l'utilisateur
{
    char extension[10];
    int TrueExtension;
    bool Valeurcorrect = false;
    while (Valeurcorrect== false)
    {

            printf("Entrez l'extension de votre choix (qui impactera les roles de la partie) \n 0: Loup Garou Classique (de 6 a 16 joueurs) \n 1: Extension Village (de 8 a 16 joueurs) \n 2: Extension Nouvelle Lune (de 10 a 17 joueur)\n 3: Extensions Nouvelles Lunes + Village (de 12 a 19 joueurs) \n =>");
            scanf("%s", extension);
            if (VerifScanfEntier(extension) == true)
            {
                TrueExtension = IntCharToInt(extension) ;
                if (TrueExtension == 0 || TrueExtension == 1 || TrueExtension == 2 || TrueExtension==3)
                    Valeurcorrect = true ;
                else
                    printf("invalid extension \n");
            }
            else
                printf("invalid extension \n");
    }
    return (TrueExtension);
}

int NombreDeJoueur(int extension, HANDLE console) //fonction de saisie du nombre de joueur par l'utilisateur
{
    char nombredejoueur[TAILLE_DES_CHAR];
    int Truenombredejoueur;
    bool Valeurcorrect = false;
    while (Valeurcorrect== false)
    {
            if(extension == 0)
                printf("entrez le nombre de joueurs entre 6 et 16 \n =>");
            else if(extension == 1)
                printf("entrez le nombre de joueurs entre 8 et 16 \n =>");
            else if(extension == 2)
                printf("entrez le nombre de joueurs entre 10 et 17 \n =>");
             else if(extension == 3)
                printf("entrez le nombre de joueurs entre 12 et 19 \n =>");
            scanf("%s", nombredejoueur);
            system("cls");
            if(VerifScanfEntier(nombredejoueur) == true)
            {
                Truenombredejoueur = IntCharToInt(nombredejoueur);
                if(extension == 0)
                    {
                        if (Truenombredejoueur > 5 && Truenombredejoueur < 17 )
                            Valeurcorrect = true ;
                        else
                            printf("invalid player number \n");
                    }
                else if(extension == 1)
                    {
                        if (Truenombredejoueur > 7 && Truenombredejoueur < 17 )
                            Valeurcorrect = true ;
                        else
                            printf("invalid player number \n");
                    }
                else if(extension == 2)
                    {
                        if (Truenombredejoueur > 9 && Truenombredejoueur < 18 )
                            Valeurcorrect = true ;
                    }
                else if(extension == 3)
                    {
                        if (Truenombredejoueur > 11 && Truenombredejoueur < 20 )
                            Valeurcorrect = true ;
                        else
                            printf("invalid player number \n");
                    }
            }
            else
                printf("invalid player number \n");
    }
    return Truenombredejoueur;
}

void CreationJoueurs(struct Joueur *TableauJoueurs, int nombredejoueur, int extension) //fonction d'entrer des nom des joueurs et repartition des roles aléatoire en fonction du nombre de joueurs et de l'extension choisit
{

    int *TableauEntier=malloc(nombredejoueur*sizeof(int)); //
    for( int i =0 ; i < nombredejoueur ; i++)              ////creation d'un tableau pour les indices
            TableauEntier[i]=i;                            ////puis melange aleatoire du tableau , pour rendre aleatoire les roles
    Shuffle(TableauEntier, nombredejoueur);                //


    bool nomDiff = false ;
    bool tmpdoubleaff = false;
    while (nomDiff != true)
    {
        for( int i=0 ; i < nombredejoueur ; i++) //boucle for d'initialisation des noms de chaque joueur de notre Tableau Joueurs avec verif si noms equivalents
        {
            tmpdoubleaff = false;
            printf("entrez nom du joueur %d \n =>", i+1);
            scanf("%s", TableauJoueurs[i].nom);
            for (int j=0 ; j<i; j++)
            {
                     if (strcmp(TableauJoueurs[i].nom , TableauJoueurs[j].nom) == 0 )
                    {
                        if(tmpdoubleaff == false)
                        {
                            printf("Erreur, au moins deux des noms sont identiques, reesayez \n");
                            tmpdoubleaff = true ;
                        }
                        j=i;
                        i=nombredejoueur;
                        nomDiff = false ;

                    }
                    else
                        nomDiff = true ;

            }
        }

    }
    for( int i=0 ; i < nombredejoueur ; i++) //boucle for d'initialisation du reste de la struct de chaque joueur de notre Tableau Joueur
        {
            TableauJoueurs[i].couple = false;
            TableauJoueurs[i].maire = false;
            TableauJoueurs[i].vie = 1;
            TableauJoueurs[i].protected = false;
            TableauJoueurs[i].charmed = false;
            TableauJoueurs[i].vote = 0;
            *TableauJoueurs[i].rolejoueur = '\0';
        }

    if (extension==0) ///Si l'extension 0 a ete choisit
    {
        //deduction du nombres de loup garou en fonction du nombre de joueurs
        int nbLoupgarou;
        if(nombredejoueur < 12 )
            nbLoupgarou=2;
        else if(nombredejoueur < 17)
            nbLoupgarou=3;
        int i;
        for(i=0 ; i < nbLoupgarou ; i++)
        {
           *TableauJoueurs[TableauEntier[i]].rolejoueur       = 'L';//
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 1) = 'o';////
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 2) = 'u';////
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 3) = 'p';////
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 4) = 'G';////// Attribution du Role de LoupGarou
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 5) = 'a';////
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 6) = 'r';////
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 7) = 'o';////
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 8) = 'u';//
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 9) = '\0';//
        }
           // printf("%d", i);

           ////Attribution des autres roles importants
           //Attribution du Role de Voyante
           *TableauJoueurs[TableauEntier[i]].rolejoueur       = 'V';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 1) = 'o';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 2) = 'y';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 3) = 'a';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 4) = 'n';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 5) = 't';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 6) = 'e';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 7) = '\0';
           //Attribution du Role de Sorciere
           i++;
           *TableauJoueurs[TableauEntier[i]].rolejoueur       = 'S';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 1) = 'o';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 2) = 'r';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 3) = 'c';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 4) = 'i';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 5) = 'e';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 6) = 'r';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 7) = 'e';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 8) = '\0';
           //Attribution du Role de Chasseur
           i++;
           *TableauJoueurs[TableauEntier[i]].rolejoueur       = 'C';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 1) = 'h';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 2) = 'a';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 3) = 's';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 4) = 's';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 5) = 'e';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 6) = 'u';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 7) = 'r';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 8) = '\0';
           //Attribution du Role de Cupidon
           i++;
           *TableauJoueurs[TableauEntier[i]].rolejoueur       = 'C';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 1) = 'u';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 2) = 'p';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 3) = 'i';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 4) = 'd';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 5) = 'o';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 6) = 'n';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 7) = '\0';

           if (nombredejoueur > 6 )
           {
            //Attribution du Role de Petite Fille
            i++;
            *TableauJoueurs[TableauEntier[i]].rolejoueur       = 'P';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 1) = 'e';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 2) = 't';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 3) = 'i';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 4) = 't';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 5) = 'e';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 6) = 'F';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 7) = 'i';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 8) = 'l';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 9) = 'l';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 10) = 'e';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 11) = '\0';

            //Attribution du Role de Villageois aux Joueurs restant
            i++;
            for(i=i ; i < nombredejoueur ; i++)
        {
           *TableauJoueurs[TableauEntier[i]].rolejoueur       = 'V';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 1) = 'i';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 2) = 'l';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 3) = 'l';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 4) = 'a';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 5) = 'g';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 6) = 'e';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 7) = 'o';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 8) = 'i';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 9) = 's';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 10) = '\0';
        }
           }

    }

    else if (extension==1) ///Si l'extension 1 a ete choisit
    {
        //deduction du nombres de loup garou en fonction du nombre de joueurs
       int nbLoupgarou;
        if(nombredejoueur < 12 )
            nbLoupgarou=2;
        else if(nombredejoueur < 17)
            nbLoupgarou=3;
        int i;
        for(i=0 ; i < nbLoupgarou ; i++)
        {
           *TableauJoueurs[TableauEntier[i]].rolejoueur       = 'L';//
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 1) = 'o';////
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 2) = 'u';////
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 3) = 'p';////
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 4) = 'G';////// Les joueurs aléatoirement choisient ont mtn le role à LoupGarou
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 5) = 'a';////
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 6) = 'r';////
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 7) = 'o';////
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 8) = 'u';//
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 9) = '\0';
        }
         ////Attribution des autres roles importants
           //Attribution du Role de Voyante
           *TableauJoueurs[TableauEntier[i]].rolejoueur       = 'V';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 1) = 'o';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 2) = 'y';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 3) = 'a';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 4) = 'n';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 5) = 't';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 6) = 'e';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 7) = '\0';
           //Attribution du Role de Sorciere
           i++;
           *TableauJoueurs[TableauEntier[i]].rolejoueur       = 'S';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 1) = 'o';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 2) = 'r';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 3) = 'c';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 4) = 'i';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 5) = 'e';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 6) = 'r';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 7) = 'e';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 8) = '\0';
           //Attribution du Role de Chasseur
           i++;
           *TableauJoueurs[TableauEntier[i]].rolejoueur       = 'C';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 1) = 'h';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 2) = 'a';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 3) = 's';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 4) = 's';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 5) = 'e';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 6) = 'u';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 7) = 'r';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 8) = '\0';
           //Attribution du Role de Cupidon
           i++;
           *TableauJoueurs[TableauEntier[i]].rolejoueur       = 'C';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 1) = 'u';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 2) = 'p';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 3) = 'i';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 4) = 'd';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 5) = 'o';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 6) = 'n';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 7) = '\0';

           //Attribution du Role de Corbeau
            i++;
           *TableauJoueurs[TableauEntier[i]].rolejoueur       = 'C';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 1) = 'o';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 2) = 'r';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 3) = 'b';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 4) = 'e';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 5) = 'a';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 6) = 'u';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 7) = '\0';

            if(nombredejoueur < 13 )
            {
            //Attribution du Role de Voleur
            i++;
            *TableauJoueurs[TableauEntier[i]].rolejoueur       = 'V';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 1) = 'o';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 2) = 'l';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 3) = 'e';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 4) = 'u';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 5) = 'r';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 6) = '\0';
            }

            if(nombredejoueur > 8 && nombredejoueur != 12)
            {
            //Attribution du Role de Loup Garou Blanc
            i++;
            *TableauJoueurs[TableauEntier[i]].rolejoueur       = 'L';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 1) = 'o';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 2) = 'u';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 3) = 'p';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 4) = 'G';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 5) = 'a';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 6) = 'r';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 7) = 'o';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 8) = 'u';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 9) = 'B';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 10) = 'l';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 11) = 'a';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 12) = 'n';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 13) = 'c';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 14) = '\0';
            }

            if(nombredejoueur > 11 )
            {
            //Attribution du Role de Petite Fille
            i++;
            *TableauJoueurs[TableauEntier[i]].rolejoueur       = 'P';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 1) = 'e';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 2) = 't';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 3) = 'i';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 4) = 't';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 5) = 'e';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 6) = 'F';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 7) = 'i';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 8) = 'l';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 9) = 'l';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 10) = 'e';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 11) = '\0';
            }

            if(nombredejoueur > 12 )
            {
            //Attribution du Role de l'Ange
            i++;
            *TableauJoueurs[TableauEntier[i]].rolejoueur       = 'A';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 1) = 'n';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 2) = 'g';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 3) = 'e';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 4) = '\0';
            }


            //Attribution du Role de Villageois aux Joueurs restants (si il en reste)
            i++;
            for(i=i ; i < nombredejoueur ; i++)
        {
           *TableauJoueurs[TableauEntier[i]].rolejoueur       = 'V';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 1) = 'i';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 2) = 'l';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 3) = 'l';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 4) = 'a';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 5) = 'g';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 6) = 'e';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 7) = 'o';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 8) = 'i';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 9) = 's';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 10) = '\0';
        }
    }
     else if (extension==2) ///Si l'extension 2 a ete choisit
     {
        //deduction du nombres de loup garou en fonction du nombre de joueurs
       int nbLoupgarou;
        if(nombredejoueur < 12 )
            nbLoupgarou=2;
        else if(nombredejoueur < 18)
            nbLoupgarou=3;
        int i;
        for(i=0 ; i < nbLoupgarou ; i++)
        {
           *TableauJoueurs[TableauEntier[i]].rolejoueur       = 'L';//
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 1) = 'o';////
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 2) = 'u';////
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 3) = 'p';////
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 4) = 'G';////// Les joueurs aléatoirement choisient ont mtn le role à LoupGarou
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 5) = 'a';////
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 6) = 'r';////
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 7) = 'o';////
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 8) = 'u';//
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 9) = '\0';//
        }
         ////Attribution des autres roles importants
           //Attribution du Role de Voyante
           *TableauJoueurs[TableauEntier[i]].rolejoueur       = 'V';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 1) = 'o';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 2) = 'y';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 3) = 'a';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 4) = 'n';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 5) = 't';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 6) = 'e';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 7) = '\0';
           //Attribution du Role de Sorciere
           i++;
           *TableauJoueurs[TableauEntier[i]].rolejoueur       = 'S';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 1) = 'o';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 2) = 'r';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 3) = 'c';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 4) = 'i';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 5) = 'e';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 6) = 'r';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 7) = 'e';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 8) = '\0';
           //Attribution du Role de Chasseur
           i++;
           *TableauJoueurs[TableauEntier[i]].rolejoueur       = 'C';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 1) = 'h';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 2) = 'a';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 3) = 's';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 4) = 's';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 5) = 'e';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 6) = 'u';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 7) = 'r';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 8) = '\0';
           //Attribution du Role de Cupidon
           i++;
           *TableauJoueurs[TableauEntier[i]].rolejoueur       = 'C';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 1) = 'u';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 2) = 'p';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 3) = 'i';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 4) = 'd';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 5) = 'o';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 6) = 'n';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 7) = '\0';

            //Attribution du Role de Petite Fille
            i++;
            *TableauJoueurs[TableauEntier[i]].rolejoueur       = 'P';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 1) = 'e';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 2) = 't';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 3) = 'i';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 4) = 't';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 5) = 'e';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 6) = 'F';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 7) = 'i';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 8) = 'l';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 9) = 'l';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 10) = 'e';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 11) = '\0';
            //Attribution du Role de Garde
           i++;
           *TableauJoueurs[TableauEntier[i]].rolejoueur       = 'G';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 1) = 'a';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 2) = 'r';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 3) = 'd';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 4) = 'e';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 5) = '\0';
           //Attribution du Role de l'Ancien
           i++;
           *TableauJoueurs[TableauEntier[i]].rolejoueur       = 'A';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 1) = 'n';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 2) = 'c';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 3) = 'i';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 4) = 'e';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 5) = 'n';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 6) = '\0';
           TableauJoueurs[TableauEntier[i]].vie = 2 ; //l'Ancien à 2 vie

            if(nombredejoueur < 13 )
            {
            //Attribution du Role de Voleur
            i++;
            *TableauJoueurs[TableauEntier[i]].rolejoueur       = 'V';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 1) = 'o';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 2) = 'l';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 3) = 'e';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 4) = 'u';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 5) = 'r';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 6) = '\0';
            }

            if(nombredejoueur > 11)
            {
            //Attribution du Role du Joueur De Flute
            i++;
            *TableauJoueurs[TableauEntier[i]].rolejoueur       = 'J';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 1) = 'o';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 2) = 'u';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 3) = 'e';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 4) = 'u';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 5) = 'r';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 6) = 'D';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 7) = 'e';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 8) = 'F';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 9) = 'l';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 10) = 'u';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 11) = 't';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 12) = 'e';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 13) = '\0';
            }

            if(nombredejoueur > 12)
            {
            //Attribution du Role du Bouc Emissaire
            i++;
            *TableauJoueurs[TableauEntier[i]].rolejoueur       = 'B';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 1) = 'o';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 2) = 'u';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 3) = 'c';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 4) = 'E';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 5) = 'm';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 6) = 'i';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 7) = 's';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 8) = 's';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 9) = 'a';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 10) = 'i';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 11) = 'r';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 12) = 'e';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 13) = '\0';
            }


            if(nombredejoueur > 13)
            {
            //Attribution du Role de l'Idiot du Village
            i++;
            *TableauJoueurs[TableauEntier[i]].rolejoueur       = 'I';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 1) = 'd';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 2) = 'i';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 3) = 'o';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 4) = 't';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 5) = 'D';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 6) = 'u';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 7) = 'V';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 8) = 'i';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 9) = 'l';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 10) = 'l';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 11) = 'a';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 12) = 'g';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 13) = 'e';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 14) = '\0';
            }


            //Attribution du Role de Villageois aux Joueurs restants (si il en reste)
            i++;
            for(i=i ; i < nombredejoueur ; i++)
        {
           *TableauJoueurs[TableauEntier[i]].rolejoueur       = 'V';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 1) = 'i';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 2) = 'l';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 3) = 'l';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 4) = 'a';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 5) = 'g';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 6) = 'e';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 7) = 'o';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 8) = 'i';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 9) = 's';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 10) = '\0';
        }
     }
    else if (extension==3) ///Si l'extension 3 a ete choisit
    {
        //deduction du nombres de loup garou en fonction du nombre de joueurs
        int nbLoupgarou;
        if(nombredejoueur < 12 )
            nbLoupgarou=2;
        else if(nombredejoueur < 20)
            nbLoupgarou=3;
        int i;
        for(i=0 ; i < nbLoupgarou ; i++)
        {
           *TableauJoueurs[TableauEntier[i]].rolejoueur       = 'L';//
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 1) = 'o';////
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 2) = 'u';////
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 3) = 'p';////
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 4) = 'G';////// Les joueurs aléatoirement choisient ont mtn le role à LoupGarou
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 5) = 'a';////
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 6) = 'r';////
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 7) = 'o';////
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 8) = 'u';//
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 9) = '\0';
        }
         ////Attribution des autres roles importants
           //Attribution du Role de Voyante
           *TableauJoueurs[TableauEntier[i]].rolejoueur       = 'V';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 1) = 'o';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 2) = 'y';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 3) = 'a';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 4) = 'n';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 5) = 't';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 6) = 'e';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 7) = '\0';
           //Attribution du Role de Sorciere
           i++;
           *TableauJoueurs[TableauEntier[i]].rolejoueur       = 'S';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 1) = 'o';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 2) = 'r';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 3) = 'c';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 4) = 'i';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 5) = 'e';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 6) = 'r';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 7) = 'e';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 8) = '\0';
           //Attribution du Role de Chasseur
           i++;
           *TableauJoueurs[TableauEntier[i]].rolejoueur       = 'C';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 1) = 'h';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 2) = 'a';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 3) = 's';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 4) = 's';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 5) = 'e';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 6) = 'u';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 7) = 'r';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 8) = '\0';
           //Attribution du Role de Cupidon
           i++;
           *TableauJoueurs[TableauEntier[i]].rolejoueur       = 'C';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 1) = 'u';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 2) = 'p';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 3) = 'i';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 4) = 'd';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 5) = 'o';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 6) = 'n';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 7) = '\0';
            //Attribution du Role de Petite Fille
            i++;
            *TableauJoueurs[TableauEntier[i]].rolejoueur       = 'P';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 1) = 'e';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 2) = 't';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 3) = 'i';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 4) = 't';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 5) = 'e';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 6) = 'F';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 7) = 'i';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 8) = 'l';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 9) = 'l';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 10) = 'e';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 11) = '\0';

            //Attribution du Role de Garde
           i++;
           *TableauJoueurs[TableauEntier[i]].rolejoueur       = 'G';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 1) = 'a';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 2) = 'r';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 3) = 'd';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 4) = 'e';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 5) = '\0';
           //Attribution du Role de l'Ancien
           i++;
           *TableauJoueurs[TableauEntier[i]].rolejoueur       = 'A';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 1) = 'n';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 2) = 'c';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 3) = 'i';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 4) = 'e';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 5) = 'n';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 8) = '\0';
           TableauJoueurs[TableauEntier[i]].vie = 2 ; //l'Ancien à 2 vie

            //Attribution du Role de Corbeau
            i++;
           *TableauJoueurs[TableauEntier[i]].rolejoueur       = 'C';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 1) = 'o';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 2) = 'r';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 3) = 'b';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 4) = 'e';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 5) = 'a';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 6) = 'u';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 7) = '\0';
           //Attribution du Role de l'Ange
            i++;
            *TableauJoueurs[TableauEntier[i]].rolejoueur       = 'A';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 1) = 'n';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 2) = 'g';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 3) = 'e';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 4) = '\0';


            if(nombredejoueur < 15 )
            {
            //Attribution du Role de Voleur
            i++;
            *TableauJoueurs[TableauEntier[i]].rolejoueur       = 'V';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 1) = 'o';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 2) = 'l';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 3) = 'e';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 4) = 'u';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 5) = 'r';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 6) = '\0';
            }

            if(nombredejoueur > 13)
            {
            //Attribution du Role du Joueur De Flute
            i++;
            *TableauJoueurs[TableauEntier[i]].rolejoueur       = 'J';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 1) = 'o';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 2) = 'u';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 3) = 'e';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 4) = 'u';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 5) = 'r';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 6) = 'D';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 7) = 'e';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 8) = 'F';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 9) = 'l';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 10) = 'u';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 11) = 't';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 12) = 'e';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 13) = '\0';
            }

            if(nombredejoueur > 14)
            {
            //Attribution du Role du Bouc Emissaire
            i++;
            *TableauJoueurs[TableauEntier[i]].rolejoueur       = 'B';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 1) = 'o';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 2) = 'u';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 3) = 'c';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 4) = 'E';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 5) = 'm';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 6) = 'i';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 7) = 's';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 8) = 's';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 9) = 'a';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 10) = 'i';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 11) = 'r';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 12) = 'e';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 13) = '\0';
            }

            if(nombredejoueur > 14)
            {
            //Attribution du Role de Loup Garou Blanc
            i++;
            *TableauJoueurs[TableauEntier[i]].rolejoueur       = 'L';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 1) = 'o';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 2) = 'u';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 3) = 'p';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 4) = 'G';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 5) = 'a';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 6) = 'r';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 7) = 'o';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 8) = 'u';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 9) = 'B';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 10) = 'l';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 11) = 'a';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 12) = 'n';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 13) = 'c';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 14) = '\0';
            }

            if(nombredejoueur > 15)
            {
            //Attribution du Role de l'Idiot du Village
            i++;
            *TableauJoueurs[TableauEntier[i]].rolejoueur       = 'I';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 1) = 'd';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 2) = 'i';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 3) = 'o';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 4) = 't';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 5) = 'D';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 6) = 'u';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 7) = 'V';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 8) = 'i';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 9) = 'l';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 10) = 'l';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 11) = 'a';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 12) = 'g';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 13) = 'e';
            *(TableauJoueurs[TableauEntier[i]].rolejoueur + 14) = '\0';
            }


            //Attribution du Role de Villageois aux Joueurs restants (si il en reste)
            i++;
            for(i=i ; i < nombredejoueur ; i++)
        {
           *TableauJoueurs[TableauEntier[i]].rolejoueur       = 'V';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 1) = 'i';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 2) = 'l';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 3) = 'l';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 4) = 'a';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 5) = 'g';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 6) = 'e';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 7) = 'o';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 8) = 'i';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 9) = 's';
           *(TableauJoueurs[TableauEntier[i]].rolejoueur + 10) = '\0';
        }
     }
    free(TableauEntier);
}

///------------------------------------------------------------------------------

///Fonctions gestion d'evenement-------------------------------------------------

void Mort(struct Joueur *TableauJoueurs, int nombredejoueur, int numerodumort, HANDLE console, struct TransfertInfoEtVisuel *transfert)  //Fonction appelé lors de la mort d'un joueur
{
        if (TableauJoueurs[numerodumort].couple == true)
         {
             TableauJoueurs[numerodumort].couple = false ;
             int numerodumort2 ;
             for (int i = 0; i<nombredejoueur; i++)
            {
                if (TableauJoueurs[i].couple == true)
                {
                    numerodumort2 = i;
                    i = nombredejoueur;
                }
            }

            TableauJoueurs[numerodumort].vie = 0 ;
            TableauJoueurs[numerodumort2].vie = 0 ;
            TableauJoueurs[numerodumort].couple = true ;

        }
        else
            (TableauJoueurs[numerodumort].vie)-- ;
}

int Vote(struct Joueur *TableauJoueurs, int nombredejoueur,HANDLE console, struct TransfertInfoEtVisuel *transfert, int numeroBC) //Fonction pour le Vote du village
{
    system("cls");
    char voter[TAILLE_DES_CHAR];
    char nom[TAILLE_DES_CHAR];
    int numerodumaire;
    int choixBC = -1;
    bool correspond = false;
    bool correspond1 =false;
    char Chasseur[TAILLE_DES_CHAR]= "Chasseur";
    printf("Vous allez voter pour tuer quelqu'un. Au prealable discutez et faites un debat\n");
    for (int i =0; i<nombredejoueur;i++)
    {
        correspond = false;
        while (correspond == false)
        {
            if (TableauJoueurs[i].vie >= 1)
            {
                if((numeroBC != -1) && (i == numeroBC))
                {
                    printf("%s vous ne pouvez pas voter car choisi par le bouc emissaire\n");
                }
                else
                {
                    printf("%s, a ton tour de voter\nPour qui veux tu voter\n =>", TableauJoueurs[i].nom);
                    scanf("%s",voter);
                    for (int k = 0; k<nombredejoueur;k++)
                    {
                        if(strcmp(TableauJoueurs[k].nom,voter)==0 && TableauJoueurs[k].vie>= 1)
                        {
                            TableauJoueurs[k].vote = TableauJoueurs[k].vote+1;
                            if(TableauJoueurs[i].maire == true)
                            {
                                TableauJoueurs[k].vote = TableauJoueurs[k].vote+1;
                                numerodumaire = i;
                            }
                            correspond = true;
                        }
                    }
                    if (correspond == false)
                    {
                        printf("Nom incorrect veuillez reessayer\n");
                    }
                }
            }
            else
                correspond = true;

        }
    }
    //recuperation du numero du joueur ayant le max de vote
    int maxvote=-1;
    int votetmp =0;
    for ( int i = 0;i<nombredejoueur;i++)
    {
        if (TableauJoueurs[i].vote >= votetmp)
        {
            maxvote = i;
            votetmp=TableauJoueurs[i].vote;
        }
    }

    //gestion du cas d'une egalité
    bool egalite = false;
    bool BC = false;
    int maxvote2 = -1;
    for (int i=0;i<nombredejoueur;i++)
    {
        if (TableauJoueurs[i].vote == TableauJoueurs[maxvote].vote && i!=maxvote)
        {
            egalite = true;
            maxvote2 = i;
        }
    }
    if (egalite == true)
        {
            for (int j=0;j<nombredejoueur;j++)
            {
                if (strcmp(TableauJoueurs[j].rolejoueur,"BoucEmissaire")== 0 && TableauJoueurs[j].vie >=1)
                {
                    BC = true;
                    while(correspond1 == false)
                    {
                        correspond1 = false;
                        Mort(TableauJoueurs,nombredejoueur,j,console,transfert);
                        printf("Le bouc emissaire est mort car il y a eu une egalite\nBouc Emissaire : Qui veux tu empeche de voter\n");
                        scanf("%s",nom);
                        for (int k=0;k<nombredejoueur;k++)
                        {
                            if(strcmp(TableauJoueurs[k].nom,nom)==0 && TableauJoueurs[k].vie >=1)
                            {
                                numeroBC = k;
                                k = nombredejoueur;
                                correspond1 = true;
                            }
                        }
                        if (correspond1 == false)
                        {
                            printf("Nom incorrect veuillez reesayer\n");
                        }
                    }
                }
            }
            if (BC = false)
            {
                char tuer[TAILLE_DES_CHAR];
                printf("Le maire va choisir le mort\nLe maire qui choisis tu ?");
                printf("Soit %s \Soit %s\n",TableauJoueurs[maxvote].nom,TableauJoueurs[maxvote2].nom);
                scanf("%s",tuer);
                for (int i=0;i<nombredejoueur;i++)
                {
                    if(strcmp(TableauJoueurs[i].nom,TableauJoueurs[maxvote].nom)==0 || strcmp(TableauJoueurs[i].nom,TableauJoueurs[maxvote2].nom)==0)
                    {
                         system("cls");
                         printf("%s est mort suite au vote du village il etait %s \n", TableauJoueurs[i].nom, TableauJoueurs[i].rolejoueur);
                         ChronoPause(6, console, transfert);
                         Mort(TableauJoueurs,nombredejoueur,i,console,transfert);
                         if(strcmp(TableauJoueurs[i].rolejoueur,Chasseur)==0)
                            MortChasseur(TableauJoueurs, nombredejoueur, i, console, transfert);
                         if(TableauJoueurs[i].maire == true)
                            SuccessionMaire(TableauJoueurs, nombredejoueur, console, transfert, i);
                    }
                }

            }

        }
    if (egalite == false)
    {
        system("cls");
        printf("%s est mort suite au vote du village il etait %s \n", TableauJoueurs[maxvote].nom, TableauJoueurs[maxvote].rolejoueur);
        ChronoPause(6, console, transfert);
        Mort(TableauJoueurs,nombredejoueur,maxvote,console,transfert);
        if(strcmp(TableauJoueurs[maxvote].rolejoueur,Chasseur)==0)
            MortChasseur(TableauJoueurs, nombredejoueur, maxvote, console, transfert);
        if(TableauJoueurs[maxvote].maire == true)
            SuccessionMaire(TableauJoueurs, nombredejoueur, console, transfert, maxvote);

    }

    //reinitialisation des vote de tous les joueurs a 0
    for (int o = 0; o<nombredejoueur; o++)
    {
        TableauJoueurs[o].vote=0;
    }
}

void SuccessionMaire(struct Joueur *TableauJoueurs, int nombredejoueur,HANDLE console, struct TransfertInfoEtVisuel *transfert, int numerodumaire) //fonction appelée lors de la mort du maire , pour qu'il élise sont successeur
{

    SetConsoleTextAttribute(console , 15);
    ChangementImage(transfert, "Images/maire.bmp");
    char nouveaumaire [TAILLE_DES_CHAR];
    bool correspond = false;

    if(TableauJoueurs[numerodumaire].vie ==0)
    {
          printf("Le maire est mort il va faire une passation de pouvoir\n");
        while (correspond == false)
        {
            correspond =false;
            printf("%s veuillez entrer la personne a qui vous voulez transmettre votre fonction de maire\n",TableauJoueurs[numerodumaire].nom);
            scanf("%s",nouveaumaire);
            for (int i = 0; i<nombredejoueur;i++)
            {
                if(strcmp(TableauJoueurs[i].nom,nouveaumaire)==0 && TableauJoueurs[i].vie>= 1)
                {
                    TableauJoueurs[i].maire = true;
                    correspond = true;
                    printf("%s est le nouveau maire\n",TableauJoueurs[i].nom);
                    ChronoPause(7, console, transfert);
                }
            }
            if (correspond == false)
            {
                printf("Nom incorrect ou joueur mort veuillez reesayer\n");
            }
        }
    }
}

///------------------------------------------------------------------------------

///Fonctions des Roles-----------------------------------------------------------

void Cupidon(struct Joueur *TableauJoueurs, int nombredejoueur, HANDLE console, struct TransfertInfoEtVisuel *transfert) // Fonction du role Cupidon
{
        system("cls");
        SetConsoleTextAttribute(console , 11);
        ChangementImage(transfert, "Images/cupidon.bmp");
        char nom1[TAILLE_DES_CHAR], nom2[TAILLE_DES_CHAR];
        bool correspon1= false;
        bool correspon2 = false ;
        printf("Cupidon se reveille \n");
        while (correspon1 == false || correspon2 == false)
        {
            correspon1 = false;
            correspon2 = false ;
            printf("Qui est la premiere personne amoureuse :  \n");
            scanf("%s", nom1);
            printf("La deuxieme : \n");
            scanf("%s", nom2);
            if (strcmp(nom1,nom2)!=0)
            {
                for (int i = 0; i<nombredejoueur; i++)
                {
                    if (strcmp(TableauJoueurs[i].nom,nom1) == 0)
                    {
                        TableauJoueurs[i].couple = true;
                        correspon1 = true;
                    }
                    if (strcmp(TableauJoueurs[i].nom, nom2) == 0)
                    {
                        TableauJoueurs[i].couple = true;
                        correspon2 = true;
                    }

                }
            }
            if(correspon1 == false || correspon2== false)
                printf("noms incorrect, reesayez \n");
        }

        printf("Cupidon, touche la tete des 2 amoureux pour qu'ils se reveillent et sachent. \n reveille donc %s , %s \n", nom1, nom2);
        if (ModeDeveloppeur == false)
            ChronoPause(10, console, transfert);
        else
            ChronoPause(1, console, transfert);
}

int MortChasseur(struct Joueur *TableauJoueurs, int nombredejoueur,int numerodumort, HANDLE console, struct TransfertInfoEtVisuel* transfert) //Fonction appelé lors de la mort du chasseur
{
    ChronoPause(1, console, transfert);
    int joueurtue = -1;
    system("cls");
    SetConsoleTextAttribute(console , 2);
    ChangementImage(transfert, "Images/chasseur.bmp");
    char cible[TAILLE_DES_CHAR];
    char chasseur[TAILLE_DES_CHAR] = "Chasseur";
    bool correspond = false;
    printf("Le chasseur est mort il va donc se venge\n");
    while (correspond == false)
    {
        correspond = false;
        printf("Qui veux tu tuer?\n");
        scanf("%s",cible);
        for (int i = 0; i<nombredejoueur;i++)
        {
            if (strcmp(TableauJoueurs[i].nom,cible)==0 && TableauJoueurs[i].vie>=1 && strcmp(TableauJoueurs[i].rolejoueur,chasseur)!=0)
            {
                Mort(TableauJoueurs, nombredejoueur, i, console, transfert);
                correspond = true;
                printf("le chasseur a tue : %s , il etait : %s\n", TableauJoueurs[i].nom, TableauJoueurs[i].rolejoueur);
                ChronoPause(7, console, transfert);
                i=nombredejoueur;
            }
        }
        if (correspond == false)
        {
            printf("Nom incorrect veuillez reesayer\n");
        }
    }
    SetConsoleTextAttribute(console, 15);
    return joueurtue;
}

void Corbeau (struct Joueur *TableauJoueurs,int nombredejoueurs, HANDLE console, struct TransfertInfoEtVisuel *transfert) // Fonction du role Corbeau
{
    system("cls");
    SetConsoleTextAttribute(console , 7);
    ChangementImage(transfert, "Images/corbeau.bmp");
    char nom[TAILLE_DES_CHAR];
    bool correspon = false;
    printf("Le corbeau se reveille\n");
    while (correspon == false)
    {
        correspon = false;
        printf("A qui veux tu attribuer les votes?\n");
        scanf("%s",nom);
        for(int i = 0; i<nombredejoueurs;i++)
        {
            if (strcmp(TableauJoueurs[i].nom,nom)==0)
            {
                if (TableauJoueurs[i].vie>=1)
                {
                    correspon = true;
                    TableauJoueurs[i].vote = TableauJoueurs[i].vote +2;
                    printf("Le corbeau se rendort\n");
                }
            }
        }
        if (correspon == false)
        {
            printf("Nom incorrect ou joueur mort veuillez reesayer\n");
        }
    }
}

void Voyante (struct Joueur *TableauJoueurs, int nombredejoueur, HANDLE console, struct TransfertInfoEtVisuel *transfert) // Fonction du role Voyante
{
    system("cls");
    SetConsoleTextAttribute(console , 13);
    ChangementImage(transfert, "Images/voyante.bmp");
    char nom[TAILLE_DES_CHAR];
    char voyante[TAILLE_DES_CHAR]= "Voyante";
    bool correspond = false;
    printf("La voyante se reveille\n");
    while (correspond == false)
    {
        correspond = false;
        printf("Decouvrir le role de quel joueur ?\n =>");
        scanf("%s",nom);
        for (int i = 0; i<nombredejoueur;i++)
        {
            if (strcmp(TableauJoueurs[i].nom,nom) == 0 && TableauJoueurs[i].vie>=1 && strcmp(TableauJoueurs[i].rolejoueur,voyante)!=0)
            {
                printf("%s a comme role : %s \n",TableauJoueurs[i].nom, TableauJoueurs[i].rolejoueur);
                correspond = true;
                if (ModeDeveloppeur== false)
                    ChronoPause(7, console, transfert);
                else
                    ChronoPause(2, console, transfert);
            }
        }
        if (correspond == false)
        {
            printf("Nom incorrect/ joueur mort / vous-meme,  veuillez reesayer\n");
        }
    }
}

void JoueurDeFlute(struct Joueur *TableauJoueurs, int nombredejoueur, int nombredetour, HANDLE console, struct TransfertInfoEtVisuel *transfert) // Fonction du role Joueur de Flute
{
    system("cls");
    SetConsoleTextAttribute(console , 14);
    ChangementImage(transfert, "Images/joueur-de-flute.bmp");
    char nom1[TAILLE_DES_CHAR];
    char nom2[TAILLE_DES_CHAR];
    bool correspond = false;
    bool correspond2 = false;
    printf("Le Joueur de Flute se reveille\n");
    while (correspond == false)
    {
        correspond = false;
        printf("Qui veux tu charmer\n =>");
        scanf("%s",nom1);
        for (int i = 0; i<nombredejoueur;i++)
        {
            if (strcmp(TableauJoueurs[i].nom,nom1)==0 && TableauJoueurs[i].vie>=1)
            {
                if (TableauJoueurs[i].charmed ==false)
                {
                    TableauJoueurs[i].charmed = true;
                    correspond = true;
                    i=nombredejoueur;
                }
            }
        }
        if(correspond == false )
        {
            printf("Nom incorrect ou joueur deja charmed veuillez reesayer\n");
        }
    }
    if(nombredetour == 1)
        {
            printf("C'est le premier tour tu peux charmer une seconde personne\n");
            while(correspond2 == false)
            {
                correspond2 = false;
                printf("Qui veux tu charmer?\n =>");
                scanf("%s",nom2);
                for (int i = 0; i<nombredejoueur;i++)
                {
                    if (strcmp(TableauJoueurs[i].nom,nom2)==0 && strcmp(nom1,nom2)!=0 && TableauJoueurs[i].vie>=1)
                    {
                        if (TableauJoueurs[i].charmed ==false)
                        {
                            TableauJoueurs[i].charmed = true;
                            correspond2 = true;
                            i=nombredejoueur;
                        }
                    }
                }
                if (correspond2 == false)
                {
                    printf("Nom incorrect ou joueur deja charmed veuillez reessayer\n");
                }
            }
        }
}

int Garde(struct Joueur *TableauJoueurs, int nombredejoueur,int ancienProtect, HANDLE console, struct TransfertInfoEtVisuel *transfert) //Fonction du role de Garde
{
    if (ancienProtect != -1)
    {
        TableauJoueurs[ancienProtect].protected = false ;
    }
    system("cls");
    SetConsoleTextAttribute(console , 3);
    ChangementImage(transfert, "Images/garde.bmp");
    char nom[TAILLE_DES_CHAR];
    bool correspond = false;
    int ProtectionTmp;
    printf("Le garde se reveille\n");
    while (correspond == false)
    {
        correspond = false;
        printf("Qui veux tu proteger?\n");
        scanf("%s",nom);
        for (int i = 0; i<nombredejoueur;i++)
        {
            if (strcmp(TableauJoueurs[i].nom,nom) == 0 && TableauJoueurs[i].vie!=0)
            {
                ProtectionTmp = i;
                if (ancienProtect != -1)
                {
                    if (strcmp(TableauJoueurs[ancienProtect].nom,TableauJoueurs[i].nom)!=0)
                    {
                        TableauJoueurs[i].protected = true;
                        correspond = true;
                    }
                }
                else
                {
                   TableauJoueurs[i].protected = true;
                   correspond = true;
                }

                i=nombredejoueur;
            }
        }
        if(correspond == false)
        {
            printf("Nom incorrect / joueur mort ou proteger au tour precedent veuillez reesayer\n");
        }
    }
    return ProtectionTmp;
}

struct sorciere Sorciere(struct Joueur *TableauJoueurs, int nombredejoueur, struct sorciere sorciere, int numerodumort, HANDLE console, struct TransfertInfoEtVisuel *transfert) //Fonction du role de Sorciere
{
    system("cls");
    SetConsoleTextAttribute(console , 5);
    ChangementImage(transfert, "Images/sorciere.bmp");
    int mortsorciere = -1 ;
    char c_sorciere[TAILLE_DES_CHAR] = "Sorciere" ;
    if (sorciere.poison == true || sorciere.potion == true )
    {
        char reponse[TAILLE_DES_CHAR];
        char cible[TAILLE_DES_CHAR];
        char Oui[TAILLE_DES_CHAR]= "Oui";
        char oui[TAILLE_DES_CHAR]= "oui";
        char non[TAILLE_DES_CHAR]= "non";
        char Non[TAILLE_DES_CHAR]= "Non";
        char ressucite[TAILLE_DES_CHAR];
        bool correspon1 = false;
        bool correspon2 = false;
        bool correspon4 = false;
        int nombreutilisation = 0;
        printf("La sorciere se reveille \n");

        if(strcmp(TableauJoueurs[numerodumort].rolejoueur, c_sorciere)==0)
            printf("Tu es est mort(e) cette nuit\n");
        else
            if (numerodumort>0)
            {
                printf("%s est mort\n",TableauJoueurs[numerodumort].nom);
            }
        if (sorciere.poison == true)
        {
            correspon1 = false;
            correspon2 = false;
            while (correspon1 == false)
            {
                correspon1 = false;
                printf("Tu as encore du poison veux tu tuer quelqu'un ? \n(si tu dis oui, tu ne pourras pas utiliser ta potion de ressurection) \n Ecrivez Oui ou Non \n =>");
                scanf("%s",reponse);
                if (strcmp(reponse,Oui)== 0 || strcmp(reponse,oui)== 0)
                {
                    correspon1 = true;
                    while (correspon2 ==false)
                    {
                        correspon2 = false;
                        printf("Qui veux-tu tuer ? (sachant que vous ne pouvez pas vous tuer vous meme) \n =>");
                        scanf("%s",cible);
                        for (int i =0;i<nombredejoueur;i++)
                        {
                            if (strcmp(TableauJoueurs[i].nom,cible)==0 && TableauJoueurs[i].vie>=1 && strcmp(TableauJoueurs[i].rolejoueur, c_sorciere)!=0)
                            {
                                if(TableauJoueurs[i].protected== false)
                                {
                                    Mort(TableauJoueurs,nombredejoueur,i,console,transfert);
                                    correspon2=true;
                                    sorciere.poison=false;
                                    nombreutilisation = nombreutilisation +1;
                                    sorciere.mort=i;
                                    i=nombredejoueur;
                                }
                                else if(TableauJoueurs[i].protected == true)
                                {
                                    printf("Impossible de tuer %s\nIl est protege\nChoisissez un autre joueur\n =>",TableauJoueurs[i].nom);
                                    scanf("%s",cible);
                                    for (int j =0;j<nombredejoueur;j++)
                                    {
                                        if (strcmp(TableauJoueurs[i].nom,cible)==0 && TableauJoueurs[i].vie>=1 && strcmp(TableauJoueurs[i].rolejoueur, c_sorciere)!=0)
                                        {
                                            Mort(TableauJoueurs,nombredejoueur,i,console,transfert);
                                            correspon2=true;
                                            sorciere.poison=false;
                                            nombreutilisation = nombreutilisation +1;
                                            sorciere.mort=i;
                                            i=nombredejoueur;
                                        }
                                    }
                                }
                            }
                        }
                        if (correspon2 == false)
                        {
                            printf("Nom incorrect ou joueur deja mort veuillez reesayer\n");
                        }
                    }
                }
                else if (strcmp(reponse,Non) == 0 || strcmp(reponse,non)==0)
                {
                    correspon1 = true;
                }
                else if (correspon1 == false)
                {
                    printf("reponse incorrecte \n");
                }
            }

        }
        if (sorciere.potion == true && nombreutilisation == 0 && numerodumort>0)
        {
            char reponse2[TAILLE_DES_CHAR];
            while (correspon4 == false)
            {
                correspon4 = false;
                printf("Tu as encore ta potion et %s est mort\n Veux tu le ressuciter ?\ Oui ou Non\n =>",TableauJoueurs[numerodumort].nom);
                scanf("%s",reponse2);
                if (strcmp(reponse2,oui)==0 || strcmp(reponse2,Oui) == 0 && nombreutilisation == 0)
                {
                    TableauJoueurs[numerodumort].vie = TableauJoueurs[numerodumort].vie + 1;
                    sorciere.potion = false;
                    correspon4 = true;
                    nombreutilisation = nombreutilisation +1;
                }
                else if (strcmp(reponse2,Non) == 0 || strcmp(reponse2,non) == 0)
                {
                    correspon4 = true;
                }
                else if (correspon4 == false)
                {
                    printf("Entree incorrecte veuillez reesayer\n");
                }
            }
        }
        if (sorciere.poison == false && sorciere.potion == false)
        {
            printf("La sorciere n'a plus de potion ni poison elle reste donc endormie \n");
        }
    }
    return sorciere;
}

void Voleur(struct Joueur *TableauJoueurs, int nombredejoueur, int extension, HANDLE console, struct TransfertInfoEtVisuel *transfert) //Fonction du role de Voleur
{
    system("cls");
    SetConsoleTextAttribute(console , 2);
    ChangementImage(transfert, "Images/voleur.bmp");
    char Voleur[TAILLE_DES_CHAR]="Voleur";
    int numeroduvoleur;
    char choix[TAILLE_DES_CHAR];
    int Truechoix;
    bool Valeurcorrect = false;
    for (int i=0; i<nombredejoueur;i++)
    {
        if(strcmp(TableauJoueurs[i].rolejoueur, Voleur)==0)
        {
            numeroduvoleur=i;
            i=nombredejoueur;
        }
    }
    printf("Le Voleur se reveille\n");
    if (extension == 1)
    {
        if (nombredejoueur < 13)
        {
            while (Valeurcorrect== false)
            {
                if (nombredejoueur < 12)
                    printf("vous pouvez voler les roles suivant :\n1: Ange ou alors 2: Petite Fille \nEntrez le numero correspondant a votre choix\n => ");
                else
                    printf("vous pouvez voler les roles suivant :\n1: Ange ou alors 2: Loup Garou Blanc \nEntrez le numero correspondant a votre choix\n => ");
                scanf("%s", choix);
                if (VerifScanfEntier(choix)==true)
                {
                    Truechoix=IntCharToInt(choix);
                    if (Truechoix == 1)
                    {
                        Valeurcorrect= true;
                         *TableauJoueurs[numeroduvoleur].rolejoueur      = 'A';
                        *(TableauJoueurs[numeroduvoleur].rolejoueur + 1) = 'n';
                        *(TableauJoueurs[numeroduvoleur].rolejoueur + 2) = 'g';
                        *(TableauJoueurs[numeroduvoleur].rolejoueur + 3) = 'e';
                        *(TableauJoueurs[numeroduvoleur].rolejoueur + 4) = '\0';
                    }
                    else if (Truechoix == 2 )
                    {
                        Valeurcorrect= true;
                        if (nombredejoueur < 12)
                        {
                             *TableauJoueurs[numeroduvoleur].rolejoueur       = 'P';
                            *(TableauJoueurs[numeroduvoleur].rolejoueur + 1)  = 'e';
                            *(TableauJoueurs[numeroduvoleur].rolejoueur + 2)  = 't';
                            *(TableauJoueurs[numeroduvoleur].rolejoueur + 3)  = 'i';
                            *(TableauJoueurs[numeroduvoleur].rolejoueur + 4)  = 't';
                            *(TableauJoueurs[numeroduvoleur].rolejoueur + 5)  = 'e';
                            *(TableauJoueurs[numeroduvoleur].rolejoueur + 6)  = 'F';
                            *(TableauJoueurs[numeroduvoleur].rolejoueur + 7)  = 'i';
                            *(TableauJoueurs[numeroduvoleur].rolejoueur + 8)  = 'l';
                            *(TableauJoueurs[numeroduvoleur].rolejoueur + 9)  = 'l';
                            *(TableauJoueurs[numeroduvoleur].rolejoueur + 10) = 'e';
                            *(TableauJoueurs[numeroduvoleur].rolejoueur + 11) = '\0';
                        }
                        else
                        {
                             *TableauJoueurs[numeroduvoleur].rolejoueur       = 'L';
                            *(TableauJoueurs[numeroduvoleur].rolejoueur + 1)  = 'o';
                            *(TableauJoueurs[numeroduvoleur].rolejoueur + 2)  = 'u';
                            *(TableauJoueurs[numeroduvoleur].rolejoueur + 3)  = 'p';
                            *(TableauJoueurs[numeroduvoleur].rolejoueur + 4)  = 'G';
                            *(TableauJoueurs[numeroduvoleur].rolejoueur + 5)  = 'a';
                            *(TableauJoueurs[numeroduvoleur].rolejoueur + 6)  = 'r';
                            *(TableauJoueurs[numeroduvoleur].rolejoueur + 7)  = 'o';
                            *(TableauJoueurs[numeroduvoleur].rolejoueur + 8)  = 'u';
                            *(TableauJoueurs[numeroduvoleur].rolejoueur + 9)  = 'B';
                            *(TableauJoueurs[numeroduvoleur].rolejoueur + 10) = 'l';
                            *(TableauJoueurs[numeroduvoleur].rolejoueur + 11) = 'a';
                            *(TableauJoueurs[numeroduvoleur].rolejoueur + 12) = 'n';
                            *(TableauJoueurs[numeroduvoleur].rolejoueur + 13) = 'c';
                            *(TableauJoueurs[numeroduvoleur].rolejoueur + 14) = '\0';
                        }
                    }
                    else
                        printf("invalid answer restart\n");
                }
                else
                    printf("invalid answer restart\n");
            }
        }
    }

        else if ( (extension == 2 && nombredejoueur < 14) || (extension == 3 && nombredejoueur < 15) )
        {
            while (Valeurcorrect== false)
            {
                printf("vous pouvez voler les roles suivant :\n1: Idiot Du Village ou alors 2: Bouc Emissaire \n Entrez le numero correspondant a votre choix\n => ");
                scanf("%s", choix);
                if (VerifScanfEntier(choix)==true)
                {
                    Truechoix=IntCharToInt(choix);
                    if (Truechoix == 1)
                    {
                        Valeurcorrect= true;
                         *TableauJoueurs[numeroduvoleur].rolejoueur       = 'I';
                        *(TableauJoueurs[numeroduvoleur].rolejoueur + 1)  = 'd';
                        *(TableauJoueurs[numeroduvoleur].rolejoueur + 2)  = 'i';
                        *(TableauJoueurs[numeroduvoleur].rolejoueur + 3)  = 'o';
                        *(TableauJoueurs[numeroduvoleur].rolejoueur + 4)  = 't';
                        *(TableauJoueurs[numeroduvoleur].rolejoueur + 5)  = 'D';
                        *(TableauJoueurs[numeroduvoleur].rolejoueur + 6)  = 'u';
                        *(TableauJoueurs[numeroduvoleur].rolejoueur + 7)  = 'V';
                        *(TableauJoueurs[numeroduvoleur].rolejoueur + 8)  = 'i';
                        *(TableauJoueurs[numeroduvoleur].rolejoueur + 9)  = 'l';
                        *(TableauJoueurs[numeroduvoleur].rolejoueur + 10) = 'l';
                        *(TableauJoueurs[numeroduvoleur].rolejoueur + 11) = 'a';
                        *(TableauJoueurs[numeroduvoleur].rolejoueur + 12) = 'g';
                        *(TableauJoueurs[numeroduvoleur].rolejoueur + 13) = 'e';
                        *(TableauJoueurs[numeroduvoleur].rolejoueur + 14) = '\0';
                    }
                    else if (Truechoix == 2 )
                    {
                        Valeurcorrect= true;
                         *TableauJoueurs[numeroduvoleur].rolejoueur       = 'B';
                        *(TableauJoueurs[numeroduvoleur].rolejoueur + 1)  = 'o';
                        *(TableauJoueurs[numeroduvoleur].rolejoueur + 2)  = 'u';
                        *(TableauJoueurs[numeroduvoleur].rolejoueur + 3)  = 'c';
                        *(TableauJoueurs[numeroduvoleur].rolejoueur + 4)  = 'E';
                        *(TableauJoueurs[numeroduvoleur].rolejoueur + 5)  = 'm';
                        *(TableauJoueurs[numeroduvoleur].rolejoueur + 6)  = 'i';
                        *(TableauJoueurs[numeroduvoleur].rolejoueur + 7)  = 's';
                        *(TableauJoueurs[numeroduvoleur].rolejoueur + 8)  = 's';
                        *(TableauJoueurs[numeroduvoleur].rolejoueur + 9)  = 'a';
                        *(TableauJoueurs[numeroduvoleur].rolejoueur + 10) = 'i';
                        *(TableauJoueurs[numeroduvoleur].rolejoueur + 11) = 'r';
                        *(TableauJoueurs[numeroduvoleur].rolejoueur + 12) = 'e';
                        *(TableauJoueurs[numeroduvoleur].rolejoueur + 13) = '\0';
                    }
                    else
                        printf("invalid answer restart\n");
                }
                else
                    printf("invalid answer restart\n");
            }
        }
}

int LoupGarouBlanc(struct Joueur *TableauJoueurs,int NombreDeJoueur,HANDLE console,struct TransfertInfoEtVisuel *transfert)// Fonction du role Loup Garou Blanc
{
    system("cls");
    SetConsoleTextAttribute(console , 15);
    ChangementImage(transfert, "Images/loup-garou-blanc.bmp");
    bool correspond = false;
    char cible[TAILLE_DES_CHAR];
    int numerocible = -1 ;
    printf("Le loup garou blanc se reveille\n");
    while(correspond == false)
    {
        correspond = false;
        for (int i;i<NombreDeJoueur;i++ )
        {
            if(strcmp(TableauJoueurs[i].rolejoueur,"LoupGarou")==0 && TableauJoueurs[i].vie >= 1 && TableauJoueurs[i].protected == false)
            {
                printf("Tu peux tuer :%s\n",TableauJoueurs[i].nom);
            }
        }
        printf("Qui veux tu tuer parmis la liste ?\n");
        scanf("%s",cible);
        for(int i =0;i<NombreDeJoueur;i++)
        {
            if (strcmp(TableauJoueurs[i].nom,cible)== 0 &&strcmp(TableauJoueurs[i].rolejoueur,"LoupGarou")==0 && TableauJoueurs[i].vie >= 1 && TableauJoueurs[i].protected == false)
            {
                Mort(TableauJoueurs,NombreDeJoueur,i,console,transfert);
                correspond = true;
                numerocible = i;
                i=NombreDeJoueur;
            }
        }
        if (correspond==false)
        {
            printf("Nom incorrect veuillez reesayer\n");
        }
    }
    return numerocible;
}

int LoupGarou (struct Joueur *TableauJoueurs,int nombredejoueur, HANDLE console, struct TransfertInfoEtVisuel *transfert) // Fonction du role Loup Garou
{
    system("cls");
    SetConsoleTextAttribute(console , 12);
    ChangementImage(transfert, "Images/loup-garou.bmp");
    bool correspond = false;
    int mort;
    char cible[TAILLE_DES_CHAR];
    printf("Les loups garous se reveillent\n");
    while (correspond == false)
    {
        printf("Qui avez vous choisis de tuer?\n =>");
        scanf("%s",cible);
        for (int i =0; i<nombredejoueur;i++)
        {
            if (strcmp(TableauJoueurs[i].nom,cible)==0 && TableauJoueurs[i].vie>=1 && TableauJoueurs[i].protected != true)
            {
                Mort(TableauJoueurs,nombredejoueur,i,console,transfert);
                mort =i;
                i=nombredejoueur;
                correspond = true;
            }
        }
        if(correspond ==false)
        {
            printf("Nom incorrect ou  joueur protege ou mort, veuillez reessayer\n");
        }
    }
    return mort;
}

///------------------------------------------------------------------------------

///Fonctions | FinDePartie | Deroule | Main |------------------------------------

bool FinDePartie(struct Joueur *TableauJoueurs, int nombredejoueur, int extension, int nombredetour, struct sorciere sorciere, int cmp_loupgarou, HANDLE console, struct TransfertInfoEtVisuel *transfert) //Fonction de verification si la partie est fini
{

    bool findepartie = false;
    int loupgaroublanc = -1;
    int cmp_joueursenvie = nombredejoueur;
    int cmp_joueurscharmed = 0;
    int numeroduflutiste = -1 ;
    bool coupleVictoireGagnante = false ;
    int couple1 = -1 ;
    int couple2 = -2 ;
    char LoupGarouBlanc[TAILLE_DES_CHAR] = "LoupGarouBlanc";
    char LoupGarou[TAILLE_DES_CHAR] = "LoupGarou";
    char Ange[TAILLE_DES_CHAR] = "Ange" ;
    char JoueurDeFlute[TAILLE_DES_CHAR] = "JoueurDeFlute" ;
    char Chasseur[TAILLE_DES_CHAR] = "Chasseur" ;

    for (int i = 0 ; i<nombredejoueur ; i++)
    {
        if(TableauJoueurs[i].vie == 0)
        {
            cmp_joueursenvie-- ;
            if (nombredetour==1 && (strcmp(TableauJoueurs[i].rolejoueur, Ange)==0))
            {
                system("cls");
                findepartie = true;
                SetConsoleTextAttribute(console, 3);
                ChangementImage(transfert, "Images/ange.bmp");
                printf("L'ange est mort des le premier tour c'est une victoire de %s \n", TableauJoueurs[i].nom);
                i = nombredejoueur ;
                ChronoPause(5, console, transfert);
            }
            if(strcmp(TableauJoueurs[i].rolejoueur, JoueurDeFlute)==0)
                numeroduflutiste = i ;
        }
        else
        {
            if(TableauJoueurs[i].charmed == true)
                cmp_joueurscharmed++;

            if(strcmp(TableauJoueurs[i].rolejoueur, LoupGarouBlanc)==0)
                loupgaroublanc = i ;


            else if(strcmp(TableauJoueurs[i].rolejoueur, JoueurDeFlute)==0)
                numeroduflutiste = i ;

            if (couple1 == -1 && couple2 == -2)
            {
               if (TableauJoueurs[i].couple == true)
                {
                    couple1 = i ;
                    TableauJoueurs[i].couple = false ;
                    for(int j=0 ; j<nombredejoueur ; j++)
                    {
                        if (TableauJoueurs[j].couple == true)
                        {
                            couple2 = j;
                            j = nombredejoueur;
                        }
                    }
                    TableauJoueurs[couple1].couple = true ;
                }

            }

        }

    }
    //verif egalite
    if (cmp_joueursenvie == 0)
    {

            findepartie = true;
            SetConsoleTextAttribute(console , 15);
            ChangementImage(transfert, "Images/egalite.bmp");
            printf("Tous les joueurs sont mort, c'est une parfaite egalite \n");
            ChronoPause(5, console, transfert);

    }
    //verif couple gagnant seul victoire
    else if(cmp_joueursenvie == 2 && (TableauJoueurs[couple1].vie != 0) && (TableauJoueurs[couple2].vie != 0) )
    {
             findepartie = true;
             SetConsoleTextAttribute(console , 13);
             ChangementImage(transfert, "Images/couple.bmp");
             printf("Le couple sont les seul en vie. \n  Bravo, c'est une victoire de %s (role : %s ) et de %s (role : %s) \n (si ce couple est un couple de gentils, c'est une victoire du village, et si c'est un couple de loup garou , c'est une victoire des loups garous \n", TableauJoueurs[couple1].nom, TableauJoueurs[couple1].rolejoueur, TableauJoueurs[couple2].nom, TableauJoueurs[couple2].rolejoueur );
             ChronoPause(5, console, transfert);
    }
    else
{
    //verif fin de partie flutiste
    if (TableauJoueurs[numeroduflutiste].charmed == true)
    {
            if(cmp_joueurscharmed ==  cmp_joueursenvie)
            {

                findepartie = true;
                SetConsoleTextAttribute(console , 14);
                ChangementImage(transfert, "Images/joueur-de-flute.bmp");
                printf("Tous les joueurs en vie ont ete charme par le joueur de flute, c'est une victoire de %s \n", TableauJoueurs[numeroduflutiste].nom);
                ChronoPause(5, console, transfert);
            }

    else if(TableauJoueurs[numeroduflutiste].charmed == false)
        {
            if(cmp_joueurscharmed ==  (cmp_joueursenvie - 1) )
            {

                findepartie = true;
                SetConsoleTextAttribute(console , 14);
                ChangementImage(transfert, "Images/joueur-de-flute.bmp");
                printf("Tous les joueurs en vie ont ete charme par le joueur de flute, c'est une victoire de %s \n", TableauJoueurs[numeroduflutiste].nom);
                ChronoPause(5, console, transfert);
            }
        }

     if (cmp_loupgarou == cmp_joueursenvie)
        {

                findepartie = true;
                SetConsoleTextAttribute(console , 12);
                ChangementImage(transfert, "Images/loup-garou.bmp");
                printf("Tous les joueurs en vie sont des Loup Garou, bien joue a eux, c'est une victoire de leur part\n");
                ChronoPause(5, console, transfert);
        }

    else if ((cmp_joueursenvie == 1) && (loupgaroublanc != -1))
        {

                findepartie = true;
                SetConsoleTextAttribute(console , 15);
                ChangementImage(transfert, "Images/loup-garou-blanc.bmp");
                printf("Victoire de %s le Loup Garou Blanc, qui est le seul survivant", TableauJoueurs[loupgaroublanc].nom);
                ChronoPause(5, console, transfert);
        }

    else if( (cmp_loupgarou == 0 ) && (loupgaroublanc == -1) && (numeroduflutiste == -1))
        {
            if ((loupgaroublanc != -1 && TableauJoueurs[loupgaroublanc].vie == 0) || loupgaroublanc == -1)
            {
                if ((numeroduflutiste != -1 && TableauJoueurs[numeroduflutiste].vie == 0) || numeroduflutiste == -1)
                {

                    findepartie = true;
                    SetConsoleTextAttribute(console , 8);
                    ChangementImage(transfert, "Images/villageois.bmp");
                    printf("Toutes les menaces du village ont ete elimines,\n Victoire des villageois ! \n");
                    ChronoPause(5, console, transfert);
                }
            }
        }
    }
}

    return findepartie;
}

void DeroulePartie(struct Joueur *TableauJoueurs, int nombredejoueur, int extension, struct sorciere sorciere, HANDLE console, struct TransfertInfoEtVisuel *transfert) //Fonction principale du Deroulement de la partie
{
    int cmp_tour = 0 ;
    bool findepartie = false ;
    int num_mortLG = -1 ;
    int num_mortLGB = -1 ;
    int choixBC = -1 ;
    int num_joueurprotege = -1 ;
    char c_LoupGarouBlanc[TAILLE_DES_CHAR] = "LoupGarouBlanc";
    char c_LoupGarou[TAILLE_DES_CHAR] = "LoupGarou";
    char c_JoueurDeFlute[TAILLE_DES_CHAR] = "JoueurDeFlute" ;
    char c_Sorciere[TAILLE_DES_CHAR] = "Sorciere" ;
    char c_Voyante[TAILLE_DES_CHAR] = "Voyante";
    char c_Cupidon[TAILLE_DES_CHAR] = "Cupidon";
    char c_Corbeau[TAILLE_DES_CHAR] = "Corbeau";
    char c_Garde [TAILLE_DES_CHAR] = "Garde";
    char c_Voleur[TAILLE_DES_CHAR] = "Voleur";
    int num_loupgaroublanc = -1;
    int cmp_loupgarou = 0 ;
    int num_joueurdeflute = -1 ;
    int num_sorciere = -1 ;
    int num_voyante = -1 ;
    int num_cupidon  = -1;
    int num_voleur = -1 ;
    int num_garde = -1 ;
    int num_corbeau = -1 ;

    //Appel de la fonction pour que les joueur prennent connaissance de leur role
    ConnaissanceRoles(TableauJoueurs, nombredejoueur, console, transfert);

    //recuperation des numeros des joueurs pour chaque role et verification de la présence  des roles dans la partie actuelle
    for(int i = 0 ; i<nombredejoueur ; i++)
    {
        if (strcmp(TableauJoueurs[i].rolejoueur , c_LoupGarouBlanc)== 0)
            num_loupgaroublanc = i ;

        else if ( strcmp(TableauJoueurs[i].rolejoueur , c_LoupGarou)== 0 )
            cmp_loupgarou++ ;

        else if ( strcmp(TableauJoueurs[i].rolejoueur , c_Cupidon)== 0 )
            num_cupidon = i ;

        else if ( strcmp(TableauJoueurs[i].rolejoueur , c_Voleur )== 0 )
            num_voleur = i ;

        else if ( strcmp(TableauJoueurs[i].rolejoueur , c_Voyante )== 0 )
            num_voyante = i ;

        else if ( strcmp(TableauJoueurs[i].rolejoueur , c_Sorciere)== 0 )
            num_sorciere = i ;

        else if ( strcmp(TableauJoueurs[i].rolejoueur , c_JoueurDeFlute)== 0 )
            num_joueurdeflute = i ;

        else if ( strcmp(TableauJoueurs[i].rolejoueur , c_Garde )== 0 )
            num_garde = i ;
        else if ( strcmp(TableauJoueurs[i].rolejoueur , c_Corbeau )== 0 )
            num_corbeau = i ;
    }

    bool sorciereenviedebutdetour =true ;
    bool Jdfenviedebutdetour =true ;
    bool LGBenviedebutdetour =true ;

    ///Boucle des Tours
    while (findepartie == false)
    {
        cmp_tour++;
        num_mortLG = -1 ;
        num_mortLGB = -1 ;

        //verification si certain role sont en vie au debut du tour
        if(TableauJoueurs[num_sorciere].vie == 0 )
        {
            sorciereenviedebutdetour = false;
        }
        if(TableauJoueurs[num_joueurdeflute].vie == 0 )
        {
            Jdfenviedebutdetour = false;
        }
        if(TableauJoueurs[num_loupgaroublanc].vie == 0 )
        {
            LGBenviedebutdetour = false;
        }

        //Appel des fonction des roles apparaissant uniquement au tour 1
        if(cmp_tour==1)
        {
            if(num_voleur != -1 && TableauJoueurs[num_voleur].vie >= 1 && findepartie == false)
            {
                if (ModeDeveloppeur == false)
                    AppelNextPlayer(TableauJoueurs, nombredejoueur, console, 2, "Le Voleur", transfert);
                Voleur(TableauJoueurs, nombredejoueur, extension, console, transfert);
                if(ModeDeveloppeur == true)
                    AfficherTableauJoueur(TableauJoueurs,nombredejoueur);
            }

            if(num_cupidon != -1 && TableauJoueurs[num_cupidon].vie >= 1 && findepartie == false)
            {
                if (ModeDeveloppeur == false)
                    AppelNextPlayer(TableauJoueurs, nombredejoueur, console, 2, "Cupidon", transfert);
                Cupidon(TableauJoueurs, nombredejoueur, console, transfert);
                if(ModeDeveloppeur == true)
                    AfficherTableauJoueur(TableauJoueurs,nombredejoueur);
            }
        }

        //Appel successif des fonctions des roles en verifiant si le role est présent dans la partie actuelle et si le joueur est encore en vie
        if(num_voyante != -1 && TableauJoueurs[num_voyante].vie >= 1 && findepartie == false)
        {
            if (ModeDeveloppeur == false)
                AppelNextPlayer(TableauJoueurs, nombredejoueur, console, 2, "La Voyante", transfert);
            Voyante(TableauJoueurs, nombredejoueur, console, transfert);
            if(ModeDeveloppeur == true)
                AfficherTableauJoueur(TableauJoueurs,nombredejoueur);
        }

         if(num_garde != -1 && TableauJoueurs[num_garde].vie >= 1 && findepartie == false)
        {
            if (ModeDeveloppeur == false)
                AppelNextPlayer(TableauJoueurs, nombredejoueur, console, 2, "Le Garde", transfert);
            num_joueurprotege = Garde(TableauJoueurs, nombredejoueur, num_joueurprotege, console, transfert);
            if(ModeDeveloppeur == true)
                AfficherTableauJoueur(TableauJoueurs,nombredejoueur);
        }

        if(num_corbeau != -1 && TableauJoueurs[num_corbeau].vie >= 1 && findepartie == false)
        {
            if (ModeDeveloppeur == false)
                AppelNextPlayer(TableauJoueurs, nombredejoueur, console, 2, "Le Corbeau", transfert);
            Corbeau(TableauJoueurs, nombredejoueur, console, transfert);
            if(ModeDeveloppeur == true)
                AfficherTableauJoueur(TableauJoueurs,nombredejoueur);
        }

        if(( cmp_loupgarou > 0 || (num_loupgaroublanc != -1 && TableauJoueurs[num_loupgaroublanc].vie >= 1) ) && findepartie == false)
        {
            if (ModeDeveloppeur == false)
                AppelNextPlayer(TableauJoueurs, nombredejoueur, console, 2, "Les Loups Garou", transfert);
            num_mortLG = LoupGarou(TableauJoueurs, nombredejoueur, console, transfert);
            if(ModeDeveloppeur == true)
                AfficherTableauJoueur(TableauJoueurs,nombredejoueur);
        }

         if(num_sorciere != -1 && sorciereenviedebutdetour == true && findepartie == false)
        {
            if (ModeDeveloppeur == false)
                AppelNextPlayer(TableauJoueurs, nombredejoueur, console, 2, "La Sorciere", transfert);
            sorciere = Sorciere(TableauJoueurs, nombredejoueur, sorciere, num_mortLG, console, transfert);
            if(ModeDeveloppeur == true)
                AfficherTableauJoueur(TableauJoueurs,nombredejoueur);
        }

         if(num_loupgaroublanc != -1 && LGBenviedebutdetour==true && cmp_tour%2 == 0 && findepartie == false)
        {
            if (ModeDeveloppeur == false)
                AppelNextPlayer(TableauJoueurs, nombredejoueur, console, 2, "Le Loup Garou Blanc", transfert);
            num_mortLGB = LoupGarouBlanc(TableauJoueurs, nombredejoueur, console, transfert);
            if(ModeDeveloppeur == true)
                AfficherTableauJoueur(TableauJoueurs,nombredejoueur);
        }

          if(num_joueurdeflute != -1 && Jdfenviedebutdetour == true && findepartie == false)
        {
            if (ModeDeveloppeur == false)
                AppelNextPlayer(TableauJoueurs, nombredejoueur, console, 2, "Le Joueur de flute", transfert);
            JoueurDeFlute(TableauJoueurs, nombredejoueur, cmp_tour, console, transfert);
            if(ModeDeveloppeur == true)
                AfficherTableauJoueur(TableauJoueurs,nombredejoueur);
        }

        //lever du jour et Affichage de ce qui s'est passé pendant la nuit (les joueurs charmés et tués)
        system("cls");
        SetConsoleTextAttribute(console , 15);
        ChangementImage(transfert, "Images/Jour.bmp");
        printf ("le jour se leve !\n\n ");
        if ( num_joueurdeflute != -1 && TableauJoueurs[num_joueurdeflute].vie != 0)
        {
             AffichageCharme(TableauJoueurs, nombredejoueur, console, transfert);
            SetConsoleTextAttribute(console , 15);
        }
        printf("\n");
        AffichageDesMorts(TableauJoueurs, nombredejoueur, num_mortLG, num_mortLGB, sorciere.mort, console, transfert);

        //verif fin de partie
        cmp_loupgarou = Actualisation_cmp_LG(TableauJoueurs, nombredejoueur);
        findepartie = FinDePartie(TableauJoueurs, nombredejoueur, extension, cmp_tour, sorciere, cmp_loupgarou, console, transfert);

        if (findepartie == false)
        {
            printf("(si rien est ecrit alors aucun mort cette nuit) \n\n");
            ChronoPause(5, console, transfert);
        }

        //election maire 1er tour
        if(cmp_tour == 1 && findepartie == false)
        {
            char nom_maire[TAILLE_DES_CHAR];
            SetConsoleTextAttribute(console , 15);
            ChangementImage(transfert, "Images/maire.bmp");
            bool nomcorrespond = false ;
            while (nomcorrespond == false)
            {
                printf("Vous allez elire le maire,\n procedez a un vote puis rentrez le nom du maire choisi \n =>");
                scanf("%s", nom_maire);
                for(int i = 0 ; i<nombredejoueur ; i++)
                {
                    if(strcmp(nom_maire, TableauJoueurs[i].nom)==0 && TableauJoueurs[i].vie >= 1 )
                    {
                        TableauJoueurs[i].maire = true ;
                        i = nombredejoueur ;
                        nomcorrespond = true ;
                        printf("%s est desormais le maire \n", nom_maire);
                    }
                }
                if(nomcorrespond == false)
                {
                    system("cls");
                    printf("nom incorrect ou joueur mort, reessayez\n");
                }
            }
            ChronoPause(7, console, transfert);
            system("cls");
            SetConsoleTextAttribute(console , 15);
            ChangementImage(transfert, "Images/Jour.bmp");
        }

        //Fonction de vote et 2nd verification de fin de partie
        if (findepartie == false)
        {
            Vote(TableauJoueurs, nombredejoueur, console, transfert, choixBC);
            cmp_loupgarou = Actualisation_cmp_LG(TableauJoueurs, nombredejoueur);
            findepartie = FinDePartie(TableauJoueurs, nombredejoueur, extension, cmp_tour, sorciere, cmp_loupgarou, console, transfert);
        }


    }
}

int main(int argc, char * argv[])
 {

    //Initialisation de la video et de l'audio SDL

    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
        SDL_FatalError ("FAIL à l'initialistaion de l'audio et de la video SDL");


    //Initialisation du visuel (fenetre, rendu texture) dans la structure TransfertInfoEtVisuel, et initialisation de la console

    struct TransfertInfoEtVisuel transfert;

    transfert.fenetre = CreationDeFenetre() ;
    transfert.rendu = CreationRendu(transfert.fenetre);
    transfert.textureimage = CreationEtAffichageTexture(transfert.fenetre, transfert.rendu, "Images/Bienvenue.bmp");

    HANDLE console = ModifConsole();

    //Info Dev
    if(ModeDeveloppeur == true)
    {
        SetConsoleTextAttribute(console, 10);
        printf(" Mode developpeur active : \n les chrono sont plus court \n vous pouvez afficher le tableau entre chaques fonctions de role\n");
        ChronoPause(4, console, &transfert);
        SetConsoleTextAttribute(console, 15);
        system("cls");
    }


    //Appel des Fonction de debut de la partie
    AffichageRegles(console);
    int extension = ChoixExtension(console);
    int nombredejoueur = NombreDeJoueur(extension, console);
    struct Joueur* TableauJoueurs ;
    TableauJoueurs = (struct Joueur*)malloc(nombredejoueur*sizeof(struct Joueur)); //Allocation Dynamique DU Tableau de Joueur
    CreationJoueurs( TableauJoueurs , nombredejoueur, extension );

    //initialisation de la structure sorciere
    struct sorciere sorciere;
    sorciere.poison = true ;
    sorciere.potion = true;
    sorciere.mort = -1;


    if(ModeDeveloppeur == true)
        AfficherTableauJoueur(TableauJoueurs, nombredejoueur);

    // Appel de la fonction de deroulement de la partie
    DeroulePartie(TableauJoueurs, nombredejoueur, extension, sorciere, console, &transfert);

    //Fin
    SDL_Delay(12000);
    free(TableauJoueurs);
    SDL_DestroyTexture(transfert.textureimage);
    SDL_DestroyRenderer(transfert.rendu);
    SDL_DestroyWindow(transfert.fenetre);
    SDL_Quit();
    exit(0);
    return 0;
 }

///------------------------------------------------------------------------------


///Buglist:----------------------------------------------------------------------
// les noms de joueurs "j" et "j " par exemple ne sont pas considéré comme identiques (Peu important, Pose peu/aucun probleme)


