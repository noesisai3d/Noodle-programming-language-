/* Noodle Seed bootstrap interpreter.
 *
 * TEMPORARY. This file exists only so Noodle can run its first programs.
 * It interprets Noodle Seed (spec/SEED.md) and nothing else: no world
 * features, no optimizations, no tooling beyond one minimal test mode.
 * It is size-capped (see BOOTSTRAP.md) and will be retired at gate G9.
 *
 * Build:  cc -std=c99 -O2 -o build/noodle-seed bootstrap/seed.c
 * Use:    noodle-seed run program.noodle
 *         noodle-seed test folder
 */
#define _POSIX_C_SOURCE 200809L
#include <ctype.h>
#include <dirent.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define MAX_DEPTH 2000
#if defined(__GNUC__) || defined(__clang__)
#define NORETURN __attribute__((noreturn))
#else
#define NORETURN
#endif

/* ---------- memory, text buffers, output ---------- */

static void oom(void) { fputs("Noodle ran out of memory and had to stop.\n", stderr); exit(3); }
static void *xalloc(size_t n) { void *p = calloc(1, n ? n : 1); if (!p) oom(); return p; }
static void *xgrow(void *p, size_t n) { p = realloc(p, n ? n : 1); if (!p) oom(); return p; }
static char *xdup(const char *s, size_t n) { char *r = xalloc(n + 1); memcpy(r, s, n); return r; }

typedef struct { char *s; size_t n, cap; } Buf;
static void bput(Buf *b, const char *s, size_t n) {
    if (b->n + n + 1 > b->cap) { b->cap = (b->n + n + 1) * 2; b->s = xgrow(b->s, b->cap); }
    memcpy(b->s + b->n, s, n); b->n += n; b->s[b->n] = 0;
}
static void bputs(Buf *b, const char *s) { bput(b, s, strlen(s)); }
static char *vfmt(const char *f, va_list ap) {
    va_list cp; va_copy(cp, ap);
    int n = vsnprintf(NULL, 0, f, cp); va_end(cp);
    char *s = xalloc(n + 1); vsnprintf(s, n + 1, f, ap); return s;
}
static char *fmt(const char *f, ...) { va_list ap; va_start(ap, f); char *s = vfmt(f, ap); va_end(ap); return s; }

static int capturing;       /* test mode: all output goes into cap */
static Buf cap;
static void emit(int to_err, const char *s, size_t n) {
    if (capturing) bput(&cap, s, n);
    else if (to_err) { fflush(stdout); fwrite(s, 1, n, stderr); }
    else fwrite(s, 1, n, stdout);
}

/* ---------- source lines, tokens, friendly errors ---------- */

enum { T_WORD = 1, T_NUM, T_TEXT, T_COMMA, T_POSS };
typedef struct { int ty; char *s, *raw; size_t len; } Tok;   /* s: lowercased word or text content */
typedef struct { char *src; Tok *t; int n; } Line;
static Line *lines;
static int nlines;
static jmp_buf on_error;

NORETURN static void fail(int line, const char *hint, const char *f, ...) {
    va_list ap; va_start(ap, f); char *msg = vfmt(f, ap); va_end(ap);
    Buf b = {0};
    if (line > 0) { char *t = fmt("Line %d:\n", line); bputs(&b, t); free(t); }
    bputs(&b, msg); bputs(&b, "\n");
    if (line > 0 && line <= nlines) {
        const char *s = lines[line - 1].src;
        while (*s == ' ' || *s == '\t') s++;
        if (*s) { bputs(&b, "\nYou wrote:\n"); bputs(&b, s); bputs(&b, "\n"); }
    }
    if (hint) { bputs(&b, "\n"); bputs(&b, hint); bputs(&b, "\n"); }
    emit(1, b.s, b.n);
    free(b.s); free(msg);
    longjmp(on_error, 1);
}

static int wordch(int c) { return isalnum(c) || c == '_' || c >= 0x80; }

static void push_tok(Tok **v, int *n, int ty, const char *s, size_t len, const char *raw, size_t rlen) {
    if ((*n & 15) == 0) *v = xgrow(*v, sizeof(Tok) * (*n + 16));
    Tok *t = &(*v)[(*n)++];
    t->ty = ty; t->s = xdup(s, len); t->len = len; t->raw = xdup(raw, rlen);
}

static void lex(const char *p, int line, Tok **out, int *on) {
    *out = NULL; *on = 0;
    while (*p) {
        unsigned char c = *p;
        if (c == ' ' || c == '\t' || c == '\r') { p++; continue; }
        if (c == '#') break;
        if (c == ',') { push_tok(out, on, T_COMMA, ",", 1, ",", 1); p++; continue; }
        if (c == '"') {
            Buf b = {0}; bput(&b, "", 0); p++;
            for (;;) {
                if (!*p) fail(line, "Text starts and ends with a quote mark (\"). Add the one at the end.",
                              "This text is missing its closing quote mark.");
                if (*p == '"') { if (p[1] == '"') { bput(&b, "\"", 1); p += 2; continue; } p++; break; }
                bput(&b, p++, 1);
            }
            push_tok(out, on, T_TEXT, b.s, b.n, b.s, b.n); free(b.s); continue;
        }
        if (isdigit(c) || (c == '-' && isdigit((unsigned char)p[1]))) {
            const char *s = p++;
            while (isdigit((unsigned char)*p)) p++;
            if (*p == '.' && isdigit((unsigned char)p[1])) { p++; while (isdigit((unsigned char)*p)) p++; }
            if (wordch((unsigned char)*p))
                fail(line, "Put a space between the number and the word.", "I can't read \"%.*s%c...\" as a number.", (int)(p - s), s, *p);
            push_tok(out, on, T_NUM, s, p - s, s, p - s); continue;
        }
        if (wordch(c)) {
            const char *s = p;
            while (wordch((unsigned char)*p)) p++;
            char *low = xdup(s, p - s);
            for (char *q = low; *q; q++) *q = (char)tolower((unsigned char)*q);
            push_tok(out, on, T_WORD, low, p - s, s, p - s); free(low);
            if (p[0] == '\'' && (p[1] == 's' || p[1] == 'S') && !wordch((unsigned char)p[2])) {
                push_tok(out, on, T_POSS, "'s", 2, "'s", 2); p += 2;
            }
            continue;
        }
        fail(line, "Noodle understands words, numbers, and text in quotes. If you want to show this symbol, put it inside quotes.",
             "I don't understand the symbol %c here.", c);
    }
}

/* ---------- exact numbers: big whole numbers (base 10^9) and fractions ---------- */

#define BASE 1000000000u
typedef struct { int n; uint32_t *d; } Big;   /* magnitude, little-endian limbs; n == 0 is zero */

