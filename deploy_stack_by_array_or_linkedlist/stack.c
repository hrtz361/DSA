/*
 * Chuong 3 - Bai 1: Cai dat ngan xep (stack) bang mang va danh sach lien ket.
 *
 * Cach dung:
 *     ./stack array  < input.txt     (cai dat bang mang, mac dinh)
 *     ./stack list   < input.txt     (cai dat bang danh sach lien ket)
 *
 * Khuon dang input:
 *     dong 1 : so nguyen N  (dung luong toi da; cach "list" bo qua N)
 *     cac dong tiep theo : mot lenh moi dong, den het file
 *         push x | pop | top | isEmpty | isFull | size | clear
 *
 * Output: moi lenh pop/top/isEmpty/isFull/size in mot dong ket qua.
 *         Loi in "Stack tràn" hoac "Stack rỗng", trang thai stack khong doi.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* =====================================================================
 * CACH 1: CAI DAT BANG MANG
 * ===================================================================== */
typedef struct {
    int *data;      /* mang chua cac phan tu                       */
    int  top;       /* chi so phan tu o dinh; -1 khi stack rong    */
    int  capacity;  /* dung luong toi da N                         */
} ArrayStack;

/* Tao stack co dung luong n. Tra ve NULL neu n khong hop le / het bo nho. O(1) */
ArrayStack *as_create(int n) {
    if (n <= 0) return NULL;
    ArrayStack *s = (ArrayStack *)malloc(sizeof(ArrayStack));
    if (s == NULL) return NULL;
    s->data = (int *)malloc((size_t)n * sizeof(int));
    if (s->data == NULL) { free(s); return NULL; }
    s->top = -1;            /* ban dau stack rong */
    s->capacity = n;
    return s;
}

/* Giai phong toan bo bo nho cua stack. O(1) */
void as_destroy(ArrayStack *s) {
    if (s == NULL) return;
    free(s->data);
    free(s);
}

int as_isEmpty(const ArrayStack *s) { return s->top == -1; }               /* O(1) */
int as_isFull (const ArrayStack *s) { return s->top == s->capacity - 1; }  /* O(1) */
int as_size   (const ArrayStack *s) { return s->top + 1; }                 /* O(1) */

/* Them x vao dinh. Tra ve 1 neu thanh cong, 0 neu stack day (tran). O(1) */
int as_push(ArrayStack *s, int x) {
    if (as_isFull(s)) return 0;
    s->top++;               /* dich dinh len mot o  */
    s->data[s->top] = x;    /* ghi gia tri vao dinh */
    return 1;
}

/* Lay phan tu o dinh ra *out va loai khoi stack.
 * Tra ve 1 neu thanh cong, 0 neu stack rong. O(1) */
int as_pop(ArrayStack *s, int *out) {
    if (as_isEmpty(s)) return 0;
    *out = s->data[s->top];
    s->top--;               /* o cu se bi ghi de o lan push sau */
    return 1;
}

/* Xem phan tu o dinh (khong loai bo). Tra ve 0 neu stack rong. O(1) */
int as_top(const ArrayStack *s, int *out) {
    if (as_isEmpty(s)) return 0;
    *out = s->data[s->top];
    return 1;
}

/* Lam rong stack: chi can dat lai top. O(1) */
void as_clear(ArrayStack *s) { s->top = -1; }

/* =====================================================================
 * CACH 2: CAI DAT BANG DANH SACH LIEN KET DON
 * Dinh stack = nut dau danh sach (head): them/xoa o dau deu O(1).
 * ===================================================================== */
typedef struct Node {
    int data;               /* gia tri cua nut                         */
    struct Node *next;      /* tro toi nut nam ngay ben duoi           */
} Node;

typedef struct {
    Node *head;             /* tro toi dinh stack; NULL khi rong       */
    int   count;            /* so phan tu, de size() chay O(1)         */
} ListStack;

/* Tao stack rong. O(1) */
ListStack *ls_create(void) {
    ListStack *s = (ListStack *)malloc(sizeof(ListStack));
    if (s == NULL) return NULL;
    s->head = NULL;
    s->count = 0;
    return s;
}

int ls_isEmpty(const ListStack *s) { return s->head == NULL; }  /* O(1) */
int ls_isFull (const ListStack *s) { (void)s; return 0; }       /* khong bao gio day (tru khi het bo nho) */
int ls_size   (const ListStack *s) { return s->count; }         /* O(1) */

