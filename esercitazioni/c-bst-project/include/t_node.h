#ifndef T_NODE_H
#define T_NODE_H

typedef struct _t_node {
    int key;
    void *value;
    struct _t_node *left;
    struct _t_node *right;
    struct _t_node *parent;
} *t_node;

t_node make_t_node(void);

#endif // T_NODE_H