static Big bnew(int n) { Big b; b.n = n; b.d = xalloc(sizeof(uint32_t) * (n ? n : 1)); return b; }
static void btrim(Big *b) { while (b->n && !b->d[b->n - 1]) b->n--; }
static Big bsmall(uint64_t v) { Big b = bnew(3); b.n = 0; while (v) { b.d[b.n++] = (uint32_t)(v % BASE); v /= BASE; } return b; }
static Big bcopy(Big a) { Big b = bnew(a.n); memcpy(b.d, a.d, sizeof(uint32_t) * a.n); return b; }
static int bone(Big a) { return a.n == 1 && a.d[0] == 1; }
static int bcmp(Big a, Big b) {
    if (a.n != b.n) return a.n < b.n ? -1 : 1;
    for (int i = a.n - 1; i >= 0; i--) if (a.d[i] != b.d[i]) return a.d[i] < b.d[i] ? -1 : 1;
    return 0;
}
static Big badd(Big a, Big b) {
    int n = (a.n > b.n ? a.n : b.n) + 1; Big r = bnew(n); uint64_t c = 0;
    for (int i = 0; i < n; i++) {
        uint64_t s = c + (i < a.n ? a.d[i] : 0) + (i < b.n ? b.d[i] : 0);
        r.d[i] = (uint32_t)(s % BASE); c = s / BASE;
    }
    btrim(&r); return r;
}
static Big bsub(Big a, Big b) {   /* a >= b */
    Big r = bnew(a.n); int64_t br = 0;
    for (int i = 0; i < a.n; i++) {
        int64_t s = (int64_t)a.d[i] - br - (i < b.n ? b.d[i] : 0);
        if (s < 0) { s += BASE; br = 1; } else br = 0;
        r.d[i] = (uint32_t)s;
    }
    btrim(&r); return r;
}
static Big bmul(Big a, Big b) {
    Big r = bnew(a.n + b.n);
    if (!a.n || !b.n) { r.n = 0; return r; }
    for (int i = 0; i < a.n; i++) {
        uint64_t c = 0;
        for (int j = 0; j < b.n; j++) {
            uint64_t t = r.d[i + j] + (uint64_t)a.d[i] * b.d[j] + c;
            r.d[i + j] = (uint32_t)(t % BASE); c = t / BASE;
        }
        r.d[i + b.n] = (uint32_t)c;
    }
    btrim(&r); return r;
}
static Big bmulsmall(Big a, uint32_t m) { Big t = bsmall(m); Big r = bmul(a, t); free(t.d); return r; }
static void bdivmod(Big a, Big b, Big *q, Big *r) {   /* b != 0; schoolbook, one limb at a time */
    Big Q = bnew(a.n), R = bnew(a.n + 1); R.n = 0;
    for (int i = a.n - 1; i >= 0; i--) {
        memmove(R.d + 1, R.d, sizeof(uint32_t) * R.n); R.d[0] = a.d[i]; R.n++; btrim(&R);
        uint32_t lo = 0, hi = BASE - 1;
        while (lo < hi) {
            uint32_t mid = lo + (hi - lo + 1) / 2; Big t = bmulsmall(b, mid);
            if (bcmp(t, R) <= 0) lo = mid; else hi = mid - 1;
            free(t.d);
        }
        Q.d[i] = lo;
        if (lo) { Big t = bmulsmall(b, lo); Big nr = bsub(R, t); memcpy(R.d, nr.d, sizeof(uint32_t) * nr.n); R.n = nr.n; free(t.d); free(nr.d); }
    }
    btrim(&Q); *q = Q; *r = R;
}
static Big bgcd(Big a, Big b) {
    a = bcopy(a); b = bcopy(b);
    while (b.n) { Big q, r; bdivmod(a, b, &q, &r); free(q.d); free(a.d); a = b; b = r; }
    free(b.d); return a;
}
static void bstr(Big a, Buf *out) {
    if (!a.n) { bputs(out, "0"); return; }
    char t[16]; snprintf(t, sizeof t, "%u", a.d[a.n - 1]); bputs(out, t);
    for (int i = a.n - 2; i >= 0; i--) { snprintf(t, sizeof t, "%09u", a.d[i]); bputs(out, t); }
}
static Big bparse(const char *s, int len) {
    Big r = bnew(len / 9 + 1); r.n = 0;
    for (int end = len; end > 0; end -= 9) {
        int st = end - 9 < 0 ? 0 : end - 9; uint32_t v = 0;
        for (int i = st; i < end; i++) v = v * 10 + (uint32_t)(s[i] - '0');
        r.d[r.n++] = v;
    }
    btrim(&r); return r;
}

/* ---------- values (immutable, reference counted) ---------- */

enum { V_NOTHING, V_NUM, V_TEXT, V_BOOL, V_LIST };
typedef struct Val {
    int rc, k, neg;
    Big num, den;              /* V_NUM: neg * num / den, always in lowest terms, den >= 1 */
    char *s; size_t len;       /* V_TEXT: UTF-8 bytes */
    struct Val **it; size_t n, cap;   /* V_LIST */
} Val;
static Val YES, NO, NOTHING;   /* shared singletons; set up in main */

static Val *nv(int k) { Val *v = xalloc(sizeof *v); v->rc = 1; v->k = k; return v; }
static Val *ref(Val *v) { v->rc++; return v; }
static void unref(Val *v) {
    if (!v || --v->rc > 0) return;
    if (v->k == V_NUM) { free(v->num.d); free(v->den.d); }
    else if (v->k == V_TEXT) free(v->s);
    else if (v->k == V_LIST) { for (size_t i = 0; i < v->n; i++) unref(v->it[i]); free(v->it); }
    free(v);
}
static Val *mkbool(int b) { return b ? &YES : &NO; }
static Val *mktext(const char *s, size_t n) { Val *v = nv(V_TEXT); v->s = xdup(s, n); v->len = n; return v; }
static Val *mknum(int neg, Big n, Big d) {   /* takes ownership of n and d */
    if (!n.n) { neg = 0; free(d.d); d = bsmall(1); }
    else if (!bone(d)) {
        Big g = bgcd(n, d);
        if (!bone(g)) {
            Big q, r;
            bdivmod(n, g, &q, &r); free(n.d); free(r.d); n = q;
            bdivmod(d, g, &q, &r); free(d.d); free(r.d); d = q;
        }
        free(g.d);
    }
    Val *v = nv(V_NUM); v->neg = neg; v->num = n; v->den = d; return v;
}
static Val *mkint(long long x) { return mknum(x < 0, bsmall(x < 0 ? -(uint64_t)x : (uint64_t)x), bsmall(1)); }
static Val *mklist(void) { return nv(V_LIST); }
static void list_push(Val *l, Val *x) {
    if (l->n == l->cap) { l->cap = l->cap ? l->cap * 2 : 8; l->it = xgrow(l->it, sizeof(Val *) * l->cap); }
    l->it[l->n++] = x;
}
static Val *list_copy(Val *l) { Val *c = mklist(); for (size_t i = 0; i < l->n; i++) list_push(c, ref(l->it[i])); return c; }

static Val *parse_num(const char *s) {
    int neg = *s == '-'; if (neg) s++;
    const char *dot = strchr(s, '.');
    if (!dot) return mknum(neg, bparse(s, (int)strlen(s)), bsmall(1));
    Buf digits = {0}; bput(&digits, s, dot - s); bputs(&digits, dot + 1);
    Big den = bsmall(1);
    for (size_t i = 0; i < strlen(dot + 1); i++) { Big t = bmulsmall(den, 10); free(den.d); den = t; }
    Val *v = mknum(neg, bparse(digits.s, (int)digits.n), den); free(digits.s); return v;
}
static Val *num_add(Val *a, Val *b, int sub) {
    int bneg = b->num.n ? b->neg ^ sub : 0;
    Big x = bmul(a->num, b->den), y = bmul(b->num, a->den), d = bmul(a->den, b->den), n; int neg;
    if (a->neg == bneg) { n = badd(x, y); neg = a->neg; }
    else if (bcmp(x, y) >= 0) { n = bsub(x, y); neg = a->neg; }
    else { n = bsub(y, x); neg = bneg; }
    free(x.d); free(y.d); return mknum(neg, n, d);
}
static Val *num_mul(Val *a, Val *b) { return mknum(a->neg ^ b->neg, bmul(a->num, b->num), bmul(a->den, b->den)); }
static Val *num_div(Val *a, Val *b) { return mknum(a->neg ^ b->neg, bmul(a->num, b->den), bmul(a->den, b->num)); }
static int num_cmp(Val *a, Val *b) {
    if (a->neg != b->neg) return a->neg ? -1 : 1;
    Big x = bmul(a->num, b->den), y = bmul(b->num, a->den);
    int c = bcmp(x, y); free(x.d); free(y.d); return a->neg ? -c : c;
}
static Val *num_trunc(Val *a) { Big q, r; bdivmod(a->num, a->den, &q, &r); free(r.d); return mknum(a->neg, q, bsmall(1)); }
static void num_text(Val *v, Buf *out) {
    if (v->neg) bputs(out, "-");
    if (bone(v->den)) { bstr(v->num, out); return; }
    Big t = bcopy(v->den); int k2 = 0, k5 = 0;
    for (;;) { Big q, r; bdivmod(t, bsmall(2), &q, &r); if (r.n) { free(q.d); free(r.d); break; } free(t.d); free(r.d); t = q; k2++; }
    for (;;) { Big q, r; bdivmod(t, bsmall(5), &q, &r); if (r.n) { free(q.d); free(r.d); break; } free(t.d); free(r.d); t = q; k5++; }
    if (!bone(t)) { free(t.d); bstr(v->num, out); bputs(out, "/"); bstr(v->den, out); return; }
    free(t.d);
    int k = k2 > k5 ? k2 : k5;
    Big s = bcopy(v->num);
    for (int i = k2; i < k; i++) { Big u = bmulsmall(s, 2); free(s.d); s = u; }
    for (int i = k5; i < k; i++) { Big u = bmulsmall(s, 5); free(s.d); s = u; }
    Buf d = {0}; bstr(s, &d); free(s.d);
    while ((int)d.n <= k) { Buf z = {0}; bputs(&z, "0"); bputs(&z, d.s); free(d.s); d = z; }
    bput(out, d.s, d.n - k); bputs(out, "."); bputs(out, d.s + d.n - k); free(d.s);
}

