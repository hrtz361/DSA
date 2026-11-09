/*
 * Chuong 3 - Bai 2: Chuyen bieu thuc trung to (infix) sang hau to (postfix)
 * bang ngan xep.
 *
 * Cach dung:
 *     gcc -Wall -Wextra -O2 -o infix2postfix infix2postfix.c
 *     ./infix2postfix < input.txt
 *
 * Input : moi dong la mot bieu thuc trung to (toi da 100000 ky tu).
 *         - Toan hang : day lien tiep chu cai / chu so (vd: 25, x, abc)
 *         - Toan tu   : + - * / ^   (nhi phan)
 *         - Ngoac     : ( )
 *         - Khoang trang tuy y (bi bo qua); dong trong bi bo qua
 * Output: moi dong vao -> mot dong ra la bieu thuc hau to, cac thanh phan
 *         cach nhau mot dau cach; hoac "Biểu thức không hợp lệ" neu ngoac
 *         khong can bang (hoac co ky tu la).
 *
 * Do phuc tap: thoi gian O(L), bo nho O(L) cho bieu thuc dai L.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LEN 100000   /* do dai toi da cua mot bieu thuc */

/* =====================================================================
 * NGAN XEP KY TU (cai dat bang mang, nhu Bai 1)
 * Dung de chua cac toan tu va dau "(" dang cho.
 * ===================================================================== */
typedef struct {
    char *data;      /* mang chua cac ky tu                      */
    int   top;       /* chi so o dinh; -1 khi stack rong         */
    int   capacity;  /* dung luong toi da                        */
} CharStack;

/* Tao stack co dung luong n. Tra ve NULL neu het bo nho. O(1) */
static CharStack *cs_create(int n) {
    CharStack *s = (CharStack *)malloc(sizeof(CharStack));
    if (s == NULL) return NULL;
    s->data = (char *)malloc((size_t)n);
    if (s->data == NULL) { free(s); return NULL; }
    s->top = -1;
    s->capacity = n;
    return s;
}

/* Giai phong stack. O(1) */
static void cs_destroy(CharStack *s) {
    if (s == NULL) return;
    free(s->data);
    free(s);
}

/* Stack co rong khong? O(1) */
static int cs_isEmpty(const CharStack *s) { return s->top == -1; }

/* Them c vao dinh. Tra ve 1 neu thanh cong, 0 neu stack day. O(1) */
static int cs_push(CharStack *s, char c) {
    if (s->top == s->capacity - 1) return 0;
    s->data[++s->top] = c;
    return 1;
}

/* Lay phan tu dinh ra *out. Tra ve 1 neu thanh cong, 0 neu stack rong. O(1) */
static int cs_pop(CharStack *s, char *out) {
    if (cs_isEmpty(s)) return 0;
    *out = s->data[s->top--];
    return 1;
}

/* Xem phan tu dinh. DIEU KIEN: stack khong rong (phai goi isEmpty truoc). O(1) */
static char cs_top(const CharStack *s) { return s->data[s->top]; }

/* =====================================================================
 * DO UU TIEN VA TINH KET HOP CUA TOAN TU
 * ===================================================================== */
static int is_operator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '^';
}

/* Do uu tien: ^ = 3 (cao nhat), * va / = 2, + va - = 1 (thap nhat). */
static int priority(char op) {
    if (op == '^') return 3;
    if (op == '*' || op == '/') return 2;
    return 1;
}

/* o2 la toan tu o dinh stack, o1 la toan tu moi den.
 * Tra ve 1 neu o2 phai thuc hien truoc o1 (tuc la phai pop o2 ra output):
 *   - uu tien(o2) > uu tien(o1), hoac
 *   - uu tien bang nhau va o1 ket hop trai (moi toan tu tru '^' ket hop trai;
 *     '^' ket hop phai nen khong pop khi hai toan tu '^' lien nhau). */
static int must_pop_first(char o2, char o1) {
    if (priority(o2) > priority(o1)) return 1;
    if (priority(o2) == priority(o1) && o1 != '^') return 1;
    return 0;
}

/* =====================================================================
 * GHI TOKEN VAO OUTPUT
 * Them cac ky tu tok[0..len-1] vao cuoi out, cach phan truoc bang 1 dau cach.
 * *pos la do dai hien tai cua out. O(len)
 * ===================================================================== */
