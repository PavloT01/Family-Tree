// genealogie.c   pour projet Genealogie SDA2 2024-2025
//

// NOM: TARASH PRENOM: PAVLO
///////////////////////////////////

#include "base.h"

// Types predefinis
#define LG_MAX 64
#define omega 0

// Les structures
typedef struct s_date
{
    Nat jour;
    Nat mois;
    Nat annee;
} Date;

typedef Nat Ident;

typedef struct s_individu
{
    Car nom[LG_MAX];
    Date naiss;
    Date deces;
    Ident id;
    Ident ipere;
    Ident imere;
    Ident icadet;
    Ident ifaine;
} *Individu;

typedef struct s_genealogie
{
    Individu *tab; // tableau des individus tries par nom
    Nat nb_individus;
    Nat id_cur;
    Nat taille_max_tab;
    Nat *rang;
    Nat taille_max_rang;
} *Genealogie;

// DEFINIR ICI VOS CONSTANTES
#define tab_cap 10 // constante pour mettre comme la taille du tab et rang et chaque fois multiplier par 2
#define max_profondeur 20 // profondeur maximale
#define max_longueur 500 // taille de buffer

// PARTIE 1: PROTOTYPES des operations imposees
Ent compDate(Date d1, Date d2);
void genealogieInit(Genealogie *g);
void genealogieFree(Genealogie *g);
Individu nouvIndividu(Ident i, Chaine s, Ident p, Ident m, Date n, Date d);
void freeIndividu(Individu id);
// selecteurs
Chaine nomIndividu(Individu ind);
Date naissIndividu(Individu ind);
Nat cardinal(Genealogie g);
Individu kieme(Genealogie g, Nat k);
Individu getByIdent(Genealogie g, Ident i);
Nat getPos(Genealogie g, Chaine name);
Individu getByName(Genealogie g, Chaine name, Date naissance);
// modificateurs
Ident adj(Genealogie g, Chaine s, Ident p, Ident m, Date n, Date d);
void insert(Genealogie g, Nat pos, Chaine s, Ident p, Ident m, Date n, Date d);
void adjFils(Genealogie g, Ident idx, Ident fils, Ident pp, Ident mm);

// PARTIE 2: PROTOTYPES des operations imposees
void affiche_freres_soeurs(Genealogie g, Ident x, Chaine buff);
void affiche_enfants(Genealogie g, Ident x, Chaine buff);
void affiche_cousins(Genealogie g, Ident x, Chaine buff);
void affiche_oncles(Genealogie g, Ident x, Chaine buff);

// PARTIE 3: PROTOTYPES des operations imposees
void deviennent_freres_soeurs(Genealogie g, Ident x, Ident y);
void devient_pere(Genealogie g, Ident x, Ident y);
void devient_mere(Genealogie g, Ident x, Ident y);

// PARTIE 4: PROTOTYPES des operations imposees
Bool estAncetre(Genealogie g, Ident x, Ident y);
Bool ontAncetreCommun(Genealogie g, Ident x, Ident y);
Ident plus_ancien(Genealogie g, Ident x);
void affiche_parente(Genealogie g, Ident x, Chaine buf);
void affiche_descendance(Genealogie g, Ident x, Chaine buf);

// PROTOTYPES DE VOS FONCTIONS INTERMEDIAIRES
void ajoutNomEnBuf(Chaine buff, Chaine nom);
void ajoutEnNivBuf(Chaine buff, Chaine nom);
void affRecursiveDescend(Genealogie g, Ident x, Nat niveau, Chaine *levels, Nat *maxNiv);
void affParentRescursive(Genealogie g, Ident x, Nat niveau, Chaine *levels, Nat *maxNiv);
Chaine chercheChaine(Chaine buff, Chaine rech);
Bool nomEstEnBuf(Chaine buffer, Chaine nom);
Bool videG(Genealogie g);
Nat dateNull(Date d1);

// PARTIE 1 : Construction de l'arbre généalogique et accès

// Compare deux dates : retourne une valeur négative si d1 est plus ancienne que d2,
// 0 si les deux dates sont égales, et une valeur positive si d1 est plus récente.
// PRE: None
Ent compDate(Date d1, Date d2)
{
    if (d1.annee != d2.annee)
    {
        return d1.annee - d2.annee;
    }
    if (d1.mois != d2.mois)
    {
        return d1.mois - d2.mois;
    }
    return d1.jour - d2.jour;
}

// Initialise une structure de généalogie avec des valeurs par défaut.
// PRE: None
void genealogieInit(Genealogie *g)
{
    *g = MALLOC(struct s_genealogie);
    if (*g == NULL)
    {
        return;
    }
    (*g)->nb_individus = omega;
    (*g)->id_cur = omega;
    (*g)->taille_max_tab = tab_cap;

    (*g)->tab = MALLOCN(Individu, (*g)->taille_max_tab);
    if ((*g)->tab == NULL)
    {
        FREE(*g);
        return;
    }

    (*g)->taille_max_rang = tab_cap;

    (*g)->rang = MALLOCN(Nat, (*g)->taille_max_rang);
    if ((*g)->rang == NULL)
    {
        FREE(*g);
        FREE((*g)->tab);
        return;
    }
}