static const char *kname(Val *v) {
    switch (v->k) {
    case V_NUM: return "a number";
    case V_TEXT: return "some text";
    case V_BOOL: return "yes or no";
    case V_LIST: return "a list";
    }
    return "nothing";
}
static void show(Val *v, Buf *out) {
    switch (v->k) {
    case V_NUM: num_text(v, out); break;
    case V_TEXT: bput(out, v->s, v->len); break;
    case V_BOOL: bputs(out, v == &YES ? "yes" : "no"); break;
    case V_LIST:
        if (!v->n) bputs(out, "an empty list");
        for (size_t i = 0; i < v->n; i++) { if (i) bputs(out, ", "); show(v->it[i], out); }
        break;
    default: bputs(out, "nothing");
    }
}
static int equal(Val *a, Val *b) {
    if (a->k != b->k) return 0;
    switch (a->k) {
    case V_NUM: return num_cmp(a, b) == 0;
    case V_TEXT: return a->len == b->len && !memcmp(a->s, b->s, a->len);
    case V_LIST:
        if (a->n != b->n) return 0;
        for (size_t i = 0; i < a->n; i++) if (!equal(a->it[i], b->it[i])) return 0;
        return 1;
    }
    return a == b;
}

/* ---------- names and teachings ---------- */

typedef struct { char **v; int *line; int n; } Names;
static int nfind(Names *s, const char *w) { for (int i = 0; i < s->n; i++) if (!strcmp(s->v[i], w)) return i; return -1; }
static int nadd(Names *s, const char *w, int line) {
    int i = nfind(s, w); if (i >= 0) return i;
    s->v = xgrow(s->v, sizeof(char *) * (s->n + 1)); s->line = xgrow(s->line, sizeof(int) * (s->n + 1));
    s->v[s->n] = xdup(w, strlen(w)); s->line[s->n] = line; return s->n++;
}

typedef struct Node Node;
typedef struct { char *name; int line, np; Names loc; Node *body; } Teach;
static Teach *T;
static int nT;
static Names G;                 /* names made at the top of the program */
static int tfind(const char *w) { for (int i = 0; i < nT; i++) if (!strcmp(T[i].name, w)) return i; return -1; }

static const char *reserved[] = {
    "say", "ask", "make", "is", "if", "otherwise", "end", "repeat", "times", "while", "until", "for", "each",
    "in", "from", "to", "add", "teach", "do", "give", "back", "and", "or", "not", "plus", "minus", "divided",
    "by", "yes", "no", "the", "of", "stop", "skip", "than", "with", "contains", NULL};
static const char *phrase_words[] = {
    "length", "item", "first", "last", "remainder", "whole", "part", "number", "bigger", "smaller", "at",
    "least", "most", "list", "new", "a", "an", NULL};
static int in(const char **set, const char *w) { for (int i = 0; set[i]; i++) if (!strcmp(set[i], w)) return 1; return 0; }
static int is_reserved(const char *w) { return in(reserved, w); }

static void check_name(int line, const char *w) {
    if (is_reserved(w)) fail(line, fmt("Pick another name, such as my_%s.", w), "\"%s\" is a Noodle word, so it can't be used as a name.", w);
    if (isdigit((unsigned char)w[0])) fail(line, NULL, "A name can't start with a digit.");
}

static int dist(const char *a, const char *b) {
    int n = (int)strlen(a), m = (int)strlen(b);
    if (n > 40 || m > 40 || abs(n - m) > 2) return 99;
    int d[41][41];
    for (int i = 0; i <= n; i++) d[i][0] = i;
    for (int j = 0; j <= m; j++) d[0][j] = j;
    for (int i = 1; i <= n; i++)
        for (int j = 1; j <= m; j++) {
            int c = d[i - 1][j - 1] + (a[i - 1] != b[j - 1]);
            if (d[i - 1][j] + 1 < c) c = d[i - 1][j] + 1;
            if (d[i][j - 1] + 1 < c) c = d[i][j - 1] + 1;
            d[i][j] = c;
        }
    return d[n][m];
}

/* ---------- syntax tree ---------- */

enum {
    N_BLOCK = 1, N_SAY, N_MAKE, N_ADD, N_IF, N_TIMES, N_WHILE, N_UNTIL, N_EACH, N_RANGE, N_STOP, N_SKIP,
    N_GIVE, N_DO, N_ASK,
    E_LIT, E_VAR, E_CALL, E_LIST, E_TMPL, E_BIN, E_NOT, E_LEN, E_ITEM, E_FIRST, E_LAST, E_WHOLE, E_NUMIN
};
enum { O_AND = 1, O_OR, O_PLUS, O_MINUS, O_TIMES, O_DIV, O_REM, O_EQ, O_NE, O_GT, O_LT, O_GE, O_LE, O_HAS };
struct Node { int k, line, op, loc, slot, t, n; const char *name; Node *a, *b, *c, **kid; Val *v; };

/* ---------- parser ---------- */

static Tok *tk;
static int tp, tn, cl, li, cur_t = -1, loops;

static Node *nn(int k) { Node *x = xalloc(sizeof *x); x->k = k; x->line = cl; return x; }
static void kid(Node *x, Node *y) { x->kid = xgrow(x->kid, sizeof(Node *) * (x->n + 1)); x->kid[x->n++] = y; }
static int end_(void) { return tp >= tn; }
static int isw(int i, const char *w) { return i < tn && tk[i].ty == T_WORD && !strcmp(tk[i].s, w); }
static int at(const char *w) { return isw(tp, w); }
static int acc(const char *w) { if (at(w)) { tp++; return 1; } return 0; }
static char *describe(int i) {
    if (i >= tn) return "the end of the line";
    switch (tk[i].ty) {
    case T_TEXT: return "some text in quotes";
    case T_COMMA: return "a comma";
    case T_POSS: return "'s";
    case T_NUM: return tk[i].s;
    }
    return fmt("\"%s\"", tk[i].raw);
}
static void need(const char *w, const char *where) {
    if (!acc(w)) fail(cl, NULL, "I expected the word \"%s\" %s, but found %s.", w, where, describe(tp));
}
static void done(void) { if (!end_()) fail(cl, NULL, "I didn't expect %s here.", describe(tp)); }

static int resolve(const char *w, int *loc) {
    if (cur_t >= 0) { int i = nfind(&T[cur_t].loc, w); if (i >= 0) { *loc = 1; return i; } }
    *loc = 0; return nfind(&G, w);
}
static int known(const char *w) { int l; return resolve(w, &l) >= 0 || tfind(w) >= 0; }
static const char *suggest(const char *w) {
    int len = (int)strlen(w), lim = len >= 6 ? 2 : len >= 3 ? 1 : 0, best = lim + 1;
    const char *r = NULL;
    Names *sets[2] = {&G, cur_t >= 0 ? &T[cur_t].loc : NULL};
    for (int s = 0; s < 2; s++)
        for (int i = 0; sets[s] && i < sets[s]->n; i++) { int d = dist(w, sets[s]->v[i]); if (d < best) { best = d; r = sets[s]->v[i]; } }
    for (int i = 0; i < nT; i++) { int d = dist(w, T[i].name); if (d < best) { best = d; r = T[i].name; } }
    return r;
}
static void unknown(const char *w) {
    const char *s = suggest(w);
    fail(cl, s ? fmt("Did you mean %s?", s) : fmt("If %s is new, make it first, for example:\nmake %s is 0", w, w),
         "I don't know anything called %s yet.", w);
}

