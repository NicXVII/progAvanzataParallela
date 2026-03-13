
struct _tree_node {
  int key;
  void *value;
  struct _tree_node *left;
  struct _tree_node *right;
  struct _tree_node *parent;
};

typedef struct _tree_node *t_node;

t_node make_t_node (void)
{
  t_node tmp = (t_node) malloc(sizeof(struct _tree_node));
  if (tmp == NULL) {
    errno = ENOMEM;
    perror("make_t_node - creazione nodo fallita");
  } else {
    tmp->value = tmp->left = tmp->right = tmp->parent = NULL;
    tmp->key = 0;
  }
  return tmp;
}

void delete_t_node (t_node tmp)
{
  if (tmp == NULL)
    return;
  if (tmp->value != NULL)
    free(tmp->value);
  free(tmp);
}