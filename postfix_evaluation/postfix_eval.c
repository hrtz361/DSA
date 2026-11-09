/*
 * Chuong 3 - Bai 3: Dinh gia tri bieu thuc hau to (postfix) bang ngan xep.
 *
 * Cach dung:
 *     gcc -Wall -Wextra -O2 -o postfix_eval postfix_eval.c -lm
 *     ./postfix_eval < input.txt
 *
 * Input : moi dong la mot bieu thuc hau to (toi da 100000 ky tu).
 *         - Cac thanh phan (token) cach nhau boi it nhat mot dau cach
 *         - Toan hang : so nguyen khong am, 0 <= x <= 10^9 (vd: 3, 25, 1000)
 *         - Toan tu   : + - * / ^   (nhi phan)
 *         - Dong trong bi bo qua
 * Output: moi dong vao -> mot dong ra:
 *         - gia tri bieu thuc, lam tron den 6 chu so thap phan (vd: 14.000000)
 *         - "Lỗi chia cho 0"          neu co phep chia cho 0
 *         - "Biểu thức không hợp lệ"  neu sai cu phap (thieu/thua toan hang,
 *                                      token la)
 *         - "Lỗi tính toán"           neu ket qua tran so hoac phep ^ khong
 *                                      xac dinh (vd: can bac chan cua so am)
 *
 * Do phuc tap: thoi gian O(L), bo nho O(L) (xau nhat) cho bieu thuc dai L.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define MAX_LEN 100000   /* do dai toi da cua mot bieu thuc */

/* Ma trang thai tra ve khi xu ly */
enum { ST_OK = 0, ST_SYNTAX, ST_DIV0, ST_MATH };

/* =====================================================================
 * NGAN XEP SO THUC (cai dat bang mang, nhu Bai 1)
 * Dung de chua cac gia tri chua duoc su dung lam toan hang.
 * ===================================================================== */
typedef struct {
    double *data;    /* mang chua cac gia tri                    */
    int     top;     /* chi so o dinh; -1 khi stack rong         */
    int     capacity;
} DoubleStack;

/* Tao stack co dung luong n. Tra ve NULL neu het bo nho. O(1) */
static DoubleStack *ds_create(int n) {
    DoubleStack *s = (DoubleStack *)malloc(sizeof(DoubleStack));
    if (s == NULL) return NULL;
    s->data = (double *)malloc((size_t)n * sizeof(double));
    if (s->data == NULL) { free(s); return NULL; }
    s->top = -1;
    s->capacity = n;
    return s;
}

/* Giai phong stack. O(1) */
static void ds_destroy(DoubleStack *s) {
    if (s == NULL) return;
    free(s->data);
    free(s);
}

/* So phan tu hien co. O(1) */
static int ds_size(const DoubleStack *s) { return s->top + 1; }

/* Them x vao dinh. Tra ve 1 neu thanh cong, 0 neu stack day. O(1) */
static int ds_push(DoubleStack *s, double x) {
    if (s->top == s->capacity - 1) return 0;
    s->data[++s->top] = x;
    return 1;
}

/* Lay phan tu dinh ra va loai khoi stack.
 * DIEU KIEN: stack khong rong (phai kiem tra ds_size truoc). O(1) */
static double ds_pop(DoubleStack *s) { return s->data[s->top--]; }

/* Xem phan tu dinh. DIEU KIEN: stack khong rong. O(1) */
static double ds_top(const DoubleStack *s) { return s->data[s->top]; }

/* =====================================================================
 * CHUYEN TOKEN SO THANH GIA TRI
 * Token hop le: toan chu so, 0 <= gia tri <= 10^9.
 * Tra ve 1 neu hop le (gia tri trong *v), 0 neu khong. O(len)
 * ===================================================================== */
static int parse_number(const char *t, int len, double *v) {
    if (len < 1 || len > 10) return 0;        /* qua dai: chac chan > 10^9 hoac khong phai so hop le */
    double x = 0;
    for (int k = 0; k < len; k++) {
        if (!isdigit((unsigned char)t[k])) return 0;
        x = x * 10 + (t[k] - '0');
    }
    if (x > 1e9) return 0;
    *v = x;
    return 1;
}

/* =====================================================================
 * TINH MOT PHEP TOAN a op b
 * Tra ve ST_OK / ST_DIV0 / ST_MATH. Ket qua trong *r. O(1)
 * ===================================================================== */