/* Them x vao dinh. Tra ve 1 neu thanh cong, 0 neu cap phat nut that bai. O(1) */
int ls_push(ListStack *s, int x) {
    Node *p = (Node *)malloc(sizeof(Node));
    if (p == NULL) return 0;
    p->data = x;
    p->next = s->head;      /* nut moi tro toi dinh cu */
    s->head = p;            /* nut moi tro thanh dinh  */
    s->count++;
    return 1;
}

/* Lay phan tu o dinh ra *out, giai phong nut. Tra ve 0 neu stack rong. O(1) */
int ls_pop(ListStack *s, int *out) {
    if (ls_isEmpty(s)) return 0;
    Node *p = s->head;      /* luu lai nut dinh de giai phong */
    *out = p->data;
    s->head = p->next;      /* dinh moi la nut ke tiep        */
    free(p);
    s->count--;
    return 1;
}

/* Xem phan tu o dinh (khong loai bo). Tra ve 0 neu stack rong. O(1) */
int ls_top(const ListStack *s, int *out) {
    if (ls_isEmpty(s)) return 0;
    *out = s->head->data;
    return 1;
}

/* Lam rong stack: giai phong tung nut. O(n), n la so phan tu hien co */
void ls_clear(ListStack *s) {
    int tmp;
    while (ls_pop(s, &tmp)) { /* pop den khi rong */ }
}

/* Huy stack: clear roi giai phong cau truc. O(n) */
void ls_destroy(ListStack *s) {
    if (s == NULL) return;
    ls_clear(s);
    free(s);
}

/* =====================================================================
 * DOC LENH VA THUC THI
 * ===================================================================== */
static void print_bool(int b) { puts(b ? "true" : "false"); }

/* Chay chuoi lenh tren stack cai dat bang mang. Tong thoi gian O(Q). */
static void run_array(int n) {
    ArrayStack *s = as_create(n);
    if (s == NULL) { fprintf(stderr, "Khong the tao stack (N khong hop le hoac het bo nho)\n"); return; }

    char cmd[16];
    int x, v;
    while (scanf("%15s", cmd) == 1) {
        if (strcmp(cmd, "push") == 0) {
            if (scanf("%d", &x) != 1) break;
            if (!as_push(s, x)) puts("Stack overflow");
        } else if (strcmp(cmd, "pop") == 0) {
            if (as_pop(s, &v)) printf("%d\n", v); else puts("Stack empty");
        } else if (strcmp(cmd, "top") == 0) {
            if (as_top(s, &v)) printf("%d\n", v); else puts("Stack empty");
        } else if (strcmp(cmd, "isEmpty") == 0) {
            print_bool(as_isEmpty(s));
        } else if (strcmp(cmd, "isFull") == 0) {
            print_bool(as_isFull(s));
        } else if (strcmp(cmd, "size") == 0) {
            printf("%d\n", as_size(s));
        } else if (strcmp(cmd, "clear") == 0) {
            as_clear(s);
        } else {
            fprintf(stderr, "Lenh khong hop le: %s\n", cmd);
        }
    }
    as_destroy(s);
}

/* Chay chuoi lenh tren stack cai dat bang danh sach lien ket. Tong thoi gian O(Q). */
static void run_list(void) {
    ListStack *s = ls_create();
    if (s == NULL) { fprintf(stderr, "Het bo nho\n"); return; }

    char cmd[16];
    int x, v;
    while (scanf("%15s", cmd) == 1) {
        if (strcmp(cmd, "push") == 0) {
            if (scanf("%d", &x) != 1) break;
            if (!ls_push(s, x)) puts("Stack overflow");   /* chi xay ra khi het bo nho */
        } else if (strcmp(cmd, "pop") == 0) {
            if (ls_pop(s, &v)) printf("%d\n", v); else puts("Stack empty");
        } else if (strcmp(cmd, "top") == 0) {
            if (ls_top(s, &v)) printf("%d\n", v); else puts("Stack empty");
        } else if (strcmp(cmd, "isEmpty") == 0) {
            print_bool(ls_isEmpty(s));
        } else if (strcmp(cmd, "isFull") == 0) {
            print_bool(ls_isFull(s));
        } else if (strcmp(cmd, "size") == 0) {
            printf("%d\n", ls_size(s));
        } else if (strcmp(cmd, "clear") == 0) {
            ls_clear(s);
        } else {
            fprintf(stderr, "Lenh khong hop le: %s\n", cmd);
        }
    }
    ls_destroy(s);
}

int main(int argc, char *argv[]) {
    int n;
    if (scanf("%d", &n) != 1) {                 /* doc dung luong N */
        fprintf(stderr, "Thieu dung luong N o dong dau\n");
        return 1;
    }
    if (argc > 1 && strcmp(argv[1], "list") == 0)
        run_list();
    else
        run_array(n);
    return 0;
}