// Libère toute la mémoire allouée à une structure de généalogie.
// PRE: None
void genealogieFree(Genealogie *g)
{
    if (*g == NULL)
    {
        return;
    }

    for (Nat i = 0; i < (*g)->nb_individus; i++)
    {
        FREE((*g)->tab[i]);
    }
    FREE((*g)->tab);
    FREE((*g)->rang);
    FREE(*g);
    *g = NULL;
    // Met le pointeur à NULL pour éviter toute utilisation ultérieure.
}

// Crée un nouvel individu en mémoire et initialise ses champs.
// PRE: None
Individu nouvIndividu(Ident i, Chaine s, Ident p, Ident m, Date n, Date d)
{
    Individu ind = MALLOC(struct s_individu);
    if (ind == NULL)
    {
        return NULL;
    }
    ind->id = i;

    chaineCopie(ind->nom, s);

    ind->naiss = n;
    ind->deces = d;
    ind->ipere = p;
    ind->imere = m;
    ind->icadet = omega;
    ind->ifaine = omega;
    return ind;
}

// Libère la mémoire associée à un individu.
// PRE: None
void freeIndividu(Individu id)
{
    if (id != NULL || id != omega)
    {
        FREE(id);
    }
}

// -----SELECTEUR-------------------------------------------

// Retourne le nom de l'individu donné.
Chaine nomIndividu(Individu ind)
{
    return ind->nom;
}

// Retourne la date de naissance de l'individu donné.
Date naissIndividu(Individu ind)
{
    return ind->naiss;
}

// Retourne le k-ième individu dans le tableau (k est un indice, pas un identifiant).
Nat cardinal(Genealogie g)
{
    return g->nb_individus;
}

//  renvoie tab[k], k n’est pas un Ident !
Individu kieme(Genealogie g, Nat k)
{
    return (videG(g) || k >= g->nb_individus || g->tab == NULL) ? NULL : g->tab[k];
}

// Recherche et retourne l'individu correspondant à l'identifiant donné.
// Retourne NULL si l'identifiant est invalide ou absent.
// PRE: None
Individu getByIdent(Genealogie g, Ident i)
{
    return (i == omega || videG(g) || i > cardinal(g)) ? NULL : kieme(g, (g->rang[i - 1]));
}

// Obtenir la position d'insertion par nom
Nat getPos(Genealogie g, Chaine name)
{
    if (videG(g) || cardinal(g) == omega)
        return omega;
    Nat gauche = omega;
    Nat droit = g->nb_individus - 1;
    Nat res = g->nb_individus;
    while (gauche <= droit)
    {
        Nat millieu = gauche + (droit - gauche) / 2;
        if (millieu >= g->nb_individus || g->tab[millieu] == NULL)
        {
            return res;
        }
        Ent cmp = chaineCompare(name, g->tab[millieu]->nom);
        if (cmp <= omega)
        {
            res = millieu;
            droit = millieu - 1;
        }
        else
        {
            gauche = millieu + 1;
        }
    }
    return res;
}

// Recherche un individu par son nom et sa date de naissance.
// - Si la date de naissance est `dateNull`, retourne l'individu le plus jeune portant ce nom.
// - Sinon, retourne l'individu correspondant exactement au nom et à la date de naissance.
// Retourne NULL si aucun individu correspondant n’est trouvé.
Individu getByName(Genealogie g, Chaine name, Date naissance)
{
    if (videG(g) || cardinal(g) == omega)
        return NULL;
    Nat pos = getPos(g, name);
    Individu best = NULL;
    while (pos < g->taille_max_tab && g->tab[pos] != NULL &&
           chaineCompare(g->tab[pos]->nom, name) == omega)
    {
        if (dateNull(naissance) || compDate(g->tab[pos]->naiss, naissance) == omega)
        {
            if (best == NULL || compDate(g->tab[pos]->naiss, best->naiss) > 0)
            {
                best = g->tab[pos];
            }
        }
        pos++;
    }
    return best;
}

// modificateurs-----------------------------------------------------------------------------------------------
// Insertion d'un individu en position pos avec tous les champs p m n d deja precises
void insert(Genealogie g, Nat pos, Chaine s, Ident p, Ident m, Date n, Date d)
{
    Individu nouvInd = nouvIndividu(++(g->id_cur), s, p, m, n, d);
    if (nouvInd == NULL)
    {
        g->id_cur--;
        return;
    } // Creation d'individu
    if (pos > g->nb_individus)
    {
        freeIndividu(nouvInd);
        g->id_cur--;
        return;
    }
    Nat capacite_indv = cardinal(g);
    if (g->nb_individus == g->taille_max_rang) // Reallocation du rang a 2 fois plus 
    { 
        Nat *new_rang = REALLOC(g->rang, Nat, g->taille_max_rang * 2);
        if (new_rang == NULL)
        {
            freeIndividu(nouvInd);
            g->id_cur--;
            return;
        }
        g->rang = new_rang;
        g->taille_max_rang *= 2;
    }
    if (g->nb_individus == g->taille_max_tab) // Reallocation du tab a 2 fois plus 
    { 
        Individu *new_tab = REALLOC(g->tab, Individu, g->taille_max_tab * 2);
        if (new_tab == NULL)
        {
            freeIndividu(nouvInd);
            g->id_cur--;
            return;
        }
        g->tab = new_tab;
        g->taille_max_tab *= 2;
    }
    while (capacite_indv > pos) // Parcours du dernier element jusqu'a pos pour bien mettre individu dans la position
    { 
        g->rang[g->tab[capacite_indv - 1]->id - 1] = capacite_indv;
        g->tab[capacite_indv] = g->tab[capacite_indv - 1];
        capacite_indv--;
    }

    // Insertion dans pos
    g->tab[pos] = nouvInd;
    g->nb_individus += 1;
    g->rang[g->id_cur - 1] = pos;
}