static int compute(double a, char op, double b, double *r) {
    switch (op) {
        case '+': *r = a + b; break;
        case '-': *r = a - b; break;
        case '*': *r = a * b; break;
        case '/':
            if (b == 0) return ST_DIV0;       /* chia cho 0 */
            *r = a / b;
            break;
        default:  *r = pow(a, b); break;      /* op = '^' */
    }
    if (!isfinite(*r)) return ST_MATH;        /* tran so hoac ^ khong xac dinh */
    return ST_OK;
}

/* =====================================================================
 * XU LY MOT TOKEN
 *  - Toan hang : push gia tri vao stack
 *  - Toan tu   : pop b (toan hang phai), pop a (toan hang trai),
 *                tinh a op b roi push ket qua
 *  - Khac      : sai cu phap
 * O(len)
 * ===================================================================== */
static int process_token(DoubleStack *S, const char *t, int len) {
    double v;

    if (parse_number(t, len, &v)) {
        if (!ds_push(S, v)) return ST_SYNTAX;       /* khong xay ra: dung luong du lon */
        return ST_OK;
    }

    if (len == 1 && strchr("+-*/^", t[0]) != NULL) {
        if (ds_size(S) < 2) return ST_SYNTAX;       /* thieu toan hang */
        double b = ds_pop(S);                       /* toan hang phai (pop truoc) */
        double a = ds_pop(S);                       /* toan hang trai (pop sau)   */
        double r;
        int st = compute(a, t[0], b, &r);
        if (st != ST_OK) return st;
        ds_push(S, r);                              /* da pop 2 nen luon con cho */
        return ST_OK;
    }

    return ST_SYNTAX;                               /* token la */
}

/* =====================================================================
 * THUAT TOAN CHINH: DINH GIA BIEU THUC HAU TO
 * s : bieu thuc hau to (chuoi ket thuc bang '\0')
 * Tra ve ST_OK (gia tri trong *result) hoac ma loi.
 * Thoi gian O(L): moi ky tu doc 1 lan, moi token xu ly trong O(do dai token).
 * ===================================================================== */
static int evaluate(const char *s, double *result) {
    int L = (int)strlen(s);
    /* So toan hang khong qua (L+1)/2 vi cac token cach nhau it nhat 1 dau cach */
    DoubleStack *S = ds_create((L + 1) / 2 + 1);
    if (S == NULL) return ST_MATH;

    int status = ST_OK;
    int i = 0;
    while (i < L && status == ST_OK) {
        if (isspace((unsigned char)s[i])) {
            i++;                                    /* bo qua khoang trang */
        } else {
            int j = i;                              /* tim het token */
            while (j < L && !isspace((unsigned char)s[j])) j++;
            status = process_token(S, s + i, j - i);
            i = j;
        }
    }

    /* Het bieu thuc: stack phai con dung 1 phan tu */
    if (status == ST_OK) {
        if (ds_size(S) != 1) status = ST_SYNTAX;    /* thua toan hang */
        else *result = ds_top(S);
    }

    ds_destroy(S);
    return status;
}

/* =====================================================================
 * DOC INPUT, MOI DONG MOT BIEU THUC
 * ===================================================================== */
static char line[MAX_LEN + 3];    /* du cho MAX_LEN ky tu + "\r\n" + '\0' */

int main(void) {
    while (fgets(line, sizeof line, stdin) != NULL) {
        size_t n = strlen(line);

        /* Dong qua dai: khong gap '\n' ma van chua het file -> bo phan con lai */
        if (n > 0 && line[n - 1] != '\n' && !feof(stdin)) {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF) { /* bo qua */ }
            printf("bieu thuc qua dai(L > %d)\n", MAX_LEN);
            continue;
        }

        /* Cat '\n' va '\r' o cuoi dong */
        while (n > 0 && (line[n - 1] == '\n' || line[n - 1] == '\r')) line[--n] = '\0';

        if (n > MAX_LEN) {
            printf("bieu thuc qua dai(L > %d)\n", MAX_LEN);
            continue;
        }

        /* Bo qua dong chi co khoang trang */
        size_t k = 0;
        while (k < n && isspace((unsigned char)line[k])) k++;
        if (k == n) continue;

        double value = 0;
        int st = evaluate(line, &value);
        if (st == ST_OK) {
            if (fabs(value) < 0.5e-6) value = 0.0;  /* tranh in "-0.000000" */
            printf("%.6f\n", value);
        } else if (st == ST_DIV0) {
            puts("loi chia cho 0");
        } else if (st == ST_SYNTAX) {
            puts("bieu thuc khong hop le");
        } else {
            puts("loi tinh toan");
        }
    }
    return 0;
}