static Node *expr(void);
static Node *unary(void);
static Node *operand(int start, int arg);
static Node *mul(void);
static Node *add(void);
static Node *bin(int op, Node *a, Node *b) { Node *x = nn(E_BIN); x->op = op; x->a = a; x->b = b; return x; }
static Node *pre(int k, Node *a) { Node *x = nn(k); x->a = a; return x; }

static char *raw_span(int a, int b) {
    Buf s = {0}; bput(&s, "", 0);
    for (int i = a; i < b && i < tn; i++) { if (i > a) bputs(&s, " "); bputs(&s, tk[i].ty == T_TEXT ? fmt("\"%s\"", tk[i].raw) : tk[i].raw); }
    return s.s;
}
/* A teaching or a phrase like "length of" takes one simple value. If arithmetic
 * follows that value, the line has two readings, so Noodle refuses to guess. */
static Node *operand(int start, int arg) {
    Node *x = unary();
    if (at("plus") || at("minus") || at("times") || at("divided")) {
        int opn = at("divided") ? 2 : 1;
        char *head = raw_span(start, arg), *val = raw_span(arg, tp), *rest = raw_span(tp, tp + opn + 1);
        fail(cl, fmt("It could mean: %s %s, and then %s.\nOr it could mean: %s the result of %s %s.\n"
                     "Give the value its own name first, for example:\nmake new_value is %s %s\nthen use: %s new_value",
                     head, val, rest, head, val, rest, val, rest, head),
             "This line could mean two different things.");
    }
    return x;
}

static Node *call_args(int ti) {
    int start = tp - 1;
    Node *x = nn(E_CALL); x->t = ti; x->name = T[ti].name;
    for (int i = 0; i < T[ti].np; i++) {
        if (i) need("and", fmt("between the values given to %s", T[ti].name));
        if (end_()) fail(cl, NULL, "%s needs %d value%s, but this line gives it %d.", T[ti].name, T[ti].np, T[ti].np == 1 ? "" : "s", i);
        kid(x, operand(start, tp));
    }
    return x;
}

static Node *sub_expr(const char *s, size_t n) {   /* parse the inside of a [ ] slot in a text */
    Tok *st = tk; int sp = tp, sn = tn;
    char *src = xdup(s, n); lex(src, cl, &tk, &tn); tp = 0;
    if (!tn) fail(cl, "Put a name inside the [ ], for example:\nsay \"Hi [name]\"", "This text has an empty [ ].");
    Node *x = expr(); done();
    tk = st; tp = sp; tn = sn; return x;
}
static Node *text_node(Tok *t) {
    const char *s = t->s; size_t n = t->len;
    Node *x = nn(E_TMPL); Buf lit = {0}; bput(&lit, "", 0);
    for (size_t i = 0; i < n;) {
        if ((s[i] == '[' || s[i] == ']') && i + 1 < n && s[i + 1] == s[i]) { bput(&lit, s + i, 1); i += 2; continue; }
        if (s[i] == ']') fail(cl, "To show a ] itself, write ]].", "This text has a ] without a [ before it.");
        if (s[i] == '[') {
            size_t j = i + 1; while (j < n && s[j] != ']') j++;
            if (j >= n) fail(cl, "To show a [ itself, write [[.", "A [ in this text has no ] to close it.");
            if (lit.n) { Node *l = nn(E_LIT); l->v = mktext(lit.s, lit.n); kid(x, l); lit.n = 0; }
            kid(x, sub_expr(s + i + 1, j - i - 1)); i = j + 1; continue;
        }
        bput(&lit, s + i, 1); i++;
    }
    if (lit.n || !x->n) { Node *l = nn(E_LIT); l->v = mktext(lit.s, lit.n); kid(x, l); }
    free(lit.s);
    if (x->n == 1 && x->kid[0]->k == E_LIT) return x->kid[0];
    return x;
}

static int op_word(int i) {
    static const char *ops[] = {"and", "or", "is", "plus", "minus", "times", "divided", "of", "contains", "to", "in", "from", "by", "than", NULL};
    return i < tn && tk[i].ty == T_WORD && in(ops, tk[i].s);
}
static int later_of(void) { for (int i = tp + 1; i < tn; i++) if (isw(i, "of")) return 1; return 0; }

static Node *primary(void) {
    if (end_()) fail(cl, NULL, "This line stops too early. I expected a value at the end.");
    Tok *t = &tk[tp];
    if (t->ty == T_NUM) { tp++; Node *x = nn(E_LIT); x->v = parse_num(t->s); return x; }
    if (t->ty == T_TEXT) { tp++; return text_node(t); }
    if (t->ty != T_WORD) fail(cl, NULL, "I expected a value here, but found %s.", describe(tp));
    if (acc("yes")) { Node *x = nn(E_LIT); x->v = &YES; return x; }
    if (acc("no")) { Node *x = nn(E_LIT); x->v = &NO; return x; }
    if (isw(tp, "a") && isw(tp + 1, "list") && isw(tp + 2, "of")) {
        tp += 3; Node *x = nn(E_LIST);
        for (;;) {
            if (isw(tp, "a") && isw(tp + 1, "list"))
                fail(cl, "Make the inner list first and give it a name, then put the name in the outer list.",
                     "A list written inside another list on one line could mean two things.");
            kid(x, add());
            if (tp < tn && tk[tp].ty == T_COMMA) tp++; else return x;
        }
    }
    if (isw(tp, "a") && isw(tp + 1, "new") && isw(tp + 2, "list")) { tp += 3; return nn(E_LIST); }
    if (is_reserved(t->s)) fail(cl, NULL, "I expected a value here, but found the Noodle word \"%s\".", t->raw);
    int ti = tfind(t->s);
    if (ti >= 0) { tp++; return call_args(ti); }
    int loc, slot = resolve(t->s, &loc);
    if (slot < 0) unknown(t->raw);
    tp++;
    Node *x = nn(E_VAR); x->loc = loc; x->slot = slot; x->name = t->s;
    if (tp < tn && tk[tp].ty == T_POSS) fail(cl, NULL, "Records, like %s's name, are not part of Noodle Seed yet.", t->raw);
    return x;
}

