/*
 * estreshados.c
 *
 *  Created on: 02/03/2016
 *      Author: ernesto
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include <stddef.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>

#ifdef CACA_COMUN_LOG
#include <execinfo.h>
#endif

#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#endif

#define CACA_COMUN_TAM_MAX_LINEA 11
#define CACA_LOG_MAX_TAM_CADENA 200

#define BITCH_VECTOR_NUM_BITS (sizeof(bitch_vector) * 8)

#define CACA_COMUN_ASSERT_DUROTE 0
#define CACA_COMUN_ASSERT_SUAVECITO 1
#define CACA_COMUN_ASSERT_NIMADRES 2

typedef unsigned long tipo_dato;

typedef long long bitch_vector;

typedef enum BOOLEANOS {
	falso = 0, verdadero
} bool;

/*
 #define CACA_COMUN_TIPO_ASSERT CACA_COMUN_ASSERT_SUAVECITO
 */
#define CACA_COMUN_TIPO_ASSERT CACA_COMUN_ASSERT_DUROTE

#if CACA_COMUN_TIPO_ASSERT == CACA_COMUN_ASSERT_DUROTE
#define assert_timeout(condition) assert(condition);
#endif
#if CACA_COMUN_TIPO_ASSERT == CACA_COMUN_ASSERT_SUAVECITO
#define assert_timeout(condition) if(!(condition)){printf("fuck\n");sleep(10);}
#endif
#if CACA_COMUN_TIPO_ASSERT == CACA_COMUN_ASSERT_NIMADRES
#define assert_timeout(condition) 0
#endif