// Insertion dans la liste des fils de idx, avec idx deja insere dans la genealogie
//  PRE: getByIdent(g,x)!=NULL) && getByIdent(g,filsa)!=NULL &&  (pp!=omega || mm!=omega)
void adjFils(Genealogie g, Ident idx, Ident fils, Ident p, Ident m)
{
    if (videG(g) || idx == omega || (p == omega && m == omega))
        return;
    Individu nouvEnf = getByIdent(g, idx);
    if (nouvEnf == NULL)
        return;
    // Mettre a jour des liasons parent - enfant
    nouvEnf->ipere = p;
    nouvEnf->imere = m;
    nouvEnf->icadet = omega;
    nouvEnf->ifaine = omega;
    Individu pere = (p != omega) ? getByIdent(g, p) : NULL;
    Individu mere = (m != omega) ? getByIdent(g, m) : NULL;
    // Si les parents ont pas des enfants
    if (fils == omega)
    {
        if (pere != NULL)
            pere->ifaine = idx;
        if (mere != NULL)
            mere->ifaine = idx;
        return;
    }
    Individu filsAine = getByIdent(g, fils);
    if (filsAine == NULL)
        return;
    Ent dateComp = compDate(nouvEnf->naiss, filsAine->naiss); // Comparaisons les dates de naissance
    // Si idx est le plus grand enfant
    if (dateComp < 0)
    {
        nouvEnf->icadet = fils;
        if (pere != NULL && pere->ifaine == fils)
            pere->ifaine = idx;
        if (mere != NULL && mere->ifaine == fils)
            mere->ifaine = idx;
        // S'ils ont la meme date de naissance alors on tri par nom
    }
    else if (dateComp == 0)
    {
        Ent nomComp = chaineCompare(nouvEnf->nom, filsAine->nom);
        if (nomComp < 0)
        {
            nouvEnf->icadet = fils;
            if (pere != NULL && pere->ifaine == fils)
                pere->ifaine = idx;
            if (mere != NULL && mere->ifaine == fils)
                mere->ifaine = idx;
        }
        else
        {
            nouvEnf->icadet = filsAine->icadet;
            filsAine->icadet = idx;
        }
        // Si l'enfant aine reste en tete, on cherche avec une boucle
    }
    else
    {
        Individu autreFils = getByIdent(g, filsAine->icadet);
        if (autreFils == NULL)
        {
            filsAine->icadet = idx;
            return;
        }
        while (autreFils != NULL && compDate(nouvEnf->naiss, autreFils->naiss) > 0)
        {
            filsAine = autreFils;
            autreFils = getByIdent(g, autreFils->icadet);
        }
        if (autreFils == NULL)
        {
            filsAine->icadet = idx;
            return;
        }
        filsAine->icadet = idx;
        nouvEnf->icadet = autreFils->id;
    }
}

// Insertion d'un individu dans la genealogie par default
Ident adj(Genealogie g, Chaine s, Ident p, Ident m, Date n, Date d)
{
    if (s == NULL || s[0] == omega || s[0] == '\0' || n.jour == omega || n.mois == omega || n.annee == omega)
        return omega;
    Nat pos = getPos(g, s);
    // Verifions is la position est valide et qu'il n'y a pas l'individu deja en genealogie
    if (pos < g->nb_individus && g->tab[pos] != NULL)
    {
        Individu existe = g->tab[pos];
        if (chaineCompare(s, existe->nom) == omega &&
            compDate(n, existe->naiss) == omega)
        {
            return omega;
        }
    }
    insert(g, pos, s, p, m, n, d);

    // On mets a jour les liaison parentales
    Ident nouv_id = g->id_cur;
    Ident faine;
    if (p != omega)
    {
        faine = getByIdent(g, p)->ifaine;
    }
    else if (m != omega)
    {
        faine = getByIdent(g, m)->ifaine;
    }
    else
    {
        faine = omega;
    }
    adjFils(g, nouv_id, faine, p, m);
    return nouv_id;
}

// PARTIE 2:-----------------------------------------------------------------------------------------------
//  Affiche les frères et sœurs d’un individu
void affiche_freres_soeurs(Genealogie g, Ident x, Chaine buff)
{
    buff[0] = '\0';
    if (g == NULL || x == omega)
        return;
    Individu cur = getByIdent(g, x);
    if (cur == NULL)
        return;
    Ident idP = cur->ipere;
    Ident idM = cur->imere;
    if (idP == omega && idM == omega)
        return; // Pour avoir des freres ou soeurs il faut qu'au moins un parent existe

    Ident parId = (idP != omega) ? idP : idM; 
    Individu parent = getByIdent(g, parId);
    if (parent == NULL || parent->ifaine == omega)
        return;
    Ident enf = parent->ifaine;
    Individu enfant;
    while (enf != omega)
    { // On parcours tous les enfants du parents de x
        enfant = getByIdent(g, enf);
        if (enfant == NULL)
            return;
        if (chaineCompare(enfant->nom, cur->nom) != omega)
        {
            ajoutNomEnBuf(buff, enfant->nom);
        } // nom de x exclu
        enf = enfant->icadet;
    }
}