static Node *unary(void) {
    int st = tp;
    if (isw(tp, "length") && isw(tp + 1, "of")) { tp += 2; return pre(E_LEN, operand(st, tp)); }
    if (isw(tp, "first") && isw(tp + 1, "item") && isw(tp + 2, "of")) { tp += 3; return pre(E_FIRST, operand(st, tp)); }
    if (isw(tp, "last") && isw(tp + 1, "item") && isw(tp + 2, "of")) { tp += 3; return pre(E_LAST, operand(st, tp)); }
    if (at("item") && tp + 1 < tn && !op_word(tp + 1) && tk[tp + 1].ty != T_COMMA && later_of()) {
        tp++; Node *x = nn(E_ITEM); x->a = add(); need("of", "after the item number"); x->b = operand(st, tp); return x;
    }
    if (isw(tp, "remainder") && isw(tp + 1, "of")) {
        tp += 2; Node *d = mul();
        if (d->k != E_BIN || d->op != O_DIV) fail(cl, "For example:\nsay remainder of 7 divided by 2", "remainder of needs a division after it.");
        d->op = O_REM; return d;
    }
    if (isw(tp, "whole") && isw(tp + 1, "part") && isw(tp + 2, "of")) { tp += 3; return pre(E_WHOLE, operand(st, tp)); }
    if (isw(tp, "the") && isw(tp + 1, "number") && isw(tp + 2, "in")) { tp += 3; return pre(E_NUMIN, operand(st, tp)); }
    if (at("the")) fail(cl, NULL, "I don't know what \"the %s\" means here.", tp + 1 < tn ? tk[tp + 1].raw : "");
    return primary();
}
static Node *mul(void) {
    Node *x = unary();
    for (;;) {
        if (acc("times")) x = bin(O_TIMES, x, unary());
        else if (isw(tp, "divided") && isw(tp + 1, "by")) { tp += 2; x = bin(O_DIV, x, unary()); }
        else if (at("divided")) { tp++; need("by", "after \"divided\""); }
        else return x;
    }
}
static Node *add(void) {
    Node *x = mul();
    for (;;) {
        if (acc("plus")) x = bin(O_PLUS, x, mul());
        else if (acc("minus")) x = bin(O_MINUS, x, mul());
        else return x;
    }
}
static Node *cmp(void) {
    Node *x = add(); int op = 0;
    if (acc("is")) {
        if (acc("not")) {
            op = O_NE;
            if (at("bigger") || at("smaller") || at("at"))
                fail(cl, "Say it the other way around, for example \"is at most\" instead of \"is not bigger than\".",
                     "I can't read \"is not %s\".", tk[tp].raw);
        }
        else if (isw(tp, "bigger") && isw(tp + 1, "than")) { tp += 2; op = O_GT; }
        else if (isw(tp, "smaller") && isw(tp + 1, "than")) { tp += 2; op = O_LT; }
        else if (isw(tp, "at") && isw(tp + 1, "least")) { tp += 2; op = O_GE; }
        else if (isw(tp, "at") && isw(tp + 1, "most")) { tp += 2; op = O_LE; }
        else op = O_EQ;
    } else if (acc("contains")) op = O_HAS;
    if (!op) return x;
    x = bin(op, x, add());
    if (at("is") || at("contains")) fail(cl, "Split it into two comparisons joined by \"and\".", "I can only compare two things at a time.");
    return x;
}
static Node *notx(void) { if (acc("not")) return pre(E_NOT, notx()); return cmp(); }
static Node *expr(void) {
    Node *x = notx(); int op = 0;
    while (at("and") || at("or")) {
        int o = at("and") ? O_AND : O_OR;
        if (op && o != op)
            fail(cl, "Split the line in two. Give the \"and\" part its own name first, then use that name with \"or\".",
                 "This line mixes \"and\" with \"or\", so it could mean two different things.");
        op = o; tp++; x = bin(o, x, notx());
    }
    return x;
}

static Node *say_what(void) {
    if (end_()) fail(cl, "For example:\nsay \"hello\"", "say needs something to show.");
    int words = 1, names = 0, plain = 0;
    for (int i = tp; i < tn; i++) {
        if (tk[i].ty != T_WORD) { words = 0; break; }
        if (known(tk[i].s)) names++;
        else if (!is_reserved(tk[i].s) && !in(phrase_words, tk[i].s)) plain++;
    }
    if (words && !names && plain) {   /* plain words: show the words themselves */
        Buf b = {0}; bput(&b, "", 0);
        for (int i = tp; i < tn; i++) {
            const char *s = suggest(tk[i].s);
            if (s) fail(cl, fmt("Did you mean %s? If you meant the word itself, put it in quotes:\nsay \"%s\"", s, tk[i].raw),
                        "\"%s\" looks like the name %s, so I'm not sure what you meant.", tk[i].raw, s);
            if (i > tp) bputs(&b, " ");
            bputs(&b, tk[i].raw);
        }
        tp = tn; Node *x = nn(E_LIT); x->v = mktext(b.s, b.n); free(b.s); return x;
    }
    if (words && plain) {
        Buf b = {0};
        for (int i = tp; i < tn; i++) {
            char *w = known(tk[i].s) ? fmt("[%s]", tk[i].raw) : fmt("%s", tk[i].raw);
            if (i > tp) bputs(&b, " ");
            bputs(&b, w); free(w);
        }
        fail(cl, fmt("Put the words in quotes, and put names inside [ ], like this:\nsay \"%s\"", b.s),
             "I can't tell which words here are names and which are just words.");
    }
    return expr();
}

static void set_target(Node *x) {
    if (end_() || tk[tp].ty != T_WORD) fail(cl, NULL, "I expected a name here, but found %s.", describe(tp));
    const char *w = tk[tp].s;
    check_name(cl, w);
    if (tfind(w) >= 0) fail(cl, NULL, "%s is the name of a teaching, so it can't hold a value too.", w);
    x->slot = resolve(w, &x->loc); x->name = w;
    if (x->slot < 0) unknown(tk[tp].raw);
    tp++;
}

static Node *block(void);
static void line_start(void) { tk = lines[li].t; tn = lines[li].n; tp = 0; cl = li + 1; }
static int first_is(const char *w) {
    return li < nlines && lines[li].n && lines[li].t[0].ty == T_WORD && !strcmp(lines[li].t[0].s, w);
}
static void skip_blank(void) { while (li < nlines && !lines[li].n) li++; }
static void need_end(int open, const char *what) {
    skip_blank();
    if (li >= nlines) fail(open, "Add a line with just the word end where the block should stop.", "This %s never ends.", what);
    if (first_is("otherwise")) {
        line_start();
        if (!strcmp(what, "if")) fail(cl, NULL, "An if can have only one plain otherwise, and it must come last.");
        fail(cl, "otherwise only works inside an if.", "I didn't expect \"otherwise\" here.");
    }
    line_start(); tp = 1; done(); li++;
}

static Node *if_rest(int open) {
    Node *x = nn(N_IF); x->a = expr(); done(); x->b = block(); skip_blank();
    if (first_is("otherwise")) {
        line_start(); li++; tp = 1;
        if (acc("if")) { x->c = if_rest(open); return x; }
        done(); x->c = block();
    }
    need_end(open, "if");
    return x;
}

static Node *stmt(void) {
    line_start(); li++;
    int open = cl;
    Node *x;
    if (tk[0].ty != T_WORD) fail(cl, "Lines start with a Noodle word, such as say, make, or if.", "I don't know what to do with this line.");
    if (acc("say")) { x = nn(N_SAY); x->a = say_what(); }
    else if (acc("make")) {
        x = nn(N_MAKE); set_target(x);
        if (at("with")) fail(cl, NULL, "Records are not part of Noodle Seed yet.");
        need("is", fmt("after make %s", x->name)); x->a = expr();
    }
    else if (acc("add")) { x = nn(N_ADD); x->a = expr(); need("to", "after the thing to add"); set_target(x); }
    else if (acc("ask")) { x = nn(N_ASK); x->a = expr(); x->slot = resolve("answer", &x->loc); x->name = "answer"; }
    else if (acc("if")) return if_rest(open);
    else if (acc("repeat")) {
        if (acc("while")) { x = nn(N_WHILE); x->a = expr(); }
        else if (acc("until")) { x = nn(N_UNTIL); x->a = expr(); }
        else {
            if (tn < 2 || !isw(tn - 1, "times"))
                fail(cl, "For example:\nrepeat 3 times\nrepeat while count is smaller than 10", "I don't know how many times to repeat.");
            x = nn(N_TIMES); tn--; x->a = expr(); done(); tn++; tp++;
        }
        done(); loops++; x->b = block(); loops--; need_end(open, "repeat"); return x;
    }
    else if (acc("for")) {
        need("each", "after \"for\"");
        Node *v = nn(0); set_target(v);
        if (acc("in")) { x = nn(N_EACH); x->a = expr(); }
        else if (acc("from")) { x = nn(N_RANGE); x->a = expr(); need("to", "after the starting number"); x->c = expr(); }
        else fail(cl, "For example:\nfor each item in shopping\nfor each n from 1 to 10", "I expected \"in\" or \"from\" after for each %s.", v->name);
        x->loc = v->loc; x->slot = v->slot; x->name = v->name;
        done(); loops++; x->b = block(); loops--; need_end(open, "for each"); return x;
    }
    else if (at("stop") || at("skip")) {
        x = nn(at("stop") ? N_STOP : N_SKIP); tp++;
        if (!loops) fail(cl, NULL, "%s only works inside repeat or for each.", x->k == N_STOP ? "stop" : "skip");
    }
    else if (acc("give")) {
        need("back", "after \"give\"");
        if (cur_t < 0) fail(cl, NULL, "give back only works inside a teaching.");
        x = nn(N_GIVE); x->a = expr();
    }
    else if (acc("do")) {
        if (end_() || tk[tp].ty != T_WORD || tfind(tk[tp].s) < 0)
            fail(cl, "For example:\ndo greet \"Mo\"", "do needs the name of a teaching after it.");
        x = nn(N_DO); x->a = call_args(tfind(tk[tp++].s));
    }
    else if (acc("teach")) {
        int ti = 0; while (T[ti].line != cl) ti++;
        cur_t = ti; int saved = loops; loops = 0;
        T[ti].body = block(); need_end(open, "teach");
        cur_t = -1; loops = saved; return NULL;
    }
    else {
        const char *w = tk[0].s;
        if (tfind(w) >= 0) fail(cl, fmt("To use the teaching %s, start the line with do:\ndo %s", w, lines[cl - 1].src), "This line starts with the teaching %s.", w);
        if (known(w)) fail(cl, fmt("To change it, use make:\nmake %s is ...", w), "This line starts with the name %s, but doesn't say what to do with it.", w);
        static const char *starts[] = {"say", "ask", "make", "add", "if", "repeat", "for", "teach", "do", "give", "stop", "skip", NULL};
        const char *best = NULL; int bd = 3;
        for (int i = 0; starts[i]; i++) { int d = dist(w, starts[i]); if (d < bd) { bd = d; best = starts[i]; } }
        fail(cl, best ? fmt("Did you mean %s?", best) : "Lines start with a Noodle word, such as say, make, or if.",
             "I don't know the word \"%s\" at the start of a line.", tk[0].raw);
    }
    done();
    return x;
}

