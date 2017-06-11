#include "minimaxtree.h"

MiniMaxTree::MiniMaxTree() {
    root = new MiniMaxNode(NULL);
}

MiniMaxTree::~MiniMaxTree() {
    delete root;
}

void MiniMaxTree::growTree() {
    MiniMaxNode* level_of_node = root;
    int xo_who_win;
    // циклами по всем ситуациям
    for(int a = 0; a < 9; a++) {
        level_of_node = new MiniMaxNode(level_of_node, a);
        for(int b = 0; b < 9; b++) {
            if (b == a) continue;
            level_of_node = new MiniMaxNode(level_of_node, b);
            for(int c = 0; c < 9; c++) {
                if (c == a) continue;
                if (c == b) continue;
                level_of_node = new MiniMaxNode(level_of_node, c);
                for(int d = 0; d < 9; d++) {
                    if (d == a) continue;
                    if (d == b) continue;
                    if (d == c) continue;
                    level_of_node = new MiniMaxNode(level_of_node, d);
                    for(int e = 0; e < 9; e++) {
                        if (e == a) continue;
                        if (e == b) continue;
                        if (e == c) continue;
                        if (e == d) continue;
                        level_of_node = new MiniMaxNode(level_of_node, e);
                        if (level_of_node->isTerminalNode(xo_who_win)) {
                            level_of_node->setEmpiricalValue(xo_who_win); // устанавливаем в листьях empirical_value
                            level_of_node = level_of_node->getParent();
                            continue;
                        }
                        for(int f = 0; f < 9; f++) {
                            if (f == a) continue;
                            if (f == b) continue;
                            if (f == c) continue;
                            if (f == d) continue;
                            if (f == e) continue;
                            level_of_node = new MiniMaxNode(level_of_node, f);
                            if (level_of_node->isTerminalNode(xo_who_win)) {
                                level_of_node->setEmpiricalValue(xo_who_win);
                                level_of_node = level_of_node->getParent();
                                continue;
                            }
                            for(int g = 0; g < 9; g++) {
                                if (g == a) continue;
                                if (g == b) continue;
                                if (g == c) continue;
                                if (g == d) continue;
                                if (g == e) continue;
                                if (g == f) continue;
                                level_of_node = new MiniMaxNode(level_of_node, g);
                                if (level_of_node->isTerminalNode(xo_who_win)) {
                                    level_of_node->setEmpiricalValue(xo_who_win);
                                    level_of_node = level_of_node->getParent();
                                    continue;
                                }
                                for(int h = 0; h < 9; h++) {
                                    if (h == a) continue;
                                    if (h == b) continue;
                                    if (h == c) continue;
                                    if (h == d) continue;
                                    if (h == e) continue;
                                    if (h == f) continue;
                                    if (h == g) continue;
                                    level_of_node = new MiniMaxNode(level_of_node, h);
                                    if (level_of_node->isTerminalNode(xo_who_win)) {
                                        level_of_node->setEmpiricalValue(xo_who_win);
                                        level_of_node = level_of_node->getParent();
                                        continue;
                                    }
                                    for(int i = 0; i < 9; i++) {
                                        if (i == a) continue;
                                        if (i == b) continue;
                                        if (i == c) continue;
                                        if (i == d) continue;
                                        if (i == e) continue;
                                        if (i == f) continue;
                                        if (i == g) continue;
                                        if (i == h) continue;
                                        level_of_node = new MiniMaxNode(level_of_node, i);
                                        level_of_node->isTerminalNode(xo_who_win);
                                        level_of_node->setEmpiricalValue(xo_who_win);
                                        level_of_node = level_of_node->getParent();
                                    }
                                    level_of_node->setEmpiricalValueFromChilds(); // получаем от детей empirical_value (минимакс)
                                    level_of_node = level_of_node->getParent();
                                }
                                level_of_node->setEmpiricalValueFromChilds();
                                level_of_node = level_of_node->getParent();
                            }
                            level_of_node->setEmpiricalValueFromChilds();
                            level_of_node = level_of_node->getParent();
                        }
                        level_of_node->setEmpiricalValueFromChilds();
                        level_of_node = level_of_node->getParent();
                    }
                    level_of_node->setEmpiricalValueFromChilds();
                    level_of_node = level_of_node->getParent();
                }
                level_of_node->setEmpiricalValueFromChilds();
                level_of_node = level_of_node->getParent();
            }
            level_of_node->setEmpiricalValueFromChilds();
            level_of_node = level_of_node->getParent();
        }
        level_of_node->setEmpiricalValueFromChilds();
        level_of_node = level_of_node->getParent();
    }
    level_of_node->setEmpiricalValueFromChilds();
}

MiniMaxNode* MiniMaxTree::getRoot() {
    return root;
}