// Affiche les enfants directs de l’individu
void affiche_enfants(Genealogie g, Ident x, Chaine buff)
{
    buff[0] = '\0';
    if (g == NULL || x == omega)
        return;
    Individu parent = getByIdent(g, x);
    if (parent == NULL || parent->ifaine == omega)
        return;
    Ident enf = parent->ifaine; // recupere le premier enfant
    Individu enfant;
    while (enf != omega)
    { 
        enfant = getByIdent(g, enf);
        if (enfant == NULL)
            return;
        ajoutNomEnBuf(buff, enfant->nom);
        enf = enfant->icadet;
    }
}

// Affiche les cousins de l’individu
void affiche_cousins(Genealogie g, Ident x, Chaine buff)
{
    buff[0] = '\0';
    if (g == NULL || x == omega)
        return;
    Individu cur = getByIdent(g, x);
    if (cur == NULL)
        return;
    Ident parents[2] = {cur->ipere, cur->imere};

    for (Ident i = 0; i < 2; i++) // Parcours des freres/soeurs de chaque parent
    { 
        if (parents[i] != omega)
        {
            Individu parent = getByIdent(g, parents[i]);
            Ident grandparents[2] = {parent->ipere, parent->imere};
            for (Ident j = 0; j < 2; j++)
            {
                if (grandparents[j] != omega)
                {
                    Individu grandparent = getByIdent(g, grandparents[j]);
                    Ident sibId = grandparent->ifaine; 
                    while (sibId != omega)
                    {
                        Individu oncle = getByIdent(g, sibId);
                        if (oncle && oncle->id != parent->id)
                        {
                            Ident cousinId = oncle->ifaine;
                            while (cousinId != omega)
                            {
                                Individu cousin = getByIdent(g, cousinId); 
                                if (cousin && chercheChaine(buff, cousin->nom) == NULL)
                                { 
                                    ajoutNomEnBuf(buff, cousin->nom);
                                }
                                cousinId = cousin->icadet;
                            }
                        }
                        sibId = oncle->icadet;
                    }
                }
            }
        }
    }
}
// Affiche les oncles et tantes de l’individu
void affiche_oncles(Genealogie g, Ident x, Chaine buff)
{
    buff[0] = '\0';
    if (g == NULL || x == omega)
        return;
    Individu current = getByIdent(g, x);
    if (current == NULL)
        return;
    Ident parents[2];
    parents[0] = current->ipere;
    parents[1] = current->imere;
    for (Ident i = 0; i < 2; i++) // Parcours des freres/soeurs de chaque parent
    {
        if (parents[i] != omega)
        {
            Individu parent = getByIdent(g, parents[i]);
            Ident grandparents[2] = {parent->ipere, parent->imere};
            for (Ident j = 0; j < 2; j++)
            {
                if (grandparents[j] != omega)
                {
                    Individu grandparent = getByIdent(g, grandparents[j]);
                    Ident sibId = grandparent->ifaine; // On recupere la liste des freres/soeurs de chaque parent
                    while (sibId != omega)
                    {
                        Individu oncle = getByIdent(g, sibId);
                        if (oncle->id != current->imere && oncle->id != current->ipere)
                        {
                            if (chercheChaine(buff, oncle->nom) == NULL)
                            { // Si pas deja dans le buff
                                ajoutNomEnBuf(buff, oncle->nom);
                            }
                        } // On ajout un oncle
                        sibId = oncle->icadet;
                    }
                }
            }
        }
    }
}

// PARTIE 3:-----------------------------------------------------------------------------------------------
//  PRE : (x->imere == omega || x->ipere == omega) || (y->imere == omega || y->ipere == omega) &&
//  x != omega && y != omega
void deviennent_freres_soeurs(Genealogie g, Ident x, Ident y)
{
    if (g == NULL || x == omega || y == omega || x == y)
        return;
    Individu indX = getByIdent(g, x);
    Individu indY = getByIdent(g, y);

    if (indX == NULL || indY == NULL)
        return;
    if (indX->ipere != omega && indY->ipere != omega && indX->ipere != indY->ipere)
        return;
    if (indX->imere != omega && indY->imere != omega && indX->imere != indY->imere)
        return;
    // Si les deux ont des parents differents, ils ne peuvent pas devenir freres ou soeurs

    Individu srcInd = NULL;
    Individu targetInd = NULL;

    if ((indX->ipere != omega || indX->imere != omega) &&
        (indY->ipere == omega && indY->imere == omega))
    {
        srcInd = indX;
        targetInd = indY;
    }
    else if ((indY->ipere != omega || indY->imere != omega) &&
             (indX->ipere == omega && indX->imere == omega))
    {
        srcInd = indY;
        targetInd = indX;
    }
    else
    {
        return;
    }

    Ident srcPar = (srcInd->imere != omega) ? srcInd->imere : srcInd->ipere;
    Ident filsAine = (getByIdent(g, srcPar) != NULL) ? getByIdent(g, srcPar)->ifaine : omega;
    Ident srcP = srcInd->ipere;
    Ident srcM = srcInd->imere;
    adjFils(g, targetInd->id, filsAine, srcP, srcM);
}

