#ifndef TREE_RBM_H_
#define TREE_RBM_H_

/* (C) 2021 Nguyen Ba Ngoc (bangoc) */

/** @file
 * @brief Ánh xạ khóa=>giá trị khái quát dựa trên cây đỏ-đen
 *
 * rbm - Red-black tree map là một triển khai ánh xạ khái quát
 * dựa trên cây đỏ đen (gọi ngắn gọn là bảng cây). Khóa và giá
 * trị được lưu trong cùng nút của cây và có kiểu ::gtype. Các
 * khóa được so sánh bằng hàm do người dùng cung cấp.
 */

#include "base/gtype.h"
#include "tree/rb.h"

/**
 * \headerfile "cgen.h"
 * Cấu trúc nút của rbm, là mở rộng nút của cây đỏ đen.
 *
 * \private Người sử dụng không cần thao tác với kiểu này.
 */
typedef struct red_black_map_node {
  struct _rb_node base;

  /** \private
   * Người sử dụng không cần trực tiếp truy cập tới
   * các thuộc tính của nút.
   */
  gtype key;
  gtype value;
} rbm_node_s, *rbm_node_t;

#define rbm_node(n) ((rbm_node_t)(n))
#define rbm_node_key(n) (rbm_node(n)->key)
#define rbm_node_value(n) (rbm_node(n)->value)

/**
 * \headerfile "cgen.h"
 * Cấu trúc điều khiển của bảng cây rbm, được tạo bằng hàm
 * rbm_create(gtype_cmp_t cmp, gtype_free_t free_key, gtype_free_t free_value).
 *
 * Các macro hỗ trợ: <br>
 *   #rbm_size(map) - Kích thước của map.
 *
 *   #rbm_traverse(k, v, map) - Duyệt tuần tự các cặp trong map.
 *
 *   #rbm_free(map) - Giải phóng bộ nhớ của map
 *
 *   #rbm_clear(map) - Làm rỗng map
 */
typedef struct red_black_map {
  struct _bn_tree t;
  gtype_cmp_t cmp;
  gtype_free_t fk, fv;
  long size;
} rbm_s, *rbm_t;

/**
 * Cấu trúc kết quả trả về của hàm rbm_insert(rbm_t t, gtype key, gtype value),
 * cho biết trạng thái kết thúc thao tác thêm dữ liệu vào rbm.
 */
typedef struct rbm_insert_result {
  gtype *value;
  int inserted;
} rbm_ires;

#define rbm_tree(t) ((rbm_t)(t))

rbm_node_t rbm_create_node(gtype key, gtype value);
int rbm_cmp_node(bn_node_t n1, bn_node_t n2, bn_tree_t t);

/**
 * Hàm tạo đối tượng điều khiển bảng cây.
 *
 * @param cmp Hàm so sánh các giá trị khóa, phải != NULL.
 * Tham khảo: gtype_cmp_s(gtype v1, gtype v2).
 * @param free_key Con trỏ tới hàm giải phóng bộ nhớ ngoài được gắn với khóa, tham khảo.
 * gtype_free_s(gtype v). Được gọi khi xóa dữ liệu nếu != NULL. Trong trường hợp không
 * có bộ nhớ ngoài hoặc không cần xóa bộ nhớ ngoài (như tình huống đang chia sẻ bộ nhớ
 * với 1 câu trúc lưu trữ khác) thì để = NULL.
 * @param free_value Con trỏ tới hàm giải phóng bộ nhớ ngoài được gắn với giá trị, tương
 * tự free_key.
 * @return Trả về đối tượng điều khiển bảng cây.
 *
 * \memberof red_black_map
 *
 * Tham khảo: hmap_create(gtype_hash_t hash_func, gtype_cmp_t cmp, gtype_free_t free_key, gtype_free_t free_value)
 */
rbm_t rbm_create(gtype_cmp_t cmp, gtype_free_t fk, gtype_free_t fv);

/**
 * Thêm cặp (key, value) vào bảng t. Nếu key đã tồn tại thì
 * bỏ qua, có thể truy cập giá trị đang được gắn với khóa
 * đã có trong t bằng con trỏ ::rbm_ires::value trong kết quả
 * được trả về.
 *
 * @param t Con trỏ tới bảng cây.
 * @param key Khóa được thêm vào.
 * @param value Giá trị được thêm vào.
 * @return Trả về đối tượng ::rbm_ires
 *
 * \memberof red_black_map
 *
 * Tham khảo: hmap_insert(hmap_t tab, gtype key, gtype value)
 */
rbm_ires rbm_insert(rbm_t t, gtype key, gtype value);

/**
 * Thêm cặp (key, value) vào bảng t. Nếu key đã tồn tại thì
 * bỏ qua và trả về con trỏ tới giá trị đang có trong t.
 * Nếu ngược lại thì thêm (key, value) và trả về NULL.
 *
 * @param t Con trỏ tới bảng cây.
 * @param key Khóa được thêm vào.
 * @param value Giá trị được thêm vào.
 * @return Trả về con trỏ tới giá trị nếu khóa đã tồn tại
 * hoặc trả về NULL nếu ngược lại, kiểu ::gtype*.
 *
 * \memberof red_black_map
 *
 * Tham khảo: hmap_put(hmap_t tab, gtype key, gtype value)
 */