static void append_token(char *out, int *pos, const char *tok, int len) {
    if (*pos > 0) out[(*pos)++] = ' ';
    memcpy(out + *pos, tok, (size_t)len);
    *pos += len;
    out[*pos] = '\0';
}

/* =====================================================================
 * THUAT TOAN CHINH: CHUYEN TRUNG TO -> HAU TO
 * s   : bieu thuc trung to (chuoi ket thuc bang '\0')
 * out : bo dem ket qua, can toi thieu 2*strlen(s)+2 byte
 * Tra ve 1 neu thanh cong, 0 neu bieu thuc khong hop le,
 *        -1 neu het bo nho.
 * Thoi gian O(L): moi ky tu doc 1 lan; moi toan tu/ngoac duoc push 1 lan
 * va pop nhieu nhat 1 lan.
 * ===================================================================== */
static int infix_to_postfix(const char *s, char *out) {
    int L = (int)strlen(s);
    CharStack *S = cs_create(L + 1);  /* stack khong bao gio chua qua L phan tu */
    if (S == NULL) return -1;

    int pos = 0;                      /* do dai hien tai cua out */
    int ok = 1;                       /* 0 neu phat hien bieu thuc loi */
    int i = 0;
    char op;
    out[0] = '\0';

    while (i < L && ok) {
        unsigned char c = (unsigned char)s[i];

        if (isspace(c)) {
            i++;                                    /* bo qua khoang trang */

        } else if (isalnum(c)) {
            /* Toan hang: gom ca day chu cai/chu so lien tiep, ghi ngay ra output */
            int j = i;
            while (j < L && isalnum((unsigned char)s[j])) j++;
            append_token(out, &pos, s + i, j - i);
            i = j;

        } else {
            if (c == '(') {
                /* Ngoac mo: push vao stack lam moc ngan cach */
                cs_push(S, '(');

            } else if (c == ')') {
                /* Ngoac dong: pop toan tu ra output cho den khi gap "(" */
                while (!cs_isEmpty(S) && cs_top(S) != '(') {
                    cs_pop(S, &op);
                    append_token(out, &pos, &op, 1);
                }
                if (cs_isEmpty(S)) {
                    ok = 0;                         /* thua ")": khong co "(" tuong ung */
                } else {
                    cs_pop(S, &op);                 /* bo "(" , khong ghi ra output */
                }

            } else if (is_operator((char)c)) {
                /* Toan tu moi: pop cac toan tu o dinh phai thuc hien truoc no */
                while (!cs_isEmpty(S) && cs_top(S) != '(' &&
                       must_pop_first(cs_top(S), (char)c)) {
                    cs_pop(S, &op);
                    append_token(out, &pos, &op, 1);
                }
                cs_push(S, (char)c);                /* roi push toan tu moi */

            } else {
                ok = 0;                             /* ky tu la, khong hop le */
            }
            i++;
        }
    }

    /* Het bieu thuc: pop not cac toan tu con lai ra output */
    while (ok && !cs_isEmpty(S)) {
        cs_pop(S, &op);
        if (op == '(') ok = 0;                      /* thua "(": khong co ")" tuong ung */
        else append_token(out, &pos, &op, 1);
    }

    cs_destroy(S);
    return ok;
}

/* =====================================================================
 * DOC INPUT, MOI DONG MOT BIEU THUC
 * ===================================================================== */
static char line[MAX_LEN + 3];    /* du cho MAX_LEN ky tu + '\n' + '\0' */

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

        /* Bieu thuc dai hon MAX_LEN ky tu (buffer con du cho vai ky tu thua) */
        if (n > MAX_LEN) {
            printf("bieu thuc qua dai(L > %d)\n", MAX_LEN);
            continue;
        }

        /* Bo qua dong chi co khoang trang */
        size_t k = 0;
        while (k < n && isspace((unsigned char)line[k])) k++;
        if (k == n) continue;

        char *out = (char *)malloc(2 * n + 2);   /* toi da moi ky tu 1 dau cach di kem */
        if (out == NULL) { fprintf(stderr, "Het bo nho\n"); return 1; }

        int r = infix_to_postfix(line, out);
        if (r == 1)       puts(out);
        else if (r == 0)  puts("bieu thuc khong hop le");
        else              fprintf(stderr, "Het bo nho\n");

        free(out);
    }
    return 0;
}
