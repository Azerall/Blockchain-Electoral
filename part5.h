#ifndef PART5_H
#define PART5_H

typedef struct block_tree_cell {
	Block * block ;
	struct block_tree_cell * father ;
	struct block_tree_cell * firstChild ;
	struct block_tree_cell * nextBro ;
	int height ;
} CellTree ;

CellTree* create_node(Block* b);
int max(int a, int b);
int update_height(CellTree* father, CellTree* child);
void add_child(CellTree* father, CellTree* child);
void print_tree(CellTree* tree);
void delete_node(CellTree* node);
void delete_tree(CellTree* tree, int bool_aut, int bool_votes);
CellTree* highest_child(CellTree* cell);
CellTree* last_node(CellTree* tree);
void fusion_decl(CellProtected **cp1, CellProtected **cp2);
CellProtected* fusion_plus_longue (CellTree *tree);
void submit_vote(Protected* p);
void create_block(CellTree* tree, Key* author, int d);
void add_block(int d, char* name);
CellTree* read_tree();
Key* compute_winner_BT(CellTree* tree, CellKey* candidates, CellKey* voters, int sizeC, int sizeV);

#endif