static Node *block(void) {
    Node *b = nn(N_BLOCK);
    for (;;) {
        skip_blank();
        if (li >= nlines || first_is("end") || first_is("otherwise")) return b;
        Node *s = stmt();
        if (s) kid(b, s);
    }
}

/* Find every teaching and every name the program makes, so lines can use
 * teachings written further down and so nothing shadows silently. */
static void prescan(void) {
    int depth = 0, cur = -1;
    for (int i = 0; i < nlines; i++) {
        Line *L = &lines[i]; int ln = i + 1;
        if (!L->n || L->t[0].ty != T_WORD) continue;
        const char *w = L->t[0].s;
        if (!strcmp(w, "end")) { if (depth > 0 && --depth == 0) cur = -1; continue; }
        if (!strcmp(w, "teach")) {
            if (depth) fail(ln, "Put each teach at the left of the program, outside other blocks.", "A teach can't go inside another block.");
            if (L->n < 2 || L->t[1].ty != T_WORD) fail(ln, "For example:\nteach double number", "teach needs a name.");
            check_name(ln, L->t[1].s);
            int old = tfind(L->t[1].s);
            if (old >= 0) fail(ln, "Give one of them a different name.", "There is already a teaching called %s, on line %d.", L->t[1].s, T[old].line);
            T = xgrow(T, sizeof(Teach) * (nT + 1));
            Teach *t = &T[nT]; memset(t, 0, sizeof *t); t->name = L->t[1].s; t->line = ln;
            for (int j = 2; j < L->n; j++) {
                if (j > 2) {
                    if (L->t[j].ty != T_WORD || strcmp(L->t[j].s, "and")) fail(ln, "For example:\nteach area width and height", "Put \"and\" between the names a teaching needs.");
                    if (++j >= L->n) fail(ln, NULL, "I expected another name after \"and\".");
                }
                if (L->t[j].ty != T_WORD) fail(ln, "For example:\nteach double number", "A teaching's inputs must be names.");
                check_name(ln, L->t[j].s);
                if (nfind(&t->loc, L->t[j].s) >= 0) fail(ln, NULL, "%s appears twice in this teach line.", L->t[j].s);
                nadd(&t->loc, L->t[j].s, ln); t->np++;
            }
            cur = nT++; depth++; continue;
        }
        if (!strcmp(w, "if") || !strcmp(w, "repeat") || !strcmp(w, "for")) depth++;
        const char *made = NULL;
        if (!strcmp(w, "make") && L->n > 1 && L->t[1].ty == T_WORD) made = L->t[1].s;
        else if (!strcmp(w, "for") && L->n > 2 && L->t[2].ty == T_WORD) made = L->t[2].s;
        else if (!strcmp(w, "ask")) made = "answer";
        if (made) { check_name(ln, made); nadd(cur >= 0 ? &T[cur].loc : &G, made, ln); }
    }
    for (int i = 0; i < nT; i++)
        for (int j = 0; j < T[i].loc.n; j++) {
            const char *w = T[i].loc.v[j]; int g = nfind(&G, w);
            if (g >= 0) fail(T[i].loc.line[j], "Inside a teaching, pick a different name, or give back the new value instead.",
                             "%s is already a name for the whole program (line %d), so a teaching can't have its own %s.", w, G.line[g], w);
            if (tfind(w) >= 0) fail(T[i].loc.line[j], NULL, "%s is already the name of a teaching (line %d).", w, T[tfind(w)].line);
        }
    for (int i = 0; i < G.n; i++)
        if (tfind(G.v[i]) >= 0) fail(G.line[i], NULL, "%s is already the name of a teaching (line %d).", G.v[i], T[tfind(G.v[i])].line);
}

/* ---------- running ---------- */

typedef struct { Val **s; } Frame;
static Val **GS;          /* values of top-level names */
static Frame *fr;
static int depth;
static Val *ret_val;
static char **answers;    /* test mode: prepared answers for ask */
static int nanswers, next_answer;
enum { GO, STOP, SKIP, GIVE };

static Val **slot_of(Node *x) { return x->loc ? &fr->s[x->slot] : &GS[x->slot]; }
static void set_slot(Node *x, Val *v) { Val **p = slot_of(x); unref(*p); *p = v; }