// PRE : y->ipere == 0 && x != omega &&
// y != omega
void devient_pere(Genealogie g, Ident x, Ident y)
{
    if (g == NULL || x == omega || y == omega || x == y)
        return;
    Individu pere = getByIdent(g, x);
    Individu enfant = getByIdent(g, y);
    Bool forLoop = true;
    if (pere == NULL || enfant == NULL)
        return;
    if (enfant->ipere != omega && enfant->ipere != x)
        return;
    enfant->ipere = x;
    if (pere->ifaine == omega)
    {
        pere->ifaine = y;
    }
    else
    {
        Ident curEnfId = pere->ifaine;
        Individu curEnf;
        while (curEnfId != omega && forLoop)
        {
            curEnf = getByIdent(g, curEnfId);
            if (curEnfId == y)
                return;
            if (curEnf->icadet == omega)
            {
                curEnf->icadet = y;
                forLoop = false;
            }
            curEnfId = curEnf->icadet;
        }
    }
}

// PRE : y->imere == 0 && x != omega &&
// y != omega
void devient_mere(Genealogie g, Ident x, Ident y)
{
    if (g == NULL || x == omega || y == omega || x == y)
        return;
    Individu mere = getByIdent(g, x);
    Individu enfant = getByIdent(g, y);
    Bool forLoop = true;
    if (mere == NULL || enfant == NULL)
        return;
    if (enfant->imere != omega && enfant->imere != x)
        return;
    enfant->imere = x;
    if (enfant->ipere != omega)
    {
        return;
    }
    if (mere->ifaine == omega)
    {
        mere->ifaine = y;
    }
    else
    {
        Ident curEnfId = mere->ifaine;
        Individu curEnf;
        while (curEnfId != omega && forLoop)
        {
            curEnf = getByIdent(g, curEnfId);
            if (curEnfId == y)
                return;
            if (curEnf->icadet == omega)
            {
                curEnf->icadet = y;
                forLoop = false;
            }
            curEnfId = curEnf->icadet;
        }
    }
}

// PARTIE 4:-----------------------------------------------------------------------------------------------
// PRE: None
//  Verifier si x est un ancetre de y (parent, grand-parent, etc.)
Bool estAncetre(Genealogie g, Ident x, Ident y)
{
    if (x == omega || y == omega)
        return false;
    Individu ind = getByIdent(g, y);
    if (ind->ipere == x || ind->imere == x)
        return true;
    if (ind->ipere == omega && ind->imere == omega)
        return false;
    else
        return estAncetre(g, x, ind->ipere) || estAncetre(g, x, ind->imere);
}

// PRE: None
//  Verifier si les deux individus on un ancetre commun
Bool ontAncetreCommun(Genealogie g, Ident x, Ident y)
{
    if (x == omega || y == omega)
        return false;
    Individu ind_x = getByIdent(g, x);
    Individu ind_y = getByIdent(g, y);
    if (ind_x == NULL || ind_y == NULL)
        return false;
    if (ind_x->ipere == ind_y->ipere && ind_x->ipere != omega)
        return true;
    if (ind_x->imere == ind_y->imere && ind_x->imere != omega)
        return true;
    else
    {
        return ontAncetreCommun(g, ind_x->ipere, ind_y->ipere) || ontAncetreCommun(g, ind_x->ipere, ind_y->imere) ||
               ontAncetreCommun(g, ind_x->imere, ind_y->ipere) || ontAncetreCommun(g, ind_x->imere, ind_y->imere);
    }
}
// PRE: None
//  Obtenir le plus ancien ancetre d'un individu
Ident plus_ancien(Genealogie g, Ident x)
{
    if (x == omega)
        return omega;
    Individu ind = getByIdent(g, x);
    if (ind == NULL)
        return omega;
    if (ind->ipere == omega && ind->imere == omega)
        return x;
    if (ind->ipere == omega)
        return plus_ancien(g, ind->imere);
    if (ind->imere == omega)
        return plus_ancien(g, ind->ipere);
    else
    {
        Ident indPere = getByIdent(g, ind->id)->ipere;
        Ident indMere = getByIdent(g, ind->id)->imere;
        Date naissP = getByIdent(g, plus_ancien(g, indPere))->naiss;
        Date naissM = getByIdent(g, plus_ancien(g, indMere))->naiss;
        Ent compareEntreDeux = compDate(naissP, naissM);
        return compareEntreDeux < 0 ? plus_ancien(g, indPere) : plus_ancien(g, indMere);
    }
}

// Afficher par niveau la parente d'un individu
void affiche_parente(Genealogie g, Ident x, Chaine buff)
{
    buff[0] = '\0';
    if (g == NULL || x == omega)
        return;
    Individu person = NULL;
    Bool forLoop = true;
    for (Nat i = 0; i < g->nb_individus && forLoop; i++)
    {
        if (g->tab[i]->id == x)
        {
            person = g->tab[i];
            forLoop = false;
        }
    }
    Chaine levels[max_profondeur]; // la profondeur maximale de l'arbre
    for (Nat i = 0; i < max_profondeur; i++)
    {
        levels[i] = MALLOCN(Car, max_longueur);
        if (levels[i] == NULL)
            return;
        levels[i][0] = '\0';
    }
    Nat maxNiv = 0;
    if (person == NULL)
    {
        for (Nat i = 0; i < max_profondeur; i++)
        {
            FREE(levels[i]);
        }
        return;
    }
    // On aplique la parente vers le parents en augmentant le niveau par 1 a l'aide d'un function recursive
    if (person->ipere != omega)
    {
        affParentRescursive(g, person->ipere, 1, levels, &maxNiv);
    }
    if (person->imere != omega)
    {
        affParentRescursive(g, person->imere, 1, levels, &maxNiv);
    }
    for (Nat i = 1; i <= maxNiv; i++)
    {
        if (levels[i][0] != '\0')
        {
            chaineConcat(buff, " - ");
            Car levelStr[3];
            if (i < 10)
            {
                levelStr[0] = '0' + i;
                levelStr[1] = '\0';
            }
            else
            {
                levelStr[0] = '1';
                levelStr[1] = '0';
                levelStr[2] = '\0';
            }
            chaineConcat(buff, levelStr);
            chaineConcat(buff, " :\n");
            chaineConcat(buff, levels[i]);
            chaineConcat(buff, "\n");
        }
    }
    for (Nat i = 0; i < max_profondeur; i++)
    {
        FREE(levels[i]);
    }
}

