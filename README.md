# Arbre Généalogique 🌳

Ce projet implémente une structure de données et des algorithmes
pour la gestion d'un **arbre généalogique** en langage **C**,
dans le cadre du cours *Structures de données et algorithmes II*
à l'Université de Strasbourg (2024-25).

## Description du projet

Le projet modélise une généalogie où chaque individu possède
exactement deux parents (père et mère). La structure repose sur
une table d'individus avec deux indexations :
- par **identifiant** — accès direct en O(1)
- par **nom** — recherche dichotomique en O(log n)

## Fonctionnalités implémentées

### 1. Construction de l'arbre (7pts)
- Initialisation et libération de la généalogie (`genealogieInit`, `genealogieFree`)
- Création et suppression d'individus (`nouvIndividu`, `freeIndividu`)
- Accès par identifiant en complexité constante (`getByIdent`)
- Recherche par nom avec recherche dichotomique (`getByName`, `getPos`)
- Ajout automatique d'un individu avec mise à jour des liens
  de parenté (`adj`, `insert`, `adjFils`)
- Gestion automatique de la liste des enfants triée du plus âgé
  au plus jeune

### 2. Affichage (5pts)
- Récupération de tous les frères et sœurs (`affiche_freres_soeurs`)
- Récupération de tous les enfants (`affiche_enfants`)
- Récupération de tous les cousins/cousines (`affiche_cousins`)
- Récupération de tous les oncles/tantes (`affiche_oncles`)

### 3. Liens de parenté (2pts)
- Fusion de fratries (`deviennent_freres_soeurs`)
- Ajout d'un père après coup (`devient_pere`)
- Ajout d'une mère après coup (`devient_mere`)

### 4. Parcours de l'arbre (6pts)
- Test d'ancêtre (`estAncetre`)
- Test d'ancêtre commun entre deux individus (`ontAncetreCommun`)
- Recherche de l'ancêtre le plus ancien (`plus_ancien`)
- Affichage de toute la parenté par génération (`affiche_parente`)
- Affichage de toute la descendance par génération (`affiche_descendance`)

## Compétences acquises

- Gestion dynamique de la mémoire en C (MALLOC, REALLOC, FREE)
- Implémentation de structures de données complexes
- Algorithmes de recherche dichotomique
- Parcours récursif d'arbres généalogiques
- Double indexation pour optimisation des accès
- Tri et insertion ordonnée dans des tableaux dynamiques
- Comparaison de dates et gestion des cas limites
- Respect strict de contraintes de programmation C pur
  (sans types C++, sans variables globales, sans printf
  hors du main)

## Structure du code

Le projet est contenu dans un unique fichier `genealogie.c`
compilable avec `gcc` sur le serveur Turing sans erreur
ni warning.
```bash
gcc -o genealogie genealogie.c
```

## Exemple d'utilisation

L'arbre généalogique de la figure 1 (familles Potter et Weasley,
inspiré de Harry Potter) est utilisé comme exemple de test tout
au long du projet.
```c
Genealogie g;
genealogieInit(&g);
Ident ih = adj(g, "Harry", ijp, ilp, hn, dnull);
affiche_parente(g, ih, buff);
printf("%s\n", buff);
```

## Documentation complète

Pour une description détaillée du sujet, les schémas de structures,
les exemples complets et les figures illustratives, veuillez
consulter le fichier PDF inclus dans ce dépôt :

📄 [Voir le sujet complet (PDF)](docs/sujetDevoir_2025.pdf)

## Auteur

PavloT01 — Université de Strasbourg, 2024-25