static Val *eval(Node *x);
static int exec(Node *b);
static Val *val(Node *x) {
    Val *v = eval(x);
    if (v->k == V_NOTHING)
        fail(x->line, fmt("Add a give back line to %s, or use it on its own line with do.", x->name),
             "%s doesn't give back anything, so there is no value to use here.", x->name);
    return v;
}
static Val *num_of(Node *x, const char *what) {
    Val *v = val(x);
    if (v->k != V_NUM) {
        const char *h = v->k == V_TEXT ? "To put text together, use [ ] inside quotes, for example:\nsay \"[first_name] [last_name]\"" : NULL;
        fail(x->line, h, "%s needs a number, but this is %s.", what, kname(v));
    }
    return v;
}
static int truth(Node *x, const char *what) {
    Val *v = val(x);
    if (v->k != V_BOOL) fail(x->line, "Compare it to something, for example:\nif count is 0", "%s needs yes or no, but this is %s.", what, kname(v));
    return v == &YES;
}
static long long whole(Node *x, const char *what) {
    Val *v = num_of(x, what);
    if (!bone(v->den) || v->num.n > 2) fail(x->line, NULL, "%s needs a whole number that isn't too big.", what);
    long long r = v->num.n ? v->num.d[0] + (v->num.n > 1 ? (long long)v->num.d[1] * BASE : 0) : 0;
    if (v->neg) r = -r;
    unref(v); return r;
}
static int ascii(Val *t, int line) {
    for (size_t i = 0; i < t->len; i++)
        if ((unsigned char)t->s[i] >= 0x80)
            fail(line, NULL, "Noodle Seed can only count the letters of plain English text for now, and this text has other characters.");
    return 1;
}
static Val *call(Node *x) {
    Teach *t = &T[x->t];
    Frame f; f.s = xalloc(sizeof(Val *) * (t->loc.n ? t->loc.n : 1));
    for (int i = 0; i < t->np; i++) f.s[i] = val(x->kid[i]);
    if (depth >= MAX_DEPTH)
        fail(x->line, fmt("Check that %s has a way to stop calling itself.", t->name),
             "This went too deep: teachings were called inside each other more than %d times, starting with %s.", MAX_DEPTH, t->name);
    Frame *saved = fr; fr = &f; depth++;
    Val *r = exec(t->body) == GIVE ? ret_val : &NOTHING;
    ret_val = NULL; depth--; fr = saved;
    for (int i = 0; i < t->loc.n; i++) unref(f.s[i]);
    free(f.s);
    return r;
}
static Val *binop(Node *x) {
    if (x->op == O_AND) return mkbool(truth(x->a, "and") && truth(x->b, "and"));
    if (x->op == O_OR) return mkbool(truth(x->a, "or") || truth(x->b, "or"));
    Val *a = val(x->a), *b = val(x->b), *r = NULL;
    if (x->op == O_EQ || x->op == O_NE) {
        if (a->k != b->k)
            fail(x->line, "Both sides of \"is\" need to be the same kind of value.", "I can't compare %s with %s.", kname(a), kname(b));
        r = mkbool(equal(a, b) == (x->op == O_EQ));
    } else if (x->op == O_HAS) {
        if (a->k == V_TEXT) {
            if (b->k != V_TEXT) fail(x->line, NULL, "Text can only contain other text, but this is %s.", kname(b));
            int f = !b->len;
            for (size_t i = 0; !f && i + b->len <= a->len; i++) f = !memcmp(a->s + i, b->s, b->len);
            r = mkbool(f);
        } else if (a->k == V_LIST) {
            int f = 0; for (size_t i = 0; !f && i < a->n; i++) f = equal(a->it[i], b);
            r = mkbool(f);
        } else fail(x->line, NULL, "contains works with text or a list, but this is %s.", kname(a));
    } else {
        static const char *names[] = {"", "", "", "plus", "minus", "times", "divided by", "remainder of", "", "",
                                      "is bigger than", "is smaller than", "is at least", "is at most"};
        const char *w = names[x->op];
        if (a->k != V_NUM || b->k != V_NUM) {
            Val *bad = a->k != V_NUM ? a : b;
            fail(x->line, bad->k == V_TEXT && x->op == O_PLUS ? "To put text together, use [ ] inside quotes, for example:\nsay \"[first_name] [last_name]\"" : NULL,
                 "\"%s\" works with numbers, but %s is %s.", w, bad == a ? "the left side" : "the right side", kname(bad));
        }
        switch (x->op) {
        case O_PLUS: r = num_add(a, b, 0); break;
        case O_MINUS: r = num_add(a, b, 1); break;
        case O_TIMES: r = num_mul(a, b); break;
        case O_DIV: case O_REM:
            if (!b->num.n) fail(x->line, "Check the number before dividing, for example:\nif count is not 0", "I can't divide by zero.");
            if (x->op == O_DIV) { r = num_div(a, b); break; }
            if (!bone(a->den) || !bone(b->den)) fail(x->line, NULL, "remainder of works with whole numbers only.");
            { Val *q = num_div(a, b), *t = num_trunc(q), *m = num_mul(t, b); r = num_add(a, m, 1); unref(q); unref(t); unref(m); }
            break;
        case O_GT: r = mkbool(num_cmp(a, b) > 0); break;
        case O_LT: r = mkbool(num_cmp(a, b) < 0); break;
        case O_GE: r = mkbool(num_cmp(a, b) >= 0); break;
        case O_LE: r = mkbool(num_cmp(a, b) <= 0); break;
        }
    }
    unref(a); unref(b); return r;
}
static Val *item_of(Val *c, long long i, Node *x) {
    size_t n = c->k == V_LIST ? c->n : c->len;
    if (c->k != V_LIST && c->k != V_TEXT) fail(x->line, NULL, "I can only take items from a list or text, but this is %s.", kname(c));
    if (c->k == V_TEXT) ascii(c, x->line);
    if (!n) fail(x->line, NULL, "This %s is empty, so it has no items.", c->k == V_LIST ? "list" : "text");
    if (i < 1 || (size_t)i > n) fail(x->line, NULL, "There is no item %lld. This %s has %zu item%s.", i, c->k == V_LIST ? "list" : "text", n, n == 1 ? "" : "s");
    return c->k == V_LIST ? ref(c->it[i - 1]) : mktext(c->s + i - 1, 1);
}
static Val *number_in(Val *t, Node *x) {
    if (t->k == V_NUM) return ref(t);
    if (t->k != V_TEXT) fail(x->line, NULL, "the number in needs some text, but this is %s.", kname(t));
    size_t a = 0, b = t->len;
    while (a < b && t->s[a] == ' ') a++;
    while (b > a && t->s[b - 1] == ' ') b--;
    char *s = xdup(t->s + a, b - a); const char *p = s;
    if (*p == '-') p++;
    int ok = isdigit((unsigned char)*p);
    while (isdigit((unsigned char)*p)) p++;
    if (*p == '.') { p++; ok = ok && isdigit((unsigned char)*p); while (isdigit((unsigned char)*p)) p++; }
    if (!ok || *p) fail(x->line, NULL, "\"%.*s\" isn't a number.", (int)t->len, t->s);
    Val *v = parse_num(s); free(s); return v;
}
static Val *eval(Node *x) {
    Val *a, *r;
    switch (x->k) {
    case E_LIT: return ref(x->v);
    case E_VAR:
        a = *slot_of(x);
        if (!a) fail(x->line, x->loc ? NULL : fmt("It gets its value on line %d.", G.line[x->slot]), "%s doesn't have a value yet.", x->name);
        return ref(a);
    case E_LIST: r = mklist(); for (int i = 0; i < x->n; i++) list_push(r, val(x->kid[i])); return r;
    case E_TMPL: {
        Buf b = {0}; bput(&b, "", 0);
        for (int i = 0; i < x->n; i++) { a = val(x->kid[i]); show(a, &b); unref(a); }
        r = mktext(b.s, b.n); free(b.s); return r;
    }
    case E_BIN: return binop(x);
    case E_NOT: return mkbool(!truth(x->a, "not"));
    case E_CALL: return call(x);
    case E_LEN:
        a = val(x->a);
        if (a->k == V_LIST) r = mkint((long long)a->n);
        else if (a->k == V_TEXT) { ascii(a, x->line); r = mkint((long long)a->len); }
        else fail(x->line, NULL, "length of works with text or a list, but this is %s.", kname(a));
        unref(a); return r;
    case E_ITEM: { long long i = whole(x->a, "item"); a = val(x->b); r = item_of(a, i, x); unref(a); return r; }
    case E_FIRST: a = val(x->a); r = item_of(a, 1, x); unref(a); return r;
    case E_LAST: a = val(x->a); r = item_of(a, a->k == V_LIST ? (long long)a->n : (long long)a->len, x); unref(a); return r;
    case E_WHOLE: a = num_of(x->a, "whole part of"); r = num_trunc(a); unref(a); return r;
    case E_NUMIN: a = val(x->a); r = number_in(a, x); unref(a); return r;
    }
    fail(x->line, "Sorry, this is a bug in the Noodle bootstrap. Please report it with your program.", "Something went wrong inside Noodle.");
    return NULL;
}

static char *read_answer(int line) {
    if (capturing) {
        if (next_answer >= nanswers) fail(line, "Add another line to the test's .input file.", "The test ran out of answers to give.");
        return answers[next_answer++];
    }
    fflush(stdout);
    Buf b = {0}; bput(&b, "", 0); int c, got = 0;
    while ((c = getchar()) != EOF && c != '\n') { got = 1; if (c != '\r') { char ch = (char)c; bput(&b, &ch, 1); } }
    if (!got && c == EOF) fail(line, NULL, "I asked a question, but there was nothing to read.");
    return b.s;
}