// Afficher la descendance jusqu'a present d'un individu par niveau
void affiche_descendance(Genealogie g, Ident x, Chaine buff)
{
    buff[0] = '\0';
    if (g == NULL || x == omega)
        return;
    Chaine levels[max_profondeur]; // la profondeur maximale de l'arbre
    for (Nat i = 0; i < max_profondeur; i++)
    {
        levels[i] = MALLOCN(Car, max_longueur);
        if (levels[i] == NULL)
            return;
        levels[i][0] = '\0';
    }
    Nat maxNiv = 0;
    affRecursiveDescend(g, x, 1, levels, &maxNiv);
    Nat buf_pos = 0;
    for (Nat i = 1; i <= maxNiv; i++)
    {
        buff[buf_pos++] = '-';
        buff[buf_pos++] = ' ';
        if (i >= 10)
        {
            buff[buf_pos++] = '1';
            buff[buf_pos++] = '0';
        }
        else
        {
            buff[buf_pos++] = '0' + i;
        }
        buff[buf_pos++] = ' ';
        buff[buf_pos++] = ':';
        buff[buf_pos++] = '\n';
        for (Nat j = 0; levels[i][j] != '\0'; j++)
        {
            buff[buf_pos++] = levels[i][j];
        }
        buff[buf_pos++] = '\n';
    }
    buff[buf_pos] = '\0';
    for (Nat i = 0; i < max_profondeur; i++)
    {
        FREE(levels[i]);
    }
}

//
/// VOS FONCTIONS AUXILIAIRES
/// ///////////////////////////////////////////////////////
///
// Date 0/0/0
Nat dateNull(Date d1)
{
    return d1.annee == 0 && d1.mois == 0 && d1.jour == 0;
}

// Copie du fonctionne chaineConcat
void ajoutNomEnBuf(Chaine buff, Chaine nom)
{
    Nat current_len = chaineLongueur(buff);
    Nat nom_len = chaineLongueur(nom);
    if (current_len + nom_len + 2 > 10 * LG_MAX)
    {
        return;
    }
    if (buff[0] == '\0' || (buff[0] >= 0 && buff[0] <= 32))
    {
        Nat j = 0;
        while (nom[j] != '\0')
        {
            buff[j] = nom[j];
            j++;
        }
        buff[j] = '\0';
        return;
    }
    else
    {
        Nat i = 0;
        while (buff[i] != '\0')
        {
            i++;
        }
        buff[i++] = ' ';
        Nat j = 0;
        while (nom[j] != '\0')
        {
            buff[i++] = nom[j++];
            buff[i] = '\0';
        }
    }
}

// Recherche d'un substring dans un string
Chaine chercheChaine(Chaine buff, Chaine rech)
{
    if (!*rech)
        return buff;
    for (Chaine p = buff; *p; p++)
    {
        Chaine h = p;
        Chaine n = rech;
        while (*h && *n && *h == *n)
        {
            h++;
            n++;
        }
        if (!*n)
            return p;
    }
    return NULL;
}

// Affichage du descendance recursive
void affRecursiveDescend(Genealogie g, Ident x, Nat niveau, Chaine *levels, Nat *maxNiv)
{
    if (g == NULL || x == omega)
        return;
    Individu current = getByIdent(g, x);
    if (current == NULL || current->ifaine == omega)
        return;
    if (niveau > *maxNiv)
    {
        *maxNiv = niveau;
    }
    Ident childId = current->ifaine;
    while (childId != omega)
    {
        Individu child = getByIdent(g, childId);
        if (child != NULL && chercheChaine(levels[niveau], child->nom) == NULL)
        {
            ajoutNomEnBuf(levels[niveau], child->nom);
        }
        affRecursiveDescend(g, childId, niveau + 1, levels, maxNiv);
        childId = child->icadet;
    }
}

// Ajouter au meme niveau avec des espaces entre les individus
void ajoutEnNivBuf(Chaine buff, Chaine nom)
{
    if (buff[0] != '\0')
    {
        chaineConcat(buff, " ");
    }
    chaineConcat(buff, nom);
}

// Verifier si le nom existe dans le buffer
Bool nomEstEnBuf(Chaine buffer, Chaine nom)
{
    Chaine trouve = chercheChaine(buffer, nom);
    if (trouve == NULL)
        return false;
    Nat LengthNom = chaineLongueur(nom);
    Bool estValidAv = (trouve == buffer) || (*(trouve - 1) == ' ');
    Bool estValidAp = (trouve[LengthNom] == '\0') || (trouve[LengthNom] == ' ');
    return estValidAv && estValidAp;
}