gtype *rbm_put(rbm_t t, gtype key, gtype value);

/**
 * Tra cứu giá trị trong t theo key.
 *
 * @param t Con trỏ tới đối tượng bảng cây.
 * @param key Khóa được sử dụng để tìm kiếm.
 * @return Trả về con trỏ tới giá trị đang được gắn với key trong t
 * nếu tồn tại, hoặc NULL nếu ngược lại.
 *
 * \memberof red_black_map
 *
 * Tham khảo: hmap_value(hmap_t tab, gtype key)
 */
gtype *rbm_value(rbm_t t, gtype key);

/**
 * Có thể sẽ xóa hàm này.
 * \private
 */
rbm_node_t rbm_search(rbm_t t, gtype key);

/**
 * Nếu key không có trong t thì bỏ qua, nếu ngược lại thì xóa cặp
 * khóa & giá trị tương ứng trong t. Các hàm free_key và free_value
 * được gọi nếu != NULL.
 *
 * @param t Con trỏ tới bảng cây.
 * @param key Khóa cần xóa.
 * @return 1 Nếu tồn tại khóa sau khi xóa dữ liệu, 0 nếu ngược lại.
 *
 * \memberof red_black_map
 *
 * Tham khảo: hmap_remove(hmap_t tab, gtype key)
 */
int rbm_remove(rbm_t t, gtype key);

/**
 * Số lượng cặp khóa & giá trị đang được lưu trong bảng.
 *
 * @param t Con trỏ tới đối tượng bảng cây.
 * @return Trả về số lượng cặp khóa/giá trị đang được lưu trong t.
 *
 */
#define rbm_size(t) ((t)->size)

static inline void _rbm_move_next(gtype **k, gtype **v) {
  rbm_node_t nd = rbm_node(container_of(*k, struct red_black_map_node, key));
  bn_node_t tmp = bn_next_inorder(bn_node(nd));
  if (!tmp) {
    *k = NULL;
    *v = NULL;
    return;
  }
  *k = &(rbm_node_key(tmp));
  *v = &(rbm_node_value(tmp));
}

/**
 * Duyệt tuần tự các cặp khóa & giá trị trong bảng map. Các tham số k và v
 * là các định danh do người sử dụng tự đặt và sẽ có kiểu ::gtype *
 *
 * @param k Định danh sẽ được sử dụng như con trỏ tới khóa hiện hành.
 * @param v Định danh sẽ được sử dụng như con trỏ tới giá trị hiện hành.
 * @param map Con trỏ tới bảng cây.
 *
 * Tham khảo: #hmap_traverse(key, value, map)
 */
#define rbm_traverse(k, v, map) \
  for (gtype *k = (rbm_size(map))? &(rbm_node_key(bn_left_most(bn_tree(map)->root))): NULL, \
             *v = (rbm_size(map))? &(rbm_node_value(bn_left_most(bn_tree(map)->root))): NULL; \
       k != NULL && v != NULL; _rbm_move_next(&k, &v)) \

/**
 * Giải phóng bộ nhớ được cấp phát cho bảng map. Các hàm free_key và
 * free_value được gọi cho từng khóa và giá trị nếu != NULL.
 *
 * @param map Con trỏ tới bảng cây.
 */
#define rbm_free(map) \
  do { \
    if (rbm_tree(map)->fk || rbm_tree(map)->fv) { \
      rbm_traverse(_k, _v, (map)) { \
        if (rbm_tree(map)->fk) { \
          rbm_tree(map)->fk(*_k); \
        } \
        if (rbm_tree(map)->fv) { \
          rbm_tree(map)->fv(*_v); \
        } \
      } \
    } \
    bn_free_tree(bn_tree(map)); \
  } while (0)

/**
 * Làm rỗng bảng map. Các hàm free_key và free_value sẽ được gọi
 * cho từng cặp khóa và giá trị nếu != NULL.
 *
 * @param map Con trỏ tới bảng cây.
 */
#define rbm_clear(map) \
  do { \
    if (rbm_tree(map)->fk || rbm_tree(map)->fv) { \
      rbm_traverse(_k, _v, (map)) { \
        if (rbm_tree(map)->fk) { \
          rbm_tree(map)->fk(*_k); \
        } \
        if (rbm_tree(map)->fv) { \
          rbm_tree(map)->fv(*_v); \
        } \
      } \
    } \
    bn_tree_t _t = (bn_tree_t)(map); \
    bn_clear_tree(_t); \
    rbm_tree(map)->size = 0; \
  } while (0)

/**
 * Hàm giải phóng bộ nhớ cho trường hợp con trỏ tới đối tượng rbm
 * được lưu trong một cấu trúc lưu trữ khác.
 *
 * @param value Giá trị gtype đang chứa con trỏ tới đối tượng rbm.
 * Con trỏ value.rbm (kiểu ::rbm_t) được truyền cho rbm_free.
 * @return Hàm không trả về giá trị.
 *
 * \memberof gtype
 */
void gtype_free_rbm(gtype value);

#endif  // TREE_RBM_H_