static int loop_body(Node *s, int *out) {   /* returns 1 if the loop must end */
    int r = exec(s->b);   /* SKIP just ends this round */
    if (r == STOP) { *out = GO; return 1; }
    if (r == GIVE) { *out = GIVE; return 1; }
    return 0;
}
static int step(Node *s) {
    Val *v; int out = GO;
    switch (s->k) {
    case N_SAY: { v = val(s->a); Buf b = {0}; show(v, &b); bput(&b, "\n", 1); emit(0, b.s, b.n); free(b.s); unref(v); return GO; }
    case N_MAKE: set_slot(s, val(s->a)); return GO;
    case N_ADD: {
        v = val(s->a); Val **p = slot_of(s);
        if (!*p) fail(s->line, NULL, "%s doesn't have a value yet.", s->name);
        if ((*p)->k != V_LIST) fail(s->line, fmt("To start a list, write:\nmake %s is a new list", s->name), "I can only add to a list, but %s is %s.", s->name, kname(*p));
        if ((*p)->rc > 1) { Val *c = list_copy(*p); unref(*p); *p = c; }   /* only copy when shared */
        list_push(*p, v); return GO;
    }
    case N_ASK: {
        v = val(s->a); Buf b = {0}; show(v, &b); unref(v);
        if (b.n && b.s[b.n - 1] != ' ') bputs(&b, " ");
        emit(0, b.s, b.n); free(b.s);
        char *ans = read_answer(s->line);
        if (capturing) { emit(0, ans, strlen(ans)); emit(0, "\n", 1); }
        set_slot(s, mktext(ans, strlen(ans))); return GO;
    }
    case N_IF:
        if (truth(s->a, "if")) return exec(s->b);
        if (s->c) return s->c->k == N_IF ? step(s->c) : exec(s->c);
        return GO;
    case N_TIMES: {
        long long n = whole(s->a, "repeat");
        if (n < 0) fail(s->line, NULL, "I can't repeat something %lld times.", n);
        for (long long i = 0; i < n; i++) if (loop_body(s, &out)) break;
        return out;
    }
    case N_WHILE: while (truth(s->a, "repeat while")) if (loop_body(s, &out)) break; return out;
    case N_UNTIL: while (!truth(s->a, "repeat until")) if (loop_body(s, &out)) break; return out;
    case N_EACH: {
        v = val(s->a);
        if (v->k == V_TEXT) ascii(v, s->line);
        else if (v->k != V_LIST) fail(s->line, NULL, "for each needs a list or text, but this is %s.", kname(v));
        size_t n = v->k == V_LIST ? v->n : v->len;
        for (size_t i = 0; i < n; i++) {
            set_slot(s, v->k == V_LIST ? ref(v->it[i]) : mktext(v->s + i, 1));
            if (loop_body(s, &out)) break;
        }
        unref(v); return out;
    }
    case N_RANGE: {
        long long a = whole(s->a, "for each"), b = whole(s->c, "for each"), d = a <= b ? 1 : -1;
        for (long long i = a;; i += d) { set_slot(s, mkint(i)); if (loop_body(s, &out) || i == b) break; }
        return out;
    }
    case N_STOP: return STOP;
    case N_SKIP: return SKIP;
    case N_GIVE: ret_val = val(s->a); return GIVE;
    case N_DO: unref(eval(s->a)); return GO;
    }
    return GO;
}
static int exec(Node *b) {
    for (int i = 0; i < b->n; i++) { int r = step(b->kid[i]); if (r != GO) return r; }
    return GO;
}

/* ---------- files, programs, the minimal test mode ---------- */

static char *read_file(const char *path, size_t *len) {
    FILE *f = fopen(path, "rb"); if (!f) return NULL;
    Buf b = {0}; bput(&b, "", 0); char t[65536]; size_t n;
    while ((n = fread(t, 1, sizeof t, f)) > 0) bput(&b, t, n);
    fclose(f); if (len) *len = b.n; return b.s;
}
static char **split_lines(char *s, int *n) {
    char **v = NULL; *n = 0;
    if (!strncmp(s, "\xEF\xBB\xBF", 3)) s += 3;
    while (*s) {
        char *e = strchr(s, '\n'); size_t len = e ? (size_t)(e - s) : strlen(s);
        if (len && s[len - 1] == '\r') len--;
        v = xgrow(v, sizeof(char *) * (*n + 1)); v[(*n)++] = xdup(s, len);
        if (!e) break;
        s = e + 1;
    }
    return v;
}
static int run_file(const char *path) {
    lines = NULL; nlines = 0; T = NULL; nT = 0; memset(&G, 0, sizeof G);
    cur_t = -1; loops = 0; fr = NULL; depth = 0; ret_val = NULL;
    if (setjmp(on_error)) return 1;
    char *src = read_file(path, NULL);
    if (!src) fail(0, "Check the file's name and folder.", "I can't find the file %s.", path);
    char **raw = split_lines(src, &nlines);
    lines = xalloc(sizeof(Line) * (nlines ? nlines : 1));
    for (int i = 0; i < nlines; i++) lines[i].src = raw[i];
    for (int i = 0; i < nlines; i++) lex(lines[i].src, i + 1, &lines[i].t, &lines[i].n);
    prescan();
    li = 0; Node *prog = block(); skip_blank();
    if (li < nlines) {
        line_start();
        fail(cl, "Check that every if, repeat, for each, and teach has exactly one end.", "This \"%s\" doesn't belong to anything.", tk[0].raw);
    }
    GS = xalloc(sizeof(Val *) * (G.n ? G.n : 1));
    exec(prog);
    return 0;
}

static int by_name(const void *a, const void *b) { return strcmp(*(char *const *)a, *(char *const *)b); }
static void collect(const char *dir, char ***v, int *n) {
    DIR *d = opendir(dir); if (!d) return;
    struct dirent *e;
    while ((e = readdir(d))) {
        if (e->d_name[0] == '.') continue;
        char *p = fmt("%s/%s", dir, e->d_name); struct stat st;
        if (stat(p, &st)) continue;
        size_t l = strlen(p);
        if (S_ISDIR(st.st_mode)) collect(p, v, n);
        else if (l > 7 && !strcmp(p + l - 7, ".noodle")) { *v = xgrow(*v, sizeof(char *) * (*n + 1)); (*v)[(*n)++] = p; }
    }
    closedir(d);
}
static int run_tests(const char *dir) {
    char **files = NULL; int n = 0, pass = 0, failed = 0;
    collect(dir, &files, &n);
    qsort(files, n, sizeof(char *), by_name);
    if (!n) { printf("I found no .noodle files in %s.\n", dir); return 1; }
    for (int i = 0; i < n; i++) {
        size_t l = strlen(files[i]);
        char *base = xdup(files[i], l - 7), *exp_path = fmt("%s.expected", base), *in_path = fmt("%s.input", base);
        char *expected = read_file(exp_path, NULL), *input = read_file(in_path, NULL);
        answers = NULL; nanswers = 0; next_answer = 0;
        if (input) answers = split_lines(input, &nanswers);
        if (!expected) { printf("FAIL %s\n     there is no %s file\n", files[i], exp_path); failed++; continue; }
        capturing = 1; cap.n = 0; bput(&cap, "", 0);
        run_file(files[i]);
        capturing = 0;
        if (!strcmp(cap.s, expected)) { pass++; continue; }
        failed++;
        printf("FAIL %s\n", files[i]);
        int ln = 1; const char *x = expected, *y = cap.s;
        while (*x && *x == *y) { if (*x == '\n') ln++; x++; y++; }
        const char *xe = strchr(x, '\n'), *ye = strchr(y, '\n');
        while (x > expected && x[-1] != '\n') x--;
        while (y > cap.s && y[-1] != '\n') y--;
        printf("     first difference on output line %d\n     expected: %.*s\n     got:      %.*s\n", ln,
               (int)(xe ? xe - x : (long)strlen(x)), x, (int)(ye ? ye - y : (long)strlen(y)), y);
    }
    printf("%d passed, %d failed\n", pass, failed);
    return failed ? 1 : 0;
}

int main(int argc, char **argv) {
    int r = 2;
    YES.rc = NO.rc = NOTHING.rc = 1 << 30; YES.k = NO.k = V_BOOL; NOTHING.k = V_NOTHING;
    if (argc == 3 && !strcmp(argv[1], "run")) r = run_file(argv[2]);
    else if (argc == 3 && !strcmp(argv[1], "test")) r = run_tests(argv[2]);
    else fputs("Noodle Seed bootstrap (temporary, see BOOTSTRAP.md)\n\n"
               "  noodle-seed run program.noodle   run a Noodle Seed program\n"
               "  noodle-seed test folder           run every .noodle file in a folder and\n"
               "                                    compare its output with its .expected file\n", stderr);
    fflush(stdout);
    return r;
}