// Affichage du parente recursive
void affParentRescursive(Genealogie g, Ident x, Nat niveau, Chaine *levels, Nat *maxNiv)
{
    if (g == NULL || x == omega)
        return;
    Individu curr = NULL;
    Bool bouc = true;
    for (Nat i = 0; i < g->nb_individus && bouc; i++)
    {
        if (g->tab[i]->id == x)
        {
            curr = g->tab[i];
            bouc = false;
        }
    }
    if (curr == NULL)
        return;
    if (niveau > *maxNiv)
    {
        *maxNiv = niveau;
    }
    if (!nomEstEnBuf(levels[niveau], curr->nom))
    {
        ajoutEnNivBuf(levels[niveau], curr->nom);
    }
    for (Nat i = 0; i < g->nb_individus; i++)
    {
        Individu indiv = g->tab[i];
        if (indiv->id != x &&
            ((indiv->ipere == curr->ipere && indiv->ipere != omega) ||
             (indiv->imere == curr->imere && indiv->imere != omega)))
        {
            if (!nomEstEnBuf(levels[niveau], indiv->nom))
            {
                ajoutEnNivBuf(levels[niveau], indiv->nom);
            }
        }
    }
    if (curr->ipere != omega)
    {
        affParentRescursive(g, curr->ipere, niveau + 1, levels, maxNiv);
    }

    if (curr->imere != omega)
    {
        affParentRescursive(g, curr->imere, niveau + 1, levels, maxNiv);
    }
}

Bool videG(Genealogie g)
{
    return g == NULL;
}

