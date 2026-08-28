# Family Tree

This project implements a data structure and algorithms for managing a **family tree** in **C**, as part of the *Data Structures and Algorithms II* course at the University of Strasbourg (2024-25).

## Project Description

The project models a genealogy where each individual has exactly two parents (father and mother). The structure relies on a table of individuals with dual indexing:
- by **identifier** — direct access in O(1)
- by **name** — binary search in O(log n)

## Implemented Features

### 1. Tree Construction
- Initialization and freeing of the genealogy (`genealogieInit`, `genealogieFree`)
- Creation and deletion of individuals (`nouvIndividu`, `freeIndividu`)
- Access by identifier with constant complexity (`getByIdent`)
- Search by name using binary search (`getByName`, `getPos`)
- Automatic addition of an individual with kinship link updates (`adj`, `insert`, `adjFils`)
- Automatic management of the children's list, sorted from oldest to youngest

### 2. Display
- Retrieval of all siblings (`affiche_freres_soeurs`)
- Retrieval of all children (`affiche_enfants`)
- Retrieval of all cousins (`affiche_cousins`)
- Retrieval of all uncles/aunts (`affiche_oncles`)

### 3. Kinship Links
- Merging sibling groups (`deviennent_freres_soeurs`)
- Adding a father after the fact (`devient_pere`)
- Adding a mother after the fact (`devient_mere`)

### 4. Tree Traversal
- Ancestor test (`estAncetre`)
- Common ancestor test between two individuals (`ontAncetreCommun`)
- Search for the oldest ancestor (`plus_ancien`)
- Display of all ancestry by generation (`affiche_parente`)
- Display of all descendants by generation (`affiche_descendance`)

## Skills Acquired

- Dynamic memory management in C (`malloc`, `realloc`, `free`)
- Implementation of complex data structures
- Binary search algorithms
- Recursive traversal of family trees
- Dual indexing for access optimization
- Sorting and ordered insertion in dynamic arrays
- Date comparison and edge case management
- Strict compliance with pure C programming constraints (no C++ types, no global variables, no `printf` outside of main)

## Code Structure

The project is contained in a single `genealogie.c` file that can be compiled with `gcc` on the Turing server without errors or warnings.
```bash
gcc -o genealogie genealogie.c
