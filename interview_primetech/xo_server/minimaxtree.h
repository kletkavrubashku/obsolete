#ifndef MINIMAXTREE_H
#define MINIMAXTREE_H

#include "minimaxnode.h"

class MiniMaxTree {

public:
                    MiniMaxTree();
                    ~MiniMaxTree();
    MiniMaxNode*    getRoot();

    void            growTree(); // строим дерево

private:
    MiniMaxNode* root;

};

#endif // MINIMAXTREE_H