//
/// MAIN
/// ///////////////////////////////////////////////////////
///
int main()
{
    Car buf[500];
    Genealogie g;

    printf("******* Init:\n\n");
    genealogieInit(&g);
    printf("done.\n");

    printf("\n******* adj+getByIndent:\n");
    Date dnull = {0, 0, 0};
    // Famille Potter
    Date jhen = {16, 2, 1867};
    Date jhed = {21, 11, 1932};
    Ident ihep = adj(g, "Henri", 0, 0, jhen, jhed);

    Date jfn = {30, 7, 1905};
    Date jfd = {2, 3, 1979};
    Ident ijfl = adj(g, "Fleamont", ihep, 0, jfn, jfd);

    Date jeu = {12, 6, 1907};
    Date jed = {14, 1, 1979};
    Ident ijm = adj(g, "Euphemia", 0, 0, jeu, jed);

    Date jpn = {27, 3, 1960};
    Date jpd = {29, 7, 1981};
    Ident ijp = adj(g, "James", ijfl, ijm, jpn, jpd);

    Date lpn = {30, 1, 1960};
    Date lpd = {29, 7, 1981};
    Ident ilp = adj(g, "Lily", 0, 0, lpn, lpd);

    Date hn = {31, 7, 1980};
    Ident ih = adj(g, "Harry", ijp, ilp, hn, dnull);

    // Famille Weasley
    Date an = {6, 2, 1950};
    Ident iaw = adj(g, "Arthur", 0, 0, an, dnull);

    Date dpre = {8, 4, 1910};
    Date ddpre = {23, 10, 1968};
    Ident ipre = adj(g, "Prewett", 0, 0, dpre, ddpre);

    Date dfab = {12, 5, 1946};
    Date ddfab = {21, 12, 1982};
    Ident ifab = adj(g, "Fabian", ipre, 0, dfab, ddfab);

    Date mn = {30, 10, 1949};
    Ident imw = adj(g, "Molly", ipre, 0, mn, dnull);

    // ajouter ici les autres individus
    Date dginny = {11, 8, 1981};
    Ident ig = adj(g, "Ginny", iaw, imw, dginny, dnull);

    Date dron = {1, 3, 1980};
    Ident ir = adj(g, "Ron", iaw, imw, dron, dnull);

    Date dalbus = {15, 8, 2006};
    Ident ia2 = adj(g, "Albus", ih, ig, dalbus, dnull);

    Date dbill = {29, 11, 1970};
    Ident ibill = adj(g, "Bill", iaw, imw, dbill, dnull);

    Date djames2 = {12, 8, 2004};
    Ident ij = adj(g, "James", ih, ig, djames2, dnull);

    Date dhermione = {19, 9, 1979};
    Ident ihg = adj(g, "Hermione", 0, 0, dhermione, dnull);

    // Fred Weasley
    Date dfred = {1, 4, 1978};
    Ident ifred = adj(g, "Fred", iaw, imw, dfred, dnull);
    ifred = ifred;

    // George Weasley
    Date dgeorge = {1, 4, 1978};
    Ident igeorge = adj(g, "George", iaw, imw, dgeorge, dnull);
    igeorge = igeorge;

    Date drose = {15, 2, 2006};
    Ident irose = adj(g, "Rose", ir, ihg, drose, dnull);

    // Lily Luna Potter (daughter of Harry and Ginny)
    Date dlily2 = {17, 2, 2008};
    Ident ilily2 = adj(g, "Lily", ih, ig, dlily2, dnull);
    ilily2 = ilily2;

    for (Nat i = 0; i < cardinal(g); i++)
    {
        printf("%s\n", nomIndividu(kieme(g, i)));
    }
    printf("nb_individus: %d\n", cardinal(g));
    printf("Identifiant de Fabian: %u (must be 9)\n", ifab);
    printf("Identifiant de Arthur: %u (must be 7)\n", iaw);

    printf("\nAdding more people:\n");
    Date dgid = {7, 2, 1945};
    Date ddgid = {21, 12, 1982};
    Ident igid = adj(g, "Gideon", 0, 0, dgid, ddgid);
    printf("Linking Gideon as brother of Molly\n");
    deviennent_freres_soeurs(g, igid, imw);
    Date dhugo = {19, 4, 2008};
    Ident ihugo = adj(g, "Hugo", 0, 0, dhugo, dnull);
    printf("Linking Hugo as son of Hermione\n");
    devient_mere(g, ihg, ihugo);

    printf("\nTry to add a double Harry:\n");
    Date hu2n = {31, 7, 1980};
    Ident ihu2 = adj(g, "Harry", 0, 0, hu2n, dnull);
    if (ihu2 != omega)
        printf("oh, no! Added double Harry!\n");
    else
        printf("No double Harry: OK!\n");
    printf("Adding more Albus...\n");
    Date ho2n = {1, 8, 1909};
    Ident itho2 = adj(g, "Albus", 0, 0, ho2n, dnull);
    if (itho2 == omega)
        printf("what! Could not add 2nd Albus!\n");
    else
        printf("2nd Albus added: OK!\n");
    Date ho3n = {2, 2, 1832};
    Ident itho3 = adj(g, "Albus", 0, 0, ho3n, dnull);
    if (itho3 == omega)
        printf("what! Could not add 3rd Albus!\n");
    else
        printf("3rd Albus added: OK!\n");
    Individu idch = getByName(g, "Albus", dnull);
    if (idch != NULL)
        printf("Youngest Albus, date birth %d:%d:%d\n", naissIndividu(idch).jour, naissIndividu(idch).mois, naissIndividu(idch).annee);
    else
        printf("what? no Albus! There is a serious problem here...\n");
    printf("Now nb_individus: %d\n", cardinal(g));

    printf("\n******* fratrie:\n");
    printf("Freres/Soeurs de %s:\n", nomIndividu(getByIdent(g, ig)));
    buf[0] = 0;
    affiche_freres_soeurs(g, ig, buf);
    printf("%s\n", buf);
    printf("Freres/Soeurs de %s:\n", nomIndividu(getByIdent(g, imw)));
    buf[0] = 0;
    affiche_freres_soeurs(g, imw, buf);
    printf("%s\n", buf);

    printf("\n******* les enfants:\n");
    printf("Enfants de %s:\n", nomIndividu(getByIdent(g, ipre)));
    buf[0] = 0;
    affiche_enfants(g, ipre, buf);
    printf("%s\n", buf);
    printf("Enfants de %s:\n", nomIndividu(getByIdent(g, imw)));
    buf[0] = 0;
    affiche_enfants(g, imw, buf);
    printf("%s\n", buf);
    printf("Enfants de %s:\n", nomIndividu(getByIdent(g, ir)));
    buf[0] = 0;
    affiche_enfants(g, ir, buf);
    printf("%s\n", buf);

    printf("\n******* les cousins:\n");
    printf("Cousins de %s:\n", nomIndividu(getByIdent(g, irose)));
    buf[0] = 0;
    affiche_cousins(g, irose, buf);
    printf("%s\n", buf);

    printf("\n******* les oncles et tantes:\n");
    printf("Oncles/tantes de %s:\n", nomIndividu(getByIdent(g, ij)));
    buf[0] = 0;
    affiche_oncles(g, ij, buf);
    printf("%s\n", buf);
    printf("Oncles/tantes de %s:\n", nomIndividu(getByIdent(g, ir)));
    buf[0] = 0;
    affiche_oncles(g, ir, buf);
    printf("%s\n", buf);

    printf("\n******* les ancetres:\n");
    printf("%s ancetre de %s: %s\n", nomIndividu(getByIdent(g, ijfl)), nomIndividu(getByIdent(g, ia2)), estAncetre(g, ijfl, ia2) ? "oui" : "non");
    printf("%s ancetre de %s: %s\n", nomIndividu(getByIdent(g, ijfl)), nomIndividu(getByIdent(g, irose)), estAncetre(g, ijfl, irose) ? "oui" : "non");

    printf("\n******* les ancetres communs:\n");
    printf("%s et %s ont ancetre commun: %s\n", nomIndividu(getByIdent(g, irose)), nomIndividu(getByIdent(g, ij)), ontAncetreCommun(g, irose, ij) ? "oui" : "non");
    printf("%s et %s ont ancetre commun: %s\n", nomIndividu(getByIdent(g, ih)), nomIndividu(getByIdent(g, ibill)), ontAncetreCommun(g, ih, ibill) ? "oui" : "non");

    printf("\n******* ancetre plus ancien:\n");
    printf("L'ancetre le plus ancien de %s est %s\n", nomIndividu(getByIdent(g, ia2)), nomIndividu(getByIdent(g, plus_ancien(g, ia2))));

    printf("\n******* parente:\n");
    printf("parents de %s\n", nomIndividu(getByIdent(g, ia2)));
    buf[0] = 0;
    affiche_parente(g, ia2, buf);
    printf("%s\n", buf);

    printf("\n******* descendence:\n");
    printf("descendence de %s\n", nomIndividu(getByIdent(g, ijm)));
    buf[0] = 0;
    affiche_descendance(g, ijm, buf);
    printf("%s\n", buf);

    printf("\n******* free:\n");
    genealogieFree(&g);
    printf("fin.(press key)\n");
    fgets(buf, 2, stdin);
    return 0;
}