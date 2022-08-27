#ifndef BASE_BNS_H_
#define BASE_BNS_H_

/*
  (C) 2021 Nguyen Ba Ngoc (bangoc)
*/

#include "base/bn.h"

#define bns_child(n, order) ((order) < 0? (n)->left: (n)->right)
#define bns_child_ref(n, order) ((order) < 0? &((n)->left): &((n)->right))

#define bns_insert_setup(loc, root, data, cmp, same, parent) \
  do { \
    bn_node_t _x = (root); \
    int _order; \
    while (_x) { \
      (parent) = _x; \
      _order = cmp(data, _x); \
      if (!_order) { \
        (same) = _x; \
      } \
      _x = bns_child(_x, _order); \
    } \
    (loc) = (parent)? bns_child_ref(parent, _order): &(root); \
  } while (0)

#define bns_search_inline(o, t, u, cmp, ...) \
  do { \
    int _order; \
    bn_node_t _x = (t)->root; \
    bn_node_t o = NULL; \
    while (_x) { \
      _order = cmp(u, _x); \
      if (!_order) { \
        o = _x; \
        break; \
      } \
      _x = bns_child(_x, _order); \
    } \
    __VA_ARGS__; \
  } while (0)

// gte = greater than or equal
#define bns_search_gte_inline(o, t, u, cmp, ...) \
  do {\
    int _order; \
    bn_node_t _x = (t)->root; \
    bn_node_t o = NULL; \
    while (_x) { \
      _order = cmp(u, _x); \
      if (!_order) { \
        o = _x; \
        break; \
      } \
      if (_order < 0) { \
        o = _x; \
        _x = _x->left; \
        continue; \
      } \
      _x = _x->right; \
    } \
    __VA_ARGS__;\
  } while (0)

// lte = less than or equal
#define bns_search_lte_inline(o, t, u, cmp, ...) \
  do { \
    int _order; \
    bn_node_t _x = (t)->root; \
    bn_node_t o = NULL; \
    while (_x) { \
      _order = cmp(u, _x); \
      if (!_order) { \
        o = _x; \
        break; \
      } \
      if (_order > 0) { \
        o = _x; \
        _x = _x->right; \
        continue; \
      } \
      _x = _x->left; \
    } \
    __VA_ARGS__; \
  } while (0)


/* Giao diện hỗ trợ gtype */

#include "base/gtype.h"

typedef struct bns_node_g {
  struct _bn_node base;
  gtype key;
} *bns_node_g_t;

#define to_bns_node_g(n) ((bns_node_g_t)(n))
#define bns_node_g_key(n) (to_bns_node_g(n)->key)

typedef struct bns_tree_g {
  struct _bn_tree base;
  bn_compare_t cmp;
} *bns_tree_g_t;

#define to_bns_tree_g(t) ((bns_tree_g_t)(t))
#define bns_tree_g_cmp(t) (to_bns_tree_g(t)->cmp)

// Hỗ trợ giản lược cách viết bns_tree_g_cmp(t)(v1, v2)
//                      thành bns_cmp_g(t, v1, v2)
#define bns_cmp_g(t, v1, v2) bns_tree_g_cmp(t)(v1, v2)

// Chỉ dùng bên trong cho triển khai
#define bns_cmp_conv(u, x) cmp(u, bns_node_g_key(x))

bn_node_t bns_create_node_g(gtype key);
bn_tree_t bns_create_tree_g(bn_node_t root, bn_compare_t cmp);

bn_node_t bns_insert_g(bn_tree_t t, gtype key);
bn_node_t bns_search_g(bn_tree_t t, gtype key);
bn_node_t bns_search_gte_g(bn_tree_t t, gtype key);
bn_node_t bns_search_lte_g(bn_tree_t t, gtype key);
void bns_delete_g(bn_tree_t t, bn_node_t n);

void bns_pprint(bn_tree_t, gtype_print_t gpp);
#define bns_gpp(n) gpp(bns_node_g_key(n))

#endif  // BASE_BNS_H_
