/**
 * Evolution test with Simple extended example fom Wikipedia: 
 * http://de.wikipedia.org/wiki/Evolutionärer_Algorithmus
 *
 * using an Vektor of ints as an Idividum
 */
#define MAX_U_INT32 4294967295L
#define TEST_NUM_IVS 128
#ifndef NULL
#define NULL 0
#endif

#include "../src/evolution.h"
#include "../src/C-Utils/Rand/src/rand.h"

typedef struct {
  int a, b, c, d, e, f;
} Vektor;

void *init_v(void *opts) {
  uint32_t *n = (uint32_t *) opts;
  Vektor *v = (Vektor *) malloc(sizeof(Vektor));

  v->a = (rand32(n) % 100) - 50;
  v->b = (rand32(n) % 100) - 50;
  v->c = (rand32(n) % 100) - 50;
  v->d = (rand32(n) % 100) - 50;
  v->e = (rand32(n) % 100) - 50;
  v->f = (rand32(n) % 100) - 50;
  
  int i;
  for (i = 0; i < 1000; i++) {
    v->a = rand32(n) * v->b + v->e;
    v->f = rand32(n) * v->a + v->a;
    v->b = rand32(n) * v->e + v->b;
    v->c = rand32(n) * v->d + v->d;
    v->d = rand32(n) * v->a + v->f;
    v->e = rand32(n) * v->b + v->a;
    v->a = rand32(n) * v->e + v->b;
    v->d = rand32(n) * v->f + v->c;
    v->e = rand32(n) * v->c + v->e;
    v->f = rand32(n) * v->a + v->d;
    v->a = rand32(n) * v->e + v->f;
    v->b = rand32(n) * v->f + v->e;
    v->c = rand32(n) * v->d + v->c;
    v->f = rand32(n) * v->d + v->a;
    v->d = rand32(n) * v->a + v->d;
    v->e = rand32(n) * v->b + v->e;
    v->c = rand32(n) * v->e + v->f;
  }        
  return v;
}

void clone_v(void *dst, void *src, void *opts) {
  (void) opts;
  Vektor *vd = (Vektor *) dst;
  Vektor *vs = (Vektor *) src;

  vd->a = vs->a;
  vd->b = vs->b;
  vd->c = vs->c;
  vd->d = vs->d;
  vd->e = vs->e;
  vd->f = vs->f;
}

void free_v(void *dst, void *opts) {
  (void) opts;
  free((Vektor *) dst);
}

void recombinate_v(Individual *src1, 
                   Individual *src2, 
                   Individual *dst, 
                   void *opts) {
  (void) opts;
  Vektor *vs1 = (Vektor *) src1->iv;
  Vektor *vs2 = (Vektor *) src2->iv;
  Vektor *vd  = (Vektor *) dst->iv;

  vd->a = vs1->a;
  vd->b = vs1->b;
  vd->c = vs1->c;
  vd->d = vs2->d;
  vd->e = vs2->e;
  vd->f = vs2->f;
}

void mutate_v(Individual *src, void *opts) {
  uint32_t *n = (uint32_t *) opts;
  Vektor *v = (Vektor *) src->iv;

    v->a += (rand32(n) % 3) - 1;
    v->b += (rand32(n) % 3) - 1;
    v->c += (rand32(n) % 3) - 1;
    v->d += (rand32(n) % 3) - 1;
    v->e += (rand32(n) % 3) - 1;
    v->f += (rand32(n) % 3) - 1;
}

int64_t fittnes_v(Individual *src, void *opts) {
  (void) opts;
  Vektor *v = (Vektor *) src->iv;

  return abs(v->a - v->b) + abs(v->b - v->c)
            + abs(v->c - v->d) + abs(v->d - v->e)
              + abs(v->e - v->f) + abs(v->f - v->a);
}

int main(int argc, char *argv[]) {

  if (argc != 4) {
    printf("%s <num generations> <num threads> <verbose level(0-3)>\n", 
           argv[0]);
    exit(1);
  }

  int verbose = EV_VEB0;

  switch (atoi(argv[3])) {
    case 1:   verbose = EV_VEB1; break;
    case 2:   verbose = EV_VEB2; break;
    case 3:   verbose = EV_VEB3; break;
    default : verbose = EV_VEB0; 
  }

  Individual best;
  void **opts = malloc(sizeof(void *) * TEST_NUM_IVS);

  uint64_t i;
  for (i = 0; i < TEST_NUM_IVS; i++) {
    opts[i] = malloc(sizeof(uint64_t));
    *(uint64_t *) opts[i] = i;
  }

  EvInitArgs args;
  args.init_iv              = init_v;
  args.clone_iv             = clone_v;
  args.free_iv              = free_v;
  args.mutate               = mutate_v;
  args.fitness              = fittnes_v;
  args.recombinate          = recombinate_v;
  args.continue_ev          = NULL;
  args.population_size      = TEST_NUM_IVS;
  args.generation_limit     = atoi(argv[1]);
  args.mutation_propability = 1.0;
  args.death_percentage     = 0.5;
  args.opts                 = opts;
  args.num_threads          = atoi(argv[2]);
  args.flags                = EV_UREC|EV_UMUT|EV_AMUT|EV_KEEP|verbose;

  best = best_evolution(&args);;

  #ifndef NO_OUTPUT
  Vektor *v = (Vektor *) best.iv;


  printf("Best Vektor: (%3d, %3d, %3d, %3d, %3d, %3d) %10li\n\n", 
         v->a, 
         v->b, 
         v->c, 
         v->d, 
         v->e, 
         v->f, 
         best.fitness);

  #else
  (void) best;
  #endif

  free_v(best.iv, (void *) opts);

  return 0;
}