#ifdef CACA_COMUN_LOG
#define caca_log_debug(formato, args...) \
		do \
		{ \
			size_t profundidad = 0; \
			void *array[CACA_LOG_MAX_TAM_CADENA]; \
 			profundidad = backtrace(array, CACA_LOG_MAX_TAM_CADENA); \
			caca_log_debug_func(formato,__FILE__, __func__, __LINE__,profundidad,##args); \
		} \
		while(0);
#else
void caca_log_debug_func(const char *format, ...);
#define caca_log_debug(formato, args...) 0
#endif

#define caca_comun_max(x,y) ((x) < (y) ? (y) : (x))
#define caca_comun_min(x,y) ((x) < (y) ? (x) : (y))

void caca_log_debug_func(const char *format, ...);

#define ESTRESHADOS_MAX_ESTRELLAS 15000
#define ESTRESHADOS_MAX_ELEMENTOS_COORDENADAS ESTRESHADOS_MAX_ESTRELLAS*2
#define ESTRESHADOS_MAX_ELEMENTOS_INPUT ESTRESHADOS_MAX_ELEMENTOS_COORDENADAS+2
#define ESTRESHADOS_MAX_VALOR 32000
#define ESTRESHADOS_VALOR_INVALIDO -1

#if 1

#define AVL_VALOR_INVALIDO ESTRESHADOS_VALOR_INVALIDO

struct avl_node_s {
	tipo_dato llave;
	int altura;
	int indice_en_arreglo;
	struct avl_node_s *left;
	struct avl_node_s *right;
	struct avl_node_s *padre;
};

typedef struct avl_node_s avl_node_t;

struct avl_tree_s {
	int max_nodos;
	int nodos_usados;
	int nodos_realmente_en_arbol;
	int nodos_realmente_creados;
	int ultimo_nodo_liberado_idx;
	struct avl_node_s *root;
	avl_node_t *nodos_mem;
};

typedef struct avl_tree_s avl_tree_t;

typedef struct avl_tree_iterator_t {
	avl_tree_t *arbolin;
	char *contador_visitas;
	avl_node_t *nodo_actual;
} avl_tree_iterator_t;

/* Create a new AVL tree. */
avl_tree_t *avl_create(avl_tree_t **arbolin, int max_nodos) {
	avl_tree_t *tree = NULL;

	assert_timeout(arbolin);

	tree = calloc(1, sizeof(avl_tree_t));

	assert_timeout(tree);

	tree->max_nodos = max_nodos;

	tree->nodos_mem = calloc(max_nodos * 2, sizeof(avl_node_t));

	assert_timeout(tree->nodos_mem);

	tree->root = NULL;

	*arbolin = tree;

	return tree;
}

static inline void avl_destroy(avl_tree_t *arbolin) {
	free(arbolin->nodos_mem);
	free(arbolin);

}

/* Initialize a new node. */
avl_node_t *avl_create_node(avl_tree_t *arbolin) {
	avl_node_t *node = NULL;

	assert(
			arbolin->ultimo_nodo_liberado_idx
					|| arbolin->nodos_usados <= arbolin->max_nodos);

	if (arbolin->ultimo_nodo_liberado_idx) {
		node = arbolin->nodos_mem + arbolin->ultimo_nodo_liberado_idx;
		node->indice_en_arreglo = arbolin->ultimo_nodo_liberado_idx;
		arbolin->ultimo_nodo_liberado_idx = 0;
	} else {
		node = arbolin->nodos_mem + arbolin->nodos_usados++;
		node->indice_en_arreglo = arbolin->nodos_usados - 1;

		arbolin->nodos_realmente_en_arbol++;
		arbolin->nodos_realmente_creados++;
	}
	return node;
}

/* Find the height of an AVL no repulsivamente */
int avl_node_height(avl_node_t *node) {
	return node ? node->altura : 0;
}

static inline void avl_node_actualizar_altura(avl_node_t *node) {
	int height_left = 0;
	int height_right = 0;

	if (node->left) {
		height_left = node->left->altura;
	}
	if (node->right) {
		height_right = node->right->altura;
	}
	if (node->left || node->right) {
		node->altura = (height_right > height_left ? height_right : height_left)
				+ 1;
	} else {
		node->altura = 0;
	}
}

/* Find the balance of an AVL node */
int avl_balance_factor(avl_node_t *node) {
	int bf = 0;

	if (node->left)
		bf += avl_node_height(node->left);
	if (node->right)
		bf -= avl_node_height(node->right);

	return bf;
}

/* Left Left Rotate */
avl_node_t *avl_rotate_leftleft(avl_node_t *node) {
	avl_node_t *a = node;
	avl_node_t *b = a->left;
	avl_node_t *padre = NULL;

	padre = node->padre;

	a->left = b->right;
	b->right = a;

	avl_node_actualizar_altura(a);
	avl_node_actualizar_altura(b);

	a->padre = b;
	b->padre = padre;
	if (a->left) {
		a->left->padre = a;
	}

	return (b);
}

/* Left Right Rotate */
avl_node_t *avl_rotate_leftright(avl_node_t *node) {
	avl_node_t *a = node;
	avl_node_t *b = a->left;
	avl_node_t *c = b->right;
	avl_node_t *padre = NULL;

	padre = node->padre;

	a->left = c->right;
	b->right = c->left;
	c->left = b;
	c->right = a;

	avl_node_actualizar_altura(a);
	avl_node_actualizar_altura(b);
	avl_node_actualizar_altura(c);

	a->padre = c;
	b->padre = c;
	c->padre = padre;
	if (a->left) {
		a->left->padre = a;
	}
	if (b->right) {
		b->right->padre = b;
	}

	return (c);
}

/* Right Left Rotate */
avl_node_t *avl_rotate_rightleft(avl_node_t *node) {
	avl_node_t *a = node;
	avl_node_t *b = a->right;
	avl_node_t *c = b->left;
	avl_node_t *padre = NULL;

	padre = node->padre;

	a->right = c->left;
	b->left = c->right;
	c->right = b;
	c->left = a;

	avl_node_actualizar_altura(a);
	avl_node_actualizar_altura(b);
	avl_node_actualizar_altura(c);

	a->padre = c;
	b->padre = c;
	c->padre = padre;
	if (a->right) {
		a->right->padre = a;
	}
	if (b->left) {
		b->left->padre = b;
	}

	return (c);
}

/* Right Right Rotate */
avl_node_t *avl_rotate_rightright(avl_node_t *node) {
	avl_node_t *a = node;
	avl_node_t *b = a->right;
	avl_node_t *padre = NULL;

	padre = node->padre;

	a->right = b->left;
	b->left = a;

	avl_node_actualizar_altura(a);
	avl_node_actualizar_altura(b);

	a->padre = b;
	b->padre = padre;
	if (a->right) {
		a->right->padre = a;
	}

	return (b);
}

static inline avl_node_t *avl_balance_node_insertar(const avl_node_t *node,
		const long llave_nueva) {
	avl_node_t *newroot = NULL;
	avl_node_t *nodo_actual = NULL;

	newroot = (avl_node_t *) node;
	nodo_actual = node->padre;
	while (nodo_actual) {

		int bf = 0;
		avl_node_t *padre = NULL;
		avl_node_t **rama_padre = NULL;

		bf = avl_balance_factor(nodo_actual);

		if (bf >= 2) {
			/* Left Heavy */

			if (llave_nueva > nodo_actual->left->llave) {
				newroot = avl_rotate_leftright(nodo_actual);
			} else {
				newroot = avl_rotate_leftleft(nodo_actual);
			}

		} else if (bf <= -2) {
			/* Right Heavy */
			if (llave_nueva < nodo_actual->right->llave) {
				newroot = avl_rotate_rightleft(nodo_actual);
			} else {
				newroot = avl_rotate_rightright(nodo_actual);
			}

		} else {
			/* This node is balanced -- no change. */

			newroot = nodo_actual;
			avl_node_actualizar_altura(nodo_actual);
		}

		if (newroot->padre) {
			padre = newroot->padre;
			if (llave_nueva < padre->llave) {
				rama_padre = &padre->left;
			} else {
				if (llave_nueva > padre->llave) {
					rama_padre = &padre->right;
				} else {
					assert_timeout(0);
				}
			}
			*rama_padre = newroot;
		}

		nodo_actual = nodo_actual->padre;
	}

	return (newroot);
}

/* Balance a given tree */
void avl_balance_insertar(avl_tree_t *tree, avl_node_t *nodo, long llave_nueva) {

	avl_node_t *newroot = NULL;

	newroot = avl_balance_node_insertar(nodo, llave_nueva);

	if (newroot != tree->root) {
		tree->root = newroot;
	}
}

/* Insert a new node. */
void avl_insert(avl_tree_t *tree, long value) {
	avl_node_t *node = NULL;
	avl_node_t *next = NULL;
	avl_node_t *last = NULL;

	/* Well, there must be a first case */
	if (tree->root == NULL ) {
		node = avl_create_node(tree);
		node->llave = value;

		tree->root = node;

		/* Okay.  We have a root already.  Where do we put this? */
	} else {
		next = tree->root;

		while (next != NULL ) {
			last = next;

			if (value < next->llave) {
				next = next->left;

			} else if (value > next->llave) {
				next = next->right;

				/* Have we already inserted this node? */
			} else if (value == next->llave) {
				caca_log_debug("llave ya existe, no insertada\n");
				assert_timeout(0);
			}
		}

		node = avl_create_node(tree);
		node->llave = value;

		if (value < last->llave) {
			last->left = node;
		}
		if (value > last->llave) {
			last->right = node;
		}

		node->padre = last;

	}

	avl_balance_insertar(tree, node, value);
}

/* Find the node containing a given value */
avl_node_t *avl_find(avl_tree_t *tree, long value) {
	avl_node_t *current = tree->root;

	while (current && current->llave != value) {
		if (value > current->llave)
			current = current->right;
		else
			current = current->left;
	}

	return current ? current->llave == value ? current : NULL :NULL;
}

avl_node_t *avl_find_descartando(avl_node_t *nodo_raiz,
		avl_node_t **primer_nodo_mayor_o_igual, long value, int tope,
		bool *tope_topado) {
	avl_node_t *current = NULL;
	avl_node_t *primer_nodo_mayor = NULL;

	current = nodo_raiz;
	assert_timeout(!tope_topado || tope);

	if (tope_topado) {
		*tope_topado = falso;
	}

	while (current && current->llave != value) {
		if (tope_topado) {
			if (current->llave >= tope && value >= tope) {
				*tope_topado = verdadero;
				break;
			}
		}
		if (value > current->llave) {
			current = current->right;
		} else {
			if (!primer_nodo_mayor) {
				primer_nodo_mayor = current;
			}
			current = current->left;
		}
	}

	*primer_nodo_mayor_o_igual = primer_nodo_mayor;
	if (!primer_nodo_mayor_o_igual) {
		if (current && (current->llave == value)) {
			*primer_nodo_mayor_o_igual = current;
		}
	}

	return current ? current->llave == value ? current : NULL :NULL;
}

	/* Do a depth first traverse of a node. */
void avl_traverse_node_dfs(avl_node_t *node, int depth) {
	int i = 0;

	if (node->left)
		avl_traverse_node_dfs(node->left, depth + 2);

	for (i = 0; i < depth; i++)
		putchar(' ');
	printf("%ld: %d\n", node->llave, avl_balance_factor(node));

	if (node->right)
		avl_traverse_node_dfs(node->right, depth + 2);
}

/* Do a depth first traverse of a tree. */
void avl_traverse_dfs(avl_tree_t *tree) {
	avl_traverse_node_dfs(tree->root, 0);
}

static inline void avl_tree_iterador_ini(avl_tree_t *arbolin,
		avl_tree_iterator_t *iter) {
	iter->contador_visitas = calloc(arbolin->nodos_realmente_creados,
			sizeof(char));
	assert_timeout(iter->contador_visitas);
	iter->arbolin = arbolin;
}

static inline void avl_tree_iterador_fini(avl_tree_iterator_t *iter) {
	free(iter->contador_visitas);
}

static inline bool avl_tree_iterador_hay_siguiente(avl_tree_iterator_t *iter) {
	return !(iter->nodo_actual == iter->arbolin->root
			&& iter->contador_visitas[iter->arbolin->root->indice_en_arreglo]
					== 2);
}

static inline avl_node_t* avl_tree_iterador_siguiente(avl_tree_iterator_t *iter) {
	int contador_actual = 0;
	avl_node_t *nodo = NULL;
	avl_node_t *last_of_us = NULL;
	avl_node_t *nodo_actual = NULL;

	if (!iter->nodo_actual) {
		nodo_actual = iter->nodo_actual = iter->arbolin->root;
	}

	if (!avl_tree_iterador_hay_siguiente(iter)) {
		return NULL ;
	}

	contador_actual =
			iter->contador_visitas[iter->nodo_actual->indice_en_arreglo];

	iter->contador_visitas[iter->nodo_actual->indice_en_arreglo]++;

	switch (contador_actual) {
	case 0:
	case 1:
		if (!contador_actual) {
			nodo_actual = iter->nodo_actual->left;
			if (!nodo_actual) {
				return iter->nodo_actual;
			}
		} else {
			nodo_actual = iter->nodo_actual->right;
			if (!nodo_actual) {
				nodo_actual = iter->nodo_actual->padre;
				while (nodo_actual
						&& iter->contador_visitas[nodo_actual->indice_en_arreglo]
								== 2) {
					last_of_us = nodo_actual;
					nodo_actual = nodo_actual->padre;
				}
				if (!nodo_actual) {
					if (last_of_us) {
						iter->nodo_actual = last_of_us;
					}
				} else {
					iter->nodo_actual = nodo_actual;
				}
				return nodo_actual;
			}
		}

		while (nodo_actual) {
			last_of_us = nodo_actual;
			iter->contador_visitas[nodo_actual->indice_en_arreglo]++;
			nodo_actual = last_of_us->left;
		}

		nodo = iter->nodo_actual = last_of_us;

		break;
	default:
		assert_timeout(0)
		;
		break;
	}

	return nodo;
}

static inline avl_node_t* avl_tree_iterador_obtener_actual(
		avl_tree_iterator_t *iter) {
	avl_node_t *nodo = NULL;

	if (!iter->nodo_actual) {
		avl_tree_iterador_siguiente(iter);
	}
	nodo = iter->nodo_actual;

	return nodo;

}

static inline char *avl_tree_inoder_node_travesti(avl_node_t *nodo, char *buf,
		int profundidad_maxima) {
	char num_buf[100] = { '\0' };
	int profundidad = 0;

	assert_timeout(profundidad_maxima == -1 || profundidad != -1);

	if (nodo != NULL ) {
		profundidad = profundidad_maxima - nodo->altura;

		assert_timeout(!nodo->right || nodo->right->padre == nodo);
		avl_tree_inoder_node_travesti(nodo->right, buf, profundidad_maxima);

		if (profundidad_maxima != -1) {
			for (int i = 0; i < profundidad; i++) {
				strcat(buf, " ");
			}
		}
		sprintf(num_buf, "%ld", nodo->llave);
		strcat(buf, num_buf);
		if (profundidad_maxima != -1) {
			strcat(buf, "\n");
		}

		assert_timeout(!nodo->left || nodo->left->padre == nodo);
		avl_tree_inoder_node_travesti(nodo->left, buf, profundidad_maxima);

		/*
		 if (profundidad_maxima != -1) {
		 strcat(buf, "\n");
		 for (int i = 0; i <= profundidad; i++) {
		 strcat(buf, " ");
		 }
		 }
		 */
	}
	return buf;
}

static inline char* avl_tree_sprint(avl_tree_t *arbolini, char *buf) {
	avl_tree_inoder_node_travesti(arbolini->root, buf, -1);
	return buf;
}

static inline char* avl_tree_sprint_identado(avl_tree_t *arbolini, char *buf) {
	int profundidad_maxima = 0;

	if (!arbolini->root) {
		return buf;
	}

	profundidad_maxima = arbolini->root->altura;
	avl_tree_inoder_node_travesti(arbolini->root, buf, profundidad_maxima);
	return buf;
}

static inline avl_node_t* avl_tree_iterador_asignar_actual(
		avl_tree_iterator_t *iter, int llave) {
	avl_tree_t *arbolazo = NULL;
	avl_node_t *nodo_actual = NULL;
	avl_node_t *last_of_us = NULL;

	arbolazo = iter->arbolin;

	nodo_actual = arbolazo->root;

	while (nodo_actual) {
		last_of_us = nodo_actual;
		if (llave < nodo_actual->llave) {
			iter->contador_visitas[nodo_actual->indice_en_arreglo] = 1;
			nodo_actual = nodo_actual->left;
		} else {
			iter->contador_visitas[nodo_actual->indice_en_arreglo] = 2;
			if (llave > nodo_actual->llave) {
				nodo_actual = nodo_actual->right;
			} else {
				break;
			}

		}
	}

	if (nodo_actual) {
		iter->nodo_actual = nodo_actual;
	} else {
		iter->nodo_actual = last_of_us;
	}
	iter->contador_visitas[iter->nodo_actual->indice_en_arreglo] = 1;
	return iter->nodo_actual;
}

static inline avl_node_t* avl_tree_max_min(avl_tree_t *arbolin, bool max) {
	avl_node_t *nodo_actual = NULL;
	avl_node_t *last_of_us = NULL;

	nodo_actual = last_of_us = arbolin->root;

	while (nodo_actual) {
		last_of_us = nodo_actual;
		if (max) {
			nodo_actual = nodo_actual->right;
		} else {
			nodo_actual = nodo_actual->left;
		}
	}

	return last_of_us;
}

static inline void avl_tree_validar_arbolin_indices(avl_tree_t *arbolin,
		avl_node_t *nodo) {
	if (nodo != NULL ) {
		assert_timeout(&arbolin->nodos_mem[nodo->indice_en_arreglo] == nodo);
		assert_timeout(!nodo->left || nodo->left->padre == nodo);
		avl_tree_validar_arbolin_indices(arbolin, nodo->left);
		assert_timeout(!nodo->right || nodo->right->padre == nodo);
		avl_tree_validar_arbolin_indices(arbolin, nodo->right);
	}
}

/* Balance a given node */
/* Given a non-empty binary search tree, return the node with minimum
 key value found in that tree. Note that the entire tree does not
 need to be searched. */
static inline avl_node_t* avl_siguiente_nodo_inorder(avl_node_t *node) {
	avl_node_t *current = node;

	/* loop down to find the leftmost leaf */
	while (current->left != NULL ) {
		current = current->left;
	}

	return current;
}

static inline avl_node_t *avl_nodo_borrar(avl_tree_t *arbolini,
		avl_node_t *root, int key) {
	// STEP 1: PERFORM STANDARD BST DELETE

	if (root == NULL ) {
		return root;
	}

	if (key < root->llave) {
		// If the key to be deleted is smaller than the root's key,
		// then it lies in left subtree
		root->left = avl_nodo_borrar(arbolini, root->left, key);
		assert_timeout(!root->left || root->left->padre == root);
	} else {
		// If the key to be deleted is greater than the root's key,
		// then it lies in right subtree
		if (key > root->llave) {
			root->right = avl_nodo_borrar(arbolini, root->right, key);
			assert_timeout(!root->right || root->right->padre == root);
		} else {
			if (root->left == NULL || root->right == NULL ) {
				avl_node_t *temp = root->left ? root->left : root->right;

				// No child case
				if (temp == NULL ) {
					temp = root;
					root = NULL;
				} else {
					// One child case
					int idx_en_arreglo = 0;
					avl_node_t *padre = NULL;

					padre = root->padre;
					idx_en_arreglo = root->indice_en_arreglo;
					*root = *temp; // Copy the contents of the non-empty child
					root->padre = padre;
					root->indice_en_arreglo = idx_en_arreglo;
					if (root->left) {
						root->left->padre = root;
					}
					if (root->right) {
						root->right->padre = root;
					}
				}

				assert_timeout(!arbolini->ultimo_nodo_liberado_idx);
				arbolini->ultimo_nodo_liberado_idx = temp->indice_en_arreglo;
				memset(temp, 0, sizeof(avl_node_t));
				temp->llave = AVL_VALOR_INVALIDO;

			} else {
				// node with two children: Get the inorder successor (smallest
				// in the right subtree)
				avl_node_t *temp = avl_siguiente_nodo_inorder(root->right);

				// Copy the inorder successor's data to this node
				root->llave = temp->llave;

				// Delete the inorder successor
				root->right = avl_nodo_borrar(arbolini, root->right,
						temp->llave);
			}
		}
	}

// If the tree had only one node then return
	if (root == NULL ) {
		return root;
	}

// STEP 2: UPDATE HEIGHT OF THE CURRENT NODE
	avl_node_actualizar_altura(root);

// STEP 3: GET THE BALANCE FACTOR OF THIS NODE (to check whether
//  this node became unbalanced)
	int balance = avl_balance_factor(root);

// If this node becomes unbalanced, then there are 4 cases

// Left Left Case
	if (balance > 1 && avl_balance_factor(root->left) >= 0) {
		return avl_rotate_leftleft(root);
	}

// Left Right Case
	if (balance > 1 && avl_balance_factor(root->left) < 0) {
		return avl_rotate_leftright(root);
	}

// Right Right Case
	if (balance < -1 && avl_balance_factor(root->right) <= 0) {
		return avl_rotate_rightright(root);
	}

// Right Left Case
	if (balance < -1 && avl_balance_factor(root->right) > 0) {
		return avl_rotate_rightleft(root);
	}

	return root;
}

void avl_borrar(avl_tree_t *tree, int value) {

	avl_node_t *newroot = NULL;

	newroot = avl_nodo_borrar(tree, tree->root, value);

	if (newroot != tree->root) {
		tree->root = newroot;
	}
	tree->nodos_realmente_en_arbol--;
}

#endif

void caca_comun_current_utc_time(struct timespec *ts) {

#ifdef __MACH__
	clock_serv_t cclock;
	mach_timespec_t mts;
	host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
	clock_get_time(cclock, &mts);
	mach_port_deallocate(mach_task_self(), cclock);
	ts->tv_sec = mts.tv_sec;
	ts->tv_nsec = mts.tv_nsec;
#else
	clock_gettime(CLOCK_REALTIME, ts);
#endif

}

void caca_comun_timestamp(char *stime) {
	time_t ltime;
	struct tm result;
	long ms;
	struct timespec spec;
	char parte_milisecundos[50];

	ltime = time(NULL );

	localtime_r(&ltime, &result);
	asctime_r(&result, stime);

	*(stime + strlen(stime) - 1) = ' ';

	caca_comun_current_utc_time(&spec);
	ms = round(spec.tv_nsec / 1.0e3);
	sprintf(parte_milisecundos, "%ld", ms);
	strcat(stime, parte_milisecundos);
}
void caca_log_debug_func(const char *format, ...) {

	va_list arg;
	va_list arg2;
	const char *PEDAZO_TIMESTAMP_HEADER = "tiempo: %s; ";
	const char *HEADER =
			"archivo: %s; funcion: %s; linea %d; nivel: %zd caca 8====D ";
	char formato[CACA_LOG_MAX_TAM_CADENA + sizeof(HEADER)
			+ sizeof(PEDAZO_TIMESTAMP_HEADER)] = { '\0' };
	char pedazo_timestamp[sizeof(PEDAZO_TIMESTAMP_HEADER) + 100] = { '\0' };
	char cadena_timestamp[100] = { '\0' };

	caca_comun_timestamp(cadena_timestamp);
	sprintf(pedazo_timestamp, PEDAZO_TIMESTAMP_HEADER, cadena_timestamp);

	strcpy(formato, pedazo_timestamp);
	strcat(formato, HEADER);
	strcat(formato, format);
	strcat(formato, "\n");

	va_start(arg, format);
	va_copy(arg2, arg);
	vprintf(formato, arg2);
	va_end(arg2);
	va_end(arg);
}
static char *caca_arreglo_a_cadena(tipo_dato *arreglo, int tam_arreglo,
		char *buffer) {
	int i;
	char *ap_buffer = NULL;
	int characteres_escritos = 0;
#ifdef ONLINE_JUDGE
	return NULL;
#endif

	memset(buffer, 0, 100);
	ap_buffer = buffer;

	for (i = 0; i < tam_arreglo; i++) {
		characteres_escritos +=
				sprintf(ap_buffer + characteres_escritos, "%2ld",
						*(arreglo + i));
		if (i < tam_arreglo - 1) {
			*(ap_buffer + characteres_escritos++) = ',';
		}
	}
	*(ap_buffer + characteres_escritos) = '\0';
	return ap_buffer;
}

void caca_comun_strreplace(char s[], char chr, char repl_chr) {
	int i = 0;
	while (s[i] != '\0') {
		if (s[i] == chr) {
			s[i] = repl_chr;
		}
		i++;
	}
}

static inline int lee_matrix_long_stdin(tipo_dato *matrix, int *num_filas,
		int *num_columnas, int num_max_filas, int num_max_columnas) {
	int indice_filas = 0;
	int indice_columnas = 0;
	long numero = 0;
	char *siguiente_cadena_numero = NULL;
	char *cadena_numero_actual = NULL;
	char *linea = NULL;

	linea = calloc(CACA_COMUN_TAM_MAX_LINEA, sizeof(char));

	while (indice_filas < num_max_filas
			&& fgets(linea, CACA_COMUN_TAM_MAX_LINEA, stdin)) {
		indice_columnas = 0;
		cadena_numero_actual = linea;
		caca_comun_strreplace(linea, '\n', '\0');
		if (!strlen(linea)) {
			caca_log_debug("weird, linea vacia\n");
			continue;
		}
		for (siguiente_cadena_numero = linea;; siguiente_cadena_numero =
				cadena_numero_actual) {
			caca_log_debug("el numero raw %s\n", linea);
			numero = strtol(siguiente_cadena_numero, &cadena_numero_actual, 10);
			if (cadena_numero_actual == siguiente_cadena_numero) {
				break;
			}
			*(matrix + indice_filas * num_max_columnas + indice_columnas) =
					numero;
			caca_log_debug("en col %d, fil %d, el valor %lu\n", indice_columnas,
					indice_filas, numero);
			indice_columnas++;
			caca_log_debug("las columnas son %d\n", indice_columnas);
		}
		if (num_columnas) {
			num_columnas[indice_filas] = indice_columnas;
		}
		indice_filas++;
		caca_log_debug("las filas son %d, con clos %d\n", indice_filas,
				indice_columnas);
	}

	*num_filas = indice_filas;
	free(linea);
	return 0;
}

void estreshados_main() {
	int num_filas = 0;
	tipo_dato num_estrellas = 0;
	tipo_dato *coordenadas_separadas = NULL;
	tipo_dato *coordenadas_enmascaradas = NULL;
	tipo_dato *matrix = NULL;

	avl_tree_t *arbolin = NULL;

	char *buffer = NULL;
	buffer = calloc(CACA_COMUN_TAM_MAX_LINEA * 10, sizeof(char));
	assert_timeout(buffer);

	matrix = calloc(ESTRESHADOS_MAX_ESTRELLAS + 1, sizeof(tipo_dato));
	assert_timeout(matrix);

	lee_matrix_long_stdin(matrix, &num_filas, NULL,
	ESTRESHADOS_MAX_ELEMENTOS_INPUT + 1, 2);

	num_estrellas = *matrix;
	coordenadas_separadas = matrix + 2;

	caca_log_debug("cuando veiamos estreshas %lu", num_estrellas);
	caca_log_debug("tu eras una de ellas %s",
			caca_arreglo_a_cadena(coordenadas_separadas,num_estrellas*2,buffer));

	coordenadas_enmascaradas = calloc(ESTRESHADOS_MAX_ESTRELLAS,
			sizeof(tipo_dato));
	assert_timeout(coordenadas_enmascaradas);

	for (int i = 0; i < num_estrellas; i++) {
		*(coordenadas_enmascaradas + i) = (unsigned int) *(coordenadas_separadas
				+ i * 2 + 1);
		caca_log_debug(
				"asignando a enmascaradas 'y' %u, keda la enmascarada %lu",
				coordenadas_separadas[i*2+1], coordenadas_enmascaradas[i]);

		*(coordenadas_enmascaradas + i) <<= 32;
		caca_log_debug("corrimiento de 32 a enmascarado keda %lu",
				coordenadas_enmascaradas[i]);

		*(coordenadas_enmascaradas + i) |=
				(unsigned int) *(coordenadas_separadas + i * 2 + 0);
		caca_log_debug(
				"asignando a enmascaradas 'x' %u, keda la enmascarada %lu",
				coordenadas_separadas[i*2+0], coordenadas_enmascaradas[i]);
	}

	caca_log_debug("las coordenadas enmascaradas %s",
			caca_arreglo_a_cadena(coordenadas_enmascaradas,num_estrellas,buffer));

	avl_create(&arbolin, num_estrellas);

	for (int i = 0; i < num_estrellas; i++) {
		tipo_dato estrella_negra = 0;

		estrella_negra = *(coordenadas_enmascaradas + i);

		caca_log_debug("insertando %u,%u (%lu)", (unsigned int)estrella_negra,
				(unsigned int)(estrella_negra>>32), estrella_negra);

		avl_insert(arbolin, estrella_negra);

#ifndef ONLINE_JUDGE
		avl_tree_validar_arbolin_indices(arbolin, arbolin->root);
#endif
		memset(buffer, '\0', CACA_COMUN_TAM_MAX_LINEA*10);

		caca_log_debug("el arbolin aora es\n%s",
				avl_tree_sprint_identado(arbolin,buffer));
	}

	avl_destroy(arbolin);

	free(coordenadas_enmascaradas);
	free(matrix);
	free(buffer);
}

int main(int argc, char **argv) {
	estreshados_main();
}
