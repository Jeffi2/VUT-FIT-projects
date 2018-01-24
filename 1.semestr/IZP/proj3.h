/** @file Proj3.h
 *  @mainpage
 *
 * This is the documentation for Project 3.
 * Autor: Petr Juda, xjudapp00 
 */

/** @defgroup group1 Structs
 * Declaration of structuts.
 * @{
 */

/**
 * @brief Struct for a object.
 *
 * @param id Identificator for object.
 * @param x Value of x coordinate.
 * @param y Value of y coordinate.
 */
struct obj_t {
    int id;
    float x;
    float y;
};

/**
 * @brief Struct for a cluster.
 *
 * @param size Value of objects saved in cluster.
 * @param capacity Value of max objects, that can be saved in cluster.
 * @param obj_t Pointer to array of objects saved in cluster.
 */
struct cluster_t {
    int size;
    int capacity;
    struct obj_t *obj;
};
/** @} */

/** @defgroup group2 Functions for work with clusters and objects.
 * Functions for work with clusters and objects.
 * @{
 */

/**
 * @brief Inicialization of cluster. Allocate memory for capacite of objects.
 *
 * @pre c != NULL
 * @pre cap >= 0
 * @param c Pointer to cluster.
 * @param cap INT value of capacite for object.
 * @return Nothing
 */
void init_cluster(struct cluster_t *c, int cap);

/**
 * @brief Remove all objects in cluster. Inicialization as a free cluster
 *
 * @param c Pointer to cluster.
 * @post c = {0, 0, NULL}
 * @return Nothing
 */
void clear_cluster(struct cluster_t *c);

/**
 * Chunk of cluster objects. Value recommended for reallocation.
 */
extern const int CLUSTER_CHUNK;

/**
 * @brief Reallocation of capacite for objects of cluster. Set capacite as new_cap.
 *
 * @pre c->capacity >= 0
 * @pre new_cap >= 0
 * @param c Pointer to cluster.
 * @param narr Value of new capacite.
 * @return Pointer to reallocated cluster or NULL if error.
 */
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap);

/**
 * @brief Add the object OBJ at the end of cluster c. If the cluster is full, reallocate capacite of cluster.
 *
 * @param c Pointer to cluster.
 * @param obj Object loaded from file.
 * @return Nothing.
 */
void append_cluster(struct cluster_t *c, struct obj_t obj);

/**
 * @brief Into the cluster c1 add all objects from cluster c2. If capacite of c1 is less than needed, reallocate capacite c1.
 All objects in cluster c1 are sort by the ID. Cluster c2 doesn't change.
 *
 * @pre c1 != NULL
 * @pre c2 != NULL
 * @post c->size++
 * @param c1 Pointer to the first cluster.
 * @param c2 Pointer to the second cluster.
 * @return Nothing.
 */
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2);
/** @} */

/** @defgroup group3 Functions for work with array of clusters.
 * Functions for work with array of clusters.
 * @{
 */

/**
 * @brief Delete one cluster at index 'idx' from the array of clusters 'carr'. The array 'carr' contains 'narr' clusters.
 *
 * @pre narr > 0
 * @pre idx < narr
 * @param carr Pointer to array of clusters.
 * @param narr Count of clusters stored in the array carr.
 * @param idx Index of the deleted cluster.
 * @return New count of clusters stored in the array carr.
 */
int remove_cluster(struct cluster_t *carr, int narr, int idx);

/**
 * @brief Calculate a Euclid distance between two objects.
 *
 * @pre o1 != NULL
 * @pre o2 != NULL
 * @param o1 Pointer to the first object.
 * @param o2 Pointer to the second object.
 * @return Euclid distance between two objects.
 */
float obj_distance(struct obj_t *o1, struct obj_t *o2);

/**
 * @brief Calculate a distance between two clusters. Using a obj_distance to choose the nearest neighbors.
 *
 * @pre c1 != NULL
 * @pre c1->size > 0
 * @pre c2 != NULL
 * @pre c2->size > 0
 * @param c1 Pointer to the first cluster.
 * @param c2 Pointer to the second cluster.
 * @return Distance between two objects.
 */
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2);

/**
 * @brief In array of clusters 'carr' find the two nearest neighbors. Using obj_distance. Array 'carr' contains 'narr' clusters.
 Indexs of found neighbors save in c1 and c2.
 *
 * @pre narr > 0
 * @post 0 <= c1 < c2 < narr
 * @param[in] carr Pointer to array of clusters.
 * @param[in] narr Count of clusters stored in the array carr.
 * @param[out] c1 Index of the first nearest cluster.
 * @param[out] c2 Index of the second nearest cluster.
 * @return Nothing.
 */
void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2);

/**
 * @brief Sort the objects in cluster by ascending ID.
 *
 * @param c Pointer to cluster.
 * @return Nothing.
 */
void sort_cluster(struct cluster_t *c);

/**
 * @brief Print one cluster at stdout.
 *
 * @param c Pointer to cluster.
 * @return Nothing.
 */
void print_cluster(struct cluster_t *c);

/**
 * @brief Load the objects from file 'filename'. For each object creates a cluster and save it in to the array of clusters.
 Allocate a memory for array of clusters.
 *
 * @pre arr != NULL
 * @post If error at 1line in file, returns 0.
 * @post If error at some other line in file, returns -number of loaded objects.
 * @post If error during opening the file, returns 2.
 * @param filename Name of file to open.
 * @param arr Pointer to pointer to array of clusters.
 * @return Count of loaded objects or 0, 2, -number_of_loaded_objects if error.
 */
int load_clusters(char *filename, struct cluster_t **arr);

/**
 * @brief Print the array of clusters at stdout.
 *
 * @param carr Pointer to firts cluster in array.
 * @param narr Count of printed clusters in array.
 * @return Nothing.
 */
void print_clusters(struct cluster_t *carr, int narr);
/** @} */